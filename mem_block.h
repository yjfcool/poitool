#ifndef MEM_BLOCK_H_
#define MEM_BLOCK_H_
#include "xmemory.h"
#include "wtypes.h"

template<class T>
class mem_block
{
public:
	mem_block():m_buf(NULL),m_len(0)
	{}
	~mem_block()
	{
		free();
	}
	static unsigned char ele_sz(void){return sizeof(T);}
	BOOL alloc(unsigned int l)
	{
		free();
		
		if ((m_buf = pod_allocator<T>::allocate(l)) != NULL) 
		{
			m_len = l * sizeof(T);
			return TRUE;
		}
		return FALSE;
	}
	mem_block& operator = (mem_block<T>& other)
	{
		free();
		
		if ((m_buf = pod_allocator<T>::allocate(other.m_len / sizeof(T))) != NULL) 
		{
			memmove(m_buf, other.m_buf, other.m_len);
			m_len = other.m_len;
		}
		return *this;
	}
	void free(void)
	{
		if(m_buf) 
		{
			pod_allocator<T>::deallocate(m_buf, m_len / sizeof(T));
			m_buf = NULL;
		}
		m_len = 0;
	}
	T*			 m_buf;
	unsigned int m_len;
};

typedef mem_block<unsigned char> b_uchar_mem;
typedef mem_block<char>			 b_char_mem;
typedef mem_block<unsigned>		 unsigned_mem;
typedef mem_block<int>			 int_mem;

BOOL adpter_str_mem(b_char_mem& char_m, int len,  char*str);
BOOL str_mem_add(b_char_mem& char_m1, b_char_mem& char_mem2, b_char_mem& char_m3);
b_char_mem& operator += (b_char_mem& char_m1, b_char_mem& char_mem2);
#endif
