// SetArea.cpp : implementation file
//

#include "stdafx.h"
#include "poiview.h"
#include "SetArea.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetArea dialog


CSetArea::CSetArea(CWnd* pParent /*=NULL*/)
	: CDialog(CSetArea::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetArea)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSetArea::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetArea)
	DDX_Control(pDX, IDC_CBCITY, m_cbCity);
	DDX_Control(pDX, IDC_CBCOUNTRY, m_cbCountry);
	DDX_Control(pDX, IDC_CBPROVINCE, m_cbProvince);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetArea, CDialog)
	//{{AFX_MSG_MAP(CSetArea)
	ON_CBN_SELCHANGE(IDC_CBPROVINCE, OnSelchangeCbprovince)
	ON_CBN_SELCHANGE(IDC_CBCOUNTRY, OnSelchangeCbcountry)
	ON_CBN_SELCHANGE(IDC_CBCITY, OnSelchangeCbcity)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetArea message handlers

void CSetArea::OnOK() 
{
	// TODO: Add extra validation here
	m_ipro = m_cbProvince.GetCurSel();
	m_icity = m_cbCity.GetCurSel();
	m_icountry = m_cbCountry.GetCurSel();
	
	CDialog::OnOK();
}
void CSetArea::rebulid(const pod_vector<Lemon<zone>*>* province, const pod_vector<Lemon<zone>*>* city,
					   const pod_vector<Lemon<zone>*>* country)
{
	CString xx;
	unsigned i = 0;
	
	char str[50] = {0};

	m_p_province = (pod_vector<Lemon<zone>*>*)province;
	m_p_city = (pod_vector<Lemon<zone>*>*)city;
	m_p_country = (pod_vector<Lemon<zone>*>*)country;
	
	m_ipro = -1;
	m_icity = -1;
	m_icountry = -1;

	m_iCityBegin = 0;
	m_iCountryBegin = 0;
	
	m_cbProvince.ResetContent();
	
	for(i = 0; i < (*province).size(); i++)
	{
		b_char_mem str;
		(*m_p_province)[i]->m_Kernel.name(str);
		m_cbProvince.AddString((char*)str.m_buf);
	}
	m_cbProvince.SetCurSel(0);
	

	UpdateAdmin(0,0,0);
	
	UpdateData(FALSE);
}
//pro,city,country全部为相对索引
void CSetArea::UpdateAdmin(int pro, int city, int country)
{
	m_iCityBegin = 0;
	m_iCountryBegin = 0;
	int cur_country = country;
	int i = 0;
	b_char_mem str;
	m_cbCity.ResetContent();
	m_cbCountry.ResetContent();
	
	if (pro >= 0 && pro < (*m_p_province).size()) 
	{
		Lemon<zone>* p = (*m_p_province)[pro];
		
		m_cbProvince.SetCurSel(pro);
		
		for(i = 0; i < p->child_num(); i++)
		{
			p->child[i].m_Kernel.name(str);
			m_cbCity.AddString((char*)str.m_buf);
		}
		
		for(i = 0; i < pro; i++)
			m_iCityBegin += (*m_p_province)[i]->child_num();
		
		if (city >= 0 && city < p->child_num()) {
			m_cbCity.SetCurSel(city);
			
			if (cur_country >= 0) {
				p = (*m_p_city)[city+m_iCityBegin];
				
				for(i = 0; i < p->child_num(); i++)
				{
					p->child[i].m_Kernel.name(str);
					
					m_cbCountry.AddString((char*)str.m_buf);
				}
				
				for(i = 0; i < city+m_iCityBegin; i++)
					m_iCountryBegin += (*m_p_city)[i]->child_num();
				
				if (country >= 0 && country < p->child_num())
					m_cbCountry.SetCurSel(country);
			}
		}
	}
	UpdateData(FALSE);
}
void CSetArea::get_admin(int& pro, int& city, int& county)
{
	if (m_ipro == -1 || m_icity == -1) 
	{
		m_ipro = 0;
		m_icity = 0;

	}
	pro = m_ipro;
	city = m_icity + m_iCityBegin;
	county = m_icountry + m_iCountryBegin;
}
void CSetArea::OnSelchangeCbprovince() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	int cur_pro = m_cbProvince.GetCurSel();
	UpdateAdmin(cur_pro, 0, 0);
}

void CSetArea::OnSelchangeCbcountry() 
{
	// TODO: Add your control notification handler code here
	
}

void CSetArea::OnSelchangeCbcity() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	int cur_pro = m_cbProvince.GetCurSel();
	int cur_city = m_cbCity.GetCurSel();
	UpdateAdmin(cur_pro, cur_city, 0);
}

int CSetArea::DoModal() 
{
	return CDialog::DoModal();
}

int CSetArea::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_ipro = -1;
	m_icity = -1;
	m_icountry = -1;
	
	m_iCityBegin = 0;
	m_iCountryBegin = 0;
	
	return 0;
}
