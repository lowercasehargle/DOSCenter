#pragma once
#include "afxwin.h"
#include "easySize.h"
#include "Stringx.h"

// CSpamPopupViewer dialog

class CSpamPopupViewer : public CDialog
{
	DECLARE_DYNAMIC(CSpamPopupViewer)
	DECLARE_EASYSIZE

public:
	CSpamPopupViewer(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpamPopupViewer();
	int m_dlgResults;
// Dialog Data
	enum { IDD = IDD_QUICKVIEWPOPUPDLG };
	CString m_viewerTxt;
	CString m_fileNameViewer;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedYesspam();
	afx_msg void OnBnClickedNotspam();
	afx_msg void OnBnClickedSpamUncle();
	CEdit m_viewerTxtEdit;
};
