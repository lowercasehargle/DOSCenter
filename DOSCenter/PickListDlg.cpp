// PickListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "PickListDlg.h"
#include "afxdialogex.h"


// CPickListDlg dialog

IMPLEMENT_DYNAMIC(CPickListDlg, CDialog)
CPickListDlg::CPickListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPickListDlg::IDD, pParent)
{

}

CPickListDlg::~CPickListDlg()
{
}

void CPickListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICKLISTSELECT, m_pickListWindow);
}

BOOL CPickListDlg::OnInitDialog()
{
	CString score;
	CDialog::OnInitDialog();
	// create our columns in each window pane
	// color column is invisible to the user, but controls the text color displayed
	m_pickListWindow.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_pickListWindow.SetHeadings(_T("Color,0; Closest DB Match,550; Score,40"));
	m_pickListWindow.LoadColumnInfo();
	m_pickListWindow.AddItem(COLOR_BLACK, L"Add as new", "0");
	INIT_EASYSIZE;
	// sort the top twenty!

	for (int i = 0; i < 20; i++)
	{
		score.Format(L"%d", m_topTwenty[i].score);
		m_pickListWindow.AddItem(COLOR_BLACK, m_topTwenty[i].filename, score);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}
BEGIN_MESSAGE_MAP(CPickListDlg, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CPickListDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CPickListDlg)
	EASYSIZE(IDC_PICKLISTSELECT, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP

// CPickListDlg message handlers
void CPickListDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

void CPickListDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	int nIndex = -1;
	nIndex = m_pickListWindow.GetNextItem(nIndex, LVNI_SELECTED);
	if (nIndex != -1)
	{
		m_tagetMDBEntry = m_pickListWindow.GetItemText(nIndex, FILENAMECOL);
		OnOK();
	}
}

