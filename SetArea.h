#if !defined(AFX_SETAREA_H__F9494081_AAF7_4158_9D4C_BCF278B9FC93__INCLUDED_)
#define AFX_SETAREA_H__F9494081_AAF7_4158_9D4C_BCF278B9FC93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetArea.h : header file
//
#include "area_tree.h"
/////////////////////////////////////////////////////////////////////////////
// CSetArea dialog

class CSetArea : public CDialog
{
// Construction
public:
	CSetArea(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetArea)
	enum { IDD = IDD_SETAREA };
	CComboBox	m_cbCity;
	CComboBox	m_cbCountry;
	CComboBox	m_cbProvince;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetArea)
	public:
		virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	void rebulid(const pod_vector<Lemon<zone>*>* province, const pod_vector<Lemon<zone>*>* city,
					   const pod_vector<Lemon<zone>*>* country);

	void UpdateAdmin(int pro, int city_code, int country_c);
	void get_admin(int& pro, int& city, int& county);
// Implementation
protected:
	pod_vector<Lemon<zone>*>* m_p_province;
	pod_vector<Lemon<zone>*>* m_p_city;
	pod_vector<Lemon<zone>*>* m_p_country;
	int m_iCityBegin; //m_cbCity控件中的第一个在所有市列表中的位置
	int m_iCountryBegin;
	int m_ipro;
	int m_icity;
	int m_icountry;
	// Generated message map functions
	//{{AFX_MSG(CSetArea)
	virtual void OnOK();
	afx_msg void OnSelchangeCbprovince();
	afx_msg void OnSelchangeCbcountry();
	afx_msg void OnSelchangeCbcity();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETAREA_H__F9494081_AAF7_4158_9D4C_BCF278B9FC93__INCLUDED_)
