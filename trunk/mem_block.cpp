#include "mem_block.h"

BOOL adpter_str_mem(b_char_mem& char_m, int len, char* str)
{
	char_m.free();
	if (char_m.alloc(len + 1)) 
	{
		memmove(char_m.m_buf, str, len);
		char_m.m_buf[len] = '\0';
		return TRUE;
	}
	return FALSE;
}

BOOL str_mem_add(b_char_mem& char_m1, b_char_mem& char_mem2, b_char_mem& char_m3)
{
	char_m3.free();

	char_m3.alloc(char_m1.m_len + char_mem2.m_len - 1);

	memcpy(char_m3.m_buf, char_m1.m_buf, char_m1.m_len - 1);

	memcpy(char_m3.m_buf + char_m1.m_len - 1, char_mem2.m_buf, char_mem2.m_len - 1);

	char_m3.m_buf[char_m1.m_len + char_mem2.m_len - 2] = 0;

	return TRUE;
}

b_char_mem& operator+= (b_char_mem& char_m1, b_char_mem& char_m2)
{
	b_char_mem char_m3 = char_m1;

	str_mem_add(char_m3, char_m2, char_m1);

	return char_m1;
}