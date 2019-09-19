#pragma once
#include "afxwin.h"


// CHexCompareDlg dialog

class CHexCompareDlg : public CDialog
{
	DECLARE_DYNAMIC(CHexCompareDlg)

public:
	CHexCompareDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHexCompareDlg();

// Dialog Data
	enum { IDD = IDD_HEXCOMPARE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_srcData;
	CEdit m_tgtData;

	CString m_srcDataTxt;
	CString m_tgtDataTxt;
	CString m_srcZipFilename;
	CString m_tgtZipFilename;
	CString m_windowTitle;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedPrevdifbtn();
	int m_prev;
	int m_next;
	afx_msg void OnBnClickedMakedifbtn();
	int m_makeDif ;
};
