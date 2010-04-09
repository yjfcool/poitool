// POIView.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "POIView.h"
#include "POIViewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPOIViewApp

BEGIN_MESSAGE_MAP(CPOIViewApp, CWinApp)
	//{{AFX_MSG_MAP(CPOIViewApp)
	ON_COMMAND(ID_OPENFILE, OnOpenfile)
	ON_COMMAND(ID_CURRENTAREA, OnCurrentarea)
	ON_COMMAND(ID_CURRENTASSORT, OnCurrentassort)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPOIViewApp construction

CPOIViewApp::CPOIViewApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPOIViewApp object

CPOIViewApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPOIViewApp initialization

BOOL CPOIViewApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CPOIViewDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CPOIViewApp::OnOpenfile() 
{
	// TODO: Add your command handler code here
}

void CPOIViewApp::OnCurrentarea() 
{
	// TODO: Add your command handler code here
	
}

void CPOIViewApp::OnCurrentassort() 
{
	// TODO: Add your command handler code here
	
}
