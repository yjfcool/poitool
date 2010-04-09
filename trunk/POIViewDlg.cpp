// POIViewDlg.cpp : implementation file
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
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPOIViewDlg dialog

CPOIViewDlg::CPOIViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPOIViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPOIViewDlg)
	m_bPoi = FALSE;
	m_bSight = FALSE;
	m_bStreet = FALSE;
	m_bAcross = FALSE;
	m_csStatus = _T("");
	m_iModeIndex = -1;
	m_csSearchStr = _T("");
	m_csCurArea = _T("");
	m_csCurAssor = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPOIViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPOIViewDlg)
	DDX_Control(pDX, IDC_POILIST, m_lsPoiList);
	DDX_Text(pDX, IDC_STATUS, m_csStatus);
	DDX_CBIndex(pDX, IDC_COMBOMODE, m_iModeIndex);
	DDX_Text(pDX, IDC_EDITSTR, m_csSearchStr);
	DDX_Text(pDX, IDC_CURAREA, m_csCurArea);
	DDX_Text(pDX, IDC_CURASSOR, m_csCurAssor);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPOIViewDlg, CDialog)
	//{{AFX_MSG_MAP(CPOIViewDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_OPENFILE, OnOpenfile)
	ON_COMMAND(ID_ADCODE, OnAdcode)
	ON_COMMAND(ID_ASSORT, OnAssort)
	ON_COMMAND(ID_SEL, OnSel)
	ON_CBN_SELCHANGE(IDC_CBCITY, OnSelchangeCbcity)
	ON_CBN_EDITCHANGE(IDC_CBPROVINCE, OnEditchangeCbprovince)
	ON_CBN_SELCHANGE(IDC_CBPROVINCE, OnSelchangeCbprovince)
	ON_BN_CLICKED(IDC_OK, OnOk)
	ON_COMMAND(ID_CURRENTAREA, OnCurrentarea)
	ON_COMMAND(ID_CURRENTASSORT, OnCurrentassort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPOIViewDlg message handlers

BOOL CPOIViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	m_iModeIndex = 0;
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	// TODO: Add extra initialization here
	m_TreeDlg.Create(CTreeDlg::IDD,this); 
	m_AreaSelDlg.Create(IDD_SETAREA, this);
	m_csStatus = "poi文件没有被打开!";
	//----------------------------------------------------------------------初始化列表控件
	m_lsPoiList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lsPoiList.SetBkColor(RGB(247,247,255));
	m_lsPoiList.SetTextColor(RGB(0,0,255));
	m_lsPoiList.SetTextBkColor(RGB(247,247,255));
	if (m_lsPoiList.GetItemCount() <= 0) 
	{
		int tt = m_lsPoiList.InsertColumn(0, "序号", LVCFMT_LEFT, 40);
		tt = m_lsPoiList.InsertColumn(1, "查询结果", LVCFMT_LEFT, 467);
	}		
	//----------------------------------------------------------------------初始化列表控件完毕
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPOIViewDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPOIViewDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPOIViewDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
void CPOIViewDlg::OnOK() 
{
	// TODO: Add extra validation here
//	auto_ptr<int> my_ptr(new int[100]);
	CDialog::OnOK();
}

void CPOIViewDlg::OnOpenfile() 
{
	// TODO: Add your command handler code here
	CString csFilter = "Resource File(*.dat)|*.dat||";
	CString	strFilePath = "";
	CFileDialog fDlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		(LPCTSTR)csFilter, NULL);
	if (fDlg.DoModal() == IDOK) {
		strFilePath = fDlg.GetPathName();
		try
		{
			if (!m_poi.Open(strFilePath.GetBuffer(50))) 
			{
				return;
			}
		}
		catch (File_Exception& fe) 
		{
			b_char_mem& name = fe.name();
			m_csStatus.Format("%s打开失败!", name.m_buf);
			AfxMessageBox(m_csStatus);
			return;
		}
		m_csStatus.Format("%s打开成功!",strFilePath);
		UpdateCurAreaInfo();
		UpdateData(FALSE);
	}
}

void CPOIViewDlg::OnAdcode() 
{
	// TODO: Add your command handler code here
	m_TreeDlg.ClearTree();
	m_TreeDlg.InsertAdmin(m_poi.admin.root(), NULL);
	UpdateData(FALSE);
	m_TreeDlg.ShowWindow(SW_SHOW);
}

void CPOIViewDlg::OnAssort() 
{
	// TODO: Add your command handler code here
	m_TreeDlg.ClearTree();
	m_TreeDlg.InsertAssort(m_poi.ass.root(), NULL);
	UpdateData(FALSE);
	m_TreeDlg.ShowWindow(SW_SHOW);
}

void CPOIViewDlg::OnSel() 
{
	// TODO: Add your command handler code here
}

void CPOIViewDlg::OnSelchangeCbcity() 
{
	// TODO: Add your control notification handler code here
}

void CPOIViewDlg::OnEditchangeCbprovince() 
{
	// TODO: Add your control notification handler code here
}

void CPOIViewDlg::OnSelchangeCbprovince() 
{
	// TODO: Add your control notification handler code here
}
/*
POI名称检索					0			OK
POI拼音检索					1			OK
景点名称检索				2			OK
景点拼音检索				3			OK
周边检索					4
道路名称检索				5			OK
道路拼音检索				6			OK
查看某地区POI某具体分类		7

剩余工作量:
周边检索
查看某地区POI具体分类
显示当前地区
根据分类号获得具体信息
*/
void CPOIViewDlg::OnOk() 
{

	// TODO: Add your control notification handler code here
	UpdateData();
	int search_mode = -1;
	int i = 0;
	search_mode = m_iModeIndex;
	int cur_city = -1;
	int cur_pro = -1;
	int cur_country = -1;
	m_AreaSelDlg.get_admin(cur_pro, cur_city, cur_country);
	char* pStr = m_csSearchStr.GetBuffer(50);
	//检索获得索引号
	pod_vector<int> vec_res_idx(500, 100);
	pod_vector<sight> vec_sight_idx(500,100);
	char pOrder[10] = {0};
	//------------------------------------------------------------------清空列表
	if (m_lsPoiList.GetItemCount() > 0) 
	{
		m_lsPoiList.DeleteAllItems();
	}
	switch(search_mode) {
	case 0:
		m_poi.Find(pStr, cur_city, POI_NAME, vec_res_idx);
		break;
	case 1:
		m_poi.Find(pStr, cur_city, POI_SPELL, vec_res_idx);
		break;
	case 2:
		m_poi.Find(pStr, vec_sight_idx, SIGHT_NAME);
		break;
	case 3:
		m_poi.Find(pStr, vec_sight_idx, SIGHT_SPELL);
		break;
	case 4:
		{
			m_poi.Find(pStr, cur_city, POI_SPELL, vec_res_idx);
		}
		break;
	case 5:
		m_poi.Find(pStr, cur_city, STREET_NAME, vec_res_idx);
		break;
	case 6:
		m_poi.Find(pStr, cur_city, STREET_SPELL, vec_res_idx);
		break;
	default:;
	}
	m_csStatus.Format("共%d条结果关于\"%s\"!",vec_res_idx.size(), pStr);

	b_char_mem str;
	switch(search_mode) {
	//---------------------------------------------------------------------	poi名称和拼音
	case 0:
	case 1:
		{
			b_char_mem ad_info;
			poi_specific specific;
			CString poi_ls;
			for(i = 0; i < vec_res_idx.size(); i++)
			{
				m_poi.str_field(cur_city, POI_NAME, vec_res_idx[i], str);
				memset(pOrder, 0, 10);
				itoa(i, pOrder, 10);
				m_lsPoiList.InsertItem(i, pOrder);
				m_poi.poi_speci(cur_city, vec_res_idx[i], specific);
				m_poi.admin.bt_lv_str_info(specific.area, ad_info);
				poi_ls.Format("%s/%s", (const char*)str.m_buf, (const char*)ad_info.m_buf);
				m_lsPoiList.SetItemText(i, 1, poi_ls);
			}
		}
		break;
	//---------------------------------------------------------------------	景点检索
	case 2:
	case 3:
		{
			for(i = 0; i < vec_sight_idx.size(); i++)
			{
				m_poi.str_field(vec_sight_idx[i].AreaId, POI_NAME, vec_sight_idx[i].Index, str);
				memset(pOrder, 0, 10);
				itoa(i, pOrder, 10);
				m_lsPoiList.InsertItem(i, pOrder);
				m_lsPoiList.SetItemText(i, 1, (const char*)str.m_buf);
			}
		}
		break;
	//---------------------------------------------------------------------	道路名称和拼音
	case 5:
	case 6:
		for(i = 0; i < vec_res_idx.size(); i++)
		{
			m_poi.str_field(cur_city, STREET_NAME, vec_res_idx[i], str);
			memset(pOrder, 0, 10);
			itoa(i, pOrder, 10);
			m_lsPoiList.InsertItem(i, pOrder);
			m_lsPoiList.SetItemText(i, 1, (const char*)str.m_buf);
		}
		break;
	default:;
	}
	UpdateData(FALSE);
}
void CPOIViewDlg::UpdateCurAreaInfo(void)
{
	const pod_vector<Lemon<zone>*>* province = m_poi.admin.dump(0);
	const pod_vector<Lemon<zone>*>* city = m_poi.admin.dump(1);
	const pod_vector<Lemon<zone>*>* country = m_poi.admin.dump(2);
	
	int ipro, icity, icountry;
	m_AreaSelDlg.get_admin(ipro, icity, icountry);
	b_char_mem str_pro;
	b_char_mem str_city;
	b_char_mem str_country;
	(*province)[ipro]->m_Kernel.name(str_pro);
	(*city)[icity]->m_Kernel.name(str_city);
	m_csCurArea.Format("%s%s", str_pro.m_buf, str_city.m_buf);
	UpdateData(FALSE);
}
void CPOIViewDlg::OnCurrentarea() 
{
	// TODO: Add your command handler code here
	const pod_vector<Lemon<zone>*>* province = m_poi.admin.dump(0);
	const pod_vector<Lemon<zone>*>* city = m_poi.admin.dump(1);
	const pod_vector<Lemon<zone>*>* country = m_poi.admin.dump(2);
		
	m_AreaSelDlg.rebulid(province, city, country);

	m_AreaSelDlg.ShowWindow(SW_SHOW);
	
	UpdateCurAreaInfo();
}

void CPOIViewDlg::OnCurrentassort() 
{
	// TODO: Add your command handler code here
}

