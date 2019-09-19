#pragma once
#include "afxwin.h"


// CDATFileInfoDlg dialog

class CDATFileInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CDATFileInfoDlg)

public:
	CDATFileInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDATFileInfoDlg();

// Dialog Data
	enum { IDD = IDD_DATFILEINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	BOOL OnInitDialog();
public:
	CEdit m_datInfoTitle;
	CEdit m_DATDescription;
	CEdit m_DATVersion;
	CEdit m_DATWebsite;
	CEdit m_DATComments;
	bool m_recursive;
	afx_msg void OnBnClickedOk();
	CString m_datTitleTxt, m_datDescTxt, m_datVerTxt, m_datWebTxt, m_datCommentTxt, m_dateTxt, m_datAuthorTxt;
	bool m_displayOnly;
	CEdit m_DATDate;
	CEdit m_datAuthor;
};
