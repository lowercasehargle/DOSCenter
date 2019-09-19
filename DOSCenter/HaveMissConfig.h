#pragma once


// HaveMissConfig dialog

class HaveMissConfig : public CDialog
{
	DECLARE_DYNAMIC(HaveMissConfig)

public:
	HaveMissConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~HaveMissConfig();

// Dialog Data
	enum { IDD = IDD_HAVEMISSSETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
