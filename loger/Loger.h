// Logger.h: interface for the CLogger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGGER_H__9A6DB843_C738_4C33_A1F4_A0DD12FBF556__INCLUDED_)
#define AFX_LOGGER_H__9A6DB843_C738_4C33_A1F4_A0DD12FBF556__INCLUDED_




class CLoger  
{
public:
	CLoger();
	virtual ~CLoger();
	virtual void DumpVar(char* lpszText, ...) = 0;
	virtual void LogStr(char* lpszText) = 0;
};

#endif // !defined(AFX_LOGGER_H__9A6DB843_C738_4C33_A1F4_A0DD12FBF556__INCLUDED_)
