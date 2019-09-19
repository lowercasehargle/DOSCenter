// PopupViewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "PopupViewerDlg.h"



// CPopupViewerDlg dialog

IMPLEMENT_DYNAMIC(CPopupViewerDlg, CDialog)
CPopupViewerDlg::CPopupViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPopupViewerDlg::IDD, pParent)
{
}

CPopupViewerDlg::~CPopupViewerDlg()
{
}

void CPopupViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_POPUPVIEWTXT, m_viewerTxtEdit);
}
BOOL CPopupViewerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
//	SetIcon(m_hIcon, TRUE);			// Set big icon
//	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_viewerTxtEdit.SetWindowText(m_viewerTxt);
	CDialog::SetWindowText(m_fileNameViewer);
	m_bStrings = false;
	
	INIT_EASYSIZE;
	return TRUE;
}

BEGIN_MESSAGE_MAP(CPopupViewerDlg, CDialog)
	ON_WM_SIZE()
	ON_EN_SETFOCUS(IDC_POPUPVIEWTXT, OnEnSetfocusGenoutputwindow)
	ON_BN_CLICKED(IDC_STRINGS, OnBnClickedStrings)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CPopupViewerDlg)
  EASYSIZE(IDC_POPUPVIEWTXT,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

// CPopupViewerDlg message handlers
void CPopupViewerDlg::OnSize(UINT nType, int cx, int cy) 
{
    CDialog::OnSize(nType, cx, cy);
    UPDATE_EASYSIZE;
} 
void CPopupViewerDlg::OnEnSetfocusGenoutputwindow()
{
	m_viewerTxtEdit.SetSel(-1,0);
}
void CPopupViewerDlg::OnBnClickedStrings()
{
	m_bStrings = true;
	OnOK();
}
