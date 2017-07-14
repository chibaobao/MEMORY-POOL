# MEMORY_POOL

MEMORY_POOL是一个基于c++的内存池，该内存池是根据memcache中的内存池改变而来，并且删除了一些不必要的概念。同时改程序是线程安全的。其中mutex线程锁依赖于c++11。
建议编译命令：``g++  -g  -std=c++11 -pthread SlabClass.cpp MemporyPOOL.cpp main.cpp``

## MemporyPOOL

是内存池类，负责维护整个内存池。并对外提供接口。

## SlabClass类

内存池由多个SlabClass组成，每个SlabClass里面有多个chunk（item），每个chunk大小相等，每个chunk是实际的存储单元。

## main.cpp

是一个测试程序,也是一个示例，是多线程下对SlabClass的调用
