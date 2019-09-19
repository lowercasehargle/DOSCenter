#pragma once


// CFileIDDizEditDlg dialog

class CFileIDDizEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileIDDizEditDlg)

public:
	CFileIDDizEditDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFileIDDizEditDlg();

// Dialog Data
	enum { IDD = IDD_FILEIDDIZEDITDLG };
	CStringW m_dizTxt;
	CStringW m_fileNameViewer;
	CEdit m_viewerTxtEdit;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
