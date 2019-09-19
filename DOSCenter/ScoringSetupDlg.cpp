// ScoringSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "ScoringSetupDlg.h"
#include "fileStuff.h"
#include "DatabaseEntryDlg.h"
#include ".\scoringsetupdlg.h"

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

	// populate the list
	for (int i=0; i<theApp.m_ignoreTheseCount; i++)
	{
		txt.Format(L"0x%x",theApp.m_ignoreThese[i].crc);
		if (theApp.m_ignoreThese[i].crc == 0xffffffff)
			txt = "*";
		m_ignoreList.AddItem(COLOR_BLACK, theApp.m_ignoreThese[i].filename, theApp.m_ignoreThese[i].ext, txt);
	}
	((CButton*)GetDlgItem(IDC_FILESIZEIGNOREENA))->SetCheck(1);	
	if (theApp.m_minFilesize == -1)
		((CButton*)GetDlgItem(IDC_FILESIZEIGNOREENA))->SetCheck(0);

	txt.Format(L"%d", theApp.m_minFilesize);
	m_fileSizeIgnoreEdit.SetWindowText(txt);
	OnBnClickedFilesizeignoreena();
	return TRUE;  // return TRUE  unless you set the focus to a control

}
// CScoringSetupDlg message handlers

void CScoringSetupDlg::OnBnClickedOk()
{
	CFileStuff fileStuff;
	CString txt, line, newline;
	int pos, pos2;
	FILE* outFile;
	CString fileData;

	// save entries from the list and re-write the file
	txt = "-1";	// assume checkbox is set
	if ( ((CButton*)GetDlgItem(IDC_FILESIZEIGNOREENA))->GetCheck())
	{
		m_fileSizeIgnoreEdit.GetWindowText(txt);
		swscanf_s(txt, L"%d",&theApp.m_minFilesize); 
	}
	
	pos = theApp.m_ignoreFileData.Find(L"min_filesize = ");
	if (pos == -1)
	{
		AfxMessageBox(L"Failed to open DCIgnore.ini!  Should be in the same path as DOSCenter.exe.", MB_ICONEXCLAMATION, 0);
		OnOK();
		return;
	}
	pos2 = theApp.m_ignoreFileData.Find(L"\n", pos);
	line = theApp.m_ignoreFileData.Mid(pos, pos2-pos );
	newline = L"\nmin_filesize = " + txt + L"\n";
	theApp.m_ignoreFileData.Replace(line, newline);

	// truncate the ignore list so we can rebuild it now that we've captured the header
	pos = theApp.m_ignoreFileData.Find(newline);
	pos += newline.GetLength();
	theApp.m_ignoreFileData.Truncate(pos);


	//theApp.m_ignoreFileData.Replace("\n","\r\n");
	int count=0;
	while (	m_ignoreList.GetItemText(count,1) != L"")
		{
			theApp.m_ignoreFileData +=m_ignoreList.GetItemText(count,1)+L", ";	// filename
			theApp.m_ignoreFileData +=m_ignoreList.GetItemText(count,2)+L", ";	// ext
		//	theApp.m_ignoreFileData +=m_ignoreList.GetItemText(count,3)+", ";	// size
			txt = m_ignoreList.GetItemText(count,3)+L" ";	// crc
			if (txt.GetLength() == 0) 
				txt = L"*";
			theApp.m_ignoreFileData +=txt;
			theApp.m_ignoreFileData +=L"\n";
			count++;
		}
	
	CString outName = theApp.g_appPath+L"\\DCignore.ini";
	theApp.m_ignoreFileData.Replace(L"\n", L"\r\n");
	outFile = fopen((CT2A)outName,"wb");
	fwrite((CT2A)theApp.m_ignoreFileData, sizeof( char ), theApp.m_ignoreFileData.GetLength(), outFile );
	fclose(outFile);
	theApp.m_reloadIgnore = true;
	OnOK();
}
void CScoringSetupDlg::populateIgnoreList()
{
	// open ignore file if it exists and populate the list control
	CFileStuff fileStuff;
	FILE* inFile;
	CString data;
	CString line;
	CString filename, ext, crc;
	int pos;

	fopen_s(&inFile, theApp.g_appPath + "\\DCignore.ini", "r");
	if (inFile == NULL)
	{
		AfxMessageBox(L"Failed to open DCIgnore.ini!  Should be in the same path as DOSCenter.exe.", MB_ICONEXCLAMATION, 0);
		return;
	}

	fileStuff.loadFile(inFile, theApp.m_ignoreFileData);

	// close input file
	fclose(inFile);



	data = theApp.m_ignoreFileData;
	data.Replace(L"\n\r", L"\r");
	data.Replace(L"\r\n", L"\r");
	data.Replace(L"\r", L"\n");
	data += L"\n";

	theApp.m_ignoreTheseCount = 0;
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
			swscanf_s(line, L"%d", &theApp.m_minFilesize);
			continue;
		}
		// must be an entry
		filename = line.Left(line.Find(L","));
		line.TrimLeft(filename);
		line.TrimLeft(L",");

		ext = line.Left(line.Find(L","));
		line.TrimLeft(ext);
		line.TrimLeft(L",");

		//		size = line.Left(line.Find(","));
		//		line.TrimLeft(size);
		//		line.TrimLeft(",");

		crc = line;


		filename.Trim();
		ext.Trim();
		//		size.Trim();
		crc.Trim();

		if (crc == "*")
			crc = "0xffffffff";

		swscanf_s(crc, L"%x", &theApp.m_ignoreThese[theApp.m_ignoreTheseCount].crc);
		//		sscanf( size, "%x",&theApp.m_ignoreThese[theApp.m_ignoreTheseCount].size); 
		theApp.m_ignoreThese[theApp.m_ignoreTheseCount].filename = filename;
		theApp.m_ignoreThese[theApp.m_ignoreTheseCount].ext = ext;
		theApp.m_ignoreTheseCount++;

		//		theApp.m_ignoreList.AddItem(COLOR_BLACK,filename, ext, size, crc);
	}
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
