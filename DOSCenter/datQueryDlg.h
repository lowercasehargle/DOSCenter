#pragma once
#include "afxwin.h"


// datQueryDlg dialog

class datQueryDlg : public CDialog
{
	DECLARE_DYNAMIC(datQueryDlg)

public:
	datQueryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~datQueryDlg();

// Dialog Data
	enum { IDD = IDD_DATQUERYDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void OnBnClickedDatSearch();

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_textItem;
};
