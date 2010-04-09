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
    struct memory_page*  next;  /* ��һ���ڴ�ҳ */
    size_t               count; /* δ������ڴ���� */
    memblk_t*            free;  /* δ������ڴ������ */
} mempage_t;

typedef struct memory_list
{
	size_t      page_size;                  /* �ڴ�ҳ�ڿɹ�������ֽ��� */
	size_t      useable;                    /* δ���Ŀɹ�������ڴ�ҳ�� */
	mempage_t*  buffer[MEMORY_POOL_BUFFER]; /* ҳ�滺�� */
	mempage_t*  first;                      /* ��һ���ڴ�ҳ */
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

            /* �Ȳ���ҳ�滺�� */
            for( i = 0; i < MEMORY_POOL_BUFFER; ++i )
            {
                if( pool[index].buffer[i] )
                {
                    ptr = pool[index].buffer[i]->free;
                    pool[index].buffer[i]->free = pool[index].buffer[i]->free->next;
                    --( pool[index].buffer[i]->count );

                    /* �����ҳ���޿��п飬�򽫸�ҳ��ҳ�滺�����˳� */
                    if( pool[index].buffer[i]->count == 0 )
                    {
                        --( pool[index].useable );
                        pool[index].buffer[i] = NULL;
                    }
                    else
                    {
                        if( i > 0 )
                        {
                            /* �����ҳ���ڻ����ף��򽫸�ҳ������������ */
                            pool[index].buffer[0] = pool[index].buffer[i];
                            pool[index].buffer[i] = NULL;
                        }
                    }

                    goto EXIT_POOL_ALLOC;
                }
            } /* end for */

            /* ҳ�滺��Ϊ�գ���������е������ڴ�ҳѰ�ҿ��е��ڴ�� */
            curr = pool[index].first;
            while( curr )
            {
                if( curr->count == 0 ) /* ��ҳ��û�п��п� */
                {
                    /* ������һҳ */
                    prev = curr;
                    curr = curr->next;
                }
                else /* ��ҳ���п��п� */
                {
                    size_t page_count = 0; /* ͳ�Ʊ������Ŀ����ڴ�ҳ */
                    ptr = curr->free;
                    curr->free = curr->free->next;
                    --( curr->count );
                    if( curr->count == 0 )
                        --( pool[index].useable );

                    /* ������������Ѱ���������п��п��ҳ�棬��֮����ҳ�滺�� */
                    while( curr && page_count < pool[index].useable )
                    {
                        if( curr->count != 0 )
                        {
                            /* ҳ�滺�滹��λ�������ҳ�滺�� */
                            if( page_count < MEMORY_POOL_BUFFER )
                                pool[index].buffer[page_count] = curr;

                            ++page_count;

                            /* �����ǰҳδ�����Ҳ������ף���֮�������� */
                            if( pool[index].first != curr )
                            {
                                prev->next = curr->next; /* ������һҳ */
                                curr->next = pool[index].first;
                                pool[index].first = curr;
                                curr = prev->next; /* ������һҳ */
                                continue;
                            }
                        }

                        /* ������һҳ */
                        prev = curr;
                        curr = curr->next;
                    }

                    goto EXIT_POOL_ALLOC;
                } /* end else */
            } /* end while */
        } /* end if pool[index].useable > 0 */
        else
        {
            /* ������δ�����ڴ�ҳ���޿��п飬��ʱ�������µ��ڴ�ҳ */
            mempage_t* pg = NULL;
            size_t blk_size = BLOCK_SIZE( index );

            /* ��� page_size = 0���������ڴ�����ÿ���ڴ�ҳ��ռ�õ��ֽ��� */
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

                 /* ���ڴ�ҳ�е������ڴ�鴮����һ������ */
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
            if( blk < begin || blk >= end ) /* �ж�ptr�Ƿ��ڵ�ǰҳ�� */
            {
                prev = curr;
                curr = curr->next;
            }
            else
            {
                size_t blk_size = BLOCK_SIZE( index );

                /* ���ptr�Ƿ���ȷ */
                if( (blk - begin) % blk_size == 0 )
                {
                    /* ���ڴ������������ײ� */
                    memblk_t* pblk = (memblk_t*)ptr;
                    pblk->next = curr->free;
                    curr->free = pblk;

                    /* �������ǰ�ڴ�ҳ�������򽫿���ҳ����һ */
                    if( curr->count == 0 )
                        ++( pool[index].useable );
                    ++( curr->count );

                    /* �����ǰҳ�������ף���֮�������� */
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

    /* ptr�������ڴ�ط��� */
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
            if( curr->count != blk_count ) /* �ж��Ƿ��ǿ���ҳ */
            {
                prev = curr;
                curr = curr->next;
            }
            else
            {
                /* ��ҳ�滺�����˳� */
                for( i = 0; i < MEMORY_POOL_BUFFER; ++i )
                {
                    if( pool[index].buffer[i] == curr )
                    {
                        pool[index].buffer[i] = NULL;
                        break;
                    }
                }

                if( prev )
                    prev->next = curr->next; /* ����ҳ�������� */
                else
                    pool[index].first = curr->next; /* ����ҳ������ */

                /* ������ҳ�ͷ� */
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