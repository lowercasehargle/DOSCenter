#pragma once
#include "afxwin.h"


// CDatabaseEntryDlg dialog

class CDatabaseEntryDlg : public CDialog
{
	DECLARE_DYNAMIC(CDatabaseEntryDlg)

public:
	CDatabaseEntryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDatabaseEntryDlg();

// Dialog Data
	enum { IDD = IDD_IGNOREENTRY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_databaseFilename;
	CEdit m_databaseExt;
	CEdit m_databaseSize;
	CEdit m_databaseCRC;

	CString m_databaseFilenameTxt;
	CString m_databaseExtTxt;
	CString m_databaseSizeTxt;
	CString m_databaseCRCTxt;
	afx_msg void OnBnClickedOk();
};
