// POIViewDlg.h : header file
//

#if !defined(AFX_POIVIEWDLG_H__C5282BDD_A2C6_4035_9FB5_61B54C5C24F7__INCLUDED_)
#define AFX_POIVIEWDLG_H__C5282BDD_A2C6_4035_9FB5_61B54C5C24F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "dump\xstring.h"
//#include "dump\filelogger.h"

#include "poiinfo.h"
#include "TreeDlg.h"
#include "SetArea.h"
#include "SetAssort.h"
/////////////////////////////////////////////////////////////////////////////
// CPOIViewDlg dialog

class CPOIViewDlg : public CDialog
{
// Construction
public:
	CPOIViewDlg(CWnd* pParent = NULL);	// standard constructor
	CString m_csFilePath;
	CPOIInfo m_poi;
	
// Dialog Data
	//{{AFX_DATA(CPOIViewDlg)
	enum { IDD = IDD_POIVIEW_DIALOG };
	CListCtrl	m_lsPoiList;
	CComboBox	m_cbAssort3;
	CComboBox	m_cbAssort2;
	CComboBox	m_cbAssort1;
	CComboBox	m_cbCity;
	CComboBox	m_cbCountry;
	CComboBox	m_cbProvince;
	BOOL	m_bPoi;
	BOOL	m_bSight;
	BOOL	m_bStreet;
	BOOL	m_bAcross;
	CString	m_csStatus;
	int		m_iModeIndex;
	CString	m_csSearchStr;
	CString	m_csCurArea;
	CString	m_csCurAssor;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPOIViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	CTreeDlg m_TreeDlg;
	CSetArea m_AreaSelDlg;
	CSetAssort m_AssortSelDlg;
// Implementation
protected:
	void UpdateCurAreaInfo(void);
//	void UpdateAdmin(int pro, int city, int country = -1);
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPOIViewDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnOpenfile();
	afx_msg void OnAdcode();
	afx_msg void OnAssort();
	afx_msg void OnSel();
	afx_msg void OnSelchangeCbcity();
	afx_msg void OnEditchangeCbprovince();
	afx_msg void OnSelchangeCbprovince();
	afx_msg void OnOk();
	afx_msg void OnCurrentarea();
	afx_msg void OnCurrentassort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POIVIEWDLG_H__C5282BDD_A2C6_4035_9FB5_61B54C5C24F7__INCLUDED_)





















