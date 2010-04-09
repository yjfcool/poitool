#include "FileLoger.h"
#include "stdio.h"
#include "memory.h"
#include "string.h"
#include <stdarg.h>

#define FILE_NAME_LEN 100

CFileLoger::CFileLoger(const char* file):m_pFile(0)
{
	int len = strlen(file);
	
	if (len > FILE_NAME_LEN)
		return;

	m_pFile = new char[len + 1];
	if (m_pFile) 
	{
		memcpy(m_pFile, file, len);
		m_pFile[len] = '\0';
	}
}

CFileLoger::~CFileLoger()
{
	if (m_pFile) {
		delete m_pFile;
		m_pFile = 0;
	}
}
#define BUFFERSIZE		256

void CFileLoger::DumpVar(char* lpszFormat, ...)
{
	if (m_pFile == 0) 
		return;

	char    lpszBuffer[BUFFERSIZE];
	va_list  fmtList;
	memset(lpszBuffer, 0x0, sizeof(lpszBuffer));
	
	va_start( fmtList, lpszFormat );
	vsprintf( lpszBuffer, lpszFormat, fmtList );
	va_end( fmtList );
	
	FILE* fp = fopen(m_pFile, "at");
	if (!fp)
		return;

	fwrite(lpszBuffer, strlen(lpszBuffer), 1, fp);

	fclose(fp);
}

void CFileLoger::LogStr(char* lpszText)
{
	if (lpszText == 0 || m_pFile == 0) {
		return;
	}
	FILE* fp = fopen(m_pFile, "at");
	if (!fp)
		return;

	fwrite(lpszText, strlen(lpszText), 1, fp);

	fclose(fp);
}
