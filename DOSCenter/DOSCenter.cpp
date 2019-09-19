
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
	if (cmdLine.Find(L"-commitMode") != -1)
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
		CString inidata, header;
		FILE* inFile;
		FILE* outFile;
		if (cmdLine.Find(L"-addpaths") != -1)
				snt.m_noPathInDat = false;

		if (cmdInfo.m_strFileName.Mid(1,1) !=':')
			fopen_s(&inFile, CT2A(g_appPath+"\\"+cmdInfo.m_strFileName),"r");
		else
			fopen_s(&inFile, CT2A(cmdInfo.m_strFileName),"r"); // fopen(cmdInfo.m_strFileName,"r");
		if (inFile != NULL)
		{
				// parse the .ini file to get the goods
				fileStuff.loadFile(inFile, inidata);
				if (inidata.GetLength() ==0)
					return FALSE;
				inidata.Replace(L"__DATE__",datenTime);
				// get the target filename
				inidata.TrimLeft(L"<target>");
				theApp.m_DATPath = inidata.Left(inidata.Find(L"</target>"));
				theApp.m_DATPath.Trim();
				inidata.Trim();
				inidata.Replace(theApp.m_DATPath,L"");
				inidata.Trim();
				inidata.TrimLeft(L"</target>");
				inidata.Trim();

				// get the header
				inidata.TrimLeft(L"<header>");
				header = inidata.Left(inidata.Find(L"</header>"));
				// write the header into the output file
				header.Trim();
				if (!header.IsEmpty())
					inidata.Replace(header,L"");

				_wfopen_s(&outFile, theApp.m_DATPath, L"w, ccs=UTF-8");
				if ( outFile != NULL )
					fwrite(header, sizeof( wchar_t ), header.GetLength(), outFile );
	
				if ( outFile != NULL )
				{
					fclose(outFile);
					outFile=NULL;
				}
				
				//inidata.Replace(header,"");
				inidata.Replace(L"</header>",L"");
				inidata.Replace(L"<paths>",L"");
				//inidata.Replace("</paths>","\r\n");
				// do this in a while loop so the .ini file can set multiple target folders
				inidata.Trim();

				while(inidata !=L"</paths>")
				{
					theApp.m_collectionPath = inidata.Left(inidata.Find(L"\n"));
					theApp.m_collectionPath.Trim();
					if (theApp.m_collectionPath.Left(1) !=";")
					{
						snt.m_recursive = true;
						snt.m_cmdLine = true;
						snt.makeDATFile(theApp.m_collectionPath);
						//snt.makeDATFile_express(theApp.m_collectionPath);
					}
					inidata.Replace(theApp.m_collectionPath, L"");
					inidata.Trim();
				}				
				fclose(inFile);
		}
		
		return FALSE;

	}


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