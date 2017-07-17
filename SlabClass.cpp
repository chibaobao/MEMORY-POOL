#include "SlabClass.h"

SlabClass::SlabClass(size_t size,int perslab,int id,
          void ** ppCurrent,size_t *pAvailSize,std::mutex *pLock)
            :m_ppCurrent(ppCurrent),m_pAvailSize(pAvailSize),m_pLock(pLock) //每个小块size大小（不包括id）,perslab:class里面chunk个数
{
    m_size = size;
    m_iPerslab = perslab;
    m_id = id;
    m_pSlots = 0;
    m_iSlots = 0;
    if(size<sizeof(Item))
        size = sizeof(Item);
    m_iTotalsize = size + sizeof(unsigned int);//unsigned int指id大小
    fprintf(stderr, "slab class %3u: chunk size %9u perslab %7u usesize %7u\n",
                m_id, (unsigned int)m_iTotalsize, m_iPerslab,(unsigned int)m_size);

}
int SlabClass::FreeItem(void* ptr)//ptr指向整个chunk(包括id)
{
    Item *it;

    it = (Item *)ptr;

    it->id = 0;//释放后chunk被添加到链表中无需存储id,将id置0代表已经被释放，重复释放就可以被检测到了
    it->prev = 0;

    m_MyLock.lock();
    it->next = (Item*)m_pSlots;
    if (it->next) it->next->prev = it;
    m_pSlots = it;
    m_iSlots ++;
    m_MyLock.unlock();

    return 0;
}

int SlabClass::InitSlabClass()
{
    if(m_iPerslab*m_iTotalsize > *m_pAvailSize)//这里是单线程暂时没加锁
    {
        return -1;
    }
    GetNewPage();
    return 0;
}
void* SlabClass::GetItem()
{
    void *ret =NULL;
    Item *it;
    int is_again = 1;//这里变量名有点歧义 1是再来一次 0是不用
    do
    {
        m_MyLock.lock();
        if (this->m_iSlots != 0)
        {
            it = (Item *)this->m_pSlots;
            this->m_pSlots = it->next;
            if (it->next) it->next->prev = 0;
            this->m_iSlots--;
            if(m_iSlots == 0)
            {
                m_pSlots = 0;
            }
            ret = (void *)it;
            *(unsigned int*)it = m_id;
            is_again = 0;
            m_MyLock.unlock();
        }
        else
        {
            m_MyLock.unlock();
            if(GetNewPage() == -1)
            {
                is_again = 0;
                ret = NULL;
                break;
            }
            else
            {
                is_again = 1;
                ret = NULL;
            }

        }
    }while(is_again);
    return ret;
}

size_t SlabClass::GetSize()//data大小，每个小块data大小（不包括id）
{
    return m_size;//不会被更改，无需加锁
}
int SlabClass::GetNewPage()//开辟新的一页并初始化，内存池不够一页，则将剩余的全部分配
{
    size_t Perslab = m_iPerslab;
    (*m_pLock).lock();
    if(m_iPerslab*m_iTotalsize > *m_pAvailSize)
    {
        Perslab = (*m_pAvailSize)/m_iTotalsize;
        if(Perslab == 0)
        {
            fprintf(stderr, "getNewPageId not enough %3u\n",this->m_id);
            (*m_pLock).unlock();
            return -1;
        }
    }
//        m_Lock.unlock();
    unsigned int x;
    for (x = 0; x < Perslab; x++)
    {
        FreeItem(*m_ppCurrent);
//            m_Lock.lock();
        *m_ppCurrent = (char*)*m_ppCurrent + m_iTotalsize;
        *m_pAvailSize = *m_pAvailSize - m_iTotalsize;
//            m_Lock.unlock();

    }
    fprintf(stderr, "getNewPageId %3u\n",this->m_id);
    (*m_pLock).unlock();
    return 0;
}
