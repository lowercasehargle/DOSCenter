// SpamPopupViewer.cpp : implementation file
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "SpamPopupViewer.h"
#include "afxdialogex.h"


// CSpamPopupViewer dialog

IMPLEMENT_DYNAMIC(CSpamPopupViewer, CDialog)

CSpamPopupViewer::CSpamPopupViewer(CWnd* pParent /*=NULL*/)
	: CDialog(CSpamPopupViewer::IDD, pParent)
{

}

CSpamPopupViewer::~CSpamPopupViewer()
{
}

void CSpamPopupViewer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_POPUPVIEWERTXT, m_viewerTxtEdit);
}
BOOL CSpamPopupViewer::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	//	SetIcon(m_hIcon, TRUE);			// Set big icon
	//	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_viewerTxtEdit.SetWindowText(m_viewerTxt);
	CDialog::SetWindowText(m_fileNameViewer);
	m_dlgResults = 0;

	INIT_EASYSIZE;
	return TRUE;
}

BEGIN_MESSAGE_MAP(CSpamPopupViewer, CDialog)
	ON_BN_CLICKED(IDC_YESSPAM, &CSpamPopupViewer::OnBnClickedYesspam)
	ON_BN_CLICKED(IDC_NOTSPAM, &CSpamPopupViewer::OnBnClickedNotspam)
	ON_BN_CLICKED(IDC_SPAM_UNCLE, &CSpamPopupViewer::OnBnClickedSpamUncle)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CSpamPopupViewer)
	EASYSIZE(IDC_POPUPVIEWERTXT, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP


// CSpamPopupViewer message handlers


void CSpamPopupViewer::OnBnClickedYesspam()
{
	m_dlgResults = 1;
	OnCancel();
}


void CSpamPopupViewer::OnBnClickedNotspam()
{
	m_dlgResults = 2;
	OnCancel();
}


void CSpamPopupViewer::OnBnClickedSpamUncle()
{
	m_dlgResults = 3;
	OnCancel();
}
