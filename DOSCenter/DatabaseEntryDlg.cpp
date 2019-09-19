// DatabaseEntryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "DatabaseEntryDlg.h"
#include ".\databaseentrydlg.h"


// CDatabaseEntryDlg dialog

IMPLEMENT_DYNAMIC(CDatabaseEntryDlg, CDialog)
CDatabaseEntryDlg::CDatabaseEntryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDatabaseEntryDlg::IDD, pParent)
{
}

CDatabaseEntryDlg::~CDatabaseEntryDlg()
{
}

void CDatabaseEntryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATABASEFILENAME, m_databaseFilename);
	DDX_Control(pDX, IDC_DATABASEEXT, m_databaseExt);
//	DDX_Control(pDX, IDC_DATABASESIZE, m_databaseSize);
	DDX_Control(pDX, IDC_DATABASECRC, m_databaseCRC);

	m_databaseFilename.SetWindowText(m_databaseFilenameTxt);
	m_databaseExt.SetWindowText(m_databaseExtTxt);
//	m_databaseSize.SetWindowText(m_databaseSizeTxt);
	m_databaseCRC.SetWindowText(m_databaseCRCTxt);
}


BEGIN_MESSAGE_MAP(CDatabaseEntryDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDatabaseEntryDlg message handlers

void CDatabaseEntryDlg::OnBnClickedOk()
{
	m_databaseFilename.GetWindowText(m_databaseFilenameTxt);
	m_databaseExt.GetWindowText(m_databaseExtTxt);
//	m_databaseSize.GetWindowText(m_databaseSizeTxt);
	m_databaseCRC.GetWindowText(m_databaseCRCTxt);
	OnOK();
}
