// HaveMissConfig.cpp : implementation file
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "HaveMissConfig.h"


// HaveMissConfig dialog

IMPLEMENT_DYNAMIC(HaveMissConfig, CDialog)
HaveMissConfig::HaveMissConfig(CWnd* pParent /*=NULL*/)
	: CDialog(HaveMissConfig::IDD, pParent)
{
}

HaveMissConfig::~HaveMissConfig()
{
}

void HaveMissConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(HaveMissConfig, CDialog)
END_MESSAGE_MAP()


// HaveMissConfig message handlers
