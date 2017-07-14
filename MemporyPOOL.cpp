#include "MemporyPOOL.h"
MemporyPOOL::MemporyPOOL()
 {
     m_iBasePageSize = 1024*1024;
     m_iItemBaseSize = 48;
     m_dfactor = 1.25;
 }

 /*
  * 完成内存池总大小的申请，SlabClass对象数组的初始化和调用InitSlabClass函数对空闲Item的插入
 */

 int MemporyPOOL::Init(size_t poolSize)
 {
     if(poolSize <= 0 || poolSize < m_iBasePageSize)
     {
         return -1;
     }
     m_iPoolSize =
     m_iAvailSize = poolSize;
     void* pPool;
     size_t i = 0;
     size_t size;
  //完成内存池总大小的申请


     size = poolSize;
     pPool = malloc(size);
     if (!pPool)
     {
         return -1;
     }
     m_pPool =
     m_pCurrent = pPool;

     int i_useSize = m_iItemBaseSize;

     int i_idSize = sizeof(unsigned int);
 //SlabClass对象数组的初始化
     while(++i <= SCLSIZE-1)
     {
         if(i_useSize % CHUNK_ALIGN_BYTES)
         {
             i_useSize += CHUNK_ALIGN_BYTES - (i_useSize % CHUNK_ALIGN_BYTES);
         }

         if(i_useSize*m_dfactor >= m_iBasePageSize )
         {
             m_iSlabClassListUsed = i;
             m_SlabClassList[m_iSlabClassListUsed]=new SlabClass(m_iBasePageSize,1,m_iSlabClassListUsed,&m_pCurrent,&m_iAvailSize,&m_slabLock);
             break;
         }
         m_SlabClassList[i] = new  SlabClass(i_useSize,m_iBasePageSize/(i_useSize+i_idSize),i,&m_pCurrent,&m_iAvailSize,&m_slabLock);
         i_useSize *= m_dfactor;

     }


     //m_pSlabClassBufAvail = ((char*)m_pSlab+sizeof(SlabClass));

    //调用InitSlabClass函数对空闲Item的插入

     for(i=1;i<=m_iSlabClassListUsed;i++)
     {
         m_SlabClassList[i]->InitSlabClass();
     }

     return 0; // success
 }

/*
* 当申请内存空间过大的时候，进行对SlabClass数组对象再次进行处理，以满足申请的内存
*/
 int MemporyPOOL::GetNewSlabClassId(size_t size)
 {
     m_poolLock.lock();
     int i = m_iSlabClassListUsed;
     if(i >= SCLSIZE-1)
     {
         m_poolLock.unlock();
         return 0;
     }
     int i_useSize = m_SlabClassList[m_iSlabClassListUsed]->GetSize();
     int i_idSize = sizeof(unsigned int);
     i_useSize = i_useSize * m_dfactor;
 //SlabClass对象数组的初始化
     while(++i < SCLSIZE-1)
     {
         if(i_useSize % CHUNK_ALIGN_BYTES)
         {
             i_useSize += CHUNK_ALIGN_BYTES - (i_useSize % CHUNK_ALIGN_BYTES);
         }

         if(i_useSize*m_dfactor >= size)
         {
             m_iSlabClassListUsed = i;
             m_SlabClassList[m_iSlabClassListUsed]=new SlabClass(size,1,m_iSlabClassListUsed,&m_pCurrent,&m_iAvailSize,&m_slabLock);
             goto end;
         }
         m_SlabClassList[i] = new  SlabClass(i_useSize,1,i,&m_pCurrent,&m_iAvailSize,&m_slabLock);
         i_useSize *= m_dfactor;

     }
     if(i >= SCLSIZE-1)
     {
          i = SCLSIZE-1;
          m_SlabClassList[i] = new  SlabClass(size,1,i,&m_pCurrent,&m_iAvailSize,&m_slabLock);
          m_iSlabClassListUsed = SCLSIZE-1;
     }

     end:
         m_poolLock.unlock();
         return i;
 }

 /*
  * 获取对应大小的slabclass的下标
 */
 int MemporyPOOL::GetSlabClassId(size_t size)
 {
     int id = 0;
     size_t i  = 0;
     m_poolLock.lock();
     while(++i <= m_iSlabClassListUsed)
     {
         if(m_SlabClassList[i]->GetSize()>=size)
         {
             id = i;
             break;
         }
     }
     m_poolLock.unlock();
     if(0 == id)
     {
         id = GetNewSlabClassId(size);
     }
     return id;
 }


/*
* 用户申请内存空间接口类似C语言中malloc
*/
 void* MemporyPOOL::Alloc(size_t size)
 {
     void* ret =NULL;
     int id = 0;
     id = GetSlabClassId(size);
     if(0 == id)
     {
         fprintf(stderr, "Alloc fail GetSlabClassId is error %3u size %7u\n", id, (unsigned int)size);
         return ret;
     }
     ret = m_SlabClassList[id]->GetItem();
     if(!ret)
     {
         fprintf(stderr, "Alloc fail %3u size %7u\n", id, (unsigned int)size);
         return ret;
     }
     ret = ((char*)ret + sizeof(unsigned int));
     fprintf(stderr, "Alloc success  %3u size %7u\n", id, (unsigned int)size);
     return ret;
 }


/*
* 用户释放内存空间接口类似C语言中free
*/
 void MemporyPOOL::Free(void* pBlock)
 {
     void *ptr = NULL;
     int id;
     Item *p_it;
     ptr = pBlock;
     if(!ptr)
         return;
     ptr = ((char*)ptr - sizeof(unsigned int));
     p_it = (Item*)ptr;
     id = p_it->id;
     m_SlabClassList[id]->FreeItem(ptr);
     fprintf(stderr, "Free %3u  pBlock %7p\n", id,pBlock);
 }


 /*
  * 销毁所有内存空间
 */
 void MemporyPOOL::Destroy()
 {
     if(!m_pPool)
         return;
     int i=1;
     for(i=1;i<m_iSlabClassListUsed;i++)
     {
         delete m_SlabClassList[i];
     }
     free(m_pPool);
     m_pPool = NULL;
     return;
 }
 MemporyPOOL::~MemporyPOOL()
 {
     Destroy();
 }
