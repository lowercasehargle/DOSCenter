
// DOSCenter2.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "DOSCenterDlg.h"
#include "settingsandTools.h"
#include "fileStuff.h"
#include "dizinfo.h"
#include "TDCSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDOSCenterApp

BEGIN_MESSAGE_MAP(CDOSCenterApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDOSCenterApp construction

CDOSCenterApp::CDOSCenterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDOSCenterApp object

CDOSCenterApp theApp;

//dizInfo	m_dizInfo;  // a global

// CDOSCenterApp initialization

BOOL CDOSCenterApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	LPTSTR buff = g_appPath.GetBuffer(MAX_PATH);
	CString tmp;
	CSettingsandTools snt;
	CFileStuff fileStuff;
	GetModuleFileName(NULL, buff, MAX_PATH);
	tmp = buff;
	g_appPath=(tmp.Left(tmp.ReverseFind('\\')));


	CWinAppEx::InitInstance();
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("DOSCenter"));

	CDOSCenterDlg dlg;
	m_detailedReport = false;
	m_reportPerfects = false;
	m_quietmode = false;
	m_commitMode = false;
	snt.m_noPathInDat = true;
	m_zipMap = new _ZIPMAP;

	// Parse command line for standard shell commands, DDE, file open
	//CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);
	CString cmdLine=GetCommandLine();
//	AfxMessageBox(cmdLine, MB_ICONINFORMATION, 0);
	if (cmdLine.Find(L"-quietmode") != -1)
		m_quietmode = true;

	if (cmdLine.Find(L"-makelists") != -1)
	{
		dlg.loadRegistrySettings();
		snt.loadMDB(m_MDBPath);
		snt.makeLists();
		return FALSE; // exit
	}
	if (cmdLine.Find(L"-commitMode") != -1)	// admin mode which enables features just for me
	{
		m_commitMode = true;
	}

	if (cmdLine.Find(L"-makedat") != -1)
	{
		CTime time = CTime::GetCurrentTime();
		CString d = time.Format( "%A, %B %d, %Y" );
		CString t = CTime::GetCurrentTime().Format("%H:%M");
		CString datenTime;
		datenTime = d+"@"+t;

		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);
		CString inidata, header, tempdatpath;
		FILE* inFile;
		FILE* outFile;
		bool isv2 = false;
		unsigned long long n,v;

		// add paths to the .dat file?  Typically no.
		// name "AAARGH! (1988)(Arcadia Systems, Inc.) [Action].zip"
		// -> name "C:\games\files\AAARGH! (1988)(Arcadia Systems, Inc.) [Action].zip"
		// this should really be an optional per path setting, not global.
		if (cmdLine.Find(L"-addpaths") != -1)
			snt.m_noPathInDat = false;
	
		snt.m_recursive = true;	// this is also maybe something to do on a per target
		snt.m_cmdLine = true;
		
		// open and load the ini file
		if (cmdInfo.m_strFileName.Mid(1,1) !=':')
			fopen_s(&inFile, CT2A(g_appPath+"\\"+cmdInfo.m_strFileName),"r");
		else
			fopen_s(&inFile, CT2A(cmdInfo.m_strFileName),"r"); // fopen(cmdInfo.m_strFileName,"r");
		if (inFile != NULL)
		{
			// parse the .ini file to get the goods
			fileStuff.loadFile(inFile, inidata);
			fclose(inFile);
			if (inidata.GetLength() ==0)
				return FALSE;
		}
		// all ini data loaded.  do stuff with it.
		inidata.Replace(L"__DATE__",datenTime);
		

		// get the header if it exists
		// todo- header file probably just needs to be written to a standalone file with a BOM.
		//todo2 - each .dat file needs to be BOMless so when concatenated they will all use the header BOM
		if (inidata.Find(L"<header>") != -1)
		{
			inidata.TrimLeft(L"<header>");
			header = inidata.Left(inidata.Find(L"</header>"));

			if (!header.IsEmpty())
				inidata.Replace(header, L"");
			header.Trim();
			inidata.Replace(L"</header>", L"");
			
		}
		inidata.Trim();
		// is there a storage location for older dats?
		if (inidata.Find(L"<tempdat>") != -1)
		{
			inidata.TrimLeft(L"<tempdat>");
			inidata.Trim();
			tempdatpath = inidata.Left(inidata.Find(L"\n"));
			inidata.TrimLeft(tempdatpath);
			inidata.Trim();
			tempdatpath.Trim();
			if (tempdatpath.Right(1) != L"\\")
				tempdatpath += L"\\";
			inidata.TrimLeft(L"</tempdat>");
			inidata.Trim();
			isv2 = true;
			
			// if we're making dat files on an as-needed basis, create 1 header
			if (!header.IsEmpty())
			{
				_wfopen_s(&outFile, tempdatpath+L"header.txt", L"w, ccs=UTF-8");
				fwrite(header, sizeof(wchar_t), header.GetLength(), outFile);
				fclose(outFile);
				outFile = NULL;
			}
		}
			
		// now go through every target (dat name) and folder (paths)
		while(inidata.GetLength() > 0)
		{
			// skip any comment line
			if (theApp.m_collectionPath.Left(1) ==";")
			{
				tmp = inidata.Left(inidata.Find(L"\n"));
				inidata.Trim(tmp);
				inidata.Trim();
			}

			// get the target .dat filename
			inidata.TrimLeft(L"<target>");
			theApp.m_DATPath = inidata.Left(inidata.Find(L"</target>"));
			theApp.m_DATPath.Trim();
			inidata.Trim();
			inidata.Replace(theApp.m_DATPath,L"");
			inidata.Trim();
			inidata.TrimLeft(L"</target>");
			inidata.Trim();
			if (isv2)
				theApp.m_DATPath.Insert(0, tempdatpath);

			// get the source path to build dat from	
			inidata.TrimLeft(L"<paths>");
			inidata.Trim();
			theApp.m_collectionPath = inidata.Left(inidata.Find(L"</paths>"));
			inidata.TrimLeft(theApp.m_collectionPath);
			theApp.m_collectionPath.Trim();
			inidata.TrimLeft(L"</paths>");
			inidata.Trim();	// this should get us at the start of the next path/target pair
			
		
			// skip creating a .dat file that doesn't need to be updated.
			v=0;
			n=1;	// assume it is.
			if (isv2)
			{
				n = fileStuff.getTimestamp(theApp.m_collectionPath);
				v = fileStuff.getTimestamp(theApp.m_DATPath);
			}
			// if this folder is newer than the .dat, do it.
			if (n > v)
			{
				_wfopen_s(&outFile, theApp.m_DATPath, L"w, ccs=UTF-8");	// crush the existing file
				if (outFile == NULL) // any error?
					return FALSE;
				fclose(outFile);
				outFile = NULL;

				snt.makeDATFile(theApp.m_collectionPath);
					//snt.makeDATFile_express(theApp.m_collectionPath); - nuke this
			}
		} // while ini data to process...
	return FALSE;
	} // -makedat


	//m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

// input: "c:\mypath\mypath2\mypath3\myzipfilename.zip"
// outpt: "myzipfilename.zip"
CStringW CDOSCenterApp::extractFilenameWOPath(CStringW filename)
{
	if (filename.ReverseFind('/') != -1)
		return filename.Right(filename.GetLength() - (filename.ReverseFind('/')) - 1);
	return filename.Right(filename.GetLength() - (filename.ReverseFind('\\')) - 1);
}
// input: "c:\mypath\mypath2\mypath3\myzipfilename.zip"
// outpt: "c:\mypath\mypath2\mypath3\"
CStringW CDOSCenterApp::extractPathFromFilename(CStringW filename)
{
	if (filename.ReverseFind('/') != -1)
		return filename.Left(filename.ReverseFind('/'));
	return filename.Left(filename.ReverseFind('\\'));
}