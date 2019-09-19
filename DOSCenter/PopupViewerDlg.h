#pragma once
#include "afxwin.h"
#include "easySize.h"


// CPopupViewerDlg dialog

class CPopupViewerDlg : public CDialog
{
	DECLARE_DYNAMIC(CPopupViewerDlg)
	DECLARE_EASYSIZE
public:
	CPopupViewerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPopupViewerDlg();
	afx_msg void OnSize(UINT nType, int cx, int cy); 
// Dialog Data
	enum { IDD = IDD_POPUPVIEWER };
	CString m_viewerTxt;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnEnSetfocusGenoutputwindow();
	DECLARE_MESSAGE_MAP()
	HICON m_hIcon;
public:
	CEdit m_viewerTxtEdit;
	CString m_fileNameViewer;
	afx_msg void OnBnClickedStrings();
	bool m_bStrings;
};
