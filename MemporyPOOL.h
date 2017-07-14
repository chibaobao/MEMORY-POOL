#ifndef MemporyPOOL_H
#define MemporyPOOL_H
#include <malloc.h>
#include<stdlib.h>
#include<stdio.h>
#include <mutex>
#include "SlabClass.h"

#define SCLSIZE 128 //SlabClassList总大小
#define SCLBEGIN 1 //SlabClassList开始
#define CHUNK_ALIGN_BYTES 8
class MemporyPOOL
{
private:
    void *m_pPool;                      //开辟内存首地址
    void *m_pCurrent;                   //当前指针
    size_t m_iPoolSize;                 //内存池总大小
    size_t m_iAvailSize;                //剩余空间大小
    size_t m_iBasePageSize;              //基准页大小
    SlabClass* m_SlabClassList[SCLSIZE];//SlabClass对象数组
    size_t m_iSlabClassList;            //SlabClassList总大小
    size_t m_iSlabClassListUsed;        //SlabClassList已使用大小
    size_t m_iItemBaseSize;             //Item的最小的内存大小
    double m_dfactor;                    //增长因子
    std::mutex m_slabLock;
    std::mutex m_poolLock;
public:
    MemporyPOOL();
    int Init(size_t poolSize);
    void* Alloc(size_t size);
    void Free(void* pBlock);
     ~MemporyPOOL();
private:
    int GetNewSlabClassId(size_t size);
    int GetSlabClassId(size_t size);
    void Destroy();
};
#endif // MemporyPOOL_H
