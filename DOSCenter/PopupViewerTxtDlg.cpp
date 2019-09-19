// PopupViewerTxtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "PopupViewerTxtDlg.h"
#include ".\popupviewertxtdlg.h"
#include "stringx.h"


// CPopupViewerTxtDlg dialog

IMPLEMENT_DYNAMIC(CPopupViewerTxtDlg, CDialog)
CPopupViewerTxtDlg::CPopupViewerTxtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPopupViewerTxtDlg::IDD, pParent)
{
}

CPopupViewerTxtDlg::~CPopupViewerTxtDlg()
{
}

void CPopupViewerTxtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_POPUPVIEWERTXT, m_viewerTxtEdit);
	DDX_Control(pDX, IDC_SEARCHDATA, m_searchData);
}

BOOL CPopupViewerTxtDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_pos = 0;
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
//	SetIcon(m_hIcon, TRUE);			// Set big icon
//	SetIcon(m_hIcon, FALSE);		// Set small icon
// IDC_EDIT is the resource ID of the edit control in your dialog.

	m_viewerTxtEdit.SetWindowText(m_viewerTxt);
	CDialog::SetWindowText(m_fileNameViewer);
    //m_viewerTxtEdit.SubclassDlgItem(IDC_POPUPVIEWERTXT, this);
	INIT_EASYSIZE;
	SetTimer(1, 25, NULL);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CPopupViewerTxtDlg, CDialog)
	ON_WM_SIZE()
//	ON_EN_SETFOCUS(IDC_POPUPVIEWTXT, OnEnSetfocusGenoutputwindow)
	ON_BN_CLICKED(IDC_SEARCH, OnBnClickedSearch)
	ON_WM_TIMER()
	ON_WM_GETDLGCODE()
    ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CPopupViewerTxtDlg)
  EASYSIZE(IDC_POPUPVIEWERTXT,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

// keep app from closing upon hitting the enter key.
void CPopupViewerTxtDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//OnOK();
}

// CPopupViewerTxtDlg message handlers
// CPopupViewerDlg message handlers
void CPopupViewerTxtDlg::OnSize(UINT nType, int cx, int cy) 
{
    CDialog::OnSize(nType, cx, cy);
    UPDATE_EASYSIZE;
} 
void CPopupViewerTxtDlg::OnBnClickedSearch()
{
	CString searchItem;

	m_searchData.GetWindowText(searchItem);
	if (searchItem.GetLength() == 0)
		return;

	m_pos = m_viewerTxt.FindNoCase(searchItem, m_pos+1);
	if (m_pos != -1)
	{
		m_viewerTxtEdit.SetFocus();
		m_viewerTxtEdit.SetSel(m_pos, m_pos+searchItem.GetLength());
	}
	else
		m_pos=0;	// reset to beginning
}

// check if F3 has been pressed and search again. 
void CPopupViewerTxtDlg::OnTimer(UINT nIDEvent)
{
	if (GetKeyState(VK_F3) <0) 
	{
		OnBnClickedSearch();
		Sleep(300);	// debounce
	}
}
