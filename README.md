# MEMORY_POOL

MEMORY_POOL是一个基于c++的内存池，该内存池是根据memcache中的内存池改变而来，并且删除了一些不必要的概念。同时改程序是线程安全的。其中mutex线程锁依赖于c++11。其中为了方便引用，将整套代码写为.h，这个并不合理。建议在实际使用中将其分离为``.c+.h`` 的格式。

## MEM_POOL类

是内存池类，负责维护整个内存池。并对外提供接口。

## SlabClass类

内存池由多个SlabClass组成，每个SlabClass里面有多个chunk（item），每个chunk大小相等，每个chunk是实际的存储单元。
