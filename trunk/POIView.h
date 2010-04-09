// POIView.h : main header file for the POIVIEW application
//

#if !defined(AFX_POIVIEW_H__408B0CE5_C534_49C4_803D_CC4F36BA6473__INCLUDED_)
#define AFX_POIVIEW_H__408B0CE5_C534_49C4_803D_CC4F36BA6473__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPOIViewApp:
// See POIView.cpp for the implementation of this class
//

class CPOIViewApp : public CWinApp
{
public:
	CPOIViewApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPOIViewApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPOIViewApp)
	afx_msg void OnOpenfile();
	afx_msg void OnCurrentarea();
	afx_msg void OnCurrentassort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POIVIEW_H__408B0CE5_C534_49C4_803D_CC4F36BA6473__INCLUDED_)
