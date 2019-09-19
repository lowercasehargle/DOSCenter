// HexCompareDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "HexCompareDlg.h"
#include ".\hexcomparedlg.h"


// CHexCompareDlg dialog

IMPLEMENT_DYNAMIC(CHexCompareDlg, CDialog)
CHexCompareDlg::CHexCompareDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHexCompareDlg::IDD, pParent)
{
}

CHexCompareDlg::~CHexCompareDlg()
{
}

void CHexCompareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SRCDATA, m_srcData);
	DDX_Control(pDX, IDC_TGTDATA, m_tgtData);
	m_srcData.SetWindowText(m_srcDataTxt);
	m_tgtData.SetWindowText(m_tgtDataTxt);
	
	if (m_srcZipFilename.GetLength() > 110)
		m_srcZipFilename = L"..."+m_srcZipFilename.Right(107);

	if (m_tgtZipFilename.GetLength() > 110)
		m_tgtZipFilename = L"..."+m_tgtZipFilename.Right(107);

	((CButton*)GetDlgItem(IDC_HEXSRCFILENAMETXT))->SetWindowText(m_srcZipFilename);
	((CButton*)GetDlgItem(IDC_HEXTGTFILENAMETXT))->SetWindowText(m_tgtZipFilename);
	CDialog::SetWindowText(m_windowTitle);
	if (m_prev == -1)
		((CButton*)GetDlgItem(IDC_PREVDIFBTN))->EnableWindow(0);


}


BEGIN_MESSAGE_MAP(CHexCompareDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_PREVDIFBTN, OnBnClickedPrevdifbtn)
	ON_BN_CLICKED(IDC_MAKEDIFBTN, OnBnClickedMakedifbtn)
END_MESSAGE_MAP()


// CHexCompareDlg message handlers

// the next dif button
void CHexCompareDlg::OnBnClickedOk()
{
	m_prev = 0;
	m_next = 1;
	OnOK();
}

// the previous dif button
void CHexCompareDlg::OnBnClickedPrevdifbtn()
{
	m_next = 0;
	m_prev = 1;
	OnOK();
}

void CHexCompareDlg::OnBnClickedMakedifbtn()
{
	m_makeDif = 1;
	OnOK();
}
