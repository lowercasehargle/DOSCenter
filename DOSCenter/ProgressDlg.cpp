// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include ".\progressdlg.h"

//#include "ProgressThd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog


CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
	, m_TimerInterval_ms(0)
	, m_fTimedMode(FALSE)
	, m_fRunTimer(FALSE)
{
	//{{AFX_DATA_INIT(CProgressDlg)
		// NOTE: the ClassWizard will add member initialization here
	m_TopTxt = _T("");
	m_BotTxt = _T("");
	//}}AFX_DATA_INIT

	m_fThdMode = FALSE;
}


void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDlg)
//	DDX_Control(pDX, IDC_PROGRESS1, m_CtlProgress1);
	DDX_Text(pDX, IDC_TOP_TEXT, m_TopTxt);
	DDX_Text(pDX, IDC_BOT_TEXT, m_BotTxt);

	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_ANIMATE1, m_Animation);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CProgressDlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_PROG_INC_COMMAND, OnTimerInc)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg message handlers

void CProgressDlg::OnOK() 
{
	// this is the stop button - set the stop flag
//	PostThreadMessage(m_nThdId, WM_SET_STOP_FLAG, 0, 0);

	// don't close me this way
//	CDialog::OnOK();
}

void CProgressDlg::UpdateText(CString sTopTxt, CString sBotTxt) // both default to ""
{
	m_TopTxt = sTopTxt;
	m_BotTxt = sBotTxt;
	GetDlgItem(IDC_TOP_TEXT)->SetWindowText(m_TopTxt);
	GetDlgItem(IDC_BOT_TEXT)->SetWindowText(m_BotTxt);
}

void CProgressDlg::OnCancel() 
{
	if ( m_fTimedMode )
		StopProgTimer();

	if ( m_fThdMode )
	{
		// kill the thread
		DestroyWindow();
	}
	else
	{
		// close normally
		CDialog::OnCancel();
	}
}

void CProgressDlg::PostNcDestroy() 
{
	if ( m_fThdMode )
	{
		delete this;
		::PostQuitMessage(0);
	}
	else
	{
		CDialog::PostNcDestroy();
	}
}

void CProgressDlg::StartProgTimer()
{
	ASSERT(m_fTimedMode);
	ASSERT(m_TimerInterval_ms != 0);

	// start worker thread
	m_fRunTimer = TRUE;
	AfxBeginThread(RunProgTimer, this);
}

void CProgressDlg::StopProgTimer(void)
{
	ASSERT(m_fTimedMode);

	m_fRunTimer = FALSE;
}

void CProgressDlg::Init(BOOL fStndMode, UINT MaxRange, CString sTopTxt, CString sBotTxt)
{
	// note: since Create will cause the InitDialog routine
	// to be called, the order of execution below is critical.

	Create(IDD_PROGRESS);

	Reset(fStndMode, MaxRange, sTopTxt, sBotTxt);
}

void CProgressDlg::Reset(BOOL fStndMode, UINT MaxRange, CString sTopTxt, CString sBotTxt)
{
	if ( fStndMode )
	{
		// standard mode
		m_fTimedMode = FALSE;
		m_CtlProgress1.SetRange32(0, (int)MaxRange);
	}

	ShowWindow(SW_SHOW);
	SetForegroundWindow();
//	m_CtlProgress1.SetStep(1);
//	m_CtlProgress1.SetPos(0);
	UpdateText(sTopTxt, sBotTxt);
}

afx_msg LRESULT CProgressDlg::OnTimerInc(WPARAM wParam, LPARAM lParam)
{
	Inc();

	return 0;
}

void CProgressDlg::Inc()
{
	//m_CtlProgress1.StepIt();
}

int CProgressDlg::SetPos(int NewPos)
{
	//int old = m_CtlProgress1.SetPos(NewPos);
	UpdateWindow();
	return 0; //old;
}

// worker thread function used to run the Progress Timer.
UINT RunProgTimer(LPVOID pParam)
{
	CProgressDlg* pDlg = (CProgressDlg*)pParam;

	do
	{
		Sleep(pDlg->m_TimerInterval_ms);
		pDlg->PostMessage(WM_PROG_INC_COMMAND);
	}
	while(pDlg->m_fRunTimer);

	return 0;
}
