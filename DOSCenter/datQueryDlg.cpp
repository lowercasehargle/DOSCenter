// datQueryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "datQueryDlg.h"


// datQueryDlg dialog

IMPLEMENT_DYNAMIC(datQueryDlg, CDialog)

datQueryDlg::datQueryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(datQueryDlg::IDD, pParent)
{

}

datQueryDlg::~datQueryDlg()
{
}

void datQueryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ROMNAME, m_textItem);
}


BEGIN_MESSAGE_MAP(datQueryDlg, CDialog)
	ON_BN_CLICKED(IDC_DATQSEARCH, OnBnClickedDatSearch)
END_MESSAGE_MAP()


// datQueryDlg message handlers

void datQueryDlg::OnBnClickedDatSearch()
{
	// get the search object
	CString txt;
	CString output=L"";
	m_textItem.GetWindowText(txt);
	for (unsigned int i=0; i<theApp.m_datfileCount; i++)
	{
		
		for (int ii=0; ii<theApp.m_datZipList[i].fileCount; ii++)
		{
			if (theApp.m_datZipList[i].ROMsMap[ii].name == txt)
			{
				output+=theApp.m_datZipList[i].filename;
				output+=theApp.m_datZipList[i].ROMsMap[ii].name;
			}
		}
	}

}