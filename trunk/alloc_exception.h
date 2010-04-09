// alloc_exception.h: interface for the alloc_exception class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALLOC_EXCEPTION_H__DD71B0BD_941A_4EC7_90B6_ADCA2AA8B9E6__INCLUDED_)
#define AFX_ALLOC_EXCEPTION_H__DD71B0BD_941A_4EC7_90B6_ADCA2AA8B9E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "base_exception.h"

class Alloc_Exception : public Base_Exception  
{
public:
	Alloc_Exception(unsigned int mem_sz);
	virtual ~Alloc_Exception();
private:
	unsigned int m_mem_sz;
};

#endif // !defined(AFX_ALLOC_EXCEPTION_H__DD71B0BD_941A_4EC7_90B6_ADCA2AA8B9E6__INCLUDED_)
