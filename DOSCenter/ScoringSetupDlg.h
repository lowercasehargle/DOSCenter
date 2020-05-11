#pragma once
#include "afxwin.h"


// CScoringSetupDlg dialog

class CScoringSetupDlg : public CDialog
{
	DECLARE_DYNAMIC(CScoringSetupDlg)

public:
	CScoringSetupDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CScoringSetupDlg();

// Dialog Data
	enum { IDD = IDD_SCORESETUPDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult);
	CSortListCtrl m_ignoreList;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	//void populateList(CString data);
	void populateIgnoreList();
	void addEntry2ignorelist(CString filename, CString ext, CString scrc);
	int prepIgnoreListForSave();
	void saveIgnoreList();
	CEdit m_fileSizeIgnoreEdit;
	afx_msg void OnBnClickedFilesizeignoreena();

};
