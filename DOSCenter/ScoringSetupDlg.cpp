// ScoringSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "ScoringSetupDlg.h"
#include "fileStuff.h"
#include "DatabaseEntryDlg.h"
#include ".\scoringsetupdlg.h"
#include <string>

// CScoringSetupDlg dialog

IMPLEMENT_DYNAMIC(CScoringSetupDlg, CDialog)
CScoringSetupDlg::CScoringSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScoringSetupDlg::IDD, pParent)
{
}

CScoringSetupDlg::~CScoringSetupDlg()
{
}

void CScoringSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IGNOREFILESLIST, m_ignoreList);
	DDX_Control(pDX, IDC_FILESIZEIGNORE, m_fileSizeIgnoreEdit);
}


BEGIN_MESSAGE_MAP(CScoringSetupDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_RCLICK, IDC_IGNOREFILESLIST, OnNMRclickList)
	ON_BN_CLICKED(IDC_FILESIZEIGNOREENA, OnBnClickedFilesizeignoreena)
END_MESSAGE_MAP()

BOOL CScoringSetupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString txt;
	m_ignoreList.SetExtendedStyle( LVS_EX_FULLROWSELECT ); 
	m_ignoreList.SetHeadings( _T("Color,0; name,80; type,40; crc,90") );
	m_ignoreList.LoadColumnInfo();
	m_ignoreList.DeleteAllItems();
	theApp.m_reloadIgnore = false;

	// populate the dialog list from the array.
	for (int i=0; i<theApp.m_ignoreTheseCount; i++)
	{
		m_ignoreList.AddItem(COLOR_BLACK, theApp.m_ignoreThese[i].filename, theApp.m_ignoreThese[i].ext, theApp.m_ignoreThese[i].scrc);
	}
	((CButton*)GetDlgItem(IDC_FILESIZEIGNOREENA))->SetCheck(1);	
	if (theApp.m_ignoreMinFileSize == -1)
		((CButton*)GetDlgItem(IDC_FILESIZEIGNOREENA))->SetCheck(0);

	txt.Format(L"%d", theApp.m_ignoreMinFileSize);

	m_fileSizeIgnoreEdit.SetWindowText(txt);
	OnBnClickedFilesizeignoreena();
	return TRUE;  // return TRUE  unless you set the focus to a control
}
// CScoringSetupDlg message handlers

void CScoringSetupDlg::OnBnClickedOk()
{
	CString txt;

	// save entries from the list and re-write the file
	txt = "-1";	// assume checkbox is set
	if ( ((CButton*)GetDlgItem(IDC_FILESIZEIGNOREENA))->GetCheck())
	{
		m_fileSizeIgnoreEdit.GetWindowText(txt);
		swscanf_s(txt, L"%d", &theApp.m_ignoreMinFileSize);
	}

	// convert the dialog list back to the array.
	theApp.m_ignoreTheseCount = 0;
	while (m_ignoreList.GetItemText(theApp.m_ignoreTheseCount, 1) != L"")
	{
		CString f = m_ignoreList.GetItemText(theApp.m_ignoreTheseCount, 1);	// filename
		CString e = m_ignoreList.GetItemText(theApp.m_ignoreTheseCount, 2);	// ext
		//	theApp.m_ignoreFileData +=m_ignoreList.GetItemText(count,3)+", ";	// size
		txt = m_ignoreList.GetItemText(theApp.m_ignoreTheseCount, 3);	// crc
		if (txt.GetLength() == 0)
			txt = L"*";
		theApp.m_ignoreThese[theApp.m_ignoreTheseCount].filename = f;
		theApp.m_ignoreThese[theApp.m_ignoreTheseCount].ext = e;
		theApp.m_ignoreThese[theApp.m_ignoreTheseCount].scrc = txt;
		theApp.m_ignoreTheseCount++;
	}

	int ret = prepIgnoreListForSave();
	if (ret == -1)
	{
		AfxMessageBox(L"Failed to open DCIgnore.ini!  Should be in the same path as DOSCenter.exe.", MB_ICONEXCLAMATION, 0);
		OnOK();
		return;
	}
	saveIgnoreList();

	theApp.m_reloadIgnore = true;
	OnOK();
}

// pulls the current ignore list into a buffer for saving.
// if there was an error, returns -1
int CScoringSetupDlg::prepIgnoreListForSave()
{
	int pos, pos2;
	CString txt, line, newline;
	CString filename, ext, scrc;

	pos = theApp.m_ignoreFileData.Find(L"min_filesize = ");
	if (pos == -1)
		return - 1;
	pos2 = theApp.m_ignoreFileData.Find(L"\n", pos);
	line = theApp.m_ignoreFileData.Mid(pos, pos2 - pos);
	txt.Format(L"%d", theApp.m_ignoreMinFileSize);
	newline = L"\nmin_filesize = " + txt + L"\n";
	theApp.m_ignoreFileData.Replace(line, newline);

	// truncate the ignore list so we can rebuild it now that we've captured the header
	pos = theApp.m_ignoreFileData.Find(newline);
	pos += newline.GetLength();
	theApp.m_ignoreFileData.Truncate(pos);

	for (int i = 0; i < theApp.m_ignoreTheseCount; i++)
	{
		filename = theApp.m_ignoreThese[i].filename;
		ext = theApp.m_ignoreThese[i].ext;
		scrc = theApp.m_ignoreThese[i].scrc;
		
		theApp.m_ignoreFileData += filename + L",";
		theApp.m_ignoreFileData += ext + L",";
		theApp.m_ignoreFileData += scrc + L"\n";
	}
	theApp.m_ignoreFileData.Replace(L"\n", L"\r\n");
	return 0;
}
// writes the contents of the current ignore list to a file.
void CScoringSetupDlg::saveIgnoreList()
{
	CFileStuff fileStuff;
	FILE* outFile;
	CString fileData;
	errno_t err;

	// write the entire ignore file.  first copy it into a normal character buffer because this is not using unicode.
	std::string buffer = CT2A(theApp.m_ignoreFileData);
	CString outName = theApp.g_appPath + L"\\DCignore.ini";
	outFile = fopen(theApp.g_appPath + "\\DCignore.ini", "wb");

	if (outFile == NULL)
	{
		AfxMessageBox(L"Failed to write DCIgnore.ini!  Whatcha doin' to me?", MB_ICONEXCLAMATION, 0);
		OnOK();
		return;
	}
	fwrite(&buffer[0], sizeof(char), strlen(&buffer[0]), outFile);
	fclose(outFile);
}
void CScoringSetupDlg::populateIgnoreList()
{
	// open ignore file if it exists and populate the list control
	CFileStuff fileStuff;
	FILE* inFile;
	CString data;
	CString line;
	CString filename, ext, scrc;
	
	int pos;

	fopen_s(&inFile, theApp.g_appPath + "\\DCignore.ini", "r");
	if (inFile == NULL)
	{
		AfxMessageBox(L"Failed to open DCIgnore.ini!  Should be in the same path as DOSCenter.exe.", MB_ICONEXCLAMATION, 0);
		return;
	}
	fileStuff.loadFile(inFile, data);
	// close input file
	fclose(inFile);



	// keep a full copy for if the table is accessed via dialog
	theApp.m_ignoreFileData = data;

	theApp.m_ignoreName.clear();
	theApp.m_ignoreExt.clear();
	theApp.m_ignoreFilename.clear();
	theApp.m_ignoreTheseCount = 0;

	data.Replace(L"\n\r", L"\r");
	data.Replace(L"\r\n", L"\r");
	data.Replace(L"\r", L"\n");
	data += L"\n";

	// parse the list
	for (int i = 0; i<data.GetLength(); i++)
	{
		pos = data.Find(L"\n", i);
		line = data.Mid(i, pos - i);
		i += line.GetLength();
		//i++;	// skip the CR
		line.Trim();

		if (line.Left(1) == ";")
			continue;
		if (line.GetLength() == 0)
			continue;


		pos = line.Find(L"min_filesize = ");
		if (pos != -1)
		{
			line.TrimLeft(L"min_filesize = ");
			swscanf_s(line, L"%d", &theApp.m_ignoreMinFileSize);
			continue;
		}
		// must be an entry
		filename = line.Left(line.Find(L","));
		line.TrimLeft(filename);
		line.TrimLeft(L",");

		ext = line.Left(line.Find(L","));
		line.TrimLeft(ext);
		line.TrimLeft(L",");

		scrc = line;

		filename.Trim();
		ext.Trim();
		scrc.Trim();


		addEntry2ignorelist(filename, ext, scrc);

	}	//while parsing data
}

// add an entry to the existing ignore list
void CScoringSetupDlg::addEntry2ignorelist(CString filename, CString ext, CString scrc)
{
	CString fullfilename;
	unsigned int crc;
	// save to the internal array.
	theApp.m_ignoreThese[theApp.m_ignoreTheseCount].filename = filename;
	theApp.m_ignoreThese[theApp.m_ignoreTheseCount].ext = ext;
	theApp.m_ignoreThese[theApp.m_ignoreTheseCount].scrc = scrc;
	theApp.m_ignoreTheseCount++;

	if (scrc == "*")
		scrc = "0xffffffff";

	swscanf_s(scrc, L"%x", &crc);
	// put each entry into the correct map
	if ((filename == L"*") && (ext == L"*"))	// specific CRC 
	{
		AfxMessageBox(L"Invalid (dangerous) entry in ignore file.\r\nFilename of *.* detected.", MB_ICONEXCLAMATION, 0);
	}

	filename.MakeLower();
	ext.MakeLower();
	fullfilename = filename + L"." + ext;

	if ((filename != L"*") && (ext == L"*"))	// any filename.* 
		theApp.m_ignoreName[filename] = crc;
	if ((filename == L"*") && (ext != L"*"))	// any *.ext
		theApp.m_ignoreExt[ext] = crc;
	if ((filename != L"*") && (ext != L"*"))	// specific filename
		theApp.m_ignoreFilename[fullfilename] = crc;
}


void CScoringSetupDlg::OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	int nIndex=-1;
	int lastDeleted=0;
	CString source,dest;
	CString fromClipboard;
	CString tmp;
	CDatabaseEntryDlg dlg;	
	int count=0;

	CPoint point;                                            
     ::GetCursorPos(&point); //where is the mouse?


	CMenu menu; //lets display our context menu :) 

    DWORD dwSelectionMade;                                       
    VERIFY(menu.LoadMenu(IDR_MENU3) );  

	CMenu *pmenuPopup = menu.GetSubMenu(0);


	 // no selection?  remove the "modify" choice
	nIndex = m_ignoreList.GetNextItem(-1, LVNI_SELECTED);
	if (nIndex == -1)
	{
		pmenuPopup->EnableMenuItem(1, MF_GRAYED+MF_BYPOSITION);	// kill the modify option if there's no selection
		pmenuPopup->EnableMenuItem(0, MF_GRAYED+MF_BYPOSITION);	// kill the delete option if there's no selection
	}
	ASSERT(pmenuPopup != NULL);                                       

	 
	dwSelectionMade = pmenuPopup->TrackPopupMenu( (TPM_LEFTALIGN|TPM_LEFTBUTTON|
                                                       TPM_NONOTIFY|TPM_RETURNCMD),
                                                       point.x, point.y, this
                                );                                

	switch (dwSelectionMade)
	{
	case ID__IGNORENEW:
		{
			CDatabaseEntryDlg dlg;
			dlg.m_databaseFilenameTxt = "*";
			dlg.m_databaseExtTxt = "*";
			//dlg.m_databaseSizeTxt = "*";
			dlg.m_databaseCRCTxt = "*";
			if (dlg.DoModal() == 2)	// cancel button hit?
				break;
			m_ignoreList.AddItem(COLOR_BLACK,dlg.m_databaseFilenameTxt, dlg.m_databaseExtTxt, dlg.m_databaseCRCTxt);
			break;
		}
	case ID__IGNOREDELETE:
		{
			m_ignoreList.DeleteItem (nIndex);
			break;
		}
	case ID_IGNOREMODIFY:
		{
			dlg.m_databaseFilenameTxt = m_ignoreList.GetItemText(nIndex, 1);
			dlg.m_databaseExtTxt = m_ignoreList.GetItemText(nIndex, 2);
			//dlg.m_databaseSizeTxt = m_ignoreList.GetItemText(nIndex, 3);
			dlg.m_databaseCRCTxt = m_ignoreList.GetItemText(nIndex, 3);
			if (dlg.DoModal() == 2)	// cancel button hit?
				break;
			m_ignoreList.SetItemText(nIndex, 1,dlg.m_databaseFilenameTxt);
			m_ignoreList.SetItemText(nIndex, 2,dlg.m_databaseExtTxt);			
			//m_ignoreList.SetItemText(nIndex, 3,dlg.m_databaseSizeTxt);			
			tmp = dlg.m_databaseCRCTxt;
			if (tmp.GetLength() == 0)
				tmp = "*";
			m_ignoreList.SetItemText(nIndex, 3,tmp);			
			break;
		}
	}
	pmenuPopup->DestroyMenu(); 	
	
	return;
}
void CScoringSetupDlg::OnBnClickedFilesizeignoreena()
{
	m_fileSizeIgnoreEdit.EnableWindow(0);
	if ( ((CButton*)GetDlgItem(IDC_FILESIZEIGNOREENA))->GetCheck() ) 
	{
		m_fileSizeIgnoreEdit.EnableWindow(1);
	}
}
