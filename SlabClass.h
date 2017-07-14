#ifndef SLABCLASS_H
#define SLABCLASS_H

#include <malloc.h>
#include<stdlib.h>
#include<stdio.h>
#include <mutex>


struct Item {
    unsigned int id;
    struct  Item *next;
    struct  Item *prev;
};

class SlabClass{
private:
    size_t m_size;      /* 每个小块data大小(不包括id)*/
    size_t m_iTotalsize;      /* 每个小块大小(包括id)*/
    unsigned int m_iPerslab;   /* class里面1page有多少chunk*/
    unsigned int m_id;
    void *m_pSlots;           /* 空闲chunk链表 */
    unsigned int m_iSlots;   /* 链表中chunk个数 */
    void ** m_ppCurrent;      /*pool中的m_pCurrent*/
    size_t *m_pAvailSize;   /*pool剩余大小*/
    std::mutex *m_pLock;    /*改变m_pCurrent，m_AvailSize时用*/
    std::mutex m_MyLock;    /*改变m_pSlots等内部变量用*/

public:
    SlabClass(size_t size,int perslab,int id,
              void ** ppCurrent,size_t *pAvailSize,std::mutex *pLock); //每个小块size大小（不包括id）,perslab:class里面chunk个数
    int FreeItem(void* ptr);//ptr指向整个chunk(包括id)
    int InitSlabClass();
    void* GetItem();
    size_t GetSize();
private:
    int GetNewPage();
};

#endif // SLABCLASS_H
