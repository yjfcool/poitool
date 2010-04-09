// FileLogger.h: interface for the CFileLogger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILELOGGER_H__131B4552_EA9E_4F44_B9D2_3CA2ED8DFB7D__INCLUDED_)
#define AFX_FILELOGGER_H__131B4552_EA9E_4F44_B9D2_3CA2ED8DFB7D__INCLUDED_


#include "Loger.h"


class CFileLoger : public CLoger  
{
public:
	CFileLoger(const char*);
	virtual ~CFileLoger();
	virtual void DumpVar(char* lpszFormat, ...);
	virtual void LogStr(char* lpszText);
private:
	char* m_pFile;
};

#endif // !defined(AFX_FILELOGGER_H__131B4552_EA9E_4F44_B9D2_3CA2ED8DFB7D__INCLUDED_)
