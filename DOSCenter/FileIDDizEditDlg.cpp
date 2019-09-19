// FileIDDizEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "FileIDDizEditDlg.h"


// CFileIDDizEditDlg dialog

IMPLEMENT_DYNAMIC(CFileIDDizEditDlg, CDialog)

CFileIDDizEditDlg::CFileIDDizEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileIDDizEditDlg::IDD, pParent)
{

}

CFileIDDizEditDlg::~CFileIDDizEditDlg()
{
}

void CFileIDDizEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILEIDDIZVIEWERTXT, m_viewerTxtEdit);
}


BEGIN_MESSAGE_MAP(CFileIDDizEditDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFileIDDizEditDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CFileIDDizEditDlg message handlers

void CFileIDDizEditDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	
	m_viewerTxtEdit.GetWindowTextW(m_dizTxt);
	OnOK();
}
BOOL CFileIDDizEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_viewerTxtEdit.SetWindowText(m_dizTxt);
	CDialog::SetWindowText(m_fileNameViewer);
	return TRUE;
}