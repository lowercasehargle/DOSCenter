/* DOSCenterDlg.cpp : implementation file

DOSCenter - a zip file scanner and renamer tool.

 jeff@silent.net - if you're looking to help/improve/take over this project, please contact me.  i have many more ideas.
 
 Similar to ROMCenter in that it can take a bunch of zip files, compare them against a .dat file and suggest to rename the top level (.zip) filename if a match occurrs.
 Better than ROMCenter (in this application) in that it doesn't get tricked by seeing the same file inside multiple zips and thinking they are all the same.
 Eg, gwbasic.exe is inside many early games.  ROMCenter finds the 1st instance of gwbasic.exe in the .dat file and assumes all games are to be renamed as the first .dat instance.
 DOSCenter looks through every entry in the .dat file and tries to find the best match against the target .zip file.
  
 DOSCenter also has other tools to assist in organizing and naming a DOS collection, including renaming files by hand, a zip merge utility, and a built in file viewer.


 we use CMaps for storing each entry from the .dat file.  i am particularly cmap dumb, so my implementation is likely very poor and memory wasting.

stuff to add:


?done? fix next/prev compare buttons, show where data miscompares
inject info from notes page into .dat file?
dcnotice.ini - files contained in here are to be flagged for attention. 
better .dat file configuration system.  method to view .dat file header from main dialog.  maybe an info block at the bottom of the list controls.
better organization of the GUIs.  settings dialog is a mess, some stuff must be set there before you can create a .dat file.
better color organization of left side panel.
.dat file editor - could easily switch to displaying the top level names as "zip files" on the left, and showing the contents of the cmap on the right. - partially implemented now.
show missing files on left side (ie, an incomplete set of zips)
allow alternate (pklite, lz91'ed) files inside the same zip

possible conditions:
file is unscanned = black
file is recognized, missing some files = red
file is recognized, additional files = purple
file is recognized, filename is wrong = brown
file is perfect, contents and name and exact matches = green.
both missing and extra files = blue


inside the zip:
file is unknown = black
file is missing = red
file is ignored = gray
filename mismatch, CRC match, alternate = brown
file is perfect = green

in dat view:
file is black = normal
file is brown = torrentzipped.


June 2017 - new scoring technique

instead of using the count of matching CRCs in a zip against the dat, use the sum of the total number of bytes in the files against how many bytes are in the .dat file.

For a perfectly matching zip against the dat, the matching size should equal the total number of bytes in the .dat entry.
- If a (1mb) zip has an additional (2k) BBS ad, but all the other files match the dat, the total score would be very close to 99%.  This gives more weight to game files.
- If a game with only 2 files total in it (an early 80s game), and game.exe matches, but hiscore.dat does not, the percentage will still be high, instead of 50% if only
file match counts are used.
- If a game is missing 



*/

#include "stdafx.h"
#include "DOSCenter.h"
#include "DOSCenterDlg.h"
#include "fileStuff.h"
#include "SettingsandTools.h"
#include "ScoringSetupDlg.h"
#include "registry.h"
#include "easysize.h"
#include "progressDlg.h"
#include "NewFilenameDlg.h"
#include "PopupViewerTxtDlg.h"
#include "popupviewerdlg.h"
#include "HaveMissDlg.h"
#include "datQueryDlg.h"
#include "dizInfo.h"
#include "FileIDDizEditDlg.h"
#include <string>
#define _ZIP_IMPL_MFC
#include <ziparchive.h>
#include "similar.h"
#include "PickListDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDOSCenterDlg dialog




CDOSCenterDlg::CDOSCenterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDOSCenterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDOSCenterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LEFTSIDELIST, theApp.m_leftSideList);
	DDX_Control(pDX, IDC_ZIPDETAILSLIST, theApp.m_zipDetailsList);
	DDX_Control(pDX, IDC_LEFTSIDEDATLIST, theApp.m_leftSideDATList);
}

BEGIN_MESSAGE_MAP(CDOSCenterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ZIPFOLDERBROWSEBTN, OnBnClickedZipfolderbrowsebtn)
	ON_NOTIFY(NM_DBLCLK, IDC_LEFTSIDELIST, OnNMDblclkZipFileList)
	ON_NOTIFY(NM_DBLCLK, IDC_ZIPDETAILSLIST, OnNMDblclkZipList)
	ON_NOTIFY(NM_DBLCLK, IDC_LEFTSIDEDATLIST, OnNMDblclkDatFileList)
	ON_NOTIFY(NM_RCLICK, IDC_LEFTSIDELIST, OnNMRclickLeftSideList)
	ON_NOTIFY(NM_RCLICK, IDC_ZIPDETAILSLIST, OnNMRclickRightSideList)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_HIDEMATCHES, OnBnClickedHidematches)
	ON_BN_CLICKED(IDC_HIDEMISSING, OnBnClickedHidemissing)
	ON_BN_CLICKED(IDC_HIDEUNKNOWN, OnBnClickedHideunknown)
	ON_NOTIFY(LVN_KEYDOWN, IDC_ZIPDETAILSLIST, OnLvnKeydownZipdetailslist)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LEFTSIDELIST, OnLvnKeydownLeftsidelist)
	ON_BN_CLICKED(IDC_SETTINGSBTN, OnBnClickedSettingsbtn)
	ON_BN_CLICKED(IDC_APPLYIGNORE, OnBnClickedApplyignore)
	ON_BN_CLICKED(IDC_HIDEALTS, OnBnClickedHidealts)
	ON_MESSAGE(WM_DROPFILES, OnDropFiles)
	ON_BN_CLICKED(IDC_DATZIPFLIP, OnBnClickedDatzipflip)
	ON_BN_CLICKED(IDC_DATQUERY, OnBnClickedDatquery)
END_MESSAGE_MAP()

// royal PITA for resizing the windows based on the splitter and dialog frame.  
// no other controls seem to work at all for moving their positions, and it flickers like a mofo.
//
BEGIN_EASYSIZE_MAP(CDOSCenterDlg) // ID, left, top, right, bottom
	EASYSIZE(IDC_ZIPDETAILSLIST,IDC_SPLITTER1,IDC_FILENAMETXT,ES_BORDER,IDC_HIDEMATCHES,0)
END_EASYSIZE_MAP

void CDOSCenterDlg::OnSize(UINT nType, int cx, int cy) 
{
	UPDATE_EASYSIZE;
    CDialog::OnSize(nType, cx, cy);
	CRect rcZip, rcLeft;	// keep the splitter inside the window
	CWnd* pWndZip;
	CWnd* pWndLeft;
	pWndZip = GetDlgItem(IDC_ZIPDETAILSLIST);
	pWndLeft = GetDlgItem(IDC_LEFTSIDELIST);
	if (pWndZip)
	{
		pWndZip->GetWindowRect(rcZip);
		pWndLeft->GetWindowRect(rcLeft);
		m_wndSplitter1.SetRange(rcLeft.left, rcZip.right-25);
	}
}

// keep app from closing upon hitting the enter key.
void CDOSCenterDlg::OnBnClickedOk()
{
	// T0DO: Add your control notification handler code here
	//OnOK();
}

// checkboxes for hiding files in the zip listing window (right side)
void CDOSCenterDlg::OnBnClickedHidematches()
{
	OnNMDblclkZipFileList(NULL,NULL);
}
void CDOSCenterDlg::OnBnClickedHidemissing()
{
	OnNMDblclkZipFileList(NULL,NULL);
}

void CDOSCenterDlg::OnBnClickedHideunknown()
{
	OnNMDblclkZipFileList(NULL,NULL);
}

void CDOSCenterDlg::OnBnClickedHidealts()
{
	OnNMDblclkZipFileList(NULL,NULL);
}


LRESULT CDOSCenterDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_NOTIFY)
	{
		if (wParam == IDC_SPLITTER1) // moving the splitter?
		{	
				SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
				DoResize1(pHdr->delta);
		}
	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CDOSCenterDlg::DoResize1(int delta)
{
	CSplitterControl::ChangeWidth(&theApp.m_leftSideList, delta);
	CSplitterControl::ChangeWidth(&theApp.m_leftSideDATList, delta);
	CSplitterControl::ChangeWidth(&theApp.m_zipDetailsList, -delta, CW_RIGHTALIGN);
//	CSplitterControl::ChangeWidth(&m_txtContent, -delta, CW_RIGHTALIGN);
//	CSplitterControl::ChangeWidth(&m_wndSplitter2, -delta, CW_RIGHTALIGN);
	Invalidate();
	UpdateWindow();
}

// CDOSCenterDlg message handlers
void CDOSCenterDlg::OnCancel() 
{
	CSettingsandTools settings;
	CProgressDlg progDlg;

	CString txt;
	theApp.m_tmpWorkingFolder.TrimRight(L"\\DOSCenter");
	CRegistry regMyReg( CREG_CREATE );  // create registry key if it doesn't exist.
	if ( regMyReg.Open(L"Software\\DOSCenter",HKEY_CURRENT_USER) ) 
		{
			regMyReg[L"workingFolder"] = theApp.m_tmpWorkingFolder;
			regMyReg[L"collectionPath"] = theApp.m_collectionPath;
			regMyReg[L"defaultMovePath"] = theApp.m_defaultMovePath;
			regMyReg[L"DOSBoxPath"] = theApp.m_dosboxPath;

			// save user prefs
			if ( theApp.m_autoLoadDat ) 
				regMyReg[L"autoLoadDAT"] = L"1";
			else
				regMyReg[L"autoLoadDAT"] = L"0";


//			if ( theApp.m_ScanCRCFilename ) 
//				regMyReg["CRCScanOnly"] = "1";
//			else
//				regMyReg["CRCScanOnly"] = "0";



			if ( ((CButton*)GetDlgItem(IDC_RECURSIZESCAN))->GetCheck() == 1)
                regMyReg[L"recursiveZipScan"] = L"1";
			else
                regMyReg[L"recursiveZipScan"] = L"0";

			if (theApp.m_detailedReport == true)
                regMyReg[L"detailedReport"] = L"1";
			else
                regMyReg[L"detailedReport"] = L"0";

			txt.Format(L"%d", theApp.m_minScoreForAutoRename);
			regMyReg[L"autoRenameMinScore"] = txt;

			regMyReg[L"FXP login"] = theApp.m_mySitesDat;
			regMyReg[L"FXP dlpath"] = theApp.m_fxpDlPath;

			regMyReg.Close();
		}

	progDlg.Init(PROG_MODE_TIMER, 1000, L"Cleaning up...",L"Flushing .dat entries out of memory");

	settings.deleteKeys();
	progDlg.DestroyWindow();
//	if (theApp.m_fileData->fileCount != 0)
//			delete[] theApp.m_fileData;	// delete existing cmap and crap under it.
	CDialog::OnCancel();

}

BOOL CDOSCenterDlg::OnInitDialog()
{
	CFileStuff files;
	CScoringSetupDlg setupDlg;
	CDialog::OnInitDialog();
	CSettingsandTools settings;
	DragAcceptFiles(TRUE); // To Accept Dropped file Set this TRUE

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	theApp.m_DATfileLoaded = false;
	theApp.m_reloadIgnore = false;
	theApp.m_MDBfileLoaded = false;
	theApp.m_recursiveScan = false;
	theApp.m_detailedReport = false;

	m_minimized=false;
	
	loadRegistrySettings();

	((CButton*)GetDlgItem(IDC_RECURSIZESCAN))->SetCheck(theApp.m_recursiveScan);

	if (theApp.m_dosboxPath.GetLength() == 0)
		theApp.m_dosboxPath = "c:\\";
	if (theApp.m_defaultMovePath.GetLength() == 0)
		theApp.m_defaultMovePath = "c:\\";
	if (theApp.m_tmpWorkingFolder.GetLength() == 0)
		theApp.m_tmpWorkingFolder = "c:\\temp";
	theApp.m_tmpWorkingFolder +="\\DOSCenter";

	((CButton*)GetDlgItem(IDC_ZIPFOLDERTXT))->SetWindowText(L"Zip folder: "+m_SourcePath);

	// clean up any leftover unzips
	CreateDirectory(theApp.m_tmpWorkingFolder, NULL); // just in case
	if (!files.DeleteDirectory(theApp.m_tmpWorkingFolder))
		AfxMessageBox(L"Warning, unable to clean up previous DOSCenter temporary working folder.\r\n"+theApp.m_tmpWorkingFolder , MB_ICONWARNING, 0);

	CreateDirectory(theApp.m_tmpWorkingFolder, NULL); // create it again, so it's clean.

	// Restore Window-Position

	WINDOWPLACEMENT *lwp;
	UINT nl;

	if(AfxGetApp()->GetProfileBinary(L"DOSCenterMAIN", L"WP", (LPBYTE*)&lwp, &nl))
	{
		lwp->showCmd=SW_SHOWNORMAL; // <<-my magic line of code Smile
		SetWindowPlacement(lwp);
	}

	// draw a splitter
	CWnd* pWnd;
	CRect rc;

	pWnd = GetDlgItem(IDC_SPLITTER1);
	pWnd->GetWindowRect(rc);
	ScreenToClient(rc);
	m_wndSplitter1.Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_SPLITTER1);
	m_wndSplitter1.SetRange(450, 1000, -1);

	// move the splitter into restored position.  all we care about is the left position.
	if(AfxGetApp()->GetProfileBinary(L"DOSCenterSPLIT", L"SPLIT", (LPBYTE*)&lwp, &nl))
	{
		m_wndSplitter1.SetWindowPlacement(lwp);
		
		DoResize1(lwp->rcNormalPosition.left - 480);	// this is a total guess.  i'm sure if you change the splitter "setrange" this needs to be updated too.
	}

	// create our columns in each window pane
	// color column is invisible to the user, but controls the text color displayed
	theApp.m_leftSideList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	theApp.m_leftSideList.SetHeadings( _T("Color,0; Filename,150; New Filename,200; Score,40; ==,30; xx,30; ??,30;") );
	theApp.m_leftSideList.LoadColumnInfo();

	theApp.m_zipDetailsList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	theApp.m_zipDetailsList.SetHeadings( _T("Color,0; name,80; type,40; size,70; date,70; crc,70") );
	theApp.m_zipDetailsList.LoadColumnInfo();

	theApp.m_leftSideDATList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	theApp.m_leftSideDATList.SetHeadings( _T("Color,0; name,150; count,40") );
	theApp.m_leftSideDATList.LoadColumnInfo();
	theApp.m_leftSideDATList.ShowWindow(0);
	m_datview = false;
	theApp.m_datParsed = false;
	theApp.m_processMode = false;


	_COLUMNWIDTHS *columns;
	if(AfxGetApp()->GetProfileBinary(L"DOSCenterCOLS", L"COLS", (LPBYTE*)&columns, &nl))
	{
		theApp.m_zipDetailsList.SetColumnWidth(COLORCOL, columns->Rcolor);
		theApp.m_zipDetailsList.SetColumnWidth(1, columns->Rname);	
		theApp.m_zipDetailsList.SetColumnWidth(2, columns->Rtype);	
		theApp.m_zipDetailsList.SetColumnWidth(3, columns->Rsize);	
		theApp.m_zipDetailsList.SetColumnWidth(4, columns->Rdate);	
		theApp.m_zipDetailsList.SetColumnWidth(5, columns->Rcrc);	

		theApp.m_leftSideList.SetColumnWidth(COLORCOL, columns->Lcolor);
		theApp.m_leftSideList.SetColumnWidth(FILENAMECOL, columns->Lfilename);
		theApp.m_leftSideList.SetColumnWidth(NEWFILENAMECOL, columns->Lnewfilename);
		if (columns->Lscore > 1000)
			columns->Lscore = 40;
		theApp.m_leftSideList.SetColumnWidth(SCORECOL, columns->Lscore);		

		if (columns->Lmatched > 1000)
			columns->Lmatched = 40;
		theApp.m_leftSideList.SetColumnWidth(MATCHCOL, columns->Lmatched);

		if (columns->Lmissing > 1000)
			columns->Lmissing = 40;
		theApp.m_leftSideList.SetColumnWidth(MISSCOL, columns->Lmissing);

		if (columns->Lunknown > 1000)
			columns->Lunknown = 40;
		theApp.m_leftSideList.SetColumnWidth(UNKCOL, columns->Lunknown);
		
		// dat view
		if (columns->LViewName > 1000)
			columns->LViewName = 150;
		theApp.m_leftSideDATList.SetColumnWidth(1, columns->LViewName);
	}

	if (m_SourcePath.GetLength() > 0)
		updateZipList();

	setupDlg.populateIgnoreList();
	theApp.m_ignoreOn = true;
	((CButton*)GetDlgItem(IDC_APPLYIGNORE))->SetCheck(theApp.m_ignoreOn);

	theApp.m_datfileCount = 0;

	if (theApp.m_autoLoadDat)
		settings.parseDATFile();

	//DragAcceptFiles(TRUE); // To Accept Dropped file Set this TRUE

	m_autoAdd = false;
	m_autoAddScan = false;	// auto-add scanning in process
	m_spamFile = L"E:\\FTP site\\DOSGuy uploads\\what to do with these\\virused games\\Junk Files (xxxx)(Intros and loaders and junk) [DELETE DELETE].zip";
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDOSCenterDlg::loadRegistrySettings()
{
	CStringW tmp;
	// dig out some registry stuff
	CRegistry regMyReg(NULL);  // No special flags
	if (regMyReg.Open(L"Software\\DOSCenter", HKEY_CURRENT_USER))
	{
		// get the data
		m_SourcePath = regMyReg[L"zipFolders"];
		theApp.m_DATPath = regMyReg[L"datFolders"];
		theApp.m_collectionPath = regMyReg[L"collectionPath"];
		theApp.m_tmpWorkingFolder = regMyReg[L"workingFolder"];
		theApp.m_defaultMovePath = regMyReg[L"defaultMovePath"];
		theApp.m_dosboxPath = regMyReg[L"DOSBoxPath"];
		theApp.m_mySitesDat = regMyReg[L"FXP login"];
		if (theApp.m_mySitesDat.GetLength() == 0)
			theApp.m_mySitesDat = L"n/a";
		theApp.m_fxpDlPath = regMyReg[L"FXP dlpath"];
		if (theApp.m_fxpDlPath.GetLength() == 0)
			theApp.m_fxpDlPath = L"c:\\";
		tmp = regMyReg[L"autoLoadDAT"];
		theApp.m_autoLoadDat = 0;
		if (tmp == L"1")
			theApp.m_autoLoadDat = 1;

		tmp = regMyReg[L"autoRenameMinScore"];
		if (tmp.GetLength() == 0)
			tmp = "100";
		swscanf_s(tmp, L"%d", &theApp.m_minScoreForAutoRename);

		if (regMyReg[L"recursiveZipScan"] == L"1")
			theApp.m_recursiveScan = true;

		if (regMyReg[L"detailedReport"] == L"1")
			theApp.m_detailedReport = true;


		//	tmp = regMyReg["CRCScanOnly"];
		//	theApp.m_ScanCRCFilename = true;
		//	if (tmp == "0") 
		//		theApp.m_ScanCRCFilename = false;


		// Close the open key
		regMyReg.Close();
	}
	theApp.m_MDBPath = L"E:\\FTP site\\DOSGuy uploads\\tdc.mdb"; //TODO- move this into registry
}

BOOL CDOSCenterDlg::DestroyWindow() 
{
    _COLUMNWIDTHS columns;
	WINDOWPLACEMENT wp;
    GetWindowPlacement(&wp);
	AfxGetApp()->WriteProfileBinary(L"DOSCenterMAIN", L"WP", (LPBYTE)&wp, sizeof(wp));

	// save the splitter position
	m_wndSplitter1.GetWindowPlacement(&wp);
	AfxGetApp()->WriteProfileBinary(L"DOSCenterSPLIT", L"SPLIT", (LPBYTE)&wp, sizeof(wp));
	
	// save the column widths
	columns.Rcolor = theApp.m_zipDetailsList.GetColumnWidth(COLORCOL);
	columns.Rname = theApp.m_zipDetailsList.GetColumnWidth(1);	
	columns.Rtype = theApp.m_zipDetailsList.GetColumnWidth(2);	
	columns.Rsize = theApp.m_zipDetailsList.GetColumnWidth(3);	
	columns.Rdate = theApp.m_zipDetailsList.GetColumnWidth(4);	
	columns.Rcrc = theApp.m_zipDetailsList.GetColumnWidth(5);	

	columns.Lcolor = theApp.m_leftSideList.GetColumnWidth(COLORCOL);
	columns.Lfilename = theApp.m_leftSideList.GetColumnWidth(FILENAMECOL);
	columns.Lnewfilename = theApp.m_leftSideList.GetColumnWidth(NEWFILENAMECOL);
	columns.Lscore = theApp.m_leftSideList.GetColumnWidth(SCORECOL);
	columns.Lmatched = theApp.m_leftSideList.GetColumnWidth(MATCHCOL);
	columns.Lmissing = theApp.m_leftSideList.GetColumnWidth(MISSCOL);
	columns.Lunknown = theApp.m_leftSideList.GetColumnWidth(UNKCOL);
	columns.LViewName = theApp.m_leftSideDATList.GetColumnWidth(1);	//dat view name column

	AfxGetApp()->WriteProfileBinary(L"DOSCenterCOLS", L"COLS", (LPBYTE)&columns, sizeof(columns));    
	return CDialog::DestroyWindow();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CDOSCenterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDOSCenterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// get the source zip files we want to interrogate
void CDOSCenterDlg::OnBnClickedZipfolderbrowsebtn()
{
	CFileStuff fileStuff;
	// open a "browse to folder" dialog
	if (fileStuff.UserBrowseFolder(m_SourcePath, L"Where's your ZIP collection?") != TRUE)
		return;
	if (m_SourcePath.Right(1) != L"\\")
		m_SourcePath+=L"\\";

	updateZipList();
	if ( ((CButton*)GetDlgItem(IDC_AUTOSCAN))->GetCheck() )
		scanAllFiles();

	CRegistry regMyReg( CREG_CREATE );  // create registry key if it doesn't exist.
	if ( regMyReg.Open(L"Software\\DOSCenter",HKEY_CURRENT_USER) ) 
		{
			// Now set the new values
			regMyReg[L"zipFolders"] = m_SourcePath;
			regMyReg.Close();
		}
	CString txt;
	txt.Format(L"Zip folder:"+m_SourcePath+",%d files",m_zipCount);
	((CButton*)GetDlgItem(IDC_ZIPFOLDERTXT))->SetWindowText(txt);
}

//refresh the left side pane with all the zip files we're looking at.
void CDOSCenterDlg::updateZipList()
{
	CString filename;
	theApp.m_leftSideList.DeleteAllItems();
	m_zipCount = 0;
	bool recurse;
	recurse = ( ((CButton*)GetDlgItem(IDC_RECURSIZESCAN))->GetCheck() == 1);

	// now we have the path+filename we want to work with.
	// scan all the files in this selected folder.
	m_zipDir.ClearDirs();
	m_zipDir.ClearFiles();
	// look in the user folder
	m_zipDir.GetDirs(m_SourcePath, recurse);


   ///////////////////////////////////////////////////
   // get all files in the dirs we found above
   m_zipDir.GetFiles(L"*.zip");

   // sort them by name, ascending
   m_zipDir.SortFiles(CSADirRead::eSortAlpha, false);
   CSADirRead::SAFileVector &files = m_zipDir.Files();
   m_zipFiles = files.begin();

   // list all files on the left side
   while ( m_zipFiles != files.end())
   {
		filename = (*m_zipFiles).m_sName;
		filename.Replace(m_SourcePath,L"");
		
		theApp.m_leftSideList.AddItem(COLOR_BLACK, filename, "", "", "", "", "");	// column 0 is color, zipfilename, new filename, score, match, missing, unknown
		m_zipFiles++;	// next file please
		m_zipCount++;
   }
}


void CDOSCenterDlg::sortTopTen()
{
  int i, j;
  int temp;
  CString stemp;
  float ftemp;
#if 0
  for (int ii=0; ii<TOPARRAYSIZE; ii++)
	{
		if (m_topTen[ii].filename.IsEmpty())
			break;
		// 4 elements are used: # of perfect matches in this zip against this dat entry
		//                      # of crc matches in this zip against this dat entry
		//                      # of of roms in the zip vs. the # in the dat entry
		//                      # of bytes matching the .dat file
			// now 2 elements - zipscore and bytematchcount

		//float score=(float)m_topTen[ii].zipScore; // = (float)m_topTen[ii].crcMatch * 0.65;
	
		//!float score = ((float)m_topTen[ii].perfects * (float)m_topTen[ii].zipScore); 
		//ftemp = (float)m_topTen[ii].datCount / (float)m_topTen[ii].perfects;
		//score *=(float)m_topTen[ii].zipScore;
			// zipscore is the number of files in this zip vs the number in the dat we've picked.  it can never be >1.0
			// this helps weed out compilations which are supersets of individual games. 
	//		float zipscore = (float)(m_topTen[m_topTenIdx].datCount / (float)m_topTen[m_topTenIdx].zipCount);
	//		if (zipscore > 1.0)	// 1/x function
	//			zipscore = ((float)m_topTen[m_topTenIdx].zipCount / (float)m_topTen[m_topTenIdx].datCount);
	//		m_topTen[m_topTenIdx].zipScore = zipscore;

	//	float bytescore = (float)m_topTen[ii].byteMatchCount / (float)m_topTen[ii].datSizeCount;
	//	if (bytescore > (float)1.0)
	//		bytescore = (float)m_topTen[ii].datSizeCount / (float)m_topTen[ii].byteMatchCount;
	//	score+=bytescore;
	//	score = score/(float)2.000;

	}
#endif
  // sort it by score, if we have a match, sort by perfects
  for (i =0; i<TOPARRAYSIZE; i++)		// yep this is a bubble sort.
  {

	for (j= i+1; j<TOPARRAYSIZE; j++)
    {
			if ( (m_topTen[i].score) < (m_topTen[j].score) )
			{
				temp = m_topTen[i].score;
				m_topTen[i].score = m_topTen[j].score;
				m_topTen[j].score = temp;

				ftemp = m_topTen[i].zipScore;
				m_topTen[i].zipScore = m_topTen[j].zipScore;
				m_topTen[j].zipScore = ftemp;

				stemp = m_topTen[i].filename;
				m_topTen[i].filename = m_topTen[j].filename;
				m_topTen[j].filename = stemp;

				temp = m_topTen[i].zipPos;
				m_topTen[i].zipPos = m_topTen[j].zipPos;
				m_topTen[j].zipPos = temp;

				temp = m_topTen[i].perfects;
				m_topTen[i].perfects = m_topTen[j].perfects;
				m_topTen[j].perfects = temp;

				//temp = m_topTen[i].crcMatch; //unused
				//m_topTen[i].crcMatch = m_topTen[j].crcMatch;
				//m_topTen[j].crcMatch = temp;

				//temp = m_topTen[i].dupes; // unused
				//m_topTen[i].dupes = m_topTen[j].dupes;
				//m_topTen[j].dupes = temp;

				temp = m_topTen[i].datCount;
				m_topTen[i].datCount = m_topTen[j].datCount;
				m_topTen[j].datCount = temp;

				//temp = m_topTen[i].byteMatchCount;
				//m_topTen[i].byteMatchCount = m_topTen[j].byteMatchCount;
				//m_topTen[j].byteMatchCount = temp;

				//temp = m_topTen[i].datSizeCount;
				//m_topTen[i].datSizeCount = m_topTen[j].datSizeCount;
				//m_topTen[j].datSizeCount = temp;
			}
    }
  }

}



// scan an entire folder worth of files
void CDOSCenterDlg::scanAllFiles()
{
	for	(int i=0; i<theApp.m_leftSideList.GetItemCount(); i++)
		theApp.m_leftSideList.SetItemState(i, LVNI_SELECTED,LVIS_SELECTED);

	scanSelectedZips();
	theApp.m_leftSideList.SetFocus();
	for	(int i=0; i<theApp.m_leftSideList.GetItemCount(); i++)
		theApp.m_leftSideList.SetItemState(i, 0, LVIS_SELECTED);

	theApp.m_leftSideList.SetItemState(0, LVNI_SELECTED, LVNI_SELECTED);	
}
/////////////////////////////////////////////////////////////////////////////////////////
// ZIP SCANNING/SCORING
// top level file scanning/scoring routine
//file is unscanned = black
//file is recognized, missing some files = red
//file is recognized, additional files = purple
//file is recognized, filename is wrong = brown
//file is perfect, contents and name and exact matches = green.
//matched file in database is torrentzipped, all colors will be slightly off
//
// this routine scans the zip and then updates the screen with the results+score info
// input: index# of file in zip list to scan

void CDOSCenterDlg::scanThisZip(int nIndex, CProgressDlg* progDlg)
{
	CString tmp, color;
	int matches=0;
	int missing=0;
	int unknown=0;
	int score=0;
	int flags=0;
	
	if (progDlg != NULL)
		progDlg->UpdateText(L"Scanning files...", theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL));

	// scan it, get the results
	score = scanAZipFile(theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL), matches, missing, unknown, flags);
	tmp.Format(L"%d",score);
	theApp.m_leftSideList.SetItemText(nIndex,SCORECOL, tmp);
	tmp.Format(L"%d",(int)matches);	
	theApp.m_leftSideList.SetItemText(nIndex,MATCHCOL, tmp);
	tmp.Format(L"%d", missing);
	theApp.m_leftSideList.SetItemText(nIndex,MISSCOL, tmp);
	tmp.Format(L"%d", unknown);
	theApp.m_leftSideList.SetItemText(nIndex,UNKCOL, tmp);
				

	if (m_autoAdd)
	{
		if (score < theApp.m_minScoreForAutoRename)
		{
			m_autoAddIndex = nIndex;
			OnNMDblclkZipFileList(0, 0);
			add2database();
		}
	}



	// any kind of match with the database?  Add the suggested name.
	if (score > 10) //matches > 0.0)
	{
		theApp.m_leftSideList.SetItemText(nIndex,NEWFILENAMECOL,theApp.extractFilenameWOPath(m_topTen[0].filename));
	
		// all files good, just top level filename is wrong, color is brown
		if ( (missing == 0) && (unknown == 0) && ( score < 100) && ( theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL) != theApp.m_leftSideList.GetItemText(nIndex,NEWFILENAMECOL) ) )
			color = COLOR_YELLOW; 

		if ( (missing >0) && (unknown == 0) ) // missing some files, but no unknown files
			color = COLOR_RED; 

		if ( (missing ==0) && (unknown >0 ) ) // missing no files, additional files in the zip
			color = COLOR_PURPLE; 
	
		if ( (missing >0) && (unknown >0 ) ) // missing files, and additional files in the zip  totally fubared.  
			color = COLOR_BLUE; 
		
		// get rid of path in filename
		CString tFilename;
		tFilename = theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL);
		if (tFilename.Find('\\') != -1) 
			tFilename = tFilename.Right(tFilename.GetLength() - (tFilename.ReverseFind('\\')) -1 );

		if ( (missing == 0) && (unknown == 0) && (score == 100) )
		{
			if ( tFilename == theApp.m_leftSideList.GetItemText(nIndex,NEWFILENAMECOL))
			{
				theApp.m_leftSideList.SetItemText(nIndex,NEWFILENAMECOL,L"perfect!");
				color = COLOR_GREEN_PERFECT; //theApp.m_leftSideList.SetItemText(nIndex,COLORCOL,COLOR_GREEN_PERFECT);
			}
			else
				color = COLOR_BROWN;
		}

		// if the file in our collection is torrentzipped (we hate those)
		// change the color slightly to let the user know our in-database file could be updated/replaced.
		// get the name of the zip file that the scanner is suggesting we use.
		CString zipFilename = m_topTen[0].filename;	// dat filename
		// scan through the cmap to find this particular entry - fixme - could the iterator be passed in the topTen[0] array?
	/*	theApp.m_datZipListit = m_topTen[0].zipPos; theApp.m_datZipList.begin();
		while (theApp.m_datZipListit != theApp.m_datZipList.end())
			{
				if (zipFilename == theApp.m_datZipListit->second.filename)
					break;
				theApp.m_datZipListit++;
			}
*/
		if ((theApp.m_datZipList[m_topTen[0].zipPos].status & TZIPPED) == TZIPPED)
		{
			DWORD icolor;
			swscanf_s(color, L"%x", &icolor); 
			icolor |=COLOR_TZIPPED;
			color.Format(L"0x%x",icolor);
				
		}


	}
	else // no matches at all (score less than 10)
	{
		theApp.m_leftSideList.SetItemText(nIndex,NEWFILENAMECOL,L"no good matches");
		color = COLOR_BLACK;
	}

	if (color != COLOR_BLACK)
	{
		CString datPos;
		datPos.Format(L"%x",m_topTen[0].zipPos);
		color += L"|"+datPos;
	}
	theApp.m_leftSideList.SetItemText(nIndex,COLORCOL,color);
}
void CDOSCenterDlg::initTopTen(void)
{
	m_topTenIdx = 0;	// flush out the topten
	for (int i = 0; i<TOPARRAYSIZE; i++)
	{
		m_topTen[i].filename = "";
		m_topTen[i].score = 0.0;
		m_topTen[i].zipPos = -1; //.datPtr = -1; // these are filled in by entering new items.
		//	m_topTen[i].dupes =0;
		//m_topTen[i].perfects = 0;
		//m_topTen[i].crcMatch = 0;
		//	m_topTen[i].byteMatchCount=0;
	}
}
// compares all CRCs in a given zip file against the .dat file.
// input: path+filename of zip to check
// output: topTen list populated with best candidates for this zip
/*
TODO - this description is out of date.
	scanning - compare the contents of a zip file against CRCs in the .dat file.

	We scan based on CRCs. When a .dat file is loaded, we populate a CMAP of CRCs, which point back to the .dat entry they belong to.
	
	All we want to do here is find the .dat file entry that has the most CRCs from this zip pointing to it.
	
	1) CRC Collisions are ignored. (same file exists in 2 .dat entries, like a soundcard driver)
	2) we keep a small list (100 entries) where files in the zip are pointing to entries in the .dat.  Typically we will find only 1 entry.

	Once the main .dat entry is located, we then take a closer examination of that .dat entry against the .zip, comments below.

	2928th attempt to figure this out...
	.dat info only has CRC+size and a ptr to the dat file entry.
	first pass of scanning finds the closest dat entry match

	once the best .dat file entry is selected, now .dat file info can be parsed exactly like zip file info.

	lcase filename, hash it so that multiple files in a zip with the same CRC can still be identified.
	to compare zip v. dat, check CRCSIZE if match, spin through collisions looking for filename hash match



*/

int CDOSCenterDlg::scanAZipFile(CString sourceFile, int &matches, int &missing, int &unknown, int &flags)
{

	CString filename, scrc, ext;
	CString zipDir;
	CString tmp;
	CStringW romName;

	unsigned int crc;


	matches = 0;
	missing = 0;
	unknown = 0;
	int datIndex = -1;
	int fileCount;
	int filesize;
	//int skipped=0;
	int collisionCount;
	int datPtr, olddatPtr=0;
	m_exactMatches=0;
	m_zipCRCmatches=0;
	int ignoreBytes=0;
	int files2scan;
	unsigned long long crcsize = 0;
	CSettingsandTools CnT;


	//         datPtr	     info
	std::map<unsigned int, _BESTHITS2> bestPicks;	// storage, make the filename the key
	std::map<unsigned int, _BESTHITS2>::const_iterator bpit;	// an iterator


	// then I can update status on it/roms directly.
	zip2map2(m_SourcePath + sourceFile);

	// for each rom in the actual zip...
	fileCount = theApp.m_zipMap[0].fileCount;
	theApp.m_zipMap[0].it = theApp.m_zipMap[0].ROMmap.begin();
	while (theApp.m_zipMap[0].it != theApp.m_zipMap[0].ROMmap.end())
	{
		filesize = theApp.m_zipMap[0].it->second.size;
		crcsize = filesize;
		crcsize = (unsigned long long)(crcsize << 32);
		crcsize |= theApp.m_zipMap[0].it->second.crc;

		//totalBytes += filesize;
		romName = theApp.m_zipMap[0].it->second.name;


		// is this roms's CRC found anywhere in the dat?
		if (theApp.m_crcMap.count(crcsize) != 0)
		{
			int prevEntry = -1;
			std::vector<unsigned int>::iterator datPtrit;
			datPtrit = theApp.m_crcMap[crcsize].datPtr.begin();
			collisionCount = theApp.m_crcMap[crcsize].collisionCount;
			do 
			{
				datPtr = *datPtrit;
				// if the new datPtr is the same as the old one, that means this CRC collision is inside the same file, so break out of this loop.
				if (datPtr != prevEntry)
				{
					CStringW f = theApp.m_datZipList[datPtr].filename; // debug

					// tag that we've found this rom...
					//theApp.m_zipMap[0].ROMmap[romName].status |= STATUS_CRC_MATCH;

					if (bestPicks.count(datPtr)) // already in the best picks list?
					{
						bestPicks[datPtr].matchCount++;
					}
					else // first entry, pluck a few values from this dat entry.
					{
						bestPicks[datPtr].matchCount = 1;
						bestPicks[datPtr].datSizeCount = theApp.m_datZipList[datPtr].numBytes;
						bestPicks[datPtr].datCount = theApp.m_datZipList[datPtr].fileCount;
						bestPicks[datPtr].byteMatchCount = 0;
					}

					// update byte count tracker for this found object
					bestPicks[datPtr].byteMatchCount += filesize;
				} // this isn't the same dat entry.
				collisionCount--;
				prevEntry = datPtr;
				datPtrit++;
			} while (collisionCount >= 0);
		} // if crc found
		theApp.m_zipMap[0].it++;
	}



	// now build the score for possible matching .dat entry
	float highscore = 0.0;
	bpit = bestPicks.begin();

	int ignoredFiles = theApp.m_zipMap[0].skippedFiles;
	int ignoredBytes = theApp.m_zipMap[0].skippedFilesSize;

	// if any "files" (folders as just folders) were skipped above, remove them from the overall count since these don't exist in the .dat file either.
	//fileCount -= skipped;
	while (bpit != bestPicks.end())
	{
		CStringW f = theApp.m_datZipList[bpit->first].filename;
		float score = (float)(bpit->second.matchCount) / (float)bpit->second.datCount;
		if (score > 1.0)	// 1/x function
			score = (float)bpit->second.datCount / (float)(bpit->second.matchCount);
		
		float score2 = (float)bpit->second.byteMatchCount / (float)bpit->second.datSizeCount;
		if (score2 > 1.0)	// 1/x function
			score2 = (float)bpit->second.datSizeCount / (float)bpit->second.byteMatchCount;
		
		float score3 = (float)bpit->second.byteMatchCount / (float)theApp.m_zipMap[0].totalBytes;
		if (score3 > 1.0)	// 1/x function
			score3 = (float)theApp.m_zipMap[0].totalBytes / (float)bpit->second.byteMatchCount;

		float score4 = (float)bpit->second.datCount / (float)fileCount;
		if (score4 > 1.0)	// 1/x function
			score4 = (float)fileCount / (float)bpit->second.datCount;

		float score5 = (float)bpit->second.matchCount / (float)fileCount;
		if (score5 > 1.0)	// 1/x function
			score5 = (float)fileCount / (float)bpit->second.matchCount;

		score += score2+score3+score4+score5;
		score = (score / (float)5.0) * 100;

		if (score > highscore) 
		{
			highscore = score;
			datIndex = bpit->first;
			matches = bpit->second.matchCount;
			missing = bpit->second.datCount - bpit->second.matchCount;
			unknown = fileCount - bpit->second.matchCount;
		}
		bpit++;
	}



	if (datIndex == -1)
		return 0;


	//CStringW zipName = theApp.m_datZipList[datIndex].filename;
	// we have now checked all the CRCs of the roms in the zip.  our topTen list should have the the .dat entry that this file matches (if any)
	// if we have multiple entries in our top ten, lets first find the best by the number of perfect matches
	// this should put topTen[0] the closest candidate to the zip we just scanned.
//	if (m_topTenIdx > 0)
//		sortTopTen();

//	m_topTen[0].zipCount = zipMap.count;
//	datIndex = m_topTen[0].zipPos;

//	if (datIndex != -1) // got some kind of match?
//	{
//		checkDatvsZip(datIndex, zipMap, true);	// go compare the given dat entry against the zip array (already populated)
//	}

	// save the selected zip filename, in case the user wants to do anything else with it.
	theApp.m_currentlyTaggedZipFile = sourceFile;
	m_topTen[0].filename = theApp.m_datZipList[datIndex].filename;
	m_topTen[0].score = (int)highscore;
	m_topTen[0].zipPos = datIndex;
	


	// ok, we have checked each file in the phsyical zip against the prime .dat entry.
	// We know the following data:
    // # of exact rom matches (crc+filename) in this zip vs. the .dat file [perfects]
	// # of crc matches, but filename mismatches [crcMatch]
	// # of files in the .dat for this entry of the best matches found [datCount]
	// # of files in this zip [zipCount]
	// # of bytes matching in this zip vs. dat entry
	


// sspectre wants the following:
// 1. If both sides have different files (i.e. file have "extra" entries and .dat have "missing" entries), do nothing new.
// 2. If file have extra, and .dat haven't missing entries, score file as 100+%.
// 3. If file haven't extra, but .dat have missing entries, score file as 100%. 
//
		// calculate the results.
		// matched = hits from this entry in the database.  Can be a floating number, since filename+crc must match.
		// missed = number of items in the entry minus the hits
		// unknown = number of files in the zip file minus the hits.

	// score is based on 2 factor
		//  zip filecount / matching files 
		// +dat filecount / matching files

	if (m_topTen[0].score > 10) // anything reportable?
	{
#if 0			
			matches = (m_topTen[0].perfects > m_topTen[0].crcMatch) ? m_topTen[0].perfects : m_topTen[0].crcMatch;	// get the better of the 2 results		

			// if we ignored any files, we have to subtract them from everywhere.
			m_topTen[0].datCount -=m_datIgnored;
			theApp.m_nFileCount -= m_zipIgnored;

			//how many files in this zip matched the dat?
			zipscore = (float)theApp.m_nFileCount / (float)matches; 
			if (zipscore > 1.0)	// 1/x function
				zipscore = ((float)matches / (float)theApp.m_nFileCount);

			//# files in zip - matching files - all skipped files
			int zipExtra = theApp.m_nFileCount-matches;
			if (zipExtra < 0)
			{// kludge! .dat file sometimes has files the zip doesn't, but they are ignored (file_id.diz for example)
				zipExtra = 0;
				matches = theApp.m_nFileCount;
			}
			unknown = zipExtra; //m_nFileCount - matches;  // this is hte same as zipExtra
			

			// how many roms in the zip matched the dat file?
			int datExtra = (m_topTen[0].datCount - matches); 
			if (datExtra < 0)	// kludge!  we can get more matches than entries in the dat when there are files in the ignore list and in the zip that are not in the dat
				datExtra = 0;
			missing = datExtra; //((theApp.m_fileData[m_topTen[0].pos].fileCount) - matches);  // this is the same as datExtra

			datscore = (float)m_topTen[0].datCount / (float)matches;
			if (datscore > 1.0)
				datscore = (float)matches / (float)m_topTen[0].datCount;
			score = (float)(datscore+zipscore) / (float)2.0;
			score *= 100;	// make it 0-100
			
			if ( (zipExtra > 0) && (datExtra==0) )	// case #2 - score file as 100%+
			{
				score = 100;
				score +=zipExtra;
			}

		if (score < 0) // juuuust in case
			score =0;

//			score = m_topTen[0].score * 100;

#endif
		// now examine the physical zip fileNAME we just scanned and see how it compares in the .dat
		// in case we scanned recursively, dump the folder and just dig the file out.
		if (sourceFile.Find('\\') != -1) 
			sourceFile = sourceFile.Right(sourceFile.GetLength() - (sourceFile.ReverseFind('\\')) -1 );

		// pull in the data from the zipArray
		//tempZIPdata = theApp.m_datZipFiles.GetAt(m_topTen[0].zipPos);
		//theApp.m_datZipList[theApp.m_datfileCount]. .filename= zipFilename;

		// perfect match, including zip filename?
		if (m_topTen[0].score == 100)
		{
			if (m_topTen[0].filename == sourceFile)
			{
				DWORD oldSts = theApp.m_datZipList[m_topTen[0].zipPos].status;
				oldSts &= 0xff000000;
				theApp.m_datZipList[datIndex].status = (oldSts + STATUS_PERFECT);
			}
			else
			{
				DWORD oldSts = theApp.m_datZipList[m_topTen[0].zipPos].status;
				oldSts &= 0xff000000;
				theApp.m_datZipList[datIndex].status = (oldSts + STATUS_RENAME_ZIP_ONLY);
			}
		}


		if ((m_topTen[0].score < 100) && (missing == 0) && (unknown == 0))
		{
			DWORD oldSts = theApp.m_datZipList[m_topTen[0].zipPos].status;
			oldSts &=0xff000000;
			theApp.m_datZipList[datIndex].status =(oldSts + STATUS_RENAME_ROMS);
		}

		if (missing < 0)	// yes.  this can happen when there is a crc match in a renamed file  huh?
		{
			int g=0;
		}

		if (missing > 0)
		{
			if (missing == unknown) // if the missing files exactly matches the # of unknown files (CRC collisions cause this), we are good.
			{		
				DWORD oldSts = theApp.m_datZipList[datIndex].status;
				oldSts &=0xff000000;		
				theApp.m_datZipList[datIndex].status = (oldSts + STATUS_CRC_COLLISION);	
			}
			else
			{
				DWORD oldSts = theApp.m_datZipList[datIndex].status;
				oldSts &=0xff000000;
				theApp.m_datZipList[datIndex].status = (oldSts + STATUS_MISSINGFILES);
			}
		}

		if (m_topTen[0].score > 100)
		{
			DWORD oldSts = theApp.m_datZipList[datIndex].status;
			oldSts &=0xff000000;
			theApp.m_datZipList[datIndex].status = (oldSts + STATUS_EXTRAFILES);
		}
	}	// anything reportable

	return m_topTen[0].score;
}


// scan multiple zips (anything highlighted)
void CDOSCenterDlg::scanSelectedZips()
{
	CProgressDlg progDlg;
	progDlg.Init(PROG_MODE_TIMER, 1000, L"Scanning files, hang on...");

	int nIndex=-1;

	while (1) 
	{
		nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
		if (nIndex == -1)
			break;
		else
			scanThisZip(nIndex, &progDlg);
	} // all selected items
	
	progDlg.DestroyWindow();
}

// user has 2x clicked on a file on the left side - show all the files inside the zip
// if we've got a .dat file loaded, check to see if there are any hits and display CRC's that are matching in green.
//inside the zip:
//file is perfect = dark green
//file is unknown to the entire .dat file = black
//file is missing = red
//file is ignored = gray
//filename mismatch, CRC match = light green
//filename match, CRC mismatch = brown
//file found elsewhere in .dat, but not in this entry = purple
//? files in this entry but also in other .dat entries? = green/brown+purple?
void CDOSCenterDlg::OnNMDblclkZipFileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	unsigned int scanned=-1;	
	CStringW zipfile;
	CString filename, ext, size, crc, date, scanSuggest, tmp;
	WORD dosDate;
	WORD dosTime;
	CTime time;
	CString color;
	CString tmpext;
	CString zipDir;
	ZIPdata tempZIPdata;		// temporary storage for the zip file (from the .dat) the data came from
	ROMdata tempROMdata;		// temporary storage for the ROM file(s) in this zip
	ROMdata lookupROMdata;		// more temporary storage for the lookup
	int status;

	bool showOnly=false;
	int	datPos;

	theApp.m_zipIsFoldered = false;

	int nIndex;
	CString zipfilename;

	// no selection?  abort
	nIndex = theApp.m_leftSideList.GetNextItem(-1, LVNI_SELECTED);
	if (m_autoAddScan)
		nIndex = m_autoAddIndex;
	if (nIndex == -1)
		return;
	
	// get the name of the zip file that the scanner is suggesting we use.
	zipfilename = theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL);	// phsyical filename
	scanSuggest = theApp.m_leftSideList.GetItemText(nIndex, NEWFILENAMECOL);		// .dat file zip filename
	
	if (theApp.m_processMode)
	{
		scanSuggest = theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL);
		zipfilename = theApp.m_currentlyTaggedZipFile;
	}

	// unless of course there are no new filename suggestions, so we need the physical filename
	if (scanSuggest == L"perfect!")
		scanSuggest = theApp.m_leftSideList.GetItemText(nIndex, FILENAMECOL);

	if (scanSuggest == L"no good matches")	// save a little time by not looking this up
		scanSuggest="";

	// yank out any folder information to get to the raw filename
	if (scanSuggest.Find('\\') != -1) 
		scanSuggest = scanSuggest.Right(scanSuggest.GetLength() - (scanSuggest.ReverseFind('\\')) -1 );

	// if there is no scanned filename, there is no need to compare CRCs, just list the zip contents
	if (scanSuggest.GetLength()==0)
		showOnly=true;
	else
	{//datPos = getDatPtr(scanSuggest);	// figure out where in the .dat we want to look
		int pos;
		CString tmp;
		tmp = theApp.m_leftSideList.GetItemText(nIndex,COLORCOL);
		pos = tmp.Find(L"|");
		if (pos != -1)
		{
			tmp = tmp.Mid(pos+1,tmp.GetLength()-pos);
			swscanf_s(tmp,L"%x",&datPos);
		}
	}
	// delete the current list of files if it's there.
	theApp.m_zipDetailsList.DeleteAllItems();


	// grab the zip and start displaying each file inside.
	zipfile=m_SourcePath+zipfilename;
	
//	theApp.m_ROMsInZip.RemoveAll();
	zip2map2(zipfile);

	// we now have a zip file with some roms in it
	// and a .dat file entry that has some roms in it.
	// we need to go through each of both and mark off the CRC, filename and timestamp matches.
	// then we print a list of all the files in the zip along with the extra stuff in the .dat
	//
	// there are some tricky bits here.
	// could have duplicate filename in the zip with the same CRC, but with different filenames (or in a folder)
	// could have a filename match, but the CRC is different (common)



	if (m_buriedZipFolder.GetLength() > 0)
	{
		theApp.m_zipDetailsList.AddItem(COLOR_PBROWN, L"Warning, all files inside folder", m_buriedZipFolder, "", "", "");	// add a note
		theApp.m_zipIsFoldered = true;
		//m_buriedZipFolder = zipDir;
	}

	if (!showOnly)	// this means we've scanned this file and can show colors compared to just a black listing.
	{
		compareDat2Zip(datPos, theApp.m_nFileCount, false);	// compare .dat entry to rom files
		theApp.m_tmpzipList[0].ptr2zipFileArray = datPos;
	}


	// now display each ROM
	// the color shown is dependent on the various bits set in the status field.
	theApp.m_zipMap[0].it = theApp.m_zipMap[0].ROMmap.begin();
	while (theApp.m_zipMap[0].it != theApp.m_zipMap[0].ROMmap.end())
	{
		status = theApp.m_zipMap[0].it->second.status;
		if ((status & STATUS_ROM_MISSING) == STATUS_ROM_MISSING) // display missing roms later on.
		{
			theApp.m_zipMap[0].it++;
			continue;
		}
		filename = theApp.m_zipMap[0].it->second.name;
		size.Format(L"%d", theApp.m_zipMap[0].it->second.size);
		crc.Format(L"%08x", theApp.m_zipMap[0].it->second.crc);
		dosDate = (WORD)(theApp.m_zipMap[0].it->second.date >> 16);
		dosTime = (WORD)(theApp.m_zipMap[0].it->second.date & 0xffff);
		CTime time(dosDate, dosTime);
		date = time.Format("%Y/%m/%d %H:%M:%S");
		ext = extractExtension(filename);

#if 0
		if ((status == 0) && (!showOnly)) // unknown file (for this dat) but it may exist elsewhere.  quick look it up.
		{
			unsigned int c = theApp.m_zipMap[0].it->second.crc;
			if (theApp.m_crcMap.count(c) != 0)
			{// CRC hit?
				std::vector<unsigned int>::iterator datPtrit;
				datPtrit = theApp.m_crcMap[c].datPtr.begin();
				int datPtr = *datPtrit;
				CStringW zipName = theApp.m_datZipList[datPtr].filename; // debug
				
				//theApp.m_datZipListit = theApp.m_datZipList.find(lookupROMdata.ptr2zipFileArray);
				//CStringW zipName = theApp.m_datZipListit->second.filename;	// 2nd element is the 2nd thing stored in this map. weird stuff.
				//theApp.m_tmpzipList[i].newFileName = zipName;
				status |= STATUS_ROM_ELSEWHERE;
				//theApp.m_tmpzipList[i].ptr2zipFileArray = lookupROMdata.ptr2zipFileArray;
				//color = COLOR_PURPLE;
			}
		}
#endif

		color = status2color(status);

		// check if the user wants to hide these files
		// hide matching CRCs?
		if (((status & STATUS_CRC_MATCH) == STATUS_CRC_MATCH ) && ((CButton*)GetDlgItem(IDC_HIDEMATCHES))->GetCheck())
			color = COLOR_HIDE;
		// hide unknown files?
		if ((status == 0) && ((CButton*)GetDlgItem(IDC_HIDEUNKNOWN))->GetCheck())
			color = COLOR_HIDE;
		
		// add this file to the output box.
		if (color != COLOR_HIDE)
			theApp.m_zipDetailsList.AddItem(color, filename, ext, size, date, crc);	// add this entry to the list

		theApp.m_zipMap[0].it++;
	}

#if 0	
	for (int i=0; i<theApp.m_nFileCount; i++)
	{
		//filename = theApp.m_tmpzipList[i].fileName;
		filename = theApp.m_zipMap[0].ROMmap[i].name;
		color = status_to_color(theApp.m_zipMap[0].ROMmap[i].status); // theApp.m_tmpzipList[i].status); // COLOR_BLACK;	// default unknown color		
		

		size.Format(L"%d",theApp.m_tmpzipList[i].size);	
		crc.Format(L"%08x", theApp.m_tmpzipList[i].crc);
		dosDate = (WORD)(theApp.m_tmpzipList[i].date >> 16);
		dosTime = (WORD)theApp.m_tmpzipList[i].date & 0xffff;
		CTime time(dosDate, dosTime);
		date =time.Format( "%Y/%m/%d %H:%M:%S" );
		ext = extractExtension(filename);

		int q = theApp.m_tmpzipList[i].status; // tmp so I can see the goddamn variable

		if ( (theApp.m_tmpzipList[i].status == 0) && (!showOnly))// unknown file (for this dat) but it may exist elsewhere.  quick look it up.
		{
			if (theApp.m_ROMDATEntryCRC.Lookup(crc, lookupROMdata))
			{// CRC hit?
				theApp.m_datZipListit = theApp.m_datZipList.find(lookupROMdata.ptr2zipFileArray);
				CString zipName = theApp.m_datZipListit->second.filename;	// 2nd element is the 2nd thing stored in this map. weird stuff.
				theApp.m_tmpzipList[i].newFileName = zipName;
				theApp.m_tmpzipList[i].status |= STATUS_ROM_ELSEWHERE;
				theApp.m_tmpzipList[i].ptr2zipFileArray = lookupROMdata.ptr2zipFileArray;
//				color = COLOR_PURPLE;
			}
		}

//		else
		// add this file into the right side window list
		// separate out the extension from the filename


		// check if the user wants to hide these files
		if ( (theApp.m_tmpzipList[i].status & STATUS_CRC_MATCH) && ((CButton*)GetDlgItem(IDC_HIDEMATCHES))->GetCheck())	// does the user want to see these items?
			color = COLOR_HIDE;

		if ((theApp.m_tmpzipList[i].status == 0) && ((CButton*)GetDlgItem(IDC_HIDEUNKNOWN))->GetCheck())	// does the user want to see these items?
			color = COLOR_HIDE;
		
		if (color != COLOR_HIDE)
			theApp.m_zipDetailsList.AddItem(color,filename, ext, size, date, crc);	// add this entry to the list
	}
#endif	
	if ( (showOnly) || ((CButton*)GetDlgItem(IDC_HIDEMISSING))->GetCheck() ) // only if the user wants this stuff
		goto exit;	// yep I just did that.

	////////////////
	// display missing files - anything in the .dat that we have not discovered in the physical zip  
	////////////////
	// missing files have been injected in the zipMap to make it easier to display.
	theApp.m_zipMap[0].it = theApp.m_zipMap[0].ROMmap.begin();
	while (theApp.m_zipMap[0].it != theApp.m_zipMap[0].ROMmap.end())
	{
		status = theApp.m_zipMap[0].it->second.status;
		if ((status & STATUS_ROM_MISSING) != STATUS_ROM_MISSING)
		{
			theApp.m_zipMap[0].it++;
			continue;
		}
		// put it in the list
		color = status2color(status);
		filename = theApp.m_zipMap[0].it->second.name;
		ext = extractExtension(filename);
		size.Format(L"%d", theApp.m_zipMap[0].it->second.size);
		crc.Format(L"%08x", theApp.m_zipMap[0].it->second.crc);
		dosDate = (WORD)(theApp.m_zipMap[0].it->second.date >> 16);
		dosTime = (WORD)theApp.m_zipMap[0].it->second.date & 0xffff;
		CTime time(dosDate, dosTime);
		date = time.Format("%Y/%m/%d %H:%M:%S");
		if (!filename.IsEmpty())
			theApp.m_zipDetailsList.AddItem(color, filename, ext, size, date, crc);	// add this entry to the list
		theApp.m_zipMap[0].it++;
	}



#if 0
	{

		int g = theApp.m_datZipList[datPos].ROMsMap[j].status;
		if ( (g != 0) && (g != STATUS_ROM_IGNORED) )
			continue;
		
		// hang on pardner.  if this file is in our ignore list, it's not really missing. 
		//if (skipafile(filename, ext, theApp.m_datZipList[datPos].ROMsMap[j].crc, theApp.m_datZipList[datPos].ROMsMap[j].size))
		
		if (theApp.m_datZipList[datPos].ROMsMap[j].status == STATUS_ROM_IGNORED)
			color = COLOR_RED_IGNORED;
		// if this file was ignored in teh dat file AND found in the zip, skip displaying it altogether.
//		if (theApp.m_datZipList[datPos].ROMsMap[j].status == (STATUS_ROM_IGNORED+STATUS_ROM_FILENAME_MATCH) )
//			color = "0x987654";
		if (!filename.IsEmpty())
			theApp.m_zipDetailsList.AddItem(color,filename, ext, size, date, crc);	// add this entry to the list
	}
#endif


	// clean up our .dat entry
	for (int j=0; j<theApp.m_datZipList[datPos].fileCount; j++)
		theApp.m_datZipList[datPos].ROMsMap[j].status = 0;

exit:


/*



	int i=0;
	while(1)	// gather all the info for each file in this zip and display it on the right hand window
	{

		filename = info.szFileName;
		filename = filename.Right(filename.GetLength() - zipDir.GetLength());	// trim off any leading directory info
		// huck each "rom" from the zip into a cmap so we can peek at it later
		theApp.m_tmpzipList[i].fileName = filename;
		theApp.m_tmpzipList[i].crc = info.dwCRC;
		theApp.m_tmpzipList[i].size = info.dwUncompressedSize;
		theApp.m_tmpzipList[i].status = 0; // default not found yet
		CString fnamBakup = filename;
		ext = extractExtension(filename);

		size.Format("%d",info.dwUncompressedSize);	
		crc.Format("%08x", info.dwCRC);
		dosDate = (WORD)(info.dwDosDate >> 16);
		dosTime = (WORD)info.dwDosDate & 0xffff;
		DosDateTimeToFileTime(dosDate, dosTime, &dateTime);
		FileTimeToSystemTime(&dateTime, &sysTime);
		date.Format("%d/%d/%d %d:%02d", sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute);

		// display all the files in this zip in color
		color = COLOR_BLACK; //default color, unscanned.



		// figure out which color to make this file
		if (scanSuggest.GetLength()>0) // only color code a file if we've scanned and compared it to something
		{
			// create a key out of the rom's CRC
			tmp=crc;
			tmp.MakeLower();
			ROMFilenameKey = tmp;
			// see if this rom filename exists in our collection
			if (theApp.m_ROMDATEntryCRC.Lookup(ROMFilenameKey, tempROMdata))	// found a match.  we have to check for collisions though.
			{
				color = COLOR_BLACK;			// assume we have no idea what this file matches
				while (1)
				{
					CString localtmp=ROMFilenameKey;	// just in case we need to look this one up multiple times
					// first check to make sure there aren't multiple files in the same zip with the same CRC.  yep, it happens.
					while (tempROMdata.collision == true)
					{
						
						CString tmpROMDatFilename = tempROMdata.fileName;
						if (tmpROMDatFilename.MakeLower() == fnamBakup.MakeLower()) // did we find it?
							break;
						
						theApp.makeNewKey(localtmp);
						theApp.m_ROMDATEntryCRC.Lookup(localtmp, tempROMdata);


					}


					// continue searching until we have a match of this name to the zip file that we've suggested.
					theApp.m_datZipListit = theApp.m_datZipList.find(tempROMdata.ptr2zipFileArray); // is this ROM file located in the ZIP file from the .dat that we're suggesting?
					CString tmpROMDatFilename = tempROMdata.fileName;
					if (scanSuggest == theApp.m_datZipListit->second.filename)	// 2nd element is the 2nd thing stored in this map.
					{
						zipPosPtr = tempROMdata.ptr2zipFileArray; // save a pointer to this zip file from the dat so we can look it up later.
						color = COLOR_GREEN;		// finally! we have a hit
						theApp.m_tmpzipList[i].status = STATUS_ROM_FOUND;	// leave ourselves a flag that we have noted this file
						theApp.m_tmpzipList[i].ptr2zipFileArray = zipPosPtr;

						
						if (tmpROMDatFilename.MakeLower() != fnamBakup.MakeLower() ) // make sure the name is exact before calling it perfect.
						{
							color = COLOR_YELLOW;
							theApp.m_tmpzipList[i].status = STATUS_ROM_NAMEWEIRD;
							theApp.m_tmpzipList[i].newFileName = tempROMdata.fileName;
							
						}
						if ( ((CButton*)GetDlgItem(IDC_HIDEMATCHES))->GetCheck() )	// does the user want to see these items?
							color = COLOR_HIDE;
						break;	// we are done.
					}	
					// are there multiple files that have this crc?  Look 'em up.
					if (tempROMdata.collision == true)
					{
						// make a new file and get the new data
						if (tmp.GetLength() > 10)
							int g=0;

						tmp = ROMFilenameKey;
						theApp.makeNewKey(tmp);
						ROMFilenameKey = tmp;
						if (!theApp.m_ROMDATEntryCRC.Lookup(ROMFilenameKey, tempROMdata))
							continue;	// keep checking
					}
					else // no more collisions, this happens when a crc exists, but not in our zip that we just opened.  These would technically be unknown, but it's cool we can catch em here.
					{
						color = COLOR_PURPLE;	// zip filename placement mismatch
						tmpZipPosPtr = tempROMdata.ptr2zipFileArray; // save a pointer to this zip file from the dat so we can look it up later.
						theApp.m_tmpzipList[i].ptr2zipFileArray = tmpZipPosPtr;
						theApp.m_tmpzipList[i].status = STATUS_ROM_ELSEWHERE;	// leave ourselves a flag that we have noted this file, but it's goofy
						break;
					}
				} // collision check
			} // hit


			// check the file against any ignore files in the database.  color those gray if there's a match.
			if (( skipafile(filename, ext, info.dwCRC, info.dwUncompressedSize) == true) && (color != COLOR_HIDE) )
			{
				theApp.m_tmpzipList[i].status = STATUS_ROM_IGNORED;
				color = COLOR_GRAY;
			}
		}  // scanned


		// if the user doesn't want to see unknown (black color) files, hide them here.
		if ( (color == COLOR_BLACK) && ((CButton*)GetDlgItem(IDC_HIDEUNKNOWN))->GetCheck() )	// does the user want to see these items?
			color = COLOR_HIDE;



		// color set, finally add this file to the list 
		if (color != COLOR_HIDE)
			theApp.m_zipDetailsList.AddItem(color,filename, ext, size, date, crc);	// everything else


		if (m_uz.GotoNextFile() == false)
			break;
		m_uz.GetFileInfo(info);
		i++;

	}
*/

/*

	////////////////
	// display missing files
	////////////////
	// now display any files that were missing from the zip file in red.
	if ( (!((CButton*)GetDlgItem(IDC_HIDEMISSING))->GetCheck()) && (scanSuggest.GetLength() != 0) ) // only if the user wants this stuff
	{
		CString x,y;
		for (int i=0; i<theApp.m_datZipList[zipPosPtr].fileCount; i++) // for each rom file in this zip's .dat entry
		{
			if ((theApp.m_datZipList[zipPosPtr].ROMsMap[i].status & DATENTRY_STATUS_IGNORE) == DATENTRY_STATUS_IGNORE)
				continue;
			x = theApp.m_datZipList[zipPosPtr].ROMsMap[i].name;
			x.MakeLower();
			color = COLOR_RED;		// assume the file is missing
			date = "--missing--";   // user flag for a missng file in date column
			bool found=false;		// assume this one is missing
			y="fd";
			int j=0;
			while (y.GetLength() > 0)
		
			//for (int j=0; j<nFileCount; j++)	// for each file in the zip - seems this number is inaccurate when there is a file in the ignore list.
            {
				int foundStatus=theApp.m_tmpzipList[j].status;
				y = theApp.m_tmpzipList[j].fileName;
				//CString x = theApp.m_datZipList[zipPosPtr].ROMsMap[i].name;
				//x.MakeLower();
				y.MakeLower();
				// check the filename from the dat against each filename in the zip
				// is this filename a match?  if so, we have a CRC mismatch, but filename is ok.
				if (x == y)
					{
						if (foundStatus == STATUS_ROM_FOUND)
						{
							found = true;
							break;
						}
						//if (foundStatus != 0)	// we only want files here that we didn't locate above to speed up this process
						//{
						//	found = true;
					//		continue;						// we don't care - we've already found this one
					//	}
						color = COLOR_BROWN;	  // filename match, but we know the CRC doesn't, otherwise it'd be green above.
						date = "--Diff. CRC--";   // user flag for a bad CRC

						found = false;			  // force this one to display when we exit the loop
					}
		//		else // filename mismatch.  we may have already covered this one.
		//			if (foundStatus == STATUS_ROM_NAMEWEIRD)	
		//			{					
		//				found=true;
		//				break;
		//			}
					j++;
			} // for each file in the zip we just opened.

			// do we want to display this one?
				if (!found) 
				{
					filename = theApp.m_datZipList[zipPosPtr].ROMsMap[i].name;
	
					ext = extractExtension(filename);

					size.Format("%d", theApp.m_datZipList[zipPosPtr].ROMsMap[i].size);	
					crc.Format("%08x", theApp.m_datZipList[zipPosPtr].ROMsMap[i].crc);
				//dosDate = (WORD)(info.dwDosDate >> 16);
				//dosTime = (WORD)info.dwDosDate & 0xffff;
				//DosDateTimeToFileTime(dosDate, dosTime, &dateTime);
				//FileTimeToSystemTime(&dateTime, &sysTime);
				//date.Format("%d/%d/%d %d:%02d", sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute);
					theApp.m_zipDetailsList.AddItem(color, filename, ext, size, date, crc);	// everything else
				}			

		} 


*/



/*
		for (int i=0; i<theApp.m_datZipList[zipPosPtr].fileCount; i++)	// for all the files in this zip dat entry...
		{
			// get the filename




		}
		CString q = theApp.m_datZipList[zipPosPtr].ROMsMap[0].name;
/*
		if (scanSuggest.GetLength()>0) // only do this if we've scanned and compared it to something from the .dat
			{
				// we need to parse through the entire dat file collection and find ROMs that point back to the zip we're suggesting.  
				POSITION pos = theApp.m_ROMDATEntry.GetStartPosition();
				CString whatever;
                while (pos !=NULL)
				{
					theApp.m_ROMDATEntry.GetNextAssoc( pos,whatever, tempROMdata);
					if (tempROMdata.ptr2zipFileArray == zipPosPtr) // is this a ROM file that came from our suggested zip?
					{
						theApp.m_datZipListit = theApp.m_datZipList.find(tempROMdata.ptr2zipFileArray); // is this ROM file located in the ZIP file from the .dat that we're suggesting?
						if (scanSuggest == theApp.m_datZipListit->second.filename)

				}
				


				theApp.m_datZipListit = theApp.m_datZipList.find(zipPosPtr); // point to the zip
					if (scanSuggest == theApp.m_datZipListit->second.filename)	// 2nd element is the 2nd thing stored in this map.
					
				m_copyfileData[0].it = m_copyfileData[0].ROMmap.begin();
				while( m_copyfileData[0].it != m_copyfileData[0].ROMmap.end() ) 
				{
					if ( (m_copyfileData[0].it->second.found != true) && (m_copyfileData[0].it->second.name != "") )
					{
						filename = m_copyfileData[0].it->second.name; 
						ext = filename.Right(filename.GetLength() - filename.ReverseFind('.'));
						filename = filename.Left(filename.GetLength() - ext.GetLength());
						if (filename == "")
						{
							filename = ext;
							ext = "";
						}
						ext.TrimLeft(".");
						size.Format("%d",m_copyfileData[0].it->second.size);
						crc.Format("%08x",m_copyfileData[0].it->second.crc);
						if (skipafile(filename, ext, m_copyfileData[0].it->second.crc, m_copyfileData[0].it->second.size) == false)
							theApp.m_zipDetailsList.AddItem(COLOR_RED,filename, ext, size, "-missing-", crc);
						if (m_copyfileData[0].it->second.alternate == true)
							theApp.m_zipDetailsList.AddItem(COLOR_PURPLE,filename, ext, size, "-alternate-", crc);
					}
				m_copyfileData[0].it++;
				}
			}
			*/
//	} // if view enabled
	// save the selected zip filename, in case the user wants to do anything else with it.
	theApp.m_currentlyTaggedZipFile = zipfilename;

	// huck the copy of the file data
//	delete[] m_copyfileData;

}


void CDOSCenterDlg::add2database()
{
	CSettingsandTools CnT;
	ROMdata tempROMdata;	// temporary storage for the lookup
	int count=0;
	unsigned int size, date;
	CString name, scrc, tmp;
	CString color;
	bool results;

	CString f= theApp.m_datZipList[theApp.m_datfileCount].filename;
	theApp.m_datZipList[theApp.m_datfileCount].filename= theApp.m_currentlyTaggedZipFile;	// store the zip filename inside the structure inside the map.
	

	//	theApp.m_fileData[theApp.m_datfileCount].filename = 
//	theApp.m_datZipListit = theApp.m_datZipList.find(theApp.m_datfileCount);
//	CString	zipName = theApp.m_datZipListit->second.filename;	
	
	while (1)
	{
		color = theApp.m_zipDetailsList.GetItemText(count,0);
		if ( (color != COLOR_RED) && (color != COLOR_PBROWN) )
		{
			name = theApp.m_zipDetailsList.GetItemText(count,1);	// filename
			tmp = theApp.m_zipDetailsList.GetItemText(count,2);
			if (tmp.GetLength() !=0)
				name+=L"."+theApp.m_zipDetailsList.GetItemText(count,2);	// ext

			scrc = theApp.m_zipDetailsList.GetItemText(count,5);	// crc
			tmp = theApp.m_zipDetailsList.GetItemText(count,3);	// size
			swscanf_s(tmp,L"%x",&size);	
			date = CnT.String2Date(theApp.m_zipDetailsList.GetItemText(count,4)); //date
			results = CnT.rom2datmap(name, size, 0, true, date, scrc, count);	
		}
	
		tmp = theApp.m_zipDetailsList.GetItemText(count,0);
		if (results)
			count++;
		if (tmp.GetLength() == 0)
			break;
		
	}
	
	theApp.m_datZipList[theApp.m_datfileCount].fileCount = count;	// store the # of ROMs the zip contains
	theApp.m_datZipList[theApp.m_datfileCount].status = 0;
	theApp.m_datfileCount++;	// global counter of # of items in the .dat file

}


// returns whatever is after the . in an 8.3 filename, truncates original filename down to what is on the left of the .
CStringW CDOSCenterDlg::extractExtension(CStringW &filename)
{
	CStringW ext = filename.Right(filename.GetLength() - filename.ReverseFind('.'));
	filename = filename.Left(filename.GetLength() - ext.GetLength());
	if (filename == "")
	{
		filename = ext;
		ext = "";
	}
	ext.TrimLeft(L".");
	return ext;
}



// right side, zip details, user wants to extract the selected files and dump them into a zip of the target (suggested) filename
void CDOSCenterDlg::zip2NewFile()
{
//	CString logData;
	int nIndex = -1;
	CStringW zipfilename;
	CStringW filename;	// rom filename
	CStringW targetZipName;
	CFileStuff files;
	CProgressDlg progDlg;
	CNewFilenameDlg dlg;


	// get destination filename - make it just a variation of the 1st filename in the selected pool
	nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
	zipfilename = theApp.m_leftSideList.GetItemText(nIndex, FILENAMECOL);

	// pop up target filename here
	CStringW source = theApp.m_leftSideList.GetItemText(nIndex, FILENAMECOL);
	CStringW subPath = source.Left(source.ReverseFind('\\'));
	if (subPath.GetLength() != 0)
		subPath += "\\";
	dlg.m_txt = source;
	if ((theApp.m_leftSideList.GetItemText(nIndex, NEWFILENAMECOL) != "") && (theApp.m_leftSideList.GetItemText(nIndex, NEWFILENAMECOL) != "no good matches")
		&& (theApp.m_leftSideList.GetItemText(nIndex, NEWFILENAMECOL) != "perfect!"))
		dlg.m_txt = subPath + theApp.m_leftSideList.GetItemText(nIndex, NEWFILENAMECOL);

	dlg.DoModal();
	if (dlg.m_ok == 0)
		return;

	targetZipName = dlg.m_txt;

	progDlg.Init(PROG_MODE_TIMER, 1000, L"Unzipping file...");

	// this extracts all the selected files into the current working folder
	extractHere();


	nIndex = -1;
	while (1) // scan through all selected files and if they are in our delete list, don't add them to the unzip map.
	{
		nIndex = theApp.m_zipDetailsList.GetNextItem(nIndex, LVNI_SELECTED);
		if (nIndex == -1)
			break;

		filename = theApp.m_zipDetailsList.GetItemText(nIndex, FILENAMECOL);
		filename += L"." + theApp.m_zipDetailsList.GetItemText(nIndex, 2);

		//if there's no extension trim it
		filename.TrimRight(L".");

		add2zip(m_SourcePath + L"\\" + targetZipName, m_SourcePath + L"\\" + filename, false);
		remove((CT2CA)(m_SourcePath + L"\\" + filename));
	}

	theApp.m_leftSideList.AddItem(COLOR_BLACK, targetZipName, "", "", "", "", "");	// column 0 is color, zipfilename, new filename, score, match, missing, unknown
	OnNMDblclkZipFileList(0, 0);
	progDlg.DestroyWindow();
	return;
}

// right side, zip details, user wants to extract the selected files and dump them into the current folder
void CDOSCenterDlg::extractHere()
{
	//CString logData;
	int nIndex=-1;
	ZIPENTRYW info;
	CString zipfilename;
	CString filename;	// rom filename
	CString tmp, ext;
	int zipIndex;
	int status;
	CZipArchive zip;
	ZIP_INDEX_TYPE index;

	// get destination filename - make it just a variation of the 1st filename in the selected pool
	nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
	zipfilename = theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL);
   	
	nIndex=-1;
	nIndex = theApp.m_zipDetailsList.GetNextItem(nIndex, LVNI_SELECTED);
	
	
	//m_uz = XOpenZip(((void*)(LPCWSTR)(m_SourcePath+zipfilename)), 0, ZIP_FILENAME);
	zip.Open(m_SourcePath + zipfilename);

	while (nIndex != -1) // scan through all selected files and if unzip them
	{
		filename = theApp.m_zipDetailsList.GetItemText(nIndex,FILENAMECOL);
		filename +=L"."+theApp.m_zipDetailsList.GetItemText(nIndex,2);
		//if there's no extension trim it
		filename.TrimRight(L".");
		tmp=filename;
		tmp.Replace('\\','/');
	
		index = zip.FindFile(tmp);
		
//		status = FindZipItem(m_uz, tmp, true, &zipIndex, &info);
//		if (status == ZR_OK)
//		{
			filename = theApp.extractFilenameWOPath(filename);
//			status = UnzipItem(m_uz, zipIndex, (void*)(LPCWSTR)(m_SourcePath+filename), 0, ZIP_FILENAME);
			zip.ExtractFile(index, m_SourcePath);
//		}
		nIndex = theApp.m_zipDetailsList.GetNextItem(nIndex, LVNI_SELECTED);
    }
	zip.Close();
}


// right side, zip details, user wants to rename a selected file in the zip itself.
void CDOSCenterDlg::renameFileInZip()
{
	CString dest;
	int nIndex=-1;
	CString zipfilename;
	CString filename;	// rom filename
	CString tmp, ext;

	CNewFilenameDlg dlg;

	// get destination filename - make it just a variation of the 1st filename in the selected pool
	nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
	zipfilename = theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL);
	nIndex = -1;
	// get the filename inside the zip the user is pointing to.
	nIndex = theApp.m_zipDetailsList.GetNextItem(nIndex, LVNI_SELECTED);
	if (nIndex == -1)
		return;
	tmp = theApp.m_zipDetailsList.GetItemText(nIndex, FILENAMECOL);
	tmp += L"." + theApp.m_zipDetailsList.GetItemText(nIndex, 2);
	// pop up the rename dialog
	dlg.m_txt = tmp;
	dlg.DoModal();
	if (dlg.m_ok == 2) // cancel?
		return;
	dest = dlg.m_txt;
	dest.Replace(L"\t", L" ");
	dest.Trim();

	CZipArchive zip;
	zip.Open(m_SourcePath + zipfilename);
	zip.SetCommitMode(CZipArchive::cmManual);
	ZIP_INDEX_TYPE index = zip.FindFile(tmp);

	// rename the first file in the archive
	zip[index]->SetFileName(dest);
	zip.CommitChanges();
	zip.Close();
}

// right side, zip details, user wants to rename a selected file in the zip itself.
void CDOSCenterDlg::deleteFileFromZip()
{
	CString dest;
	int nIndex = -1;
	int count;
	CString zipfilename;
	CZipArchive zip;
	CString tmp, ext;

	CNewFilenameDlg dlg;

	// get destination filename - make it just a variation of the 1st filename in the selected pool
	nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
	zipfilename = theApp.m_leftSideList.GetItemText(nIndex, FILENAMECOL);
	
	// count number of items in list-popup verify message.
	nIndex = -1;
	count = 0;
	while (1)
	{
		nIndex = theApp.m_zipDetailsList.GetNextItem(nIndex, LVNI_SELECTED);
		if (nIndex == -1)
			break;
		count++;
	}
	tmp.Format(L"Delete %d file(s)?", count);
	int choice = AfxMessageBox(tmp, MB_ICONWARNING | MB_YESNO, 0);
	if (choice == IDNO)
		return;

	nIndex = -1;	// ok then, delete away!
	

	zip.Open(m_SourcePath + zipfilename);

	while (1)
	{
		// get the filename inside the zip the user is pointing to.
		nIndex = theApp.m_zipDetailsList.GetNextItem(nIndex, LVNI_SELECTED);
		if (nIndex == -1)
			break;
		tmp = theApp.m_zipDetailsList.GetItemText(nIndex, FILENAMECOL);
		ext = theApp.m_zipDetailsList.GetItemText(nIndex, 2);
		if (ext.GetLength() > 0)
			tmp += L"." + ext;

		ZIP_INDEX_TYPE index = zip.FindFile(tmp);

		// rename the first file in the archive
		zip.RemoveFile(index);
	}
		zip.Close();
}


// move all files in a zip file up 1 level. 
void CDOSCenterDlg::unNestZipFile()
{
	CString dest;
	int nIndex=-1;
	CString zipfilename;
	CString filename;	// rom filename
	CString targetZipName;
	CString tmp, ext;
	CFileStuff files; 
	CStringW zipDir;

	CProgressDlg progDlg;
	CNewFilenameDlg dlg;
	progDlg.Init(PROG_MODE_TIMER, 1000, L"Unzipping file...");
	
	// get destination filename - make it just a variation of the 1st filename in the selected pool
	nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
	zipfilename = theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL);
	targetZipName = zipfilename;
	// trim off any folder
	targetZipName = theApp.extractFilenameWOPath(targetZipName);
	targetZipName.Insert(0,L"WIP");

	zip2map2(m_SourcePath+zipfilename);
	// get the name of the root folder that we're about to elimate
	zipDir = m_buriedZipFolder; // findRootZipFolder();

	if (zipDir.GetLength() == 0)
	{
		AfxMessageBox(L"Not all files are buried", MB_ICONWARNING, 0);
		progDlg.DestroyWindow();
		//delete[] zipMerge;
		return;
	}


	

	// create a temporary folder and unzip to it
	CreateDirectory((theApp.m_tmpWorkingFolder+L"\\DOSCenterUz"), NULL);
	if (!unzipAFile(m_SourcePath+zipfilename, (theApp.m_tmpWorkingFolder+L"\\DOSCenterUz")))
		{
			AfxMessageBox(L"Error unzipping file! "+zipfilename, MB_ICONEXCLAMATION, 0);
			// delete temporary work space
			if (!files.DeleteDirectory(theApp.m_tmpWorkingFolder+L"\\DOSCenterUz\\"))
				AfxMessageBox(L"Warning, unable to clean up temporary working folder.\r\n"+theApp.m_tmpWorkingFolder , MB_ICONWARNING, 0);
			//delete[] zipMerge;
			progDlg.DestroyWindow();
			return;
		}

	// zip it all back up.
	progDlg.UpdateText(L"Creating new zip:"+targetZipName);

	SetCurrentDirectory(theApp.m_tmpWorkingFolder+"\\DOSCenterUz\\"+zipDir);
	if (!zipAFile(m_SourcePath+"\\"+targetZipName, theApp.m_tmpWorkingFolder+"\\DOSCenterUz\\"+zipDir))
	{
		//bad zip file created!
	}

	// copy the global zip map over to a temp map so we can compare the new zip against the old one.
	std::map<CStringW, int> tmpROMmap;	// storage, make the filename the key
	std::map<CStringW, int>::const_iterator it;	// an iterator

	CStringW romfilenameZip;

	theApp.m_zipMap[0].it = theApp.m_zipMap[0].ROMmap.begin();
	while (theApp.m_zipMap[0].it != theApp.m_zipMap[0].ROMmap.end())
	{
		romfilenameZip = theApp.m_zipMap[0].it->second.name;	// get updated raw rom name from the zip and keep track of it for later.
		if (romfilenameZip.GetLength())
		{
			//romfilenameZip.TrimLeft(zipDir);
			tmpROMmap[romfilenameZip] = 1;
		}
		theApp.m_zipMap[0].it++;
	}

	zip2map2(m_SourcePath + targetZipName, true);

	// compare to temp map
	bool badzip = false;
	theApp.m_zipMap[0].it = theApp.m_zipMap[0].ROMmap.begin();
	while (theApp.m_zipMap[0].it != theApp.m_zipMap[0].ROMmap.end())
	{
		romfilenameZip = theApp.m_zipMap[0].it->second.name;	// get updated raw rom name from the zip and keep track of it for later.
		if (romfilenameZip.GetLength())
		{
			if (tmpROMmap.count(romfilenameZip)==0)
			{
				badzip = true;
				break;
			}
		}
		theApp.m_zipMap[0].it++;
	}


	if (badzip)
	{
		AfxMessageBox(L"Error, two zip files are not equal.\r\n", MB_ICONWARNING, 0);
		SetCurrentDirectory(theApp.m_tmpWorkingFolder);
		remove((CT2CA)(m_SourcePath + targetZipName));
	}
	else
	{
		SetCurrentDirectory(theApp.m_tmpWorkingFolder);
		remove((CT2CA)(m_SourcePath + zipfilename));
		MoveFile(m_SourcePath + targetZipName, m_SourcePath + zipfilename);
	}

	// delete the original file, move the new one back in.
	progDlg.UpdateText(L"Cleaning up...");
	// delete temporary work space
	if (!files.DeleteDirectory(theApp.m_tmpWorkingFolder+L"\\DOSCenterUz\\"))
		AfxMessageBox(L"Warning, unable to clean up DOSCenter temporary working folder.\r\n"+theApp.m_tmpWorkingFolder , MB_ICONWARNING, 0);


	OnNMDblclkZipFileList(0, 0);
	//delete[] zipMerge;
	progDlg.DestroyWindow();
	return;
}

// deep dive the zip file contents against the .dat entry.  Expose every detail we can about the files compared to the .dat
// This primarily updates the zipmap status field for each entry in the zipMap but we're only going through the files in the dat entry.
// the datMap is also updated so that we can output scan have/miss lists.
// by passing in the datPos, this allows this routine to compare zip to any arbitrary dat entry.
void CDOSCenterDlg::compareDat2Zip(int datPos, int m_nFileCount, bool cleanup)
{
	m_zipCRCmatches = 0;
	m_exactMatches = 0;

	CStringW lcaseROMfilenameDat, tmp;
	int status;
	unsigned long long crcsize;
	unsigned int crc;
	CStringW romfilenameDat;
	CStringW romfilenameZip;
	
	// in a zip file (and .dat entry) the _filename_ is the unique part. zips can have duplicate files with duplicate CRCs but cannot have duplicate filenames.
	for (int j = 0; j < theApp.m_datZipList[datPos].fileCount; j++) // for every entry in the .dat file
	{
		status = 0;
		
		// get this ROM filename out of the dat
		romfilenameDat = theApp.m_datZipList[datPos].ROMsMap[j].name;
		romfilenameZip = romfilenameDat; // assume dat filename matches zip filename.

		// 1st check for an exact match, case and everything.
		theApp.m_zipMap[0].it = theApp.m_zipMap[0].ROMmap.find(romfilenameDat);
		if (theApp.m_zipMap[0].it == theApp.m_zipMap[0].ROMmap.end()) // not found? try case insenstive or CRC matching
		{
			lcaseROMfilenameDat = theApp.m_datZipList[datPos].ROMsMap[j].name;
			lcaseROMfilenameDat.MakeLower();
			crc = theApp.m_datZipList[datPos].ROMsMap[j].crc;

			theApp.m_zipMap[0].it = theApp.m_zipMap[0].ROMmap.begin();
			while (theApp.m_zipMap[0].it != theApp.m_zipMap[0].ROMmap.end())
			{
				romfilenameZip = theApp.m_zipMap[0].it->second.name;	// get updated raw rom name from the zip and keep track of it for later.
				tmp = romfilenameZip;
				tmp.MakeLower();
				if (tmp == lcaseROMfilenameDat)	// wrong case?
				{
					status |= STATUS_ROM_WRONGCASE;
					break;
				}
				if (theApp.m_zipMap[0].it->second.crc == crc) // CRC match? // this could burn us when zip has multiple files of the same CRC.
				{
					status |= STATUS_ROM_WRONGNAME;
					theApp.m_zipMap[0].ROMmap[romfilenameZip].correctFilename = theApp.m_datZipList[datPos].ROMsMap[j].name; // tuck it away so the user can look at it later.
					break;
				}
				
				theApp.m_zipMap[0].it++;
			}
		}
		
		// If we've found something that matches... filename, case insensitive filename or CRC32
		if (theApp.m_zipMap[0].it != theApp.m_zipMap[0].ROMmap.end())  
		{
			if (theApp.m_zipMap[0].it->second.crc == theApp.m_datZipList[datPos].ROMsMap[j].crc)
				status |= STATUS_CRC_MATCH;
			if (theApp.m_zipMap[0].it->second.date == theApp.m_datZipList[datPos].ROMsMap[j].date)
				status |= STATUS_TIMESTAMP_MATCH;
			if (theApp.m_zipMap[0].it->second.size == theApp.m_datZipList[datPos].ROMsMap[j].size)
				status |= STATUS_SIZE_MATCH;
			if (theApp.m_zipMap[0].it->second.name == theApp.m_datZipList[datPos].ROMsMap[j].name)
				status |= STATUS_FILENAME_MATCH;
			else // we should have this status already from above
			{
				tmp = theApp.m_zipMap[0].it->second.name;
				tmp.MakeLower();
				if (tmp == lcaseROMfilenameDat)
				{
					status |= STATUS_ROM_WRONGCASE;
					//tmp = theApp.m_zipMap[0].it->second.name; // get the filename as was found in the .dat
					theApp.m_zipMap[0].ROMmap[romfilenameZip].correctFilename = theApp.m_datZipList[datPos].ROMsMap[j].name; // tuck it away so the user can look at it later.
				}
			}

			// get some more status info from the .dat file
			// check for torrentzip .dat entry 
			if (theApp.m_datZipList[datPos].ROMsMap[j].status & STATUS_ROM_TZIPPED)
				status |= STATUS_ROM_TZIPPED;
			// is this rom also found in other dat entries? - not sure where that would be getting set.
			//if (theApp.m_datZipList[datPos].ROMsMap[j].status & STATUS_ROM_ELSEWHERE)
			//	status |= STATUS_ROM_ELSEWHERE;
			
			// update the status fields for both the .dat file and the romsmap (zip file)
			tmp = theApp.m_zipMap[0].it->second.name;
			theApp.m_zipMap[0].ROMmap[romfilenameZip].status = status;

			// should not be updating the .dat file at this level?
			theApp.m_datZipList[datPos].ROMsMap[j].status = status;
			if ((theApp.m_datZipList[datPos].ROMsMap[j].status & STATUS_ROM_PERFECT_MATCH) == STATUS_ROM_PERFECT_MATCH)
				m_exactMatches++;
			else if ((theApp.m_datZipList[datPos].ROMsMap[j].status & STATUS_CRC_MATCH) == STATUS_CRC_MATCH)
				m_zipCRCmatches++;
		}
		else // this .dat entry is not found in this zip.  Make a fake entry in the zipmap - todo- if we matched filename but not CRC, we need to do this too.
		{
			// do some screening first to see if this is even something we care about.

		//	if (theApp.m_zipMap[0].ROMmap[romfilenameDat].status & STATUS_ROM_SKIPPED == 0)
		//	{
				theApp.m_zipMap[0].ROMmap[romfilenameDat].name = romfilenameDat;  // don't even need this, since the filename is the index.
				theApp.m_zipMap[0].ROMmap[romfilenameDat].size = theApp.m_datZipList[datPos].ROMsMap[j].size;
				theApp.m_zipMap[0].ROMmap[romfilenameDat].crc = theApp.m_datZipList[datPos].ROMsMap[j].crc;
				theApp.m_zipMap[0].ROMmap[romfilenameDat].status = STATUS_ROM_MISSING;
				theApp.m_zipMap[0].ROMmap[romfilenameDat].date = theApp.m_datZipList[datPos].ROMsMap[j].date;
				
				// ok, a missing file has been detected.  is that file accounted for somewhere?
				int datPtr = doesROMExistinDat(theApp.m_datZipList[datPos].ROMsMap[j].crc);
				if ((datPtr != -1) && (datPtr != datPos))	// make sure we don't just point back to ourselves.
				{
					CStringW zipName = theApp.m_datZipList[datPtr].filename; // debug
					theApp.m_zipMap[0].ROMmap[romfilenameDat].status |= STATUS_ROM_ELSEWHERE;
					theApp.m_zipMap[0].ROMmap[romfilenameDat].infoData = datPtr;
				}
				
				//theApp.m_zipMap[0].totalBytes += zip[index]->m_uUncomprSize;
				//theApp.m_zipMap[0].fileCount++;
		//	}
		}
	}

	// now we need to see if there is anything in the zip file not accounted for vs the dat.
	theApp.m_zipMap[0].it = theApp.m_zipMap[0].ROMmap.begin();
	while (theApp.m_zipMap[0].it != theApp.m_zipMap[0].ROMmap.end())
	{
		if ( ((theApp.m_zipMap[0].it->second.status & STATUS_ROM_PERFECT_MATCH) == 0) && ((theApp.m_zipMap[0].it->second.status & STATUS_ROM_MISSING) == 0) )// no .dat record for this one.
		{
			romfilenameZip = theApp.m_zipMap[0].it->second.name;
			theApp.m_zipMap[0].ROMmap[romfilenameZip].status |= STATUS_ROM_EXTRA;	// mark this file as extranious
			int size = theApp.m_zipMap[0].it->second.size;
			crcsize = size;
			crcsize = (unsigned long long)(crcsize << 32);
			crcsize |= theApp.m_zipMap[0].it->second.crc;

			// is this crc even in the .dat file?
			//if (theApp.m_crcMap.count(crc) != 0)
			//{
			//	//std::vector<unsigned int>::iterator datPtrit;
			//	//datPtrit = theApp.m_crcMap[crc].datPtr.begin();
				int datPtr = doesROMExistinDat(crcsize);
				if (datPtr != -1)
				{
					CStringW zipName = theApp.m_datZipList[datPtr].filename; // debug
					theApp.m_zipMap[0].ROMmap[romfilenameZip].status |= STATUS_ROM_ELSEWHERE;
					theApp.m_zipMap[0].ROMmap[romfilenameZip].infoData = datPtr;
				}
			//}
		}
		theApp.m_zipMap[0].it++;
	}

	// no one is using this option.
	//if (cleanup)
	//{
	//	// cleanup the .dat map so we can scan it again
	//	for (int j = 0; j<theApp.m_datZipList[datPos].fileCount; j++) // ..and for each dat entry
	//		theApp.m_datZipList[datPos].ROMsMap[j].status &= STATUS_ROM_PERFECT_MATCH;
	//}
}

// is this crc even in the .dat file?
// returns datPtr if so.
// returns -1 if not.
int CDOSCenterDlg::doesROMExistinDat(unsigned long long crcsize)
{
	int datPtr = -1;
	if (theApp.m_crcMap.count(crcsize))
	{
		std::vector<unsigned int>::iterator datPtrit;
		datPtrit = theApp.m_crcMap[crcsize].datPtr.begin();
		datPtr = *datPtrit;
	}
	return datPtr;
}


// checks a given file to see if it should be ignored or not, based on name, file size and crc.
// returns TRUE if file should be skipped, false if not.
// database of skippable files is built off the settings and tools->ignore database
// on entry, filename has already been extracted by the caller
bool CDOSCenterDlg::skipafile(CStringW filename, CString ext, unsigned int crc, unsigned int fileSize)
{
	filename.MakeLower();
	ext.MakeLower();
	CStringW fullFilename = filename + L"." + ext;
	if (theApp.m_ignoreOn == false)
		return false;

	if ((theApp.m_ignoreMinFileSize != -1) && (fileSize < theApp.m_ignoreMinFileSize))
		return true;

	// full filename check, with matching CRC or wildcard.
	if (theApp.m_ignoreFilename.count(fullFilename) != 0)
	{
		if ((theApp.m_ignoreFilename[fullFilename] == crc) || (theApp.m_ignoreFilename[fullFilename] == 0xffffffff))
			return true;
	}

	// extension check, with matching CRC or wildcard.
	if (theApp.m_ignoreExt.count(ext) != 0)
	{
		if ((theApp.m_ignoreExt[ext] == crc) || (theApp.m_ignoreExt[ext] == 0xffffffff))
			return true;
	}

	if (theApp.m_ignoreName.count(filename) != 0)
	{
		if ((theApp.m_ignoreName[filename] == crc) || (theApp.m_ignoreName[filename] == 0xffffffff))
			return true;
	}


#if 0 // old brute force method going through the array.
	for (int i=0; i<theApp.m_ignoreTheseCount; i++)
	{
		// check for filename or any filename
		if ((filename == theApp.m_ignoreThese[i].filename.MakeLower() ) || (theApp.m_ignoreThese[i].filename == "*"))
		{
			// check extension, or any extension
			if ( (ext == theApp.m_ignoreThese[i].ext.MakeLower() ) || (theApp.m_ignoreThese[i].ext == "*") )
			{
				if ( (theApp.m_ignoreThese[i].crc == -1) || (theApp.m_ignoreThese[i].crc == crc) ) // any CRC or match?
					return true;
			}

		}
	}
#endif
	return false;
}

// user has right clicked inside the zip file list window
// display a pop up window with available options (rename, scan, merge, etc)
void CDOSCenterDlg::OnNMRclickLeftSideList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// positions in left popup menu item
#define MENU_MERGE 4
#define MENU_SCAN 3
#define MENU_COMMIT 7
#define MENU_FIXZIP 8

	*pResult = 0;
	CNewFilenameDlg dlg;
	CSettingsandTools settings;
	int nIndex=-1;
	int lastDeleted=0;
	CStringW source,dest;
	CStringW fromClipboard;
	CStringW tmp;
	int count=0;
	dizInfo m_dizInfo;


	CPoint point;                                            
     ::GetCursorPos(&point); //where is the mouse?


	CMenu menu; //lets display our context menu :) 

    DWORD dwSelectionMade;                                       
    VERIFY(menu.LoadMenu(IDR_MENU1) );  

	CMenu *pmenuPopup = menu.GetSubMenu(0);


	 // no selection?  no menu
	nIndex = theApp.m_leftSideList.GetNextItem(-1, LVNI_SELECTED);
	if (nIndex == -1)
		return;

	if (!theApp.m_commitMode)
	{
		pmenuPopup->RemoveMenu(MENU_COMMIT, MF_BYPOSITION); 
	}


	// gray merged selection if only 1 file is selected.
	while (1) 
		{
			count++;
			nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
			if (nIndex == -1)
				break;
		}
	if ( (count < 2) || (count > 100) )
	 pmenuPopup->EnableMenuItem(MENU_MERGE, MF_GRAYED+MF_BYPOSITION);	// kill the merge option if only 1 selected file


	nIndex = -1;	// reset index

	if ( (theApp.m_datfileCount == 0) && (!m_autoAdd) ) //(theApp.m_DATfileLoaded == false)
	{
		pmenuPopup->EnableMenuItem(MENU_SCAN, MF_GRAYED+MF_BYPOSITION);	// kill the scan option if no dat file has been loaded
		pmenuPopup->EnableMenuItem(MENU_FIXZIP, MF_GRAYED+MF_BYPOSITION);	// kill the fix option if no dat file has been loaded
	}
	ASSERT(pmenuPopup != NULL);                                       

	 
	dwSelectionMade = pmenuPopup->TrackPopupMenu( (TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD), point.x, point.y, this);                                


	 if (dwSelectionMade == ID__RENAMEFROMCLIPBOARD)
	 {
		char * buffer = NULL;
		//open the clipboard to get the new filename
		if ( OpenClipboard() ) 
		{
			HANDLE hData = GetClipboardData( CF_TEXT );
			char * buffer = (char*)GlobalLock( hData );
			fromClipboard = buffer;
			GlobalUnlock( hData );
			CloseClipboard();
		}
		if ( (fromClipboard.Right(4) != ".zip") && (fromClipboard.Right(4) != ".ZIP") )
			fromClipboard+=".zip";
		nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
		source = theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL);
		dlg.m_txt = fromClipboard;
		// popup an edit box
		dlg.DoModal();
		if (dlg.m_ok == 1)
		{
			dest = dlg.m_txt;
			dest.Replace(L"\t",L" ");
		// go do the rename
			if (!MoveFile(m_SourcePath+source, m_SourcePath+dest))
				AfxMessageBox(L"Error renaming file! "+m_SourcePath+source, MB_ICONEXCLAMATION, 0);
			else
			{
			//updateZipList(); // can't do this-if we've scanned a lot of files, we lose all that data!
				theApp.m_leftSideList.SetItemText(nIndex, FILENAMECOL, dest);
				theApp.m_leftSideList.SetItemText(nIndex,NEWFILENAMECOL,L"");
			}
		}
	}

	 if (dwSelectionMade == ID_RENAME_RENAMEBYHAND)
		renameZipFileWithPopUpDialog();

	 
	 // automagic rename of "suggested" title in the 2nd column.
	if (dwSelectionMade == ID_RENAME_RENAMESELECTED)
	{
		CString sScore;
		CString subPath;
		int score;
		int renamed=0;

		while (1) 
			{
				nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
				if (nIndex == -1)
					break;
				sScore = theApp.m_leftSideList.GetItemText(nIndex,SCORECOL);
				swscanf_s(sScore, L"%d",&score);
				if ( (score >= theApp.m_minScoreForAutoRename) && 
					(theApp.m_leftSideList.GetItemText(nIndex,NEWFILENAMECOL).GetLength() > 0) && 
					(theApp.m_leftSideList.GetItemText(nIndex,NEWFILENAMECOL) != "perfect!" ) && 
					(theApp.m_leftSideList.GetItemText(nIndex,NEWFILENAMECOL) != "no good matches") )
				{
					source = theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL);
					subPath = source.Left(source.ReverseFind('\\'));
					if (subPath.GetLength() != 0)
						subPath+="\\";
					dest = theApp.m_leftSideList.GetItemText(nIndex,NEWFILENAMECOL);				
					
						if (!MoveFile(m_SourcePath+source, m_SourcePath+subPath+dest))
							AfxMessageBox(L"Error renaming file! "+m_SourcePath+source, MB_ICONEXCLAMATION, 0);
					else
						{
							theApp.m_leftSideList.SetItemText(nIndex,FILENAMECOL,subPath+dest);
							theApp.m_leftSideList.SetItemText(nIndex,NEWFILENAMECOL,L"");
							renamed++;
						}

						// rename a file in captures folder too.
						source.Replace(L".zip",L".7z");
						dest.Replace(L".zip",L"[Screenshots].7z");
						MoveFile(m_SourcePath+source, m_SourcePath+subPath+dest);
				}
			}
		if (renamed ==0)
			AfxMessageBox(L"Warning, no files were renamed.  Check the \"auto rename score\" value in settings and tools.", MB_ICONINFORMATION, 0);
	}

	 if (dwSelectionMade == ID__DELETE)
	 {
		// count number of items in list-popup verify message.
		count=0;
		while (1) 
			{
				nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
				if (nIndex == -1)
					break;
				count++;
			}
		tmp.Format(L"Delete %d file(s)?", count);
		int choice = AfxMessageBox(tmp, MB_ICONWARNING | MB_YESNO, 0);
			if ( choice == IDNO )
				return;

		nIndex = -1;	// ok then, delete away!

		while (1)
		{
			nIndex = theApp.m_leftSideList.GetNextItem(-1, LVNI_SELECTED);
			if (nIndex == -1)
				break;
			else
			{
				source = theApp.m_leftSideList.GetItemText(nIndex, FILENAMECOL);
				remove((CT2A)(m_SourcePath + source));
				theApp.m_leftSideList.DeleteItem(nIndex);
			}
		}
	 }

	 if (dwSelectionMade == ID__SCAN)
	 {
		 m_autoAddScan = m_autoAdd;
		 scanSelectedZips();
		 m_autoAddScan = false;
	 }

	 if (dwSelectionMade == ID__MERGE)
	 {
		 mergeZipFiles();
	 }

	 if (dwSelectionMade == ID__RENAME_FORGET)
	 {
		 while (1) // find the selection
		 {
			 nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
			 if (nIndex == -1)
				 break;
			 theApp.m_leftSideList.SetItemText(nIndex, NEWFILENAMECOL, L"no good matches");
			 theApp.m_leftSideList.SetItemText(nIndex, COLORCOL, COLOR_BLACK);
		 }
	 }

	 if (dwSelectionMade == ID_MOVE_HOME)
	 {
		 moveSelectedFiles(theApp.m_collectionPath);
	 }
	 if (dwSelectionMade == ID_MOVE_BROWSE)
	 {
		 CFileStuff fileStuff;
		 // open a "browse to folder" dialog
		 if (fileStuff.UserBrowseFolder(theApp.m_defaultMovePath, L"Move file(s) to...") != TRUE)
			 return;

		 tmp = theApp.m_defaultMovePath;
		 if (tmp.Right(1) != "\\")
			 tmp += "\\";
		 moveSelectedFiles(tmp);
	 }

	 if (dwSelectionMade == ID__UNZIPTOTEMP)
	 {

		 nIndex = -1;
		 nIndex = theApp.m_leftSideList.GetNextItem(-1, LVNI_SELECTED);
		 source = theApp.m_leftSideList.GetItemText(nIndex, FILENAMECOL);
		 unzipAFile(m_SourcePath + source, theApp.m_dosboxPath);

	 }

	 //	if (dwSelectionMade == ID__YEARSCAN)
	 //TODO		doYearScan();

	 if (dwSelectionMade == ID__REFRESH)
		 updateZipList();

	 //	if (dwSelectionMade == ID__FIXZIPFILE)
	 //TODO		fixAZip();

	 if (dwSelectionMade == ID__COMMIT)
	 {

		 if (!theApp.m_MDBfileLoaded)
		 {
			 settings.OnBnClickedLoadMDBfile();
			 if (!theApp.m_MDBfileLoaded)
				 return;
		 }
		while (1) // find the selection
		{
			nIndex = -1; // reset index every time through the loop since we're moving the file at the end, thus no current index.
			nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
			if (nIndex == -1)
				break;
			// generate a file_id.diz
			source = theApp.m_leftSideList.GetItemText(nIndex, FILENAMECOL);
			//if (!checkForDiz(m_SourcePath + source))
			//{
				if (make_diz(source, m_dizInfo) != true)
					break;
			//}
			// now check the database for a match, or add it as new.
			if (file2DB(source))
			{
				// and move the file into the archive
				if (moveZipIntoTDCArchive(source))
				{
					theApp.m_leftSideList.DeleteItem(nIndex);
				}
			}
			else
				break;
		} // selection(s)
	}

	 pmenuPopup->DestroyMenu(); 
}



// if there is an existing file_id.diz, open it and display it to figure out if we should overwrite it.
bool CDOSCenterDlg::checkForDiz(CStringW filename)
{
	bool retVal = false;
	CZipArchive zip;
	FILE* inFile;
	CString fileData = L"";
	CFileIDDizEditDlg dlg;
	CFileStuff fileStuff;

	zip.Open(filename);
	ZIP_INDEX_TYPE index = zip.FindFile(_T("file_id.diz"));
	if (index != ZIP_FILE_INDEX_NOT_FOUND)
	{
		// extract and display 
		zip.ExtractFile(index, theApp.m_tmpWorkingFolder);
		inFile = fopen(theApp.m_tmpWorkingFolder + "\\file_id.diz", "r");

		if (inFile != NULL)
			fileStuff.loadFile(inFile, fileData);

		// close input file
		if (inFile != NULL)
			fclose(inFile);

		dlg.m_dizTxt = fileData;
		if ((dlg.m_dizTxt.Find(L"\r\n") == -1) && (dlg.m_dizTxt.Find(L"\n") != -1))
		{
			dlg.m_dizTxt.Replace(L"\n", L"\r\n");
		}
		dlg.m_fileNameViewer = L"Nuke this file_id.diz?  Press Cancel for no.";
		if (dlg.DoModal() == 2)	// cancel button hit?
			retVal = true;
		else
		{	// yes, delete file_id.diz
			zip.RemoveFile(index);
			zip.CommitChanges();
		}
	}
	
	zip.Close();
	return retVal;
}

bool CDOSCenterDlg::make_diz(CStringW filename, dizInfo &m_dizInfo)
{
	CStringW name, flags, version, year, publisher, genre, dizname, language;
	CStringW diz;
	CStringW zipfilename;
	CdizInfo fdiz;	// file_id.diz structure class
	FILE* outFile;

	flags = L"Flags:";
	language = L"Language/Country:";
	version = L"Version:";
	year = L"Year:";
	publisher = L"Publisher:";
	genre = L"Genre(s):";
	name = L"Name:";
	if (filename.Find(L".ZIP") != -1)
	{
		AfxMessageBox(L"Please rename "+filename, MB_ICONEXCLAMATION, 0);
		return false;
	}

	zipfilename = theApp.extractFilenameWOPath(filename);


	if (fdiz.zipName2Vars(zipfilename, false, m_dizInfo) != true)
		return false;
	
	diz = m_dizInfo.filename+L"\r\n";
	diz+= name+m_dizInfo.name+L"\r\n";
	diz+= version+m_dizInfo.version+L"\r\n";
	diz+= language+m_dizInfo.languageTxt+L"\r\n";
	diz+= flags+m_dizInfo.flagsTxt+L"\r\n";
	diz+= year+m_dizInfo.year+L"\r\n";
	diz+=publisher+m_dizInfo.publisher+L"\r\n";
	diz+=genre+m_dizInfo.genre+L"\r\n";
	
	CFileIDDizEditDlg dlg;


	dlg.m_dizTxt = diz;
	dlg.m_fileNameViewer = zipfilename;
	if (dlg.DoModal() == 2)	// cancel button hit?
		return false;
#if 0 // want to display, but not create/inject file_id.diz to make sure the filename has been generated correctly.	
	// pull in any changes
	diz = dlg.m_dizTxt;
	
	CStringW fileData=L"";
	FILE* inFile;
  
	CString dizFile = theApp.m_tmpWorkingFolder;
	

	CStringA strToCovert(zipfilename);
	char* str = (char *)(LPCSTR)strToCovert;
	if (is_utf8(str))
	{
		_wfopen_s(&outFile, dizFile + "\\file_id.diz", L"w, ccs=UTF-8");
		diz.Replace(L"\r\n", L"\r");
		if ( outFile != NULL )
			fwrite(diz, sizeof( wchar_t ), diz.GetLength(), outFile );
	}
	else
	{
		outFile = fopen(dizFile + "\\file_id.diz", "wb");
		if ( outFile != NULL )
			fwrite((CT2A)diz, sizeof( char ), diz.GetLength(), outFile );
	}

	if ( outFile != NULL )
	{
		fclose(outFile);
		outFile=NULL;
	}

	dizFile += "\\file_id.diz";
	filename.Insert(0,m_SourcePath);
	add2zip(filename, dizFile);

	CFile::Remove(dizFile);
	inFile = fopen(theApp.m_tmpWorkingFolder+"\\file_id.diz","r");
	if ( inFile != NULL )
	{
		AfxMessageBox(L"Cannot delete file_id.diz.  Probably RO attribute", MB_ICONEXCLAMATION, 0);
		fclose(inFile);
	}
#endif
	return true;
}

// ask user for new filename, rename it
void CDOSCenterDlg::renameZipFileWithPopUpDialog()
 {
		int nIndex=-1;
		CString source, dest, subPath;
		CNewFilenameDlg dlg;

		nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
		source = theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL);
		subPath = source.Left(source.ReverseFind('\\'));
		if (subPath.GetLength() != 0)
			subPath+="\\";
		dlg.m_txt = source;
		if ( (theApp.m_leftSideList.GetItemText(nIndex,NEWFILENAMECOL) != "") && (theApp.m_leftSideList.GetItemText(nIndex,NEWFILENAMECOL) != "no good matches") 
		  && (theApp.m_leftSideList.GetItemText(nIndex,NEWFILENAMECOL) != "perfect!")  )
			dlg.m_txt = subPath+theApp.m_leftSideList.GetItemText(nIndex,NEWFILENAMECOL);
			
		dlg.DoModal();
		// popup an edit box
		if (dlg.m_ok == 1)
		{
			dest = dlg.m_txt;
			dest.Replace(L"\t",L" ");
			dest.Trim();



			if (!MoveFile(m_SourcePath+source, m_SourcePath+dest))
				AfxMessageBox(L"Error renaming file! "+m_SourcePath+source, MB_ICONEXCLAMATION, 0);
			else
			{
		//	updateZipList();  // can't do this-if we've scanned a lot of files, we lose all that data!
				theApp.m_leftSideList.SetItemText(nIndex,FILENAMECOL,dest);
				theApp.m_leftSideList.SetItemText(nIndex,NEWFILENAMECOL,L"");
			}
		}

}


// huck a zipped file's contents to a folder
// warning!  current folder is set (and left) in destination, so don't try to delete the folder without moving out.
bool CDOSCenterDlg::unzipAFile(CStringW zipFilename, CStringW destination)
{
	HZIP hz;
	ZIPENTRYW ze;
	int numitems;
	ZRESULT status;

	SetCurrentDirectory(destination);
    hz = XOpenZip(((void*)(LPCWSTR)zipFilename), 0, ZIP_FILENAME);
    GetZipItemW(hz, -1, &ze);
	numitems=ze.index;
	for (int i=0; i<numitems; i++)
	{ 
		GetZipItem(hz,i,&ze);
		status = UnzipItem(hz,i,ze.name,0,ZIP_FILENAME);
		if ( (status != ZR_OK) && (status != ZR_NOFILE) ) // NOFILE means we're overwriting an existing file, which we do in the case of collisions, so that's ok.
		{
	        AfxMessageBox(L"Error unzipping file!\n"+zipFilename, MB_ICONEXCLAMATION, 0);
			XCloseZip(hz);
			return false;
		}
	}
	XCloseZip(hz);
	return true;
}

// create a new zip file of zipFilename of the contents of sourceFolder
bool CDOSCenterDlg::zipAFile(CStringW zipFilename, CStringW sourceFolder)
{
	CSADirRead zipDir;
	CSADirRead::SAFileVector::const_iterator Files; // index thingyabob into file array
	CStringW filename;
	CStringW targetFilename;

	HZIP hz = CreateZip((void*)(LPCWSTR)zipFilename,0,ZIP_FILENAME);
	// now we have the path+filename we want to work with.
	// scan all the files in this selected folder.
	zipDir.ClearDirs();
	zipDir.ClearFiles();
	// get a list of all the files and folders
	zipDir.GetDirs(sourceFolder, true);
    zipDir.GetFiles(L"*.*");

	CSADirRead::SAFileVector &files = zipDir.Files();
	Files = files.begin();
	while ( Files != files.end())
	{
		filename = (*Files).m_sName;
		targetFilename = filename;
		targetFilename.Delete(0, sourceFolder.GetLength());
		ZipAdd(hz, (LPCWSTR)targetFilename,  (void*)(LPCWSTR)filename,0,ZIP_FILENAME);
		Files++;
	}

    XCloseZip(hz);
	return true;
}

// add a file targetFile to an existing zip zipFilname
// extracts the path off the targetFile if it exists
bool CDOSCenterDlg::add2zip(CStringW zipFilename, CString targetFile, bool storeFolder)
{
	CStringW fileName;
	CZipArchive zip;
	
	fileName = theApp.extractFilenameWOPath(targetFile);

	// if the archive doesn't exist yet, create one to open.
	GetFileAttributes(zipFilename); // from winbase.h
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(zipFilename) && GetLastError() == ERROR_FILE_NOT_FOUND)
	{
		//File not found
		zip.Open(zipFilename, CZipArchive::zipCreate);
	}
	else
	{
		// create a new archive
		zip.Open(zipFilename, CZipArchive::zipOpen);
	}
	// set the compression method
	zip.SetCompressionMethod(CZipCompressor::methodDeflate);

	ZIP_INDEX_TYPE index = zip.FindFile(fileName);
	if (index != ZIP_FILE_INDEX_NOT_FOUND)
	{
		AfxMessageBox(L"This file already exists in the zip!\r\n" + targetFile, MB_ICONWARNING, 0);
		zip.Close();
		return false;
	}

	// by default the targetFile has a full path prepended to it.  We typically don't care to store that too.
	if (storeFolder)	
	{
		zip.AddNewFile(targetFile);
	}
	else
	{
		zip.AddNewFile(targetFile, fileName);
	}
	zip.Close();
	return true;
}

// replacement for the below, eventually.
void CDOSCenterDlg::zip2map2(CStringW zipFilename, bool dontadjustpaths)
{
	WORD dosDate;
	WORD dosTime;
	ROMdata tempROMdata;
	CStringW filename;
	CStringW folder, tmp, ext;
	CZipArchive zip;
	CSettingsandTools CnT;
	
	ZIP_INDEX_TYPE index=0;

	
	if (!zip.Open(zipFilename))
	{
		if (!theApp.m_quietmode)
			AfxMessageBox(L"There's something goofy with this file. - can't open it!\r\n" + zipFilename, MB_ICONWARNING, 0);
		return;
	}

	
	theApp.m_zipMap[0].filename = zipFilename;
	theApp.m_zipMap[0].ROMmap.erase(theApp.m_zipMap[0].ROMmap.begin(), theApp.m_zipMap[0].ROMmap.end());
	theApp.m_zipMap[0].totalBytes = 0;
	theApp.m_zipMap[0].skippedFiles = 0;
	theApp.m_zipMap[0].skippedFilesSize = 0;
	theApp.m_zipMap[0].fileCount = 0;

	// while we're here, look to see if this zip file is buried.
	m_buriedZipFolder = L"";
	folder = zip[index]->GetFileName();
	if (folder.Find('\\') != -1)
	{	// ok, first file is inside a folder, get the name of it
		folder = folder.Left(folder.Find('\\') + 1);
		// make sure everything else is inside that folder.
		for (index = 0; index < zip.GetCount(); index++)
		{
			tmp = zip[index]->GetFileName();
			if (tmp.Left(folder.GetLength()) != folder)
			{
				folder = L"";
				break;
			}
		}
		m_buriedZipFolder = folder;
	}




	// for each rom in the actual zip we've just peeked at...
	// load them into a temporary map so we can check 'em against the .dat
	for (index = 0; index < zip.GetCount(); index++)
	{
		// store the roms in a scratch cmap
		filename = zip[index]->GetFileName();
		unsigned int crc = zip[index]->m_uCrc32;
		unsigned int size = zip[index]->m_uUncomprSize;
		filename.Replace('/', '\\');

		if (!dontadjustpaths)
		{
			if (!m_buriedZipFolder.IsEmpty())
			{
				filename.Replace(m_buriedZipFolder, L"");
			}
		}

		// if CRC is 0, create a fake CRC based off the filename. This non-CRCable files to still be uniquely identified.
		if ((crc == 0))
		{
			if (filename.Right(1) == '\\') // is this a folder only?  Skipit.
			{
				theApp.m_zipMap[0].skippedFiles++;
				//theApp.m_zipMap[0].ROMmap[filename].status |= STATUS_ROM_SKIPPED;
				continue;
			}
			tmp = filename;
			tmp.MakeLower();
			crc = CnT.hashAString((CT2A)tmp, 0);
		}

		if (!filename.IsEmpty())
		{
			dosDate = zip[index]->m_uModDate;
			dosTime = zip[index]->m_uModTime;

			//FileTimeToDosDateTime((FILETIME*)g, (LPWORD)&dosDate, (LPWORD)&dosTime);
			theApp.m_zipMap[0].ROMmap[filename].name = filename;  // don't even need this, since the filename is the index.
			theApp.m_zipMap[0].ROMmap[filename].size = zip[index]->m_uUncomprSize; // > m_uLocalUncomprSize;
			theApp.m_zipMap[0].ROMmap[filename].crc = crc;
			theApp.m_zipMap[0].ROMmap[filename].status = 0;
			theApp.m_zipMap[0].ROMmap[filename].date = (dosDate << 16) + dosTime;
			theApp.m_zipMap[0].ROMmap[filename].infoData = 0;
			theApp.m_zipMap[0].totalBytes += zip[index]->m_uUncomprSize;
			theApp.m_zipMap[0].fileCount++;
		}

		// see if this file is to be ignored.
		tmp = filename;
		ext = extractExtension(tmp);
		if (skipafile(tmp, ext, crc, size))
		{
			theApp.m_zipMap[0].skippedFiles++;
			theApp.m_zipMap[0].skippedFilesSize += theApp.m_zipMap[0].ROMmap[filename].size;
			theApp.m_zipMap[0].ROMmap[filename].status |= STATUS_ROM_IGNORED;
		}


	}
	zip.Close();
}

// based on the status field in the zipmap, display some hopefully useful information.
void CDOSCenterDlg::displayZipFileInfoText(void)
{
	ROMdata lookupROMdata;		// more temporary storage for the lookup
	CStringW tmp;
	CStringW filename;
	CStringW romName, ext;
	int nIndex = -1;
	int count = 0;
	int pos;
	int status = 0;
	int datPtr;
	CString s_crc;
	CString color;
	
	// only allow 1 item to be displayed
	while (1)
	{
		nIndex = theApp.m_zipDetailsList.GetNextItem(nIndex, LVNI_SELECTED);
		if (nIndex == -1)
			break;
		count++;
	}
	if (count > 1)
		return;
	
	nIndex = theApp.m_zipDetailsList.GetNextItem(nIndex, LVNI_SELECTED);
	filename = theApp.m_zipDetailsList.GetItemText(nIndex, 1);
	ext = theApp.m_zipDetailsList.GetItemText(nIndex, 2);	// extension
	ext.Trim();
	if (!ext.IsEmpty())
		filename += L"." + ext;
	
	tmp = L"";
	
	romName = theApp.m_zipMap[0].ROMmap[filename].correctFilename;
	status = theApp.m_zipMap[0].ROMmap[filename].status;
	datPtr = theApp.m_zipMap[0].zipDatPtr;

	if ((status & STATUS_ROM_ELSEWHERE) == 0)
		tmp += "Unknown file.\r\nThe CRC32 of this file does not exist in the entire .dat\r";
	

	switch (status & STATUS_ROM_PERFECT_MATCH)
	{
		case 0:	// covered above
			break;
		case STATUS_CRC_MATCH:			// 1
		case STATUS_CRC_MATCH + STATUS_TIMESTAMP_MATCH:		// 9
			tmp += L"This filename is incorrect according to the DAT file.\rShould be:" + romName;
			break;
		case STATUS_SIZE_MATCH:			// 02
		case STATUS_SIZE_MATCH + STATUS_FILENAME_MATCH + STATUS_TIMESTAMP_MATCH:	// 0xe
			tmp += L"Size matches, but not CRC; file is modified compared to .dat.\r";
			break;
		case STATUS_CRC_MATCH + STATUS_SIZE_MATCH:		// 3
			tmp = L"This file has a CRC+size match, but filename+timestamp are wrong.\rShould be:" + romName;
			break;
		case STATUS_FILENAME_MATCH:			// 04
			tmp += L"Filename matches, but nothing else!\r";
			break;
		case STATUS_CRC_MATCH + STATUS_FILENAME_MATCH:	// 5,d - CRC match,but size mismatch.  should be impossible.
		case STATUS_CRC_MATCH + STATUS_FILENAME_MATCH + STATUS_TIMESTAMP_MATCH:	
			tmp += L"CRC match, but file size is correct.\rThis should be impossible!\r";
			break;
		case STATUS_SIZE_MATCH + STATUS_FILENAME_MATCH:							// 6
			tmp += L"CRC mismatch for this file.\rFilename and size are correct.\r";
			tmp += L"File has likely been modified.\r";
			break;
		case STATUS_CRC_MATCH + STATUS_SIZE_MATCH + STATUS_FILENAME_MATCH:		// 7
			tmp = L"Everything except the timestamp is correct.\r";
			break;
		case STATUS_TIMESTAMP_MATCH:		// 8
			tmp = L"Only the timestamp matched the dat!\r";
			break;
		case STATUS_TIMESTAMP_MATCH + STATUS_SIZE_MATCH:	// 0xa
			tmp = L"File has been modified and renamed compared to the .dat file.\r";
			break;
		case STATUS_CRC_MATCH + STATUS_SIZE_MATCH + STATUS_TIMESTAMP_MATCH:		// 0xb
			tmp = L"File has likely been renamed.\rShould be:" + romName;
			break;
		case STATUS_TIMESTAMP_MATCH + STATUS_TIMESTAMP_MATCH:	// 0xc
			tmp = L"Filename correct, size+CRC mismatch\r";
			break;
		case STATUS_ROM_PERFECT_MATCH:		// 0xf
			tmp = L"This file is a perfect match!";
			break;
	}

	//int status = theApp.m_tmpzipList[nIndex].status;
	if (status & STATUS_ROM_EXTRA)
	{
		tmp += L"This file is extra in this zip when compared to the dat.\r\r";
	}
	if (status & STATUS_ROM_IGNORED)
	{
		tmp += L"This type of file is in your ignore list.\r\r";
	}



	// add in tzipcheck

	//if (theApp.m_zipDetailsList.GetItemText(nIndex, COLORCOL) == COLOR_RED_IGNORED)
	//	tmp += "According to the DAT, this file is missing from the scanned zip, but it's in your ignore list so we don't really care.\r";	
	//if (theApp.m_zipDetailsList.GetItemText(nIndex, COLORCOL) == COLOR_GREEN_PERFECT)	// could also check status bits
	//	tmp += "CRC and filename match target DAT file\r";
	//if (theApp.m_zipDetailsList.GetItemText(nIndex, COLORCOL) == COLOR_BROWN)
	//	tmp += "CRC mismatch for this file. Filename is correct.\r";

	// If this file is found elsewhere in the dat, let the user optionally switch to viewing that .dat entry.
	// if the file is missing from the zip, don't let the user switch to that dat entry because it makes no sense.
	if ( (status & STATUS_ROM_ELSEWHERE) && ((status & STATUS_ROM_MISSING) == 0) )
	{
		int zipPosPtr = theApp.m_zipMap[0].ROMmap[filename].infoData;
		CStringW entryName = theApp.m_datZipList[zipPosPtr].filename;
		tmp += L"This file is found in a different entry in the DAT:\r\n" + entryName;
		
		tmp += L"\r\nSwitch to this zip?"; // repurposed romname to mean different zip file for this condition
		int choice = AfxMessageBox(tmp, MB_ICONWARNING | MB_YESNO, 0);
		if (choice == IDYES)
		{
			nIndex = theApp.m_leftSideList.GetNextItem(-1, LVNI_SELECTED);
			// fixme- change to the dat index and update the color field.
			tmp = theApp.m_leftSideList.GetItemText(nIndex, COLORCOL);
			pos = tmp.Find(L"|");
			if (pos != -1)
			{
				color = tmp.Left(pos);
				tmp.Format(L"%x", zipPosPtr);
				color += L"|" + tmp;
			}
			theApp.m_leftSideList.SetItemText(nIndex, COLORCOL, color);
			theApp.m_leftSideList.SetItemText(nIndex, NEWFILENAMECOL, entryName);
			OnNMDblclkZipFileList(NULL, NULL);
		}
		tmp = "";	// avoid the message box below
	}

	if ((status & STATUS_ROM_MISSING + STATUS_ROM_ELSEWHERE) == STATUS_ROM_MISSING + STATUS_ROM_ELSEWHERE)
	{
		tmp += "This file is missing from this zip.\r";
		unsigned int size = theApp.m_zipMap[0].ROMmap[filename].size;
		unsigned long long crcsize = (unsigned long long)(size << 32);
		crcsize |= theApp.m_zipMap[0].ROMmap[filename].crc;
		datPtr = doesROMExistinDat(crcsize);
		if (datPtr != -1)
		{
			CStringW f = theApp.m_datZipList[datPtr].filename;
			int count = theApp.m_crcMap[crcsize].collisionCount;
			CString txt;
			txt.Format(L"%d", count);
			tmp += L"This file exists in at least " + txt;
			tmp += L" other zips, such as\r";
			tmp += f;
		}
	}

	//change this to looking at status and show where in the dat the file is found.
	// why am i not using status here?
	if (theApp.m_zipDetailsList.GetItemText(nIndex, COLORCOL) == COLOR_RED)
		tmp = "This file is missing from the zip file.\rNo other dat entries for that file found.";

	if (!tmp.IsEmpty())
		AfxMessageBox(tmp, MB_ICONINFORMATION, 0);
}


// settings and tools popup+exchange
void CDOSCenterDlg::OnBnClickedSettingsbtn()
{
	m_autoAdd = false;
	CSettingsandTools dlg;
	// pass the current working path into the dialog just in case the user wants to create a .dat file.
	dlg.m_SourcePath = m_SourcePath;		// path of files we're scanning
	dlg.DoModal();
	theApp.m_tmpWorkingFolder +=L"\\DOSCenter";
	if ((m_datview) && (!theApp.m_datParsed))	// just (re)loaded a .dat file
		 populateDATFileDataView();
	if (dlg.m_autoadd == true)
		m_autoAdd = true;
}
void CDOSCenterDlg::populateDATFileDataView()
{
	CString zipName;
	CString romCount;
	CString color;
	// if we are sitting on a zip file when this button is pressed, see if we can find it in the dat view

	theApp.m_leftSideDATList.DeleteAllItems();	// clean up the left side list

	theApp.m_datZipListit = theApp.m_datZipList.begin();

	while (theApp.m_datZipListit != theApp.m_datZipList.end())
	{
		zipName = theApp.m_datZipListit->second.filename;	// 2nd element is the 2nd thing stored in this map. weird stuff.
		romCount.Format(L"%d",theApp.m_datZipListit->second.fileCount);
		color=COLOR_BLACK;
		if ((theApp.m_datZipListit->second.status & TZIPPED) == TZIPPED)
			color=COLOR_PBROWN;
		if (zipName.GetLength() > 0)
		{
			theApp.m_leftSideDATList.AddItem(color, zipName, romCount);	
		}
		theApp.m_datZipListit++;
	}
	theApp.m_datParsed = true;
	theApp.m_leftSideDATList.Sort(1,true);	// sort by filename
}

void CDOSCenterDlg::OnBnClickedApplyignore()
{
	theApp.m_ignoreOn = (((CButton*)GetDlgItem(IDC_APPLYIGNORE))->GetCheck()==1);
}


///////////////////////////////////////////////////////////////////////////////////////////
// ZIP MERGE
// take multiple zip files, check for CRC collisions, unzip and then rezip.
// TODO: add an option to delete source files when finished?
// TODO: add logging capabilities so that we know we've done the job properly.
bool CDOSCenterDlg::mergeZipFiles()
{
//	FILE* logFile;	//todo - i wanted to have a log file of all the input files+paths so that it can be verified against the final zip file
	CString logData;
	CZipArchive zipDest;
	CZipArchive zipSource;
	ZIP_INDEX_TYPE zipindex;

	int nIndex=-1;
	int nFileCount;
	ZIPENTRYW info;
	ZIPMERGE* zipMerge;
	zipMerge = new _ZIPMERGE[1];	
	CStringW zipfilename;
	CStringW filename;	// rom filename
	CStringW targetZipName;
	CStringW tmp, ext;
	CFileStuff files; 
	CStringW collideName[5];
	int collideCount=0;
	unsigned int crc;
	long size;
	bool found;

	CProgressDlg progDlg;

	// get destination filename - make it just a variation of the 1st filename in the selected pool
	nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
	targetZipName = theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL);
	logData = L"\r\nMerging zip files:\r\n"+targetZipName;
	ext = extractExtension(targetZipName);

	
	while (1)  // get the last filename of the set to be merged
	{
		tmp = theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL);
		logData+=L"\r\n"+tmp;
		nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
		if (nIndex == -1)
			break;
	}
	logData+=L"\r\n";


	targetZipName +=L"_"+tmp.Left(tmp.GetLength() - ext.GetLength());
	targetZipName +=L"_merged.zip";

	logData+=L"Destination file: "+targetZipName+L"\r\n";

	// maybe popup a new filename dialog, with options to delete original files.
	
	nIndex = -1; // reset
	progDlg.Init(PROG_MODE_TIMER, 1000, L"Checking for CRC/Filename collisions...");
	
	// scan all selected files and make sure there are no collisions (same filename+diferent crc)
	// list collisions.
	while (1) 
	{
		nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
		if (nIndex == -1)
			break;
		else
		{	// put all files inside this zip into the array
			zipfilename = theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL);

			m_uz = XOpenZipU((void*)(LPCWSTR)(m_SourcePath+zipfilename), 0, ZIP_FILENAME); 

	
			// gather all the info for each file in this zip so we can look for filename/crc collisions
			GetZipItemW(m_uz, -1, &info);
			nFileCount=info.index;
			
			for (int i=0; i<nFileCount; i++)	// for each file in this zip...
			{
				GetZipItemW(m_uz, i, &info);
				filename = info.name;
				filename.Replace('/', '\\');
				if (zipMerge[0].ROMmap[filename].name.GetLength() != 0)
				{
					// collision - check crc
					if ( (zipMerge[0].ROMmap[filename].crc != info.crc) || (zipMerge[0].ROMmap[filename].name != filename) )
					{
						found=false;

						// see if we know about this collision and have already overridden the quit decision
						for (int i=0; i<collideCount; i++)
						{
							if (filename == collideName[i])
								found=true;
						}
						
						if (!found)
						{
							tmp = L"Filename/crc collision in zip files! "+filename+L"\r\nContinue?";
							int choice = AfxMessageBox(tmp, MB_ICONWARNING | MB_YESNO, 0);
							if ( choice == IDNO )
							{
								progDlg.DestroyWindow();
								XCloseZip(m_uz);
								delete[] zipMerge;
								return false;
							}
						
							collideName[collideCount] = filename;
							if (collideCount < 5)
								collideCount++;
						}
					}
				}
				
				else // new/unknown file
				{
					zipMerge[0].ROMmap[filename].name = filename;
					zipMerge[0].ROMmap[filename].size = info.unc_size;
					zipMerge[0].ROMmap[filename].crc = info.crc;
					zipMerge[0].ROMmap[filename].status = nIndex;	// which zip file this rom came from
				}
            } // each ROM in the zip
		} // each zip
		XCloseZip(m_uz);
	} // for each selected zip

	progDlg.UpdateText(L"Unzipping files...");

	// create a new target (merged) archive
	zipDest.Open(m_SourcePath+targetZipName, CZipArchive::zipCreate);
	nIndex = -1;


	// open each archive
	while (1)
	{
		nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
		if (nIndex == -1)
			break;
		else
		{
			// open source zip
			zipfilename = theApp.m_leftSideList.GetItemText(nIndex, FILENAMECOL);
			zipSource.Open(m_SourcePath+zipfilename);
			zipMerge[0].it = zipMerge[0].ROMmap.begin();
			// pull out each rom name from the map
			while (zipMerge[0].it != zipMerge[0].ROMmap.end())
			{
				filename = zipMerge[0].it->first;
				if (zipMerge[0].ROMmap[filename].status == nIndex)	// pick out the files from this zip (they are not in order)
				{
					zipindex = zipSource.FindFile(filename);
					if (zipindex == ZIP_FILE_INDEX_NOT_FOUND)
					{
						AfxMessageBox(L"Error unzipping file! " + zipfilename +L"/r/nUnable to locate "+filename, MB_ICONEXCLAMATION, 0);
						progDlg.DestroyWindow();
						delete[] zipMerge;
						return false;
					}
					zipDest.GetFromArchive(zipSource, zipindex);
				}
					zipMerge[0].it++;
			}
			zipSource.Close();
		} // next source zip
	} 
	zipDest.Close();



	// verify step
	// unzip the target zip again and compare its filename+crc with previous map.
	//
	progDlg.UpdateText(L"Verifying new zip...");

	m_uz = XOpenZipU((void*)(LPCWSTR)(m_SourcePath+L"\\"+targetZipName), 0, ZIP_FILENAME); 
	GetZipItemW(m_uz, -1, &info);
	nFileCount=info.index;


	zipMerge[0].it = zipMerge[0].ROMmap.begin();
	while( zipMerge[0].it != zipMerge[0].ROMmap.end() ) 
	{
		filename = zipMerge[0].it->first;
		
		crc = zipMerge[0].ROMmap[filename].crc;
		size = zipMerge[0].ROMmap[filename].size;
		found=false;

		for (int i=0; i<nFileCount; i++)	// gather all the info for each file in this zip and display it on the right hand window
		{
			GetZipItemW(m_uz, i, &info);
			tmp = info.name;
			tmp.Replace('/', '\\');
			if ( (tmp == filename) && (info.crc == crc) && (size == info.unc_size) )
			{
				zipMerge[0].it++;
				found=true;
				break;
			}
		}

		if (!found)
		{
			for (int ii=0; ii<collideCount; ii++)
			if (collideName[ii] == filename)
			{
				found = true;
				zipMerge[0].it++;
			}

		}

		if (!found)
		{
			// oh shit.  not a known collision
			logData+=L"\r\n**Error unzipping file:"+zipfilename;
			tmp.Format(L"\r\nexpected CRC 0x%x not found!",crc);
			AfxMessageBox(L"Error! zip/unzip contents not equal.\r\n"+filename+tmp, MB_ICONEXCLAMATION, 0);
			break;
		}
	} // for every rom in zip file
	XCloseZip(m_uz);

	theApp.m_leftSideList.AddItem(COLOR_BLACK, targetZipName, L"", L"", L"", L"", L"");	// column 0 is color, zipfilename, new filename, score, match, missing, unknown
	m_zipCount++; // one new zip file in our left side list

	delete[] zipMerge;
	progDlg.DestroyWindow();
	return true;
}

// move any files highlighted in the left side list to tgtPath
void CDOSCenterDlg::moveSelectedFiles(CString tgtPath)
{
	int nIndex;
	CStringW zipfilename;
	CStringW source;
	nIndex=-1;
	while (1) 
	{
		nIndex = theApp.m_leftSideList.GetNextItem(-1, LVNI_SELECTED);
		if (nIndex == -1)
			break;
		else
		{
			source = theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL);
			zipfilename = source;
			
			// "total DOS center" update
			// moving a file "home" to staging?  add it to the database.
			//if (tgtPath == theApp.m_collectionPath)
			//{
			//	OnNMDblclkDatFileList(0, 0);			
			//	add2database();
			//}
			
			if (zipfilename.Find('\\') != -1) 
				zipfilename = zipfilename.Right(zipfilename.GetLength() - (zipfilename.ReverseFind('\\')) -1 );

			if (!MoveFile((m_SourcePath+source), (tgtPath+zipfilename)))
			{
				int err = GetLastError();
				AfxMessageBox(L"Error moving file!? "+m_SourcePath+source, MB_ICONEXCLAMATION, 0);
				return;
			}
			theApp.m_leftSideList.DeleteItem (nIndex);
		}
	}
}

void CDOSCenterDlg::OnLvnKeydownZipdetailslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if (pLVKeyDow->wVKey == 0x2e) // delete key - delete and entry from a zip file
	{
		deleteFileFromZip();
		OnNMDblclkZipFileList(NULL, NULL); // refresh the zip listing
	}

	if ( (HIBYTE(GetKeyState(VK_CONTROL))) && (pLVKeyDow->wVKey == 65) )	// ctrl-A
	{
		for	(int i=0; i<theApp.m_leftSideList.GetItemCount(); i++)
			theApp.m_zipDetailsList.SetItemState(i, LVNI_SELECTED,LVIS_SELECTED);
	}
	
	if (pLVKeyDow->wVKey == 0x71) // F2 - rename
	{
		renameFileInZip();
		OnNMDblclkZipFileList(NULL, NULL); // refresh the zip listing
	}
}

void CDOSCenterDlg::OnLvnKeydownLeftsidelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	*pResult = 0;

	int nIndex=-1;
	int lastDeleted=0;
	CStringW source,dest;
	CString tmp;
	int count=0;

	if (pLVKeyDow->wVKey == 0xdc)	// Enter key, remapped to \  argh!  enter doesn't go into this routine! wtf?
		OnNMDblclkZipFileList(0, 0);


	if ( (HIBYTE(GetKeyState(VK_CONTROL))) && (pLVKeyDow->wVKey == 65) )	// ctrl-A
	{
		for	(int i=0; i<theApp.m_leftSideList.GetItemCount(); i++)
			theApp.m_leftSideList.SetItemState(i, LVNI_SELECTED,LVIS_SELECTED);
		
	}

	if (pLVKeyDow->wVKey == 0x71) // F2 key
	{
		// count number of items in list-popup verify message.
		count=0;
		while (1) 
			{
				nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
				if (nIndex == -1)
					break;
				count++;
			}
		if (count > 0)
			renameZipFileWithPopUpDialog();
	}

	if (pLVKeyDow->wVKey == 0x2e) // delete key
	{	
		// count number of items in list-popup verify message.
		count=0;
		while (1) 
			{
				nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
				if (nIndex == -1)
					break;
				count++;
			}
		tmp.Format(L"Delete %d file(s)?", count);
		int choice = AfxMessageBox(tmp, MB_ICONWARNING | MB_YESNO, 0);
			if ( choice == IDNO )
				return;

		nIndex=-1;	// ok then, delete away!
		
		while (1) 
		{
			nIndex = theApp.m_leftSideList.GetNextItem(-1, LVNI_SELECTED);
			if (nIndex == -1)
				break;
			else
			{
				source = theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL);
				remove((CT2CA)(m_SourcePath+source));
				theApp.m_leftSideList.DeleteItem(nIndex);
			}
		}
	} // delete key

	if (pLVKeyDow->wVKey == 0x53)	// S key
		scanSelectedZips();

	if (pLVKeyDow->wVKey == 0x4d)	// M key
		mergeZipFiles();

	if (pLVKeyDow->wVKey == 0x74) // F5 - refresh dir
		updateZipList();
}

// generate a full report of files
void CDOSCenterDlg::OnBnClickedMissinglist()
{
	FILE* outFile;
	CStringW perfect=L"";
	CStringW missingFiles=L"";
	CStringW missingZipFiles=L"";
	CStringW renameOnly=L"";
	CStringW crcCollision=L"";
	CStringW extraFiles=L"";
	CStringW timeTxt;
	CStringW output=L"";
	CStringW txt;
	CSettingsandTools snt;
	CHaveMissDlg hmd;
	int	extraFilesCount=0;
	int crcCCount=0;
	int renameCount=0;
	int perfectCount=0;
	int unknownCount=0;
	int missingCount=0;
	int missingZips=0;
	CdizInfo fiz;	// file_id.diz structure class
	
	dizInfo m_dizInfo;


	CTime time = CTime::GetCurrentTime();
	CString d = time.Format("%A, %B %d %Y");
	CString t = CTime::GetCurrentTime().Format("%H:%M");
	CString datenTime;
	datenTime = d + " at " + t;
	timeTxt = L"Report generated on: " + datenTime;

	for (unsigned int i=0; i<theApp.m_datfileCount; i++)
	{
		// populate the file_id.diz variable list from this filename
		//snt.zipName2Vars(theApp.m_datZipList[i].filename);

		//if (!hmd.yearInSelectedList(snt.m_dizInfo.year))
		//	continue;

		if ( (theApp.m_datZipList[i].status & ~0xff000000) == STATUS_PERFECT)	// only available after scanning
		{
			perfect+=theApp.m_datZipList[i].filename+L"\n";
			perfectCount++;
		}
//		if (tempZIPdata.status == STATUS_RENAME_ZIP_ONLY)
//		{
//			renameOnly+=theApp.m_fileData[i].realFilename;
//			renameOnly+="-->";
		//	renameOnly+=tempZIPdata.filename+"\r\n";
//			renameCount++;
//		}
		if ((theApp.m_datZipList[i].status & ~0xff000000) == STATUS_CRC_COLLISION)
		{
			crcCollision+=theApp.m_datZipList[i].filename+L"\n";
			crcCCount++;
		}	
		if ((theApp.m_datZipList[i].status & ~0xff000000) == STATUS_EXTRAFILES)
		{
			extraFiles+=theApp.m_datZipList[i].filename+L"\n";
			extraFilesCount++;
			// if really detailed reporting requested
			//generateExtraFileReport(i, extraFiles);
		}	
		if ((theApp.m_datZipList[i].status & ~0xff000000) == STATUS_MISSINGFILES)
		{
			missingFiles+=theApp.m_datZipList[i].filename+L"\n";
			missingCount++;
		}

		if ((theApp.m_datZipList[i].status & ~0xff000000) == 0)	// totally missing
		{
			missingZipFiles+=theApp.m_datZipList[i].filename+L"\n";
			missingZips++;
		}

	}	
	
	output=timeTxt;
	txt.Format(L"\nYou are missing %d of %d known files.\n",missingZips,theApp.m_datfileCount);
	output+=txt;
	txt.Format(L"You have %d perfect zips out of %d known files.\n",perfectCount,theApp.m_datfileCount);
	output+=txt;
	txt.Format(L".zip filename is incorrect: %d\n",renameCount);
	output+=txt;
	txt.Format(L"CRC collisions: %d\n",crcCCount);
	output+=txt;
	txt.Format(L".zip has extra files: %d\n",extraFilesCount);
	output+=txt;
	txt.Format(L".zip is missing files: %d\n",missingCount);
	output+=txt;

	
	if (theApp.m_detailedReport)
	{
		output +="Detailed Report\n";
		output +="-----------------------------------\n";
		output +="--        Missing ZIP Files      --\n";
		output +="-----------------------------------\n";
		output+=missingZipFiles;
		
		output +="-----------------------------------\n";
		output +="--        Rename these           --\n";
		output +="-----------------------------------\n";
		output +="use DOSCenter to rename zips that display as BROWN in the left hand window\n";
		output +=renameOnly;
		
		output +="-----------------------------------\n";
		output +="--        CRC collisions         --\n";
		output +="-----------------------------------\n";
		output +=crcCollision;

		output +="-----------------------------------\n";
		output +="--        ZIP has extra files    --\n";
		output +="use DOSCenter to examine these zips to know which files are extra\n";
		output +="-----------------------------------\n";
		output +=extraFiles;
		
		// display extra/missing files here too
		output +="-----------------------------------\n";
		output +="--        ZIP is missing files   --\n";
		output +="use DOSCenter to examine these zips to know which files are missing\n";
		output +="-----------------------------------\n";
		output +=missingFiles;
		
		if (theApp.m_reportPerfects)
		{
			output +="-----------------------------------\n";
			output +="--        Perfect Files          --\n";
			output +="-----------------------------------\n";
			output+=perfect;
		}

	}

	if (1)	// checkbox...
	{
		output +="-----------------------------------\n";
		output +="--        Scan Results           --\n";
		output +="-----------------------------------\n";

		// read the results right off the screen
		int nIndex = 0;
		CStringW zipfilename;
		CStringW tgtFilename;
		while (1)
		{
			// get destination filename - make it just a variation of the filename we've selected
			//nIndex = theApp.m_leftSideList.GetNextItem(0nIndex, LVNI_SELECTED);
			
			zipfilename = theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL);
			tgtFilename = theApp.m_leftSideList.GetItemText(nIndex,NEWFILENAMECOL);
			if (tgtFilename.IsEmpty())
				break;
			// trim off any folder
			zipfilename = theApp.extractFilenameWOPath(zipfilename);
			output+=zipfilename+L"| "+tgtFilename+L"\n";
			nIndex++;
		}
	}


	// if generating a flashFXP queue, this is the command line to run:
	// FLASHFXP.EXE -c2 -go DCupdate.fqf
	// contents of DCupdate.fqf:
	// <file type> [1] <direction> [1] <source ftp://>                     [1] <target ftp://> [1] <source file> [1] <target file> [1] <file size> [CR/LF]
	//       0     [1]       5     [1][2]40900.6690024537[1]208.42.141.191 [1][2]-             [1]/DOSGuy/year/filename.zip[1]destination path+filename[1]filesize[CR/LF]
	//
	// need all missing, missing files added to the fqf file.  missingZipFiles, missingFiles
	//


	//outFile = fopen(theApp.m_tmpWorkingFolder+"\\DCreport.txt","wb");
	_wfopen_s(&outFile, theApp.m_dosboxPath + "\\DCreport.txt", L"w, ccs=UTF-8");
	if ( outFile != NULL )
		fwrite(output, sizeof(wchar_t), output.GetLength(), outFile);
	
	if ( outFile != NULL )
	{
		fclose(outFile);
		outFile=NULL;
	}
	AfxMessageBox(L"File " + theApp.m_dosboxPath + "\\DCreport.txt created.", MB_ICONINFORMATION, 0);
}

void CDOSCenterDlg::OnBnClickedDatzipflip()
{
	m_datview = !m_datview;
	theApp.m_zipDetailsList.DeleteAllItems();	// clean up whatever was on the right side
	if (m_datview)
	{
		CString sourceName;
		int	nIndex = theApp.m_leftSideList.GetNextItem(-1, LVNI_SELECTED);
		sourceName = theApp.m_leftSideList.GetItemText(nIndex,NEWFILENAMECOL);
		// get first filename if 2nd filename is "perfect!"  
		if ( (sourceName == "perfect!") || (sourceName == "no good matches") )
			sourceName = theApp.m_leftSideList.GetItemText(nIndex,FILENAMECOL);

		if ((m_datview) && (!theApp.m_datParsed))
			populateDATFileDataView();	// if we have not put all the data into the view, do so now. (only needs to be done once)

		theApp.m_leftSideList.ShowWindow(0);
		theApp.m_leftSideDATList.ShowWindow(1);
		((CButton*)GetDlgItem(IDC_DATZIPFLIP))->SetWindowText(L"Zip view");

		
		// find the selected filename in this new view
		// highlight+goto our matched item. 
		for (unsigned int i=0; i<theApp.m_datfileCount; i++)
		{
			theApp.m_leftSideDATList.SetItemState(i, 0, LVIS_SELECTED); // unselect each
			if (theApp.m_leftSideDATList.GetItemText(i,1) == sourceName)
				nIndex=i;
		}	
			if (nIndex != -1)
			{
				theApp.m_leftSideDATList.EnsureVisible(nIndex, 1);
				theApp.m_leftSideDATList.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
				OnNMDblclkDatFileList(NULL,NULL);
			}
	}
	else
	{
		theApp.m_leftSideList.ShowWindow(1);
		theApp.m_leftSideDATList.ShowWindow(0);
		((CButton*)GetDlgItem(IDC_DATZIPFLIP))->SetWindowText(L"DAT view");
	}
}

// user has 2x clicked on the dat viewer window - display the files inside this dat entry
void CDOSCenterDlg::OnNMDblclkDatFileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CString zipFilename;
	CString filename, ext, date, size, crc;
	int	nIndex;
	int romCount;
	WORD dosDate;
	WORD dosTime;
	ROMdata tempROMdata;	// temporary storage for the lookup
	//ZIPdata tempZIPdata;		// temporary storeage for the zip file (from the .dat) the data came from
	

	// no selection?  abort
	nIndex = theApp.m_leftSideDATList.GetNextItem(-1, LVNI_SELECTED);
	if (nIndex == -1)
		return;
	
	// get the name of the zip file that the scanner is suggesting we use.
	zipFilename = theApp.m_leftSideDATList.GetItemText(nIndex,FILENAMECOL);	// dat filename

	// scan through the cmap to find this particular entry
	theApp.m_datZipListit = theApp.m_datZipList.begin();
	while (theApp.m_datZipListit != theApp.m_datZipList.end())
	{
		if (zipFilename == theApp.m_datZipListit->second.filename)
			break;
		theApp.m_datZipListit++;
	}
	// update the zip contents list
	theApp.m_zipDetailsList.DeleteAllItems();

	romCount = theApp.m_datZipListit->second.fileCount;
	for (int i=0; i<romCount; i++)
	{
		filename = theApp.m_datZipListit->second.ROMsMap[i].name;
		ext = extractExtension(filename);
		crc.Format(L"%x",theApp.m_datZipListit->second.ROMsMap[i].crc);
		size.Format(L"%d",theApp.m_datZipListit->second.ROMsMap[i].size);
		int q = theApp.m_datZipListit->second.ROMsMap[i].date;
		dosDate = (WORD)(theApp.m_datZipListit->second.ROMsMap[i].date >> 16);
		dosTime = (WORD)theApp.m_datZipListit->second.ROMsMap[i].date & 0xffff;
		CTime time(dosDate, dosTime);
		date =time.Format( L"%Y/%m/%d %H:%M:%S" );

		theApp.m_zipDetailsList.AddItem(COLOR_BLACK,filename, ext, size, date, crc);	
	}
}
void CDOSCenterDlg::CopyToClipboard(const char* stringbuffer)
{
   //try to open clipboard first
   if (!OpenClipboard())
   {
      return;
   }
  
   //clear clipboard
   EmptyClipboard();
 
   HGLOBAL clipbuffer;
   char * buffer;
   
   //alloc enough mem for the string;
   //must be GMEM_DDESHARE to work with the clipboard
   clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(stringbuffer)+1);
   buffer = (char*)GlobalLock(clipbuffer);
   strcpy(buffer, LPCSTR(stringbuffer));
   GlobalUnlock(clipbuffer);
   
   //fill the clipboard with data
   //CF_TEXT indicates that the buffer is a text-string
   ::SetClipboardData(CF_TEXT,clipbuffer);
   //close clipboard as we don't need it anymore
   CloseClipboard();
}

// popup menu for when user right clicks something onthe zip details window side.
void CDOSCenterDlg::OnNMRclickRightSideList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	int count=0;
	CString tmp;
	FILE* outFile;
	int nIndex=-1;
//	int pos;
	CString color;
	CPoint point;                                            
     ::GetCursorPos(&point); //where is the mouse?


	CMenu menu; //lets display our context menu :) 

    DWORD dwSelectionMade;                                       
    VERIFY(menu.LoadMenu(IDR_MENU2) );  

	CMenu *pmenuPopup = menu.GetSubMenu(0);


	 // no files in the list?  no menu
	if (theApp.m_zipDetailsList.GetItemText(0,1) == "")
		return;

	// gray the "move files up" option if this zip is not in a nested folder
	if (theApp.m_zipIsFoldered == false)
		pmenuPopup->EnableMenuItem(ID_ZIPCOMMANDS_MOVEFILESUP, MF_GRAYED+MF_BYPOSITION);

	// gray view and compare selection if many files are selected.
	count=0;
	while (1) 
	{
		nIndex = theApp.m_zipDetailsList.GetNextItem(nIndex, LVNI_SELECTED);
		if (nIndex == -1)
			break;
		count++;
	}
	if (count >1)
	{
		pmenuPopup->EnableMenuItem(3, MF_GRAYED+MF_BYPOSITION);	// kill the compare selection
		pmenuPopup->EnableMenuItem(4, MF_GRAYED+MF_BYPOSITION);	// kill the view selection
	}
	
	if (theApp.m_DATfileLoaded == false)
	{
//		pmenuPopup->EnableMenuItem(1, MF_GRAYED+MF_BYPOSITION);	// kill the add2database option
		pmenuPopup->EnableMenuItem(2, MF_GRAYED+MF_BYPOSITION);	// kill the add2datfile option
	}

	if (!theApp.m_commitMode)
		pmenuPopup->RemoveMenu(8, MF_BYPOSITION); // hide the add to SPAM option

	nIndex = theApp.m_zipDetailsList.GetNextItem(nIndex, LVNI_SELECTED);
	if (theApp.m_zipDetailsList.GetItemText(0, COLORCOL) == COLOR_PBROWN)	// if we've dropped our warning in the list, skip over it.
		nIndex--;
	if (nIndex<0)
		return;

	ASSERT(pmenuPopup != NULL);                                       

	 
	dwSelectionMade = pmenuPopup->TrackPopupMenu( (TPM_LEFTALIGN|TPM_LEFTBUTTON| TPM_NONOTIFY|TPM_RETURNCMD), point.x, point.y, this);                                



	 if (dwSelectionMade == ID__FILEINFO)
	 {
		displayZipFileInfoText();
	 }




	 if ( (dwSelectionMade == ID_TOFILE_NOCOLOR) || (dwSelectionMade == ID_TOCLIPBOARD_NOCOLOR) )
	 {
		CString logData=theApp.m_currentlyTaggedZipFile+"\r\n";
		while (	theApp.m_zipDetailsList.GetItemText(count,1) != "") 
		{
			if  (theApp.m_zipDetailsList.GetItemText(count,4) != "-alternate-") 
			{
				logData +=theApp.m_zipDetailsList.GetItemText(count,1)+L".";	// filename
				logData +=theApp.m_zipDetailsList.GetItemText(count,2)+L" ";	// ext
				logData +=theApp.m_zipDetailsList.GetItemText(count,3)+L" ";	// size
				logData +=theApp.m_zipDetailsList.GetItemText(count,4)+L" ";	// date
				logData +=theApp.m_zipDetailsList.GetItemText(count,5)+L" ";	// crc
				logData +=L"\r\n";
			}
			count++;
		}
	 	if (dwSelectionMade == ID_TOCLIPBOARD_COLOR)
		{
			CopyToClipboard((CT2CA)logData);
			return;
		}
		outFile = fopen(theApp.m_tmpWorkingFolder+"\\ziplist.txt","wb");
		fwrite(logData, sizeof( char ), logData.GetLength(), outFile );
		fclose(outFile);
		AfxMessageBox(L"File listing exported to "+theApp.m_tmpWorkingFolder+"\\ziplist.txt" , MB_ICONINFORMATION, 0);
	 }
	
	if ( (dwSelectionMade == ID_TOFILE_COLOR) || (dwSelectionMade == ID_TOCLIPBOARD_COLOR) )
	{
		//[color=blue]This text is blue[/color]
		CString logData=theApp.m_currentlyTaggedZipFile+"\r\n";
		while (	theApp.m_zipDetailsList.GetItemText(count,1) != "") 
		{
			if  (theApp.m_zipDetailsList.GetItemText(count,4) != "-alternate-") 
			{
				if (theApp.m_zipDetailsList.GetItemText(count,0) ==COLOR_HIDE)
					continue;
				if (theApp.m_zipDetailsList.GetItemText(count,0) ==COLOR_RED)
					logData+="[color=red]";
				if (theApp.m_zipDetailsList.GetItemText(count, 0) == COLOR_GREEN_PERFECT)
					logData+="[color=green]";
				if (theApp.m_zipDetailsList.GetItemText(count,0) ==COLOR_BLACK)
					logData+="[color=black]";
				if (theApp.m_zipDetailsList.GetItemText(count,0) ==COLOR_YELLOW)
					logData+="[color=yellow]";
				if (theApp.m_zipDetailsList.GetItemText(count,0) ==COLOR_BLUE)
					logData+="[color=blue]";
				if (theApp.m_zipDetailsList.GetItemText(count,0) ==COLOR_PURPLE)
					logData+="[color=purple]";
				if (theApp.m_zipDetailsList.GetItemText(count,0) ==COLOR_BROWN)
					logData+="[color=brown]";
				if (theApp.m_zipDetailsList.GetItemText(count,0) ==COLOR_GRAY)
					logData+="[color=gray]";
				
				logData +=theApp.m_zipDetailsList.GetItemText(count,1)+L".";	// filename
				logData +=theApp.m_zipDetailsList.GetItemText(count,2)+L" ";	// ext
				logData +=theApp.m_zipDetailsList.GetItemText(count,3)+L" ";	// size
				logData +=theApp.m_zipDetailsList.GetItemText(count,4)+L" ";	// date
				logData +=theApp.m_zipDetailsList.GetItemText(count,5)+L" ";	// crc
				logData +=L"[/color]\r\n";

			}
			count++;
		}
	 	if (dwSelectionMade == ID_TOCLIPBOARD_COLOR)
		{
			CopyToClipboard((CT2CA)logData);
			return;
		}
		outFile = fopen(theApp.m_tmpWorkingFolder+"\\ziplist.txt","wb");
		fwrite(logData, sizeof( char ), logData.GetLength(), outFile );
		fclose(outFile);
		AfxMessageBox(L"File listing exported to "+theApp.m_tmpWorkingFolder+"\\ziplist.txt" , MB_ICONINFORMATION, 0);
	 }
	

	 // temporarily add this entry to the internal database
	 if (dwSelectionMade == ID__ADDTODATABASE)
	 {
		add2database();
	 }

	// permanently add this entry to the .dat file
	 if (dwSelectionMade == ID__ADDTODATFILE)
	 {
		 add2database();
		 add2datFile();
	 }		

	 if (dwSelectionMade == ID__VIEWFILE)
		 hexViewFile();

	 if (dwSelectionMade == ID__ADDTOIGNORELIST)
		 add2Ignore();

	 if (dwSelectionMade == ID_ZIPCOMMANDS_DELETE)
	 {
		deleteFileFromZip();
		OnNMDblclkZipFileList(NULL, NULL); // refresh the zip listing
	 }

 /*TODO
	
	 if (dwSelectionMade == ID__COMPARE)
		 compare2Files();

*/
	 if (dwSelectionMade == ID_ZIPCOMMANDS_EXTRACTTOZIP)
		 zip2NewFile();
	 
	 if (dwSelectionMade == ID_ZIPCOMMANDS_RENAME)
	 {
		 renameFileInZip();
		 OnNMDblclkZipFileList(NULL, NULL); // refresh the zip listing
	 }

	 if (dwSelectionMade == ID_ZIPCOMMANDS_MOVEFILESUP)
	 {
		 unNestZipFile();
		 OnNMDblclkZipFileList(NULL, NULL); // refresh the zip listing
	 }

	 if (dwSelectionMade == ID_ZIPCOMMANDS_EXTRACTHERE)
		 extractHere();
	 if (dwSelectionMade == ID__ADDTOSPAM)
		 addFile2Spam();

	 pmenuPopup->DestroyMenu(); 

}
// add the currently open zip file to the .dat file
// this is kinda slow and stupid, since it opens and closes the .dat file everytime it's called.
void CDOSCenterDlg::add2datFile()
{
	CSettingsandTools tools;
	tools.Add2DatFile();
}
// from the right side zip details list, add an entry to the ignore list.
void CDOSCenterDlg::add2Ignore()
{
	//CFileStuff fileStuff;
	CScoringSetupDlg setupDlg;
	CString filename, ext, scrc;
	int pos, nIndex;
	//FILE* outFile;
	//CString fileData;
	nIndex= -1;
	nIndex = theApp.m_zipDetailsList.GetNextItem(nIndex, LVNI_SELECTED);

	// make sure file was loaded- may not need this.
	pos = theApp.m_ignoreFileData.Find(L"min_filesize = ");
	if (pos == -1)
	{
		AfxMessageBox(L"Failed to open DCIgnore.ini!  Should be in the same path as DOSCenter.exe.", MB_ICONEXCLAMATION, 0);
		return;
	}

	while (1)
	{
		filename = theApp.m_zipDetailsList.GetItemText(nIndex,1);	// filename
		filename = theApp.extractFilenameWOPath(filename);
		ext = theApp.m_zipDetailsList.GetItemText(nIndex, 2);
		scrc = theApp.m_zipDetailsList.GetItemText(nIndex, 5);

		setupDlg.addEntry2ignorelist(filename, ext, scrc);

		// point to next item		
		nIndex = theApp.m_zipDetailsList.GetNextItem(nIndex, LVNI_SELECTED);
		if (nIndex == -1)
			break;
	}

	theApp.m_reloadIgnore = true;
	//setupDlg.populateIgnoreList();
	setupDlg.prepIgnoreListForSave();
	setupDlg.saveIgnoreList();
}

// user has 2x clicked on a file inside a zip - popup a view dialog and display the contents
void CDOSCenterDlg::OnNMDblclkZipList(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nIndex;
	CString filename;

	nIndex = theApp.m_zipDetailsList.GetNextItem(-1, LVNI_SELECTED);
	if (theApp.m_zipDetailsList.GetItemText(nIndex, COLORCOL) == COLOR_RED)	// don't allow us to 2x click a missing file.
		return;

	filename = theApp.m_zipDetailsList.GetItemText(nIndex, 1);
	filename += L"." + theApp.m_zipDetailsList.GetItemText(nIndex, 2);	// extension

	filename.TrimRight(L".");

	extractFileFromZip(m_SourcePath + theApp.m_currentlyTaggedZipFile, filename, theApp.m_tmpWorkingFolder + L"\\tmpfile.xyz");
	
	hexViewAFile(theApp.m_tmpWorkingFolder + L"\\tmpfile.xyz", filename);
}

bool CDOSCenterDlg::extractFileFromZip(CString zipFilename, CString filename, CString targetFilename)
{
	// open an existing archive
	CZipArchive zip;
	zip.Open(zipFilename);
	ZIP_INDEX_TYPE index = zip.FindFile(filename);
	if (index == ZIP_FILE_INDEX_NOT_FOUND)
	{
		zip.Close();
		return false;
	}
	// separate out path from file
	CString path = theApp.extractPathFromFilename(targetFilename);
	CString tmpfile = theApp.extractFilenameWOPath(targetFilename);
	zip.ExtractFile(index, path, false, tmpfile); // _T("tmpfile.xyz"));
	zip.Close();
	// make sure the file is not set to read-only.
	SetFileAttributes(targetFilename, GetFileAttributes(targetFilename) & ~FILE_ATTRIBUTE_READONLY);

	return true;
}

void CDOSCenterDlg::hexViewFile(void)
{
	OnNMDblclkZipList(0, 0);
}

// popup a file viewer for the supplied file. (typically c:\doscenter\temp\tempfile.xyz)
void CDOSCenterDlg::hexViewAFile(CString filename, CString displayfilename)
{
	FILE* inFile;
	CString fileData;
	CFileStuff file;
	CString txt, ascii;
	CProgressDlg progDlg;
	CPopupViewerDlg dlg;
	bool showAscii = true;
	unsigned int size;

	// now open the file and display it in the popup viewer
	inFile = fopen((CT2A)filename,"rb");
	fseek(inFile, 0, SEEK_END); // seek to end of file
	size = ftell(inFile); // get current file pointer
	fseek(inFile, 0, SEEK_SET); // seek back to beginning of file

	char* buffer = new char[size+1];
	memset(buffer,0,size+1);


	if (inFile != NULL)
		fread(buffer,size,1 ,inFile);

	// close input file
	if (inFile != NULL)
			fclose(inFile);	

	if (size > 1024 * 1024)
	{	
		progDlg.Init(PROG_MODE_TIMER, 1000, L"Creating display...");
	}


	// check for all ASCII characters.  popup the correct viewer depending on data.
	for (long i=0; i<size; i++)
		if ((unsigned char)buffer[i] < 9)	// ascii
		{
			showAscii = false;
			break;
		}

	if (showAscii)
	{
		CPopupViewerTxtDlg dlg;

		fileData = buffer;

		dlg.m_viewerTxt = fileData;
		dlg.m_fileNameViewer = displayfilename;
		dlg.DoModal();
	}
	else	// hex view - it would be super sweet to be able to exit the strings view in the hex view and come back to the hex viewer.  Since the hex viewer doesn't actually have
			// the data in it, we can't really parse the data again inside thehex viewer to launch the strings view.  we'd need to have the buffer created globally for that, which would
			// be doable...  or could do the strings stuff anyway in anticipation that the user is going to click the button, then pass the data into the hex viewer to be launched from there.
	{


		// convert data into nice hex display with offset and ascii bytes.
		for (unsigned int q=0; q<size; q++)
		{
			if ((q & 0xf) == 0x00)	// offset
			{
				txt.Format(L"%08x", q);
				fileData+=txt+L"  ";
			}
			txt.Format(L"%02X", (unsigned char)buffer[q]);
			fileData+=txt+L" ";

			if ( ( (unsigned char)buffer[q] < 31) || ( (unsigned char)buffer[q] >0x80) )	// ascii
				ascii+=".";
			else 
				ascii+=buffer[q];

			if ((q & 0xf) == 0xf) // add ascii and CRLF
			{
				fileData+=L"  "+ascii+L"\r\n";	
				ascii="";
			}
			if ( ((q & 0xf) !=0xf) && ((q & 7) == 7)) 
				fileData+="- ";
		}
	
		dlg.m_viewerTxt = fileData;
		dlg.m_fileNameViewer = displayfilename;
		dlg.DoModal();

		//
		// display the strings viewer from within the hex viewer.  neato.
		// 
		if (dlg.m_bStrings == true)
		{
			CPopupViewerTxtDlg dlg;
			fileData = "";
			txt = "";
			int count = 0;
				
			for (unsigned int q=0; q<size; q++)
				{
					if ( ((unsigned char)buffer[q] > 31) && ((unsigned char)buffer[q] < 127) )	// ascii
					{
						txt+=buffer[q];
						count++;
					}
					else
						if (count > 4)	// add string to our output
						{
							fileData+=txt+L"\r\n";
							count = 0;
							txt = "";
						}
						else	// just start over
						{
							count = 0;
							txt = "";
						}


				}
	
			dlg.m_viewerTxt = fileData;
			dlg.m_fileNameViewer = filename;
			dlg.DoModal();
		} // end of strings view

	} // hex display

	if (size > 1024 * 1024)
	{	
		progDlg.DestroyWindow();
	}

	delete []buffer;

}

void CDOSCenterDlg::OnBnClickedDatquery()
{
	datQueryDlg dlg;	
	dlg.DoModal();
}

// moves a ready zip from the working directory into the "home" path + year folder.
bool CDOSCenterDlg::moveZipIntoTDCArchive(CStringW filename)
{
	CdizInfo fiz;	// file_id.diz structure class
	dizInfo tmpDiz;
	CString dest; 
	CString newfilename = theApp.extractFilenameWOPath(filename);
	
	fiz.zipName2Vars(filename, 1, tmpDiz);
	dest = theApp.m_collectionPath + tmpDiz.year+ "\\" + newfilename;
	if (!MoveFile(m_SourcePath + filename, dest))
	{
		AfxMessageBox(L"Error moving file! " + m_SourcePath + filename, MB_ICONEXCLAMATION, 0);
		return false;
	}
	return true;
}

// put this file into the database
// first compares a given filename against entries in the mdb file in case we're updating
// pops up a viewer to pick the best match or to add as a new entry.
// writes the database with the new or updated entry.
bool CDOSCenterDlg::file2DB(CStringW filename)
{
	CPickListDlg dlg;
	CSettingsandTools settings;
	ZipInfoMDB tmpZipInfo;
	int idx = 0;

	filename = theApp.extractFilenameWOPath(filename);

	if (theApp.m_ZIPDATEntryNameMDB.Lookup(filename, tmpZipInfo)) // does it exist in the db?
	{
		AfxMessageBox(L"This file is already in the database! ", MB_ICONEXCLAMATION, 0);
		return false;
	}
	// get a list of the top ten matches of full filename and name only.
	compareFile2db(filename,true);
	
	// add to a popup list
	for (int i = 0; i < 10; i++)
	{
		if (m_topTen[i].filename.GetLength() > 0)
		{
			dlg.m_topTwenty[idx] = m_topTen[i];
			idx++;
		}
	}
	compareFile2db(filename, false);
	
	// add these too.
	for (int i = 0; i < 10; i++)
	{
		if (m_topTen[i].filename.GetLength() > 0)
		{
			dlg.m_topTwenty[idx] = m_topTen[i];
			idx++;
		}
	}

	if (dlg.DoModal() == 2)	// cancel button hit?
		return false;
	if (dlg.m_tagetMDBEntry == L"Add as new")
	{
		dlg.m_tagetMDBEntry = L"";
	}
	int editFlag = -1;
	settings.updateDBRecord(editFlag, dlg.m_tagetMDBEntry, filename);
	return true;
}

// attempts to find a name match compared to the MDB.
// can either match entire name, year, publisher or just the name.
void CDOSCenterDlg::compareFile2db(CStringW filename, bool nameOnly)
{
	int onlyMissing = 0;

	float results;
	CdizInfo fiz;	// file_id.diz structure class
	dizInfo tmpDiz;
	int index = 0;
	CStringW name1, name2, tmp;
	name1 = filename;
	initTopTen();
	if (nameOnly)
	{
		fiz.zipName2Vars(filename, 1, tmpDiz);
		name1 = tmpDiz.name;
	}
	
	for (unsigned int l = 0; l< theApp.m_romCountMDB; l++)
	{
		//if ((theApp.m_zipListMDB[l].ftp == 1) && (onlyMissing == 1))
		//	continue;
		tmp = theApp.m_zipListMDB[l].filename; //.dbMap[l].longName;
		
		if (nameOnly)
		{
			fiz.zipName2Vars(tmp, 1, tmpDiz);
			name2 = tmpDiz.name;
		}
		else
			name2 = tmp;
		
		results = compareStrings(name1, name2);
		int resi = (int)(results * 100);
		if (resi > m_topTen[0].score)
		{
			m_topTen[index].filename = tmp;	// save off the zip filename
			m_topTen[index].score = resi;

			sortTopTen();	// put the best of the best at the top.  better matches in the dat should float to the top
			if (index < (TOPARRAYSIZE - 1))
			{
				index++;
			}

		}

	} 
	//AfxMessageBox(m_topTen[0].filename, MB_ICONEXCLAMATION, 0);
}


float CDOSCenterDlg::compareStrings(CString a, CString b)
{	
	USES_CONVERSION;
	CSimilar txtCmp;
	float results, results2;
	char* s = T2A((LPTSTR)(LPCTSTR)a);
	char* t = T2A((LPTSTR)(LPCTSTR)b);

	results = txtCmp.simil(s, t);  // is 1 inside 2?
	results2 = txtCmp.simil(t, s); // is 2 inside 1?
	results = (results + results2);  // the closer we are to 2.0, the better the match
	return results;
}
// drag a new file into a zip file.
// this is exactly backwards.  the setting for the left side window on the GUI editor says "accept files=true" and it doesn't come here when the user drags a file to that side.
// on the other hand, the right side window says "accept files=false" and that gets called here when the file is dragged.  lovely.  whatever, it works.
LRESULT  CDOSCenterDlg::OnDropFiles(WPARAM wParam, LPARAM lParam)
{
	wchar_t	szDroppedFile[MAX_PATH];
	HDROP	hDrop;
	int nFiles = 0;
	CStringW csString;
	int	nIndex = -1;
	CStringW dest;

	CStringW zipfilename;
	CStringW filename;	// rom filename
	CStringW targetZipName;
	CStringW tmp, ext;
	CFileStuff files;


	// do we have multiple files selected on the left?  can't handle that either.
	// ok, we could do multiple zips on the left, but let's not for now.
	while (1)
	{
		nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
		if (nIndex == -1)
			break;
		nFiles++;
	}

	if (nFiles != 1)
		return 0;

	// get destination filename - make it just a variation of the filename we've selected
	nIndex = theApp.m_leftSideList.GetNextItem(nIndex, LVNI_SELECTED);
	targetZipName = theApp.m_leftSideList.GetItemText(nIndex, FILENAMECOL);

	// now add the extra files dragged in.
	hDrop = (HDROP)wParam;
	UpdateData();

	nFiles = DragQueryFile(hDrop,		// Struture Identifier
		-1,			// -1 to Drop more than one file
		szDroppedFile,// Droped File Name
		MAX_PATH);	// Max char

	// for each file, add the path into our zip array.
	for (int i = 0; i<nFiles; i++)
	{
		DragQueryFile(hDrop, i, szDroppedFile, MAX_PATH);
		filename = szDroppedFile;
		add2zip(m_SourcePath + L"\\" + targetZipName, filename, false);
	}

	OnNMDblclkZipFileList(0, 0);
	return 0;
}
void CDOSCenterDlg::addFile2Spam()
{
	//	CString logData;
	int nIndex = -1;
	CStringW zipfilename;
	CStringW filename;	// rom filename
	CStringW targetZipName;
	CFileStuff files;

	zipfilename = m_spamFile;
	if (zipfilename.GetLength() == 0)
		return;

	// this extracts all the selected files into the current working folder
	extractHere();

	nIndex = -1;
	while (1) // add each file highlighted into the target zip
	{
		nIndex = theApp.m_zipDetailsList.GetNextItem(nIndex, LVNI_SELECTED);
		if (nIndex == -1)
			break;

		filename = theApp.m_zipDetailsList.GetItemText(nIndex, FILENAMECOL);
		filename += L"." + theApp.m_zipDetailsList.GetItemText(nIndex, 2);

		//if there's no extension trim it
		filename.TrimRight(L".");

		add2zip(zipfilename, m_SourcePath + L"\\" + filename, false);
		remove((CT2CA)(m_SourcePath + L"\\" + filename));
	}
}

// convert a rom (or zip) status field into a displayable color
// should maybe consider converting this into playing with an uint instead of CString
// color key chart:
// black = completely unknown file.
// red = missing file
// green = file match.  may be discolored based on timestamp and case (fixable)
// brown = crc mismatch, filename match.
// 
// this routine is seriously fubared.
CString CDOSCenterDlg::status2color(int status)
{
	CString color = COLOR_BLACK;

	if ((status & STATUS_ROM_MISSING + STATUS_ROM_ELSEWHERE) == STATUS_ROM_MISSING + STATUS_ROM_ELSEWHERE)
	{
		color = COLOR_MISSING_ELSEWHERE;
		return color;
	}

	if (status & STATUS_ROM_MISSING)
	{
		color = COLOR_RED;
		return color;
	}
	if (status & STATUS_ROM_ELSEWHERE)
	{
		color = COLOR_PURPLE;
		return color;
	}

	if ((status & STATUS_ROM_EXTRA + STATUS_ROM_IGNORED) == STATUS_ROM_EXTRA + STATUS_ROM_IGNORED )
	{
		return COLOR_GRAY;
	}

	/*
	// these are inverted values as calculated here: http://www.drpeterjones.com/colorcalc/
	#define COLOR_HIDE L"0xffffffff"
	#define COLOR_RED L"0x0000ff"
	#define COLOR_RED_IGNORED L"0x9999ff"
	#define COLOR_GREEN L"0x009900"
	//#define COLOR_GREEN2 "0x00ccff"
	#define COLOR_BLUE   L"0xA0522D"
	#define COLOR_LGREEN L"0x009999"
	#define COLOR_PURPLE L"0xff00ff"
	#define COLOR_BROWN_PURPLE L"0xff66AA"
	#define COLOR_BROWN	L"0x006699"
	#define COLOR_GRAY L"0x999999"
	#define COLOR_PBROWN L"0x042690" // tzip color reddish brown
	#define COLOR_TZIPPED 0x770000

	#define ALTERNATE_CRC L"0x80000000"		// flag that this ROM has multiple CRCs

#define STATUS_CRC_MATCH		(1 << 0)
#define STATUS_SIZE_MATCH		(1 << 1)
#define STATUS_FILENAME_MATCH	(1 << 2)
#define STATUS_TIMESTAMP_MATCH	(1 << 3)
#define STATUS_ROM_TZIPPED 		(1 << 4)	// sadly, this rom's timestamp has been wrecked by torrentzip
#define STATUS_ROM_ELSEWHERE 	(1 << 5)	// same file is located in other .dat entries
#define STATUS_ROM_IGNORED 		(1 << 6)	// this file is in the user's ignore list
#define STATUS_ROM_WRONGCASE 	(1 << 7)	// case is incorrect, we know the filename matched.
#define STATUS_ROM_PERFECT_MATCH (STATUS_CRC_MATCH + STATUS_SIZE_MATCH + STATUS_FILENAME_MATCH + STATUS_TIMESTAMP_MATCH)
*/
// as long as the CRC and size match, go green.
// filename, timestamps, case can all be fixed, but make it less green.
// a wrong CRC goes yellow/brown.  if located in another dat entry, go purple+brown.
//

	switch (status & STATUS_ROM_PERFECT_MATCH)
	{
	case 0:
	case STATUS_TIMESTAMP_MATCH:				// timestamp only
		color = COLOR_BLACK; 					// completely unknown file
		break;
	case STATUS_CRC_MATCH:						// 1
	case STATUS_CRC_MATCH + STATUS_SIZE_MATCH + STATUS_TIMESTAMP_MATCH: // b
		color = COLOR_GREEN2;
		break;
	case STATUS_SIZE_MATCH:						// 2
		color = COLOR_BROWN;
		break;
	case STATUS_FILENAME_MATCH:					// 4
	case STATUS_FILENAME_MATCH + STATUS_TIMESTAMP_MATCH:	// c
		color = COLOR_BROWN;
		break;
	case 5:		// CRC match, but not size.
	case STATUS_CRC_MATCH + STATUS_TIMESTAMP_MATCH: // 9
	case 0xd:
		color = COLOR_WTF;
		break;
	case STATUS_FILENAME_MATCH + STATUS_SIZE_MATCH:// 6	likely a hack or modified known file
	case STATUS_SIZE_MATCH + STATUS_TIMESTAMP_MATCH: // a
	case STATUS_FILENAME_MATCH + STATUS_SIZE_MATCH + STATUS_TIMESTAMP_MATCH:	// e
		color = COLOR_BLUE;
		break;
	case STATUS_FILENAME_MATCH + STATUS_CRC_MATCH + STATUS_SIZE_MATCH:	// 07.  probably impossible to get CRC without size.
	case STATUS_CRC_MATCH + STATUS_SIZE_MATCH: 	// 3	is it possible to get a CRC but not size match?
		color = COLOR_GREEN3;
		break;
	case STATUS_ROM_PERFECT_MATCH:	// 0xf
		color = COLOR_GREEN_PERFECT;
		break;
	}
	// modifiers to the above colors
		if (status & STATUS_ROM_WRONGCASE)
		{
			switch (status & STATUS_ROM_PERFECT_MATCH)
			{
			case 0:
			case STATUS_TIMESTAMP_MATCH:				// timestamp only
				color = COLOR_BLACK; 					// unknown file
				break;
			case STATUS_CRC_MATCH:						// 1
			case STATUS_CRC_MATCH + STATUS_TIMESTAMP_MATCH: // 9
			case STATUS_CRC_MATCH + STATUS_SIZE_MATCH + STATUS_TIMESTAMP_MATCH: // b
				color = COLOR_GREEN2;
				break;
			case STATUS_SIZE_MATCH:						// 2
				color = COLOR_BROWN;
				break;
			case STATUS_CRC_MATCH + STATUS_SIZE_MATCH: 	// 3	is it possible to get a CRC but not size match?
				color = COLOR_LGREEN;
				break;
			case STATUS_FILENAME_MATCH:					// 4
			case STATUS_FILENAME_MATCH + STATUS_TIMESTAMP_MATCH:	// c
				color = COLOR_BROWN;
				break;
			case 5:		// CRC match, but not size.
			case 0xd:
				color = COLOR_WTF;
				break;
			case STATUS_FILENAME_MATCH + STATUS_SIZE_MATCH:// 6	likely a hack or modified known file
			case STATUS_SIZE_MATCH + STATUS_TIMESTAMP_MATCH: // a
			case STATUS_FILENAME_MATCH + STATUS_SIZE_MATCH + STATUS_TIMESTAMP_MATCH:	// e
				color = COLOR_BLUE;
				break;
			case STATUS_FILENAME_MATCH + STATUS_CRC_MATCH + STATUS_SIZE_MATCH:	// 07.  probably impossible to get CRC without size.
			case STATUS_ROM_PERFECT_MATCH:	// 0xf
				color = COLOR_GREEN3;
				break;
			}
		}

		//		if ((theApp.m_tmpzipList[i].status & STATUS_ROM_PERFECT_MATCH) == STATUS_ROM_PERFECT_MATCH)
		//			color = COLOR_GREEN_PERFECT;
		//		else // CRC match, filename mismatch
		//			if ((theApp.m_tmpzipList[i].status & STATUS_CRC_MATCH) == STATUS_CRC_MATCH)
		//			color = COLOR_LGREEN;
		//		else // CRC mismatch, filename match
		//			if ((theApp.m_tmpzipList[i].status & STATUS_FILENAME_MATCH) == STATUS_FILENAME_MATCH)
		//			color = COLOR_BROWN;

		// CRC mismatch, filename match, CRC exists elsewhere
		//		if ( ((theApp.m_tmpzipList[i].status & STATUS_ROM_FILENAME_MATCH) == STATUS_ROM_FILENAME_MATCH) && ((theApp.m_tmpzipList[i].status & STATUS_ROM_ELSEWHERE) == STATUS_ROM_ELSEWHERE) )
		//			color = COLOR_BROWN_PURPLE;

		// check if this file is in our ignore list.  if so, color it gray.
		// but only do that if we haven't already matched it.
		//		if (theApp.m_tmpzipList[i].status == STATUS_ROM_IGNORED)
		//			color = COLOR_GRAY;
	
	return color;
}

//fixme = cannot delete zips in utf-8 filename
// esc closes program
// update mdb item should update the CMap
// update mdb should be able to get the recordPosition from the CMAP instead of searching for it.
// hard coded mdb path- store in regisery
// scanner speedup.  checkDatvsZip is very slow.
// progress dlg loses focus and turns to not responding.  
// zip extract to new zip/move to new zip
