#if !defined(AFX_PROGRESSDLG_H__7FC47AE8_04F2_405F_9D55_DA6C0C0957DF__INCLUDED_)
#define AFX_PROGRESSDLG_H__7FC47AE8_04F2_405F_9D55_DA6C0C0957DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressDlg.h : header file
//

#include "afxcmn.h"
#include "resource.h"

#define PROG_MODE_STANDARD	TRUE
#define PROG_MODE_TIMER		FALSE

const int WM_PROG_INC_COMMAND = WM_USER + 104;

// worker thread routine used as timer
UINT RunProgTimer(LPVOID pParam);

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

class CProgressDlg : public CDialog
{
// Construction
public:
	UINT m_TimerInterval_ms;
	BOOL m_fRunTimer;
	int m_nThdId;
	BOOL m_fThdMode;
	void StartProgTimer();
	void StopProgTimer(void);
	void Inc();
	int SetPos(int NewPos);
	void Reset(BOOL fStndMode, UINT MaxRange, CString sTopTxt = L"", CString sBotTxt = L"");
	void Init(BOOL fStndMode, UINT MaxRange, CString sTopTxt = L"", CString sBotTxt = L"");
	void UpdateText(CString sTopTxt = L"", CString sBotTxt = L"");
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProgressDlg)
	enum { IDD = IDD_PROGRESS };
		// NOTE: the ClassWizard will add data members here

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProgressDlg)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg LRESULT OnTimerInc(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	CProgressCtrl	m_CtlProgress1;
	CString	m_TopTxt;
	CString	m_BotTxt;
	BOOL m_fTimedMode;
	CAnimateCtrl m_Animation;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSDLG_H__7FC47AE8_04F2_405F_9D55_DA6C0C0957DF__INCLUDED_)
