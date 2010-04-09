// x_file.cpp: implementation of the x_file class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "File.h"


CLFile::~CLFile()
{
	if (m_hFile != NULL && m_hFile != INVALID_HANDLE_VALUE) 
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}
}
const char* CLFile::fname(unsigned int* slen)
{
	if(slen) *slen = m_bName.m_len;
	return m_bName.m_buf;
}
BOOL CLFile::attach(char* file, FILE_MODE mode)
{
	if (m_hFile != NULL && m_hFile != INVALID_HANDLE_VALUE) 
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}

	adpter_str_mem(m_bName, strlen(file), file);

	DWORD access, share, create;
	switch(mode) 
	{
	case READ_ONLY:
		access = GENERIC_READ;
		share  = FILE_SHARE_READ;
		create = OPEN_EXISTING;
		break;
	case WRITE_ONLY:
		access = GENERIC_WRITE;
		share  = FILE_SHARE_READ;
		create = OPEN_EXISTING;
		break;
	case READ_WIRTE:
		access = GENERIC_READ|GENERIC_WRITE;
		share  = FILE_SHARE_READ;
		create = OPEN_EXISTING;
		break;
	case CREATE:
		access = GENERIC_READ|GENERIC_WRITE;
		share  = NULL;
		create = CREATE_ALWAYS;
		break;
	default:
		return FALSE;
	}
	m_hFile = CreateFile(file, access, share, NULL, create, FILE_ATTRIBUTE_NORMAL, 0);
	if (m_hFile == INVALID_HANDLE_VALUE || m_hFile == NULL) 
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
BOOL CLFile::seek(int off, FILE_POS begin)
{
	if(m_hFile == INVALID_HANDLE_VALUE || m_hFile == NULL) return FALSE;
	DWORD from;
	switch(begin) 
	{
	case CURRENT:
		from = FILE_CURRENT;
		break;
	case BEGIN:
		from = FILE_BEGIN;
		break;
	case END:
		from = FILE_END;
		break;
	default:
		return FALSE;
	}
	return SetFilePointer(m_hFile, off, 0, from);
}
BOOL CLFile::read(void * buf, size_t sz)
{
	if(m_hFile == INVALID_HANDLE_VALUE || m_hFile == NULL) return FALSE;
	DWORD dwRead = 0;
	ReadFile(m_hFile, buf, sz, &dwRead, NULL);
	return sz == dwRead;
}
BOOL CLFile::write(void * buf, size_t sz)
{
	if(m_hFile == INVALID_HANDLE_VALUE || m_hFile == NULL) return FALSE;
	DWORD dwWrite = 0;
	WriteFile(m_hFile, buf, sz, &dwWrite, NULL);
	return sz == dwWrite;
}
BOOL CLFile::fsize(size_t& size)
{
	if(m_hFile == INVALID_HANDLE_VALUE || m_hFile == NULL) return FALSE;
	size = GetFileSize(m_hFile, NULL);
	return TRUE;
}
BOOL CLFile::ftell(size_t& fp)
{
	if(m_hFile == INVALID_HANDLE_VALUE || m_hFile == NULL) return FALSE;
	fp = SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);
	return TRUE;
}