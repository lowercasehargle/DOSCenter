// DATFileInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "DATFileInfoDlg.h"
#include ".\datfileinfodlg.h"


// CDATFileInfoDlg dialog

IMPLEMENT_DYNAMIC(CDATFileInfoDlg, CDialog)
CDATFileInfoDlg::CDATFileInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDATFileInfoDlg::IDD, pParent)
{
}

CDATFileInfoDlg::~CDATFileInfoDlg()
{
}


BOOL CDATFileInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString txt;

	if (m_displayOnly == true)
	{
		m_datInfoTitle.SetWindowText(theApp.m_datName);
		m_DATDescription.SetWindowText(theApp.m_datDescription);
		m_DATVersion.SetWindowText(theApp.m_datVersion);
		m_DATWebsite.SetWindowText(theApp.m_datHomepage);
		m_DATDate.SetWindowText(theApp.m_datdate);
		m_datAuthor.SetWindowText(theApp.m_datAuthor);
		m_DATComments.SetWindowText(theApp.m_datComment);

		((CButton*)GetDlgItem(IDCANCEL))->ShowWindow(0);
		((CButton*)GetDlgItem(IDC_DATRECURSIVE))->ShowWindow(0);
		if (theApp.m_DATfileLoaded)
		{
			// print # of zips and ROMs in the .dat
			txt.Format(L".DAT contains %d zip files with %d files", theApp.m_datfileCount, theApp.m_datROMCount);
			((CButton*)GetDlgItem(IDC_TOTALZIPSTXT))->ShowWindow(1);
			((CButton*)GetDlgItem(IDC_TOTALZIPSTXT))->SetWindowText(txt);

		}
	}
	else
	{
		CTime time = CTime::GetCurrentTime();
		CString d = time.Format( "%A, %B %d, %Y" );
		CString t = CTime::GetCurrentTime().Format("%H:%M");
		CString datenTime;
		datenTime = d+"@"+t;
		m_DATDate.SetWindowText(datenTime);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}



void CDATFileInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATTITLE, m_datInfoTitle);
	DDX_Control(pDX, IDC_DATDESC, m_DATDescription);
	DDX_Control(pDX, IDC_DATVERSION, m_DATVersion);
	DDX_Control(pDX, IDC_DATWEBSITE, m_DATWebsite);
	DDX_Control(pDX, IDC_DATCOMMENTS, m_DATComments);
	DDX_Control(pDX, IDC_DATDATE, m_DATDate);
	DDX_Control(pDX, IDC_DATAUTHOR, m_datAuthor);
}


BEGIN_MESSAGE_MAP(CDATFileInfoDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDATFileInfoDlg message handlers

void CDATFileInfoDlg::OnBnClickedOk()
{
	if (m_displayOnly == false)
	{
		m_recursive = ( 1 == ((CButton*)GetDlgItem(IDC_DATRECURSIVE))->GetCheck() );
		m_datInfoTitle.GetWindowText(m_datTitleTxt);
		m_DATDescription.GetWindowText(m_datDescTxt);
		m_DATVersion.GetWindowText(m_datVerTxt);
		m_DATWebsite.GetWindowText(m_datWebTxt);
		m_DATComments.GetWindowText(m_datCommentTxt);
		m_DATDate.GetWindowText(m_dateTxt);
		m_datAuthor.GetWindowText(m_datAuthorTxt);
	}
	OnOK();
}
