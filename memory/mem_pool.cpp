// mem_pool.cpp: implementation of the mem_pool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mem_pool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
enum LIBRARY_MEMORY_CONSTANT
{
    DEFAULT_PAGE_SIZE = 1024 * sizeof(void*),
		MEMORY_POOL_BUFFER = 8,
		MEMORY_POOL_ALIGN = sizeof(void*),
		MEMORY_POOL_MIN = sizeof(void*),
		MEMORY_POOL_MAX = MEMORY_POOL_MIN * 64,
		MEMORY_POOL_LISTS = ( MEMORY_POOL_MAX - MEMORY_POOL_MIN )
		/ MEMORY_POOL_ALIGN + 1
};

typedef  struct memory_block
{
	struct memory_block*  next;
} memblk_t;

typedef struct memory_page
{
    struct memory_page*  next;  /* 下一个内存页 */
    size_t               count; /* 未分配的内存块数 */
    memblk_t*            free;  /* 未分配的内存块链表 */
} mempage_t;

typedef struct memory_list
{
	size_t      page_size;                  /* 内存页内可供分配的字节数 */
	size_t      useable;                    /* 未满的可供分配的内存页数 */
	mempage_t*  buffer[MEMORY_POOL_BUFFER]; /* 页面缓存 */
	mempage_t*  first;                      /* 第一个内存页 */
} memlist_t;

#define  MEMALLOC( bytes )  malloc( bytes )

#define  MEMFREE( ptr )  free( ptr )

#define  LIST_INDEX( bytes ) \
	( (bytes) <= MEMORY_POOL_MIN ? 0 \
	: ((bytes) - MEMORY_POOL_MIN + MEMORY_POOL_ALIGN - 1) \
/ MEMORY_POOL_ALIGN )

#define  BLOCK_SIZE( index )  ( MEMORY_POOL_MIN + (index) * MEMORY_POOL_ALIGN )

#define  BLOCK_COUNT( index )  ( pool[index].page_size / BLOCK_SIZE(index) )

/******************************************************************************/
/******************************************************************************/

static size_t pool_alloc_count = 0;
static size_t pool_dealloc_count = 0;
static void (*pool_lock)( size_t ) = NULL;
static void (*pool_unlock)( size_t ) = NULL;
static memlist_t pool[MEMORY_POOL_LISTS] = { {0, 0, {NULL}, NULL} };

mem_pool::mem_pool()
{

}

mem_pool::~mem_pool()
{

}
size_t mem_pool::get_pool_lists_count( void )
{
    return MEMORY_POOL_LISTS;
}

size_t mem_pool::get_pool_alloc_count( void )
{
    return pool_alloc_count;
}

size_t mem_pool::get_pool_dealloc_count( void )
{
    return pool_dealloc_count;
}

void mem_pool::set_pool_lock( void (*lock)(size_t) )
{
    pool_lock = lock;
}

void mem_pool::set_pool_unlock( void (*unlock)(size_t) )
{
    pool_unlock = unlock;
}

/******************************************************************************/

void mem_pool::pool_print( CLogger* log )
{
    int i, j;
    mempage_t* curr;

    for( i = 0; i < MEMORY_POOL_LISTS; ++i )
    {
        if( !(pool[i].first) )
            continue;

        if( pool_lock )
            pool_lock( i );

        log->DumpVar( "\npool[%d] chunk = %d", i, BLOCK_SIZE(i) );
        log->DumpVar( "\n\tpool[%d].page_size = %lu", i,
                (unsigned long)pool[i].page_size );
        log->DumpVar( "\n\tpool[%d].useable = %lu", i,
                (unsigned long)pool[i].useable );
        for( j = 0; j < MEMORY_POOL_BUFFER; ++j )
            log->DumpVar( "\n\tpool[%d].buffer[%d] = %p", i, j, pool[i].buffer[j] );
        log->DumpVar( "\n\tpool[%d].first = %p: ", i, pool[i].first );

        curr = pool[i].first;

        while( curr )
        {
            log->DumpVar( "\n\tnext = %p | count = %lu | free = %p",
                    curr->next, (unsigned long)curr->count, curr->free );
            curr = curr->next;
        }

        if( pool_unlock )
            pool_unlock( i );
    }
}

/******************************************************************************/

void* mem_pool::pool_alloc( size_t bytes )
{
    void* ptr = NULL;

    if( bytes > MEMORY_POOL_MAX )
    {
        ptr = MEMALLOC( bytes );
    }
    else
    {
        size_t index = LIST_INDEX( bytes );

        if( pool_lock )
            pool_lock( index );

        if( pool[index].first && pool[index].useable > 0 )
        {
            int i;
            mempage_t *prev = NULL, *curr = NULL;

            /* 先查找页面缓存 */
            for( i = 0; i < MEMORY_POOL_BUFFER; ++i )
            {
                if( pool[index].buffer[i] )
                {
                    ptr = pool[index].buffer[i]->free;
                    pool[index].buffer[i]->free = pool[index].buffer[i]->free->next;
                    --( pool[index].buffer[i]->count );

                    /* 如果该页已无空闲块，则将该页自页面缓存中退出 */
                    if( pool[index].buffer[i]->count == 0 )
                    {
                        --( pool[index].useable );
                        pool[index].buffer[i] = NULL;
                    }
                    else
                    {
                        if( i > 0 )
                        {
                            /* 如果该页不在缓存首，则将该页调整至缓存首 */
                            pool[index].buffer[0] = pool[index].buffer[i];
                            pool[index].buffer[i] = NULL;
                        }
                    }

                    goto EXIT_POOL_ALLOC;
                }
            } /* end for */

            /* 页面缓存为空，则遍历链中的所有内存页寻找空闲的内存块 */
            curr = pool[index].first;
            while( curr )
            {
                if( curr->count == 0 ) /* 该页中没有空闲块 */
                {
                    /* 进入下一页 */
                    prev = curr;
                    curr = curr->next;
                }
                else /* 该页中有空闲块 */
                {
                    size_t page_count = 0; /* 统计遍历过的可用内存页 */
                    ptr = curr->free;
                    curr->free = curr->free->next;
                    --( curr->count );
                    if( curr->count == 0 )
                        --( pool[index].useable );

                    /* 继续遍历链表，寻找其他还有空闲块的页面，将之放入页面缓存 */
                    while( curr && page_count < pool[index].useable )
                    {
                        if( curr->count != 0 )
                        {
                            /* 页面缓存还有位置则放入页面缓存 */
                            if( page_count < MEMORY_POOL_BUFFER )
                                pool[index].buffer[page_count] = curr;

                            ++page_count;

                            /* 如果当前页未满并且不在链首，则将之移至链首 */
                            if( pool[index].first != curr )
                            {
                                prev->next = curr->next; /* 保存下一页 */
                                curr->next = pool[index].first;
                                pool[index].first = curr;
                                curr = prev->next; /* 进入下一页 */
                                continue;
                            }
                        }

                        /* 进入下一页 */
                        prev = curr;
                        curr = curr->next;
                    }

                    goto EXIT_POOL_ALLOC;
                } /* end else */
            } /* end while */
        } /* end if pool[index].useable > 0 */
        else
        {
            /* 该链下未分配内存页或无空闲块，此时需增加新的内存页 */
            mempage_t* pg = NULL;
            size_t blk_size = BLOCK_SIZE( index );

            /* 如果 page_size = 0，则计算该内存链下每个内存页需占用的字节数 */
            if( 0 == pool[index].page_size )
            {
                if( DEFAULT_PAGE_SIZE % blk_size == 0 )
                    pool[index].page_size = DEFAULT_PAGE_SIZE;
                else
                    pool[index].page_size = (DEFAULT_PAGE_SIZE / blk_size)
                                            * blk_size;
            }

            pg = (mempage_t*)MEMALLOC( sizeof(mempage_t)
                                       + pool[index].page_size );

            if( pg )
            {
                memblk_t* curr = NULL;
                size_t i, blk_count = BLOCK_COUNT( index );

                pg->next = pool[index].first;
                pool[index].first = pg;

                 /* 将内存页中的所有内存块串联成一个链表 */
                curr = (memblk_t*)((unsigned char*)pg + sizeof(mempage_t));
                pg->free = curr;
                for( i = 1; i < blk_count; ++i )
                {
                    curr->next = (memblk_t*)((unsigned char*)curr + blk_size);
                    curr = curr->next;
                }
                curr->next = NULL;

                ptr = pg->free;
                pg->free = pg->free->next;
                pg->count = blk_count - 1;
                ++( pool[index].useable );
                pool[index].buffer[0] = pg;
            }
        }

EXIT_POOL_ALLOC:
        if( pool_unlock )
            pool_unlock( index );
    } /* end else */

    if( ptr )
        ++pool_alloc_count;

    return ptr;
}

/******************************************************************************/

void mem_pool::pool_dealloc( void* ptr, size_t bytes )
{
    if( !ptr )
        return;

    if( bytes <= MEMORY_POOL_MAX )
    {
        size_t index = LIST_INDEX( bytes );
        mempage_t *curr, *prev = NULL;
        unsigned char *begin, *end, *blk = (unsigned char*)ptr;

        if( pool_lock )
            pool_lock( index );

        curr = pool[index].first;

        while( curr )
        {
            begin = (unsigned char*)curr + sizeof(mempage_t);
            end = begin + pool[index].page_size;
            if( blk < begin || blk >= end ) /* 判断ptr是否在当前页内 */
            {
                prev = curr;
                curr = curr->next;
            }
            else
            {
                size_t blk_size = BLOCK_SIZE( index );

                /* 检查ptr是否正确 */
                if( (blk - begin) % blk_size == 0 )
                {
                    /* 将内存块回收至链表首部 */
                    memblk_t* pblk = (memblk_t*)ptr;
                    pblk->next = curr->free;
                    curr->free = pblk;

                    /* 如果回收前内存页已满，则将可用页数加一 */
                    if( curr->count == 0 )
                        ++( pool[index].useable );
                    ++( curr->count );

                    /* 如果当前页不在链首，则将之移至链首 */
                    if( pool[index].first != curr )
                    {
                        prev->next = curr->next;
                        curr->next = pool[index].first;
                        pool[index].first = curr;
                    }

                    ++pool_dealloc_count;
                }
                break;
            } /* end else */
        } /* end while */

        if( pool_unlock )
            pool_unlock( index );

        return;
    } /* end if */

    /* ptr不是由内存池分配 */
    MEMFREE( ptr );
    ++pool_dealloc_count;
}

/******************************************************************************/

void mem_pool::pool_free( void* ptr, size_t bytes )
{
    if( ptr )
        pool_dealloc( ptr, bytes );

    if( bytes <= MEMORY_POOL_MAX )
    {
        int i;
        size_t index = LIST_INDEX( bytes );
        size_t blk_count = BLOCK_COUNT( index );
        mempage_t *erase = NULL, *prev = NULL, *curr = pool[index].first;

        if( pool_lock )
            pool_lock( index );

        while( curr )
        {
            if( curr->count != blk_count ) /* 判断是否是空闲页 */
            {
                prev = curr;
                curr = curr->next;
            }
            else
            {
                /* 从页面缓存中退出 */
                for( i = 0; i < MEMORY_POOL_BUFFER; ++i )
                {
                    if( pool[index].buffer[i] == curr )
                    {
                        pool[index].buffer[i] = NULL;
                        break;
                    }
                }

                if( prev )
                    prev->next = curr->next; /* 空闲页不在链首 */
                else
                    pool[index].first = curr->next; /* 空闲页在链首 */

                /* 将空闲页释放 */
                erase = curr;
                curr = curr->next;
                MEMFREE( erase );
                --( pool[index].useable );
            } /* end if */
        } /* end while */

        if( pool_unlock )
            pool_unlock( index );
    }
}