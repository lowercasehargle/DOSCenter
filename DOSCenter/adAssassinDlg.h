#pragma once

typedef struct _SPAMHAMWORD	// 
{
	std::string word;		// the word itself
	unsigned int count;		// the count or weight
} SPAMHAMWORD;


// adAssassinDlg dialog

class adAssassinDlg : public CDialog
{
	DECLARE_DYNAMIC(adAssassinDlg)

public:
	adAssassinDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~adAssassinDlg();

// Dialog Data
	enum { IDD = IDD_ADASSASSIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	void saveTokenfile(bool spam);
	void loadTokenfile(bool spam);
	void saveHamTokenfile(void);
	void saveSpamTokenfile(void);
	void loadHamTokenFile(void);
	void loadSpamTokenFile(void);
	void doSpamTraining(void);
	void doHamTraining(void);
	int spamOrHam(CString filename);
	void addToHamToken(char* buffer);
	void addToSpamToken(char* buffer);
public:
	CString m_spamTokenPath;
	CString m_hamTokenPath;

	CString m_spamAssZipsPath;

	std::map<unsigned int, _SPAMHAMWORD> m_spamhashes;
	std::map<unsigned int, _SPAMHAMWORD>::const_iterator spam_it;

	std::map<unsigned int, _SPAMHAMWORD> m_hamhashes;
	std::map<unsigned int, _SPAMHAMWORD>::const_iterator ham_it;

	afx_msg void OnBnClickedAddspam();
	afx_msg void OnBnClickedAddham();

	afx_msg void OnBnClickedSpamassLoadbtn();
	afx_msg void OnBnClickedSpamassTrainspamBtn();
	afx_msg void OnBnClickedSpamassTrainhamBtn();
	afx_msg void OnBnClickedSpamassbrowsebutton4();
	afx_msg void OnBnClickedSpamassGo();
};
