#pragma once
#include "afxcmn.h"
#include "SortListCtrl.h"
#include "easysize.h"

// CPickListDlg dialog

class CPickListDlg : public CDialog
{
	DECLARE_EASYSIZE
	DECLARE_DYNAMIC(CPickListDlg)

public:
	CPickListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPickListDlg();

// Dialog Data
	enum { IDD = IDD_LISTPICKDLG };
	_BESTHITS m_topTwenty[20];
protected:
	void OnSize(UINT nType, int cx, int cy);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	
	CSortListCtrl m_pickListWindow;
	afx_msg void OnBnClickedOk();
	CStringW m_tagetMDBEntry;
	afx_msg void OnBnClickedOnok();
};
