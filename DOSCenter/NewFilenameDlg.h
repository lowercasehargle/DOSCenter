#pragma once
#include "afxwin.h"


// CNewFilenameDlg dialog

class CNewFilenameDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewFilenameDlg)

public:
	CNewFilenameDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewFilenameDlg();
	virtual BOOL OnInitDialog();
// Dialog Data
	enum { IDD = IDD_NEWFILENAMEDLG };
	afx_msg void OnBnClickedOk();
	CString m_txt;
	int m_ok;  

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_newFilenameTxt;

};
