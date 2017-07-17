#include <stdio.h>
#include"MemporyPOOL.h"
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include <thread>
#include <vector>
MemporyPOOL* pool;

void *thread_fun()
{
    fprintf(stderr, "pid  %lu start =======\n",(unsigned long)pthread_self());
    void* ptr = NULL;
    for(int i =0 ;i<3;i++)
    {
        int size =   (rand() % ((64*1024*1024)+1)) ;
        sleep(rand()%3);
        ptr = pool->Alloc(size);
        if(!ptr)
        {
             fprintf(stderr, "pid ptr NULL %lu sleepping =======\n",(unsigned long)pthread_self());
        }
        else
        {
            fprintf(stderr, "pid %lu ,ptr:%p =======\n",(unsigned long)pthread_self(),ptr);
        }
        sleep(rand()%3);
        pool->Free(ptr);
        fprintf(stderr,"\n");
    }
    return 0;
}

int main()
{
    int ret =0;
    std::vector<std::thread> vec;
     srand( (unsigned)time( NULL ) );
    pool = new MemporyPOOL;
    ret = pool->Init(512*1024*1024);
    int i = 0;

    for(i=0;i<200;i++)
    {
        vec.push_back(std::thread(thread_fun));
    }
    for(i=0;i<200;i++)
    {
        vec[i].join();
    }
    return 0;
}
