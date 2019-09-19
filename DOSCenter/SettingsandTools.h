#pragma once
#include "SADirRead.h"
#include "afxwin.h"
#include "DOSCenter.h"
#include "TDCSet.h"
#include <fstream>


#define DELETE_ENTRY	0xDEADDB

// CSettingsandTools dialog

class CSettingsandTools : public CDialog
{
	DECLARE_DYNAMIC(CSettingsandTools)

public:
	CSettingsandTools(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsandTools();

// Dialog Data
	enum { IDD = IDD_SETTINGSANDTOOLS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CSADirRead m_zipDir;
	CSADirRead::SAFileVector::const_iterator m_zipFiles; // index thingyabob into file array
	DECLARE_MESSAGE_MAP()
	void entry2File(CStringW entry);
	int zip2dat(CStringW zipfilename);
//	CUnzipper m_uz;
public:
	CString m_SourcePath;		// path of files we're scanning
	void parseDATFile();
	CEdit m_minScore4Rename;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	BOOL OnInitDialog();
	afx_msg void OnBnClickedLoaddatfile();
	void OnBnClickedLoadMDBfile();
	bool m_autoadd;
	unsigned int String2Date(CString datetime);
	void rom2datmap(CStringW name, unsigned int size, int status, bool found, unsigned int date, CString scrc, unsigned int ptr2zip, int romCount);
	bool m_recursive;
	bool m_cmdLine;

	bool m_noPathInDat;


	void Add2DatFile();
	
	void CSettingsandTools::zipName2Vars(CString filename);

	void makeDATFile(CString m_SourcePath);
	void makeDATFile_express(CString m_SourcePath);



	afx_msg void OnBnClickedMakeadat();
	afx_msg void OnBnClickedScratchpadbrowse();
	afx_msg void OnBnClickedSetdoscollectionpathbtn();
	afx_msg void OnBnClickedScoringcfgbtn();

	afx_msg void OnBnClickedDosboxbrowse();
	std::map <CString, unsigned int> tmpZipList; // a checker for duplicate incoming files

	afx_msg void OnBnClickedDatfileinfo();
	//afx_msg void OnBnClickedCrcscan();
	//afx_msg void OnBnClickedCrcfilename();
	afx_msg void OnBnClickedMissinglist();
	afx_msg void OnBnClickedDetailedreports();
	afx_msg void OnBnClickedPerfectstoo();

	//dizInfo	m_dizInfo;
	afx_msg void OnBnClickedHavemisslistcfg();
	void deleteKeys();
	void loadMDB(CString mdbFile);
	void updateDBRecord(int inputFlag, CStringW sold, CStringW snew);
	void db2cmap(CStringW zipname, ZipInfoMDB tmpZipInfo);
	CDatabase m_database;
	void makeLists();
	afx_msg void OnBnClickedMdbCheck();
	unsigned int hashAString(char *str, unsigned int hval);
};

