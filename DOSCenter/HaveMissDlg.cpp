// HaveMissDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "HaveMissDlg.h"
#include ".\havemissdlg.h"


// CHaveMissDlg dialog

IMPLEMENT_DYNAMIC(CHaveMissDlg, CDialog)
CHaveMissDlg::CHaveMissDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHaveMissDlg::IDD, pParent)
{
}

CHaveMissDlg::~CHaveMissDlg()
{
}

void CHaveMissDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CHaveMissDlg::OnInitDialog()
{
	ULONGLONG bit=1;
	//bitmask: 198x-1989, 199x, 1990-1999, 19xx, 2000-2014, win, dupes
	for (int i=IDC_SCAN198x; i<IDC_SCANDUPES; i++)
	{
		if ((theApp.m_selectedYears & bit) == bit)
			((CButton*)GetDlgItem(i))->SetCheck(1);
		bit =(bit <<1);
	}
	return TRUE;
}

BEGIN_MESSAGE_MAP(CHaveMissDlg, CDialog)
	ON_BN_CLICKED(IDC_TOGLALL, OnBnClickedToglall)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CHaveMissDlg message handlers

void CHaveMissDlg::OnBnClickedToglall()
{
	bool state=(bool)(((CButton*)GetDlgItem(IDC_TOGLALL))->GetCheck());
	for (int i=IDC_SCAN198x; i<IDC_SCANDUPES; i++)
		(((CButton*)GetDlgItem(i))->SetCheck(state));

	
}

void CHaveMissDlg::OnBnClickedOk()
{
	ULONGLONG bit=1;
	theApp.m_selectedYears=0;
	//bitmask: 198x-1989, 199x, 1990-1999, 19xx, 2000-2014, win, dupes
	for (int i=IDC_SCAN198x; i<IDC_SCANDUPES; i++)
	{
		if ( (((CButton*)GetDlgItem(i))->GetCheck()) )
			theApp.m_selectedYears |=bit;
		bit =(bit <<1);
	}

	OnOK();
}

// returns true if year text is in the selected years checkbox.
bool CHaveMissDlg::yearInSelectedList(CString year)
{
CString yearTxt[44] = {L"198x",L"1981",L"1982",L"1983",L"1984",L"1985",L"1986",L"1987",L"1988",L"1989",L"199x",L"1990",L"1991",L"1992",L"1993",L"1994",L"1995",L"1996",L"1997",L"1998",L"1999",L"19xx", \
L"2000", L"2001", L"2002", L"2003", L"2004", L"2005", L"2006", L"2007", L"2008", L"2009", L"2010", L"2011", L"2012", L"2013", L"2014", L"2015", L"2016", L"2017", L"2018",L"2019", L"win", L"dupes" };

	ULONGLONG bit=1;

	for (int i=0; i<40; i++)
	{
		if ( ((yearTxt[i]==year) && ((theApp.m_selectedYears & bit) == bit)) ) 
			return true;
		bit =(bit <<1);
	}
	return false;
}