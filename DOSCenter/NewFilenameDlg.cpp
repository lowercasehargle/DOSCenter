// NewFilenameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "NewFilenameDlg.h"


// CNewFilenameDlg dialog

IMPLEMENT_DYNAMIC(CNewFilenameDlg, CDialog)
CNewFilenameDlg::CNewFilenameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewFilenameDlg::IDD, pParent)
{
}

CNewFilenameDlg::~CNewFilenameDlg()
{
}

void CNewFilenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NEWFILENAMETXT, m_newFilenameTxt);
	m_newFilenameTxt.SetWindowText(m_txt);
}
BOOL CNewFilenameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ok=0;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CNewFilenameDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CNewFilenameDlg message handlers
void CNewFilenameDlg::OnBnClickedOk()
{

	m_newFilenameTxt.GetWindowText(m_txt);
	m_ok=1;
	OnOK();

}