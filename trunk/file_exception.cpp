// file_exception.cpp: implementation of the file_exception class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "poiview.h"
#include "file_exception.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

File_Exception::File_Exception(const char* file)
{
	int str_len = strlen(file);
	
	m_file_name.alloc(str_len + 1);
	
	memcpy(m_file_name.m_buf, file, str_len);
	
	m_file_name.m_buf[str_len] = '\0';
}

File_Exception::~File_Exception()
{

}
