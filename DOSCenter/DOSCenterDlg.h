
// DOSCenterDlg.h : header file
//

#pragma once
#include "splittercontrol.h"
#include "easysize.h"
#include "SADirRead.h"
#include "progressDlg.h"
#include "xunzip.h"
#include "xzip.h"
#include "dizInfo.h"


#define TOPARRAYSIZE 10	// it would be so much better to make this a map instead.

#define TZIPPED_TIMESTAMP 	0x2198bc00


// for generating a full report
#define STATUS_PERFECT 100
#define STATUS_RENAME_ZIP_ONLY 99
#define STATUS_RENAME_ROMS 85
#define STATUS_CRC_COLLISION 45
#define STATUS_EXTRAFILES 101
#define STATUS_MISSINGFILES 72


// ROM info details
#define STATUS_CRC_MATCH		(1 << 0)
#define STATUS_SIZE_MATCH		(1 << 1)
#define STATUS_FILENAME_MATCH	(1 << 2)
#define STATUS_TIMESTAMP_MATCH	(1 << 3)
#define STATUS_ROM_TZIPPED 		(1 << 4)	// sadly, this rom's timestamp has been wrecked by torrentzip
#define STATUS_ROM_ELSEWHERE 	(1 << 5)	// same file is located in other .dat entries
#define STATUS_ROM_IGNORED 		(1 << 6)	// this file is in the user's ignore list
#define STATUS_ROM_WRONGCASE 	(1 << 7)	// case is incorrect, we know the filename matched.
#define STATUS_ROM_PERFECT_MATCH (STATUS_CRC_MATCH + STATUS_SIZE_MATCH + STATUS_FILENAME_MATCH + STATUS_TIMESTAMP_MATCH)
//unused #define STATUS_ROM_FOUND (STATUS_CRC_MATCH + STATUS_FILENAME_MATCH)


// CDOSCenterDlg dialog
class CDOSCenterDlg : public CDialog
{
// Construction
public:
	CDOSCenterDlg(CWnd* pParent = NULL);	// standard constructor
	DECLARE_EASYSIZE

// Dialog Data
	enum { IDD = IDD_DOSCENTER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
// Implementation
	HICON m_hIcon;
	CSplitterControl     m_wndSplitter1; 
	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy); 
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void DoResize1(int delta);
	afx_msg void OnBnClickedSettingsbtn();
	void populateDATFileDataView();

public:
	HZIP m_uz;
	HZIP m_zip;
	unsigned int m_zipCount;
	CSADirRead m_zipDir;
	CSADirRead::SAFileVector::const_iterator m_zipFiles; // index thingyabob into file array
	void scanSelectedZips();
	void scanThisZip(int nIndex, CProgressDlg* progDlg);
	afx_msg void OnNMDblclkZipFileList(NMHDR *pNMHDR, LRESULT *pResult);
	int m_topTenIdx;
	_BESTHITS m_topTen[TOPARRAYSIZE];
	afx_msg void OnNMDblclkZipList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickLeftSideList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickRightSideList(NMHDR *pNMHDR, LRESULT *pResult);

	int m_zipCRCmatches;
	int m_zipROMNameMatches;
	int m_zipTimestampMatches;
	int m_exactMatches;
	int m_zipIgnored;
	int m_datIgnored;
	CString m_SourcePath;		// path of files we're scanning
	CString m_spamFile;			
	bool m_autoAdd;
	bool m_autoAddScan;
	int scanAZipFile(CString sourceFile, int &matches, int &missing, int &unknown, int &flags);
	void zip2map(CString zipFilename);
	void zip2map2(CString zipFilename);
	void sortTopTen();
	int scanCRCs(unsigned int crc, CString romName, unsigned int size);
	int addtoTop10(CString zipName);
	int m_scannedZipFileCount;
	void initTopTen(void);
	int findDatMatch(unsigned long long crcsize, CStringW romName);
	int addtoTop102(int datPtr);
	void zip2_crcmap(CString zipFilename, DATFILESTRUC &zipMap);
	// column widths for left and right panels
// Color,0; Filename,150; New Filename,200; Score,50
// Color,0; name,80; type,40; size,70; date,70; crc,70
typedef struct _COLUMNWIDTHS	
{
	unsigned int Lcolor;
	unsigned int Lfilename;
	unsigned int Lnewfilename;
	unsigned int Lscore;
	unsigned int Lmatched;
	unsigned int Lmissing;
	unsigned int Lunknown;
	unsigned int Rcolor;
	unsigned int Rname;		
	unsigned int Rtype;
	unsigned int Rsize;
	unsigned int Rdate;
	unsigned int Rcrc;
	unsigned int LViewName;

	_COLUMNWIDTHS()		// set default values.  these are pulled from teh registry and overridden if they have been changed.
	{
		Lcolor = 0;
		Rcolor = 0;
		Lfilename = 150;
		Lnewfilename = 200;
		Lmatched = 30;
		Lmissing = 30;
		Lunknown = 30;
		Lscore = 40;
		Rname = 80;
		Rtype = 40;
		Rsize = 70;
		Rdate = 70;
		Rcrc = 70;
		LViewName = 150;
	};
} datEntry; //TODO- why is this called this?
	
	afx_msg void OnBnClickedApplyignore();
	afx_msg void OnBnClickedHidealts();
	afx_msg void OnBnClickedMissinglist();
	afx_msg void OnBnClickedDatzipflip();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedHidematches();
	afx_msg void OnBnClickedHidemissing();
	afx_msg void OnBnClickedHideunknown();
	bool m_datview;
	int m_autoAddIndex;
	CString m_updatePackSourcePath;
	afx_msg void OnBnClickedZipfolderbrowsebtn();

	CString findRootZipFolder();
	CString extractExtension(CStringW &filename);
	void updateZipList();
	void scanAllFiles();
	bool mergeZipFiles();
	bool skipafile(CStringW filename, CString ext, unsigned int crc, unsigned int fileSize);
	void add2database();
	void add2datFile();
	void renameZipFileWithPopUpDialog();
	bool unzipAFile(CStringW zipFilename, CStringW destination);
	bool zipAFile(CStringW zipFilename, CStringW sourceFolder);
	CString m_buriedZipFolder;
	void moveSelectedFiles(CString tgtPath);
	void checkDatvsZip(int zipPos, DATFILESTRUC &zipMap, bool cleanup);
	void compareDat2Zip(int zipPos, int m_nFileCount, bool cleanup);
	void OnLvnKeydownLeftsidelist(NMHDR *pNMHDR, LRESULT *pResult);
	void OnLvnKeydownZipdetailslist(NMHDR *pNMHDR, LRESULT *pResult);
	void CopyToClipboard(const char* stringbuffer);
	void OnNMDblclkDatFileList(NMHDR *pNMHDR, LRESULT *pResult);
	void hexViewFile();
	void add2Ignore();
	void unNestZipFile();
	void renameFileInZip();
	void deleteFileFromZip();
	void extractHere();
	bool m_minimized;
	BOOL DestroyWindow();
	bool make_diz(CStringW filename, dizInfo &m_dizInfo);
	bool checkForDiz(CStringW filename);
	afx_msg void OnBnClickedDatquery();
	bool is_utf8(const char* string);
	bool add2zip(CStringW zipFilename, CString targetFile, bool storeFolder=false);
	bool file2DB(CStringW filename);
	bool moveZipIntoTDCArchive(CStringW filename);
	void compareFile2db(CStringW filename, bool nameOnly);
	float compareStrings(CString a, CString b);
	void loadRegistrySettings();
	void zip2NewFile();
	void addFile2Spam();
	LRESULT  OnDropFiles(WPARAM wParam, LPARAM lParam);
	void OnUpdateMyMenuItem(CCmdUI *pCmdUI);
	CString status_to_color(int status);
	void displayZipFileInfoText();
};
