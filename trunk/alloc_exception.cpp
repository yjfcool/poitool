// alloc_exception.cpp: implementation of the alloc_exception class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "poiview.h"
#include "alloc_exception.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Alloc_Exception::Alloc_Exception(unsigned int mem_sz):m_mem_sz(mem_sz)
{

}

Alloc_Exception::~Alloc_Exception()
{

}
