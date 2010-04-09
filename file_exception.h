// file_exception.h: interface for the file_exception class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILE_EXCEPTION_H__9342F2A2_F566_4068_8F76_870A4C6B66A6__INCLUDED_)
#define AFX_FILE_EXCEPTION_H__9342F2A2_F566_4068_8F76_870A4C6B66A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "base_exception.h"
#include "mem_block.h"
class File_Exception : public Base_Exception  
{
public:
	File_Exception(const char* file);
	virtual ~File_Exception();
	b_char_mem& name(void)
	{
		return m_file_name;
	}
private:
	b_char_mem m_file_name;
};

#endif // !defined(AFX_FILE_EXCEPTION_H__9342F2A2_F566_4068_8F76_870A4C6B66A6__INCLUDED_)
