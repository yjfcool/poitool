// poi_exception.h: interface for the poi_exception class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POI_EXCEPTION_H__43A7923C_D77A_4CE5_B7C1_8CE6668062B9__INCLUDED_)
#define AFX_POI_EXCEPTION_H__43A7923C_D77A_4CE5_B7C1_8CE6668062B9__INCLUDED_
#include "base_exception.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class file_exception :public base_exception 
{
public:
	file_exception();
	virtual ~file_exception();

};

#endif // !defined(AFX_POI_EXCEPTION_H__43A7923C_D77A_4CE5_B7C1_8CE6668062B9__INCLUDED_)
