#pragma once
#include "afxwin.h"
#include "easySize.h"
#include "Stringx.h"

// CPopupViewerTxtDlg dialog

class CPopupViewerTxtDlg : public CDialog
{
	DECLARE_DYNAMIC(CPopupViewerTxtDlg)
	DECLARE_EASYSIZE
public:
	CPopupViewerTxtDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPopupViewerTxtDlg();

// Dialog Data
	enum { IDD = IDD_POPUPVIEWTXTDLG };
	CStringEx m_viewerTxt;
	CString m_fileNameViewer;
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void OnBnClickedOk();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void CPopupViewerTxtDlg::OnTimer(UINT nIDEvent);
	UINT_PTR m_UpdateTimer;
//	afx_msg void OnKeyDown(UINT nChar,  UINT nRepCnt,   UINT nFlags );

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	int m_pos;

public:
	CEdit m_viewerTxtEdit;
	afx_msg void OnBnClickedSearch();
	CEdit m_searchData;


};
