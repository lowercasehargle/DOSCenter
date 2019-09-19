#pragma once

// CHaveMissDlg dialog

class CHaveMissDlg : public CDialog
{
	DECLARE_DYNAMIC(CHaveMissDlg)

public:
	CHaveMissDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHaveMissDlg();

// Dialog Data
	enum { IDD = IDD_HAVEMISSSETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedToglall();
	afx_msg void OnBnClickedOk();
	BOOL OnInitDialog();
	bool yearInSelectedList(CString year);
	//bitmask: 198x-1989, 199x, 1990-1999, 19xx, 2000-2014, win, dupes
};
