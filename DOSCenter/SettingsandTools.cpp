// SettingsandTools.cpp : implementation file
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "SettingsandTools.h"
#include "fileStuff.h"
#include "registry.h"
#include "progressDlg.h"
#include "DOSCenterDlg.h"
#include "DATFileInfoDlg.h"
#include "ScoringSetupDlg.h"
#include "HaveMissDlg.h"
#include "PopupViewerTxtDlg.h"
#include "adAssassinDlg.h"



// CSettingsandTools dialog

IMPLEMENT_DYNAMIC(CSettingsandTools, CDialog)
CSettingsandTools::CSettingsandTools(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsandTools::IDD, pParent)
{
}

CSettingsandTools::~CSettingsandTools()
{
}

void CSettingsandTools::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MINRENAMESCORE_EDIT, m_minScore4Rename);
}


BEGIN_MESSAGE_MAP(CSettingsandTools, CDialog)
	ON_BN_CLICKED(IDC_LOADDATFILE, OnBnClickedLoaddatfile)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_MAKEADAT, OnBnClickedMakeadat)
	ON_BN_CLICKED(IDC_SCRATCHPADBROWSE, OnBnClickedScratchpadbrowse)
	ON_BN_CLICKED(IDC_SETDOSCOLLECTIONPATHBTN, OnBnClickedSetdoscollectionpathbtn)
	ON_BN_CLICKED(IDC_SCORINGCFGBTN, OnBnClickedScoringcfgbtn)
	ON_BN_CLICKED(IDC_DOSBOXBROWSE, OnBnClickedDosboxbrowse)
	ON_BN_CLICKED(IDC_DATFILEINFO, OnBnClickedDatfileinfo)
//	ON_BN_CLICKED(IDC_CRCSCAN, OnBnClickedCrcscan)
//	ON_BN_CLICKED(IDC_CRCFILENAME, OnBnClickedCrcfilename)
	ON_BN_CLICKED(IDC_MISSINGLIST, OnBnClickedMissinglist)
	ON_BN_CLICKED(IDC_DETAILEDREPORTS, OnBnClickedDetailedreports)
	ON_BN_CLICKED(IDC_PERFECTSTOO, OnBnClickedPerfectstoo)
	ON_BN_CLICKED(IDC_HAVEMISSLISTCFG, OnBnClickedHavemisslistcfg)
	ON_BN_CLICKED(IDC_MDB_CHECK, &CSettingsandTools::OnBnClickedMdbCheck)
	ON_BN_CLICKED(IDC_AD_ASSASSIN_BTN, &CSettingsandTools::OnBnClickedAdAssassinBtn)
END_MESSAGE_MAP()


BOOL CSettingsandTools::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_cmdLine = false;
	m_noPathInDat = true;
	CString txt;
	((CButton*)GetDlgItem(IDC_DATAUTOLOAD))->SetCheck(theApp.m_autoLoadDat);
	txt = theApp.m_tmpWorkingFolder.TrimRight(L"\\DOSCenter");
	((CButton*)GetDlgItem(IDC_SCRATCHPATHTXT))->SetWindowText(txt);
	((CButton*)GetDlgItem(IDC_DOSCOLLECTIONPATHTXT))->SetWindowText(theApp.m_collectionPath);
	((CButton*)GetDlgItem(IDC_DOBOXPATHTXT))->SetWindowText(theApp.m_dosboxPath);
	if (theApp.m_DATfileLoaded)
		((CButton*)GetDlgItem(IDC_DATFILENAMETXT))->SetWindowText(theApp.m_DATPath);
	else
	{
		((CButton*)GetDlgItem(IDC_DATFILENAMETXT))->SetWindowText(L"no .dat file loaded");
		((CButton*)GetDlgItem(IDC_DATFILEINFO))->EnableWindow(0);
		((CButton*)GetDlgItem(IDC_MISSINGLIST))->EnableWindow(0);
	}
	txt.Format(L"%d", theApp.m_minScoreForAutoRename);
	m_minScore4Rename.SetWindowText(txt);

	((CButton*)GetDlgItem(IDC_DETAILEDREPORTS))->SetCheck(theApp.m_detailedReport);
	((CButton*)GetDlgItem(IDC_PERFECTSTOO))->SetCheck(theApp.m_reportPerfects);

	OnBnClickedDetailedreports();

	m_autoadd = false;
//	if (theApp.m_ScanCRCFilename)
//		((CButton*)GetDlgItem(IDC_CRCFILENAME))->SetCheck(1);
//	else
//		((CButton*)GetDlgItem(IDC_CRCSCAN))->SetCheck(1);
	if (!theApp.m_commitMode)
		((CButton*)GetDlgItem(IDC_MDB_CHECK))->EnableWindow(0);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// CSettingsandTools message handlers

// browse and load in the .dat file
void CSettingsandTools::OnBnClickedLoaddatfile()
{
	CFileStuff fileStuff;
	//CDOSCenterDlg m_pMain;
	// open a "browse to folder" dialog
	CString arg;
	CString filename;
	
	LPCWSTR FileFilter = L"dat files (*.dat)\0*.dat\0"; 
	LPCWSTR FileExt = L"dat";
	fileStuff.m_windowTitle = L"Gimme a dat file please...";

	if (fileStuff.UserBrowseOpen(&fileStuff.m_sFilePath, FileFilter, FileExt, true))	//true=allow multiple .dat files
	{
		
		theApp.m_DATPath = fileStuff.m_sFilePath;
		CRegistry regMyReg( CREG_CREATE );  // create registry key if it doesn't exist.
		if ( regMyReg.Open(L"Software\\DOSCenter",HKEY_CURRENT_USER) ) 
			{
				// Now set the new values
				regMyReg[L"datFolders"] = theApp.m_DATPath; 
				regMyReg.Close();
			}

		parseDATFile();
		((CButton*)GetDlgItem(IDC_DATFILEINFO))->EnableWindow(1);
		((CButton*)GetDlgItem(IDC_MISSINGLIST))->EnableWindow(1);
		((CButton*)GetDlgItem(IDC_DATFILENAMETXT))->SetWindowText(theApp.m_DATPath);
	}

}

// open .dat file and pull in names and CRC values. 
// at the end of this routine, we have two globally available maps:
// one for the zipfile and one for the roms inside each zip
// for each rom,we have an index pointer back to the zip that contains said rom.
//TODO - think about a method of loading in binary data so that parsing would be faster.  if one I could get a cmap memory dump and just load that bitch in!
/*
build 2 maps at this level.
1 flat map, where
 datmap[datEntryNum] = filename, rom count, totalsize, map of all roms.  No collisions here since it's just a binary version of the .dat file.
 crcmap[crc] = datEntryNum.  many CRC collisions here with gwbasic.exe, etc.  so the datEntryNum itself is a map
*/

void CSettingsandTools::parseDATFile()
{
	FILE* inFile;
	CFileStuff fileStuff;
	CString romDesc;
	int pos, pos2; 
	int eof=0;
	int romCount=0;
	int romstart=0;
	int romend=0;
	int zipSize;
	CStringW name;
	CString ssize = L"";
	CString scrc;
	CStringW tmp;
	CString datetime;
	unsigned int date;
	CStringW zipFilename;
	CDOSCenterDlg DC;
	bool tZipped;
	CString header;
	int status;
	float percent;
    std::wstring buffer;            // stores file contents
    size_t filesize;
    struct _stat fileinfo;
	std::wstring stmp;
	
	unsigned int size;
	CProgressDlg progDlg;
	progDlg.Init(PROG_MODE_TIMER, 1000, L"Loading .dat file...",L"");

	progDlg.UpdateText(L"Loading .dat file...", L"Purging any previous dat entries...");
	deleteKeys();
	progDlg.UpdateText(L"Loading .dat file...", L"");
	
	//progDlg.UpdateText(L"Parsing .dat file...", ssize);
    _wfopen_s(&inFile, theApp.m_DATPath, L"rtS, ccs=UTF-8");

	if (inFile == NULL)
	{
		AfxMessageBox(L"Failed to open dat file:"+theApp.m_DATPath+"\r\nGo to settings to browse to your .dat file.", MB_ICONEXCLAMATION, 0);
		goto exit;
	}

	_wstat(theApp.m_DATPath, &fileinfo);
	filesize = fileinfo.st_size;
	// Read entire file contents in to memory
    if (filesize > 0)
    {
        buffer.resize(filesize);
        size_t wchars_read = fread(&(buffer[0]), sizeof(wchar_t), filesize, inFile);
        buffer.resize(wchars_read);
    }

    fclose(inFile);
	
	progDlg.UpdateText(L"Parsing .dat file...", ssize);

	theApp.m_totalCollisions = 0;

	pos =0;
	pos2 = buffer.find(L"\n)\n");
	stmp = buffer.substr(pos, pos2-pos);
	header = stmp.c_str();




	// get the header info from the .dat file
	if (header.Find(L"DOSCenter") == -1)
	{
		AfxMessageBox(L"This .dat file is weird.  Couldn't locate header information.", MB_ICONEXCLAMATION, 0);
		goto exit;
	}

	// populate our internal strings with the header info from the .dat file
	AfxExtractSubString(theApp.m_datName, header, 1, '\n');
	AfxExtractSubString(theApp.m_datDescription, header, 2, '\n');
	AfxExtractSubString(theApp.m_datVersion, header, 3, '\n');
	AfxExtractSubString(theApp.m_datdate, header, 4, '\n');
	AfxExtractSubString(theApp.m_datAuthor, header, 5, '\n');
	AfxExtractSubString(theApp.m_datHomepage, header, 6, '\n');
	AfxExtractSubString(theApp.m_datComment, header, 7, '\n');
	theApp.m_datName.Replace(L"Name:",L"");
	theApp.m_datName.Trim();
	theApp.m_datDescription.Replace(L"Description:",L"");
	theApp.m_datDescription.Trim();
	theApp.m_datAuthor.Replace(L"Author:",L"");
	theApp.m_datAuthor.Trim();
	theApp.m_datHomepage.Replace(L"Homepage:",L"");
	theApp.m_datHomepage.Trim();
	theApp.m_datdate.Replace(L"Date:",L"");
	theApp.m_datdate.Trim();
	theApp.m_datVersion.Replace(L"Version:",L"");
	theApp.m_datVersion.Trim();
	theApp.m_datComment.Replace(L"Comment:",L"");
	theApp.m_datComment.Trim();


	// for every game{...} entry in the .dat file
	while (1)
	{
		tZipped = false;
		zipSize = 0;	// total number of bytes across all files in this zip
		romCount=0;		// number of roms in this zip

		// get a romDesc: get the data between game( and )
		//romstart=romend;
		romstart = buffer.find(GAMEID,romstart);
		romend = buffer.find(L")\n)",romstart);
		if (romend == -1)
			break;


		percent = ((float)romstart / (float)filesize) * 100;
		pos = (int)percent;
		if ((pos % 10) == 0)
		{
			ssize.Format(L"%02d",pos);
			ssize+="% Complete";
			progDlg.UpdateText(L"Parsing .dat file...",ssize);
		}

		stmp = buffer.substr(romstart, romend-romstart);
		romDesc = stmp.c_str();

		romDesc.TrimLeft(GAMEID);	// trim a romDesc down to just the meat of the files
		romDesc.Trim();
		romDesc.TrimLeft(L"name");
		romDesc.Trim();
		romDesc.Append(L")");
		pos=0;
		romstart += 10;	// advance ahead to get the next game entry.  amount is arbitrary as long as the pointer is past the "game (" start signature.
		zipFilename = romDesc.Tokenize(L"\"",pos);
		theApp.m_datZipList[theApp.m_datfileCount].filename= zipFilename;	// store the zip filename inside the structure inside the map.


		int tpos=0;
		while (zipFilename.GetLength() > 0)	// get the files (roms) inside this zip
		{
			//alternate = false;
			status = 0;
			tpos = romDesc.Find(L"file ( name ",pos);	// get the name (new .dat file version)
			if (tpos == -1) break;

			pos=tpos;
			pos+=12;
			pos2 = romDesc.Find(L" size ",pos);
			if (pos2 == -1) break;
			name = romDesc.Mid(pos, pos2-pos);
			//name.Trim();
			
			pos = romDesc.Find(L" size ",pos2);		// get the size
			if (pos == -1) break;
			pos+=6;
			pos2 = romDesc.Find(L" date ",pos);
			if (pos2 == -1) break;

			ssize = romDesc.Mid(pos, pos2-pos);
			swscanf_s(ssize,L"%d",&size);
			zipSize += size;

			// get the date
			pos = pos2+6;	
			pos2 = romDesc.Find(L" crc ",pos2);
			datetime = romDesc.Mid(pos, pos2-pos);
			//datetime.Trim();

			date = String2Date(datetime);
			if (date == TZIPPED_TIMESTAMP) // bit encoded date/time of 12/24/1996 - 23:32
				tZipped = true;

			pos = pos2; //romDesc.Find(" crc ",pos2);		// get the crc
			if (pos == -1) break;
			pos+=5;
			pos2 = romDesc.Find(L")",pos);
			if (pos2 == -1) break;
			scrc = romDesc.Mid(pos, pos2-pos);
			scrc.Trim();
		
			// ok, we have a ROM to store in the database.
			// plug the above parsed values into our .dat entry
			if (rom2datmap(name, size, status, false, date, scrc, romCount))
				romCount++;	
		}	// while parsing rom entries


		// OK, we've stored all the ROMs from this zip into the cmap.
		// update the totals for the zip file
		theApp.m_datZipList[theApp.m_datfileCount].fileCount = romCount;	// store the # of ROMs the zip contains
		theApp.m_datZipList[theApp.m_datfileCount].status = 0;
		theApp.m_datZipList[theApp.m_datfileCount].numBytes = zipSize;
		if (tZipped) 
			theApp.m_datZipList[theApp.m_datfileCount].status = TZIPPED;
		
		theApp.m_datfileCount++;	// global counter of # of items in the .dat file

	} // while parsing the .dat file


exit:
	
	// close input file
	if (inFile != NULL)
	{
		fclose(inFile);	
		theApp.m_DATfileLoaded = true;
	}
	
	progDlg.DestroyWindow();
	theApp.m_datParsed = false;	// update the dat view if selected
	theApp.m_leftSideDATList.DeleteAllItems();
}



// add this rom into the main cmap based on CRC and filename
// status is currently unused but could be set to "ignore" or "tzipped" for example.  This is on a per "ROM" basis.

/*

a "ROM" (file in a zip to avoid confusion) is stored via CRC32 as a key.  
If the key exists, then there is a collision (the rom exists elsewhere in the dat file.  see gwbasic.exe, drivers, etc)
If the key does not exist, add the dat entry count as the data for that key.
If the key is a collision, generate an update key and try again.

The above map CRC/dat ptr map is used for scanning an incoming zip file to quickly get a sense of which dat entry best matches the zip.

Once the dat pointer is disocovered, that zip can get a closer examination of content.






*/
bool CSettingsandTools::rom2datmap(CStringW romName, unsigned int size, int status, bool found, unsigned int date, CString scrc, int romCount)
{
	//ROMdata romInfo, tempROMdata;	// tempRomdata for lookups only
    LPCWSTR ROMCRCKey;			// CRC is the key to storage
	CString tmpCrc;
	unsigned int crc;
	CString tmp;
	
	unsigned long long crcsize;
	unsigned long long collisionCount = 0;
//	_NEWROMINFO myROMInfo;
	bool collision = false;
	CStringW tmprom;

	swscanf_s(scrc,L"%x",&crc);				

	// i dont need to do this.  i'm just putting all the things passed in into a structure only to pull them back out again later.
	//romInfo.fileName = romName;
	//romInfo.size = size;
	//romInfo.status = status;	
	//romInfo.collision = 0;
	//romInfo.date = date;
	//romInfo.crc = crc;
	//romInfo.ptr2zipFileArray = ptr2zip;	// save a pointer (index) into the zip filename array

	// CRC collisions.
	// collisions take several forms.
	// same file across many zips (gwbasic.exe is prime example)
	// renamed file across many zips (sometimes fonts or drivers are renamed in different games even when the content of that file is identical.
	// multiple files inside 1 zip.  Names are unique but the CRC is identical.  take a look at soko-ban (1984)
	// 
	// we also need to be able to detect an incoming file where perhaps game.exe with CRC 0x18675309 is renamed to start.exe with the same CRC.
	// in the gwbasic.exe example, scanning can get thrown off track entirely to the
	// wrong title, while duplicate files inside a single zip/.dat entry can result in a zip's "score" being lower because the CRC was only counted once.
	//
	// lookup raw CRC.  
	// no collision? --> store it in the cmap
	// collision? = the first cmap entry is then purely an indicator of how many collisions


#if 0	
	// theApp.m_ROMDATEntryCRC is the master CRC list of all the roms in the .dat file.  We use this to lookup a zip file we're scanning.
	// the stuff stored (ROMdata structure) in theApp.m_ROMDATEntryCRC include a pointer back to the .zip file this rom is found in the .dat file
	// 
	scrc.MakeLower(); 
	romName.MakeLower();
	ROMCRCKey = scrc;
	tempROMdata.collision = 0; //false;	

	while (theApp.m_ROMDATEntryCRC.Lookup(ROMCRCKey, tempROMdata)) // before storing, check to see if there's someone home already.  get the structure into the temporary storage
	{	
		// bang!  we've got a collision
		tempROMdata.collision++; 			    // set the collision detected bit in the old one
		theApp.m_ROMDATEntryCRC.SetAt(ROMCRCKey, tempROMdata); // update the first entry that we have a collision.
	
		// make a new key and try again.  - all new keys will be crc|filename?1
		tmpCrc = scrc;
		//tmpCrc.Append(L"|");
		//tmpCrc.Append(romName);
		tmp.Format(L"?%d",tempROMdata.collision);
		tmpCrc.Append(tmp); // += tmp.Format(L"?%d",tempROMdata.collision);
		ROMCRCKey=tmpCrc;
	}

	

	// do I need to store this stuff?  each CRC has a pointer back to the zipfilename already.
	// romCount might be able to be replaced with the romsmap iterator.
	theApp.m_datZipList[theApp.m_datfileCount].ROMsMap[romCount].name = romInfo.fileName;
	theApp.m_datZipList[theApp.m_datfileCount].ROMsMap[romCount].crc = romInfo.crc;
	theApp.m_datZipList[theApp.m_datfileCount].ROMsMap[romCount].size = romInfo.size;
	theApp.m_datZipList[theApp.m_datfileCount].ROMsMap[romCount].date = romInfo.date;
	theApp.m_datZipList[theApp.m_datfileCount].ROMsMap[romCount].status = romInfo.status;
	//theApp.m_datZipList[theApp.m_datfileCount].status = romInfo.status;
	theApp.m_datROMCount++;

	// finally, no collisions here, we can store this rom entry here.
	theApp.m_ROMDATEntryCRC.SetAt(ROMCRCKey, romInfo );	 // store it the new ROM entry in a different location.

#endif 
#if 0
	// new method
	unsigned long long crcsize;
	_NEWROMINFO myROMInfo;
	unsigned long long collisionCount = 0;
	
	// if CRC is 0, create a fake CRC based off the filename. This allows folders and disk labels to still be uniquely identified.
	if ( (crc == 0) && (romInfo.size == 0) )
		crc = hashAString((CT2A)romInfo.fileName, 0);
	
	crcsize = crc + size;
	myROMInfo.date = romInfo.date;
	myROMInfo.name = romInfo.fileName;
	myROMInfo.status = romInfo.status;
	myROMInfo.size = romInfo.size;
	myROMInfo.datPtr = ptr2zip;
	//myROMInfo.datOff = romCount;
	myROMInfo.collsionCount = 0;

	while (theApp.m_datCRCMap.count(crcsize) == 1)
	{
		// CRCsize collision, make a note in the original entry
		theApp.m_datCRCMap[crcsize].status |= NEW_STATUS_COLLISION;
		// make a new key by simply incrementing a number outside of the possible max value of CRC+size
		collisionCount++;
		crcsize &= 0xfffffffff;
		theApp.m_datCRCMap[crcsize].collsionCount = collisionCount;	// update the 1st ROM map entry with the new count
		crcsize |= (collisionCount << COLLISION_COUNT_START_BIT);
	}

	theApp.m_datCRCMap[crcsize] = myROMInfo;

#endif

/* 
Zip file and dat handling annoyance:
	 
some zip have folders encoded like this:
 file.exe
 \folder\
 \folder\file.txt
	
 where some zips have folders encoded like this
 file.exe
 \folder\file.txt

 This appears to be a byproduct of whichever zip tool was used to generate the zip file, thus we can't know which way a zip is going to 
 present itself when scanning incoming files.  
 the blank/empty folders are counted as files, so when the .dat file has no empty folders but the zip does, now we get missing "files" during scanning.

 Best solution here is to simply skip over empty folder names both when creating the .dat file and when scanning.
 We're not really losing any information by doing this, since we can always extrapolate the folder name from the filename if needed.

*/

	// if CRC is 0, create a fake CRC based off the filename. This allows disk labels or other 0 byte files to still be uniquely identified.
	if ((crc == 0) && (size == 0))
	{
		if (romName.Right(1) == '\\') // is this a folder only?  Skipit entirely. (see note above)
			return false;
		tmprom = romName;
		tmprom.MakeLower();
		crc = hashAString((CT2A)tmprom, 0);
	}
		
	crcsize = size;
	crcsize = (unsigned long long)(crcsize << 32);
	crcsize |= crc; // +size;


	// add this ROM's CRC into the global CRC map

	if (theApp.m_crcMap.count(crcsize) == 0)
	{
		// first time this CRC has been detected
		theApp.m_crcMap[crcsize].collisionCount = 0;
		// save a pointer for this CRC back to the dat entry so when scanning incoming zips, we can back trace to the dat file.
		theApp.m_crcMap[crcsize].datPtr.push_back(theApp.m_datfileCount);
	}
	else
	{
		// a crc collision.
		theApp.m_crcMap[crcsize].collisionCount++;
		theApp.m_totalCollisions++;
		
		// if this CRC collision is in the same .dat entry, hash the filename and store it so we can keep collisions inside an entry separate.
		// don't bother recording the dat entry for this CRC since we already have it.
		// this is experimental!
		int prevEntry = theApp.m_crcMap[crcsize].datPtr.back();
		if (prevEntry == theApp.m_datfileCount)
		{
			tmprom = romName;
			tmprom.MakeLower();
			theApp.m_crcMap[crcsize].filenameHash = hashAString((CT2A)tmprom, 0);
		}
		// save a pointer for this CRC back to the dat entry so when scanning incoming zips, we can back trace to the dat file.
		theApp.m_crcMap[crcsize].datPtr.push_back(theApp.m_datfileCount);
	}


//	while (theApp.m_datMap.CRCMap.count(crcsize) == 1)
//	{
//		// CRCsize collision, make a note in the original entry
//	//	theApp.m_datMap.CRCMap[crcsize].status |= NEW_STATUS_COLLISION;
//		// make a new key by simply incrementing a number outside of the possible max value of CRC+size
//		collisionCount++;
//		crcsize &= 0xfffffffff;
//		crcsize |= (collisionCount << COLLISION_COUNT_START_BIT);
//	//	theApp.m_datMap.CRCMap[crcsize].collsionCount = collisionCount;	// update the 1st ROM map entry with the new count
//
//		collision = true;
//	}
//	theApp.m_datMap.CRCMap[crcsize] = theApp.m_datfileCount; // pointer to the below data.

	// store this same data indexed via the ptr2zip
	//  master dat list [ index (aka dat ptr)]  "ROM" info
	theApp.m_datZipList[theApp.m_datfileCount].ROMsMap[romCount].name = romName;
	theApp.m_datZipList[theApp.m_datfileCount].ROMsMap[romCount].crc = crcsize;
	theApp.m_datZipList[theApp.m_datfileCount].ROMsMap[romCount].size = size;
	theApp.m_datZipList[theApp.m_datfileCount].ROMsMap[romCount].date = date;
	theApp.m_datZipList[theApp.m_datfileCount].ROMsMap[romCount].status = status;

	theApp.m_datROMCount++;
	return true;
}

// convert a date+time string to a 32bit value 
// example input: '2007\1\31 19:46'
unsigned int CSettingsandTools::String2Date(CString datetime)
{
	unsigned int y;                  // year
	unsigned int n;                  // month
	unsigned int d;                  // day
	unsigned int h;                  // hour
	unsigned int m;                  // minute
	unsigned int s;                  // second

	datetime.Replace(L"\\",L"/");

	swscanf_s( datetime, L"%04d/%02d/%02d%02d:%02d:%02d", &y, &n, &d, &h, &m, &s );
	if (s > 59)
		s=0;

	if (y<1980)
	{
		y=1980;
		n=1;
		d=1;
		h=0;
		m=0;
		s=0;
	}
	// Convert the date y/n/d and time h:m:s to a four byte DOS date and
	// time (date in high two bytes, time in low two bytes allowing magnitude comparison). 
	// y < 1980 ? dostime(1980, 1, 1, 0, 0, 0) :
	
	return (((unsigned long)y - 1980) << 25) | ((unsigned long)n << 21) | ((unsigned long)d << 16) | ((unsigned long)h << 11) | ((unsigned long)m << 5) | ((unsigned long)s >> 1);
}

void CSettingsandTools::OnBnClickedCancel()
{
	OnCancel();
}

void CSettingsandTools::OnBnClickedOk()
{
	CString txt;
	theApp.m_autoLoadDat = 0;
	if (((CButton*)GetDlgItem(IDC_DATAUTOLOAD))->GetCheck())
		theApp.m_autoLoadDat = 1;

	m_minScore4Rename.GetWindowText(txt);
	swscanf_s(txt, L"%d", &theApp.m_minScoreForAutoRename);

//	theApp.m_ScanCRCFilename = ((CButton*)GetDlgItem(IDC_CRCFILENAME))->GetCheck();


	if (((CButton*)GetDlgItem(IDC_AUTOADD))->GetCheck())
		m_autoadd = true;

	OnOK();
}

void CSettingsandTools::Add2DatFile()
{
	CStringW datEntry;
	int count=0;

	
	datEntry = L"\ngame (\n\tname \"";
//	datEntry+=m_currentlyTaggedZipFile + "\"\n\tdescription \"";
	datEntry+=theApp.m_currentlyTaggedZipFile + L"\"\n";

	while (  (theApp.m_zipDetailsList.GetItemText(count,FILENAMECOL) != "") && (theApp.m_zipDetailsList.GetItemText(count,COLORCOL) != COLOR_RED) )
	{
		datEntry +=L"\tfile ( name ";
		datEntry +=theApp.m_zipDetailsList.GetItemText(count,FILENAMECOL)+L".";	// filename
		datEntry +=theApp.m_zipDetailsList.GetItemText(count,2);	// ext
		datEntry.TrimRight(L".");	// remove dots from filenames without extensions
		datEntry+=L" size ";
		datEntry +=theApp.m_zipDetailsList.GetItemText(count,3);	// size
		datEntry+=L" date ";
		datEntry +=theApp.m_zipDetailsList.GetItemText(count,4);	// date
		datEntry+=L" crc ";
		datEntry +=theApp.m_zipDetailsList.GetItemText(count,5);	// crc

		datEntry +=L" )\n";
		count++;
	}
	datEntry +=L")\n";

	// save it
	entry2File(datEntry);
}

void CSettingsandTools::entry2File(CStringW entry)
{
	FILE* outFile;
	errno_t err;
	// append to the .dat file
	err = _wfopen_s(&outFile, theApp.m_DATPath, L"a, ccs=UTF-8");
	if (outFile == NULL)
	{
		AfxMessageBox(L"Failed to open dat file:"+theApp.m_DATPath, MB_ICONEXCLAMATION, 0);
		return;
	}

	fwrite(entry, sizeof( wchar_t ), entry.GetLength(), outFile );
	fclose(outFile);
}

//todo - add a simple map here which keeps track of filenames, so we don't get a duplicate on the way in.

//		theApp.m_datZipListit = theApp.m_datZipList.find(zipFilename);
//		if (theApp.m_datZipListit != theApp.m_datZipList.end())
//		{
//			//AfxMessageBox("Duplicate entry in .dat file detected! "+zipFilename, MB_ICONEXCLAMATION, 0);
//			duplicates+=zipFilename+"\r\n";
  //          zipFilename="";
//			duplicateEntries = true;
//		}
//		
//		else

// sourcepath is which folder we're currently browsing/scanning.  formerly used the "home path"
void CSettingsandTools::makeDATFile(CString m_SourcePath)
{
	CStringW filename;
	CString txt;
	unsigned int m_zipCount=0;
	unsigned int m_romCount=0;
	CProgressDlg progDlg;
	if (m_SourcePath.GetLength() == 0)
		return;

	if (!m_cmdLine)
	{
		
		progDlg.Init(PROG_MODE_TIMER, 1000, L"Scanning files, hang on...");
	}

	// now we have the path+filename we want to work with.
	// scan all the files in this selected folder.
	m_zipDir.ClearDirs();
	m_zipDir.ClearFiles();
	// look in the user folder, recursively is decided by the popup dialog
	m_zipDir.GetDirs(m_SourcePath, m_recursive);


   ///////////////////////////////////////////////////
   // get all files in the dirs we found above
   m_zipDir.GetFiles(L"*.zip");

   // sort them by name, ascending
   m_zipDir.SortFiles(CSADirRead::eSortAlpha, false);
   CSADirRead::SAFileVector &files = m_zipDir.Files();
   m_zipFiles = files.begin();

   // loop until we're out of files
   while ( m_zipFiles != files.end())
   {
		filename = (*m_zipFiles).m_sName;
		
		if (!m_cmdLine)
			progDlg.UpdateText(L"Scanning files, hang on...",filename);
			
		// here we open the zip file, and get the contents out.
		m_romCount +=zip2dat(filename);
		m_zipFiles++;	// next file please
		m_zipCount++;
   }
	if (!m_cmdLine)
	{
		progDlg.DestroyWindow();
		txt.Format(L"\r\n.DAT contains %d zip files with %d files (roms)", m_zipCount, m_romCount);
		AfxMessageBox(L"Finished!\r\n"+theApp.m_DATPath+" created."+txt, MB_ICONINFORMATION, 0);
	}
}


// sourcepath is which folder we're currently browsing/scanning.  formerly used the "home path"
// express version examines existing dat files and only builds when things are updated.
void CSettingsandTools::makeDATFile_express(CString m_SourcePath)
{
CStringW datsPath = L"E:\\FTP site\\DOSGuy uploads\\TDC_WIP\\Games\\dats"; // todo: ini this

	CStringW filename;
	CStringW txt;
	unsigned int m_zipCount = 0;
	unsigned int m_romCount = 0;
	CProgressDlg progDlg;
	WIN32_FIND_DATAW FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	if (!m_cmdLine)
	{

		progDlg.Init(PROG_MODE_TIMER, 1000, L"Scanning files, hang on...");
	}



	// now we have the path+filename we want to work with.
	// scan all the files in this selected folder.
	m_zipDir.ClearDirs();
	m_zipDir.ClearFiles();
	// look in the user folder, recursively is decided by the popup dialog
	m_zipDir.GetDirs(datsPath, m_recursive);




	///////////////////////////////////////////////////
	// get all files in the dirs we found above
	m_zipDir.GetFiles(L"*.dat");

	// sort them by name, ascending
	m_zipDir.SortFiles(CSADirRead::eSortAlpha, false);
	CSADirRead::SAFileVector &files = m_zipDir.Files();
	m_zipFiles = files.begin();

	// loop until we're out of files
	while (m_zipFiles != files.end())
	{
		filename = (*m_zipFiles).m_sName;

		if (!m_cmdLine)
			progDlg.UpdateText(L"Scanning files, hang on...", filename);

		// here we open the zip file, and get the contents out.
		//m_romCount += zip2dat(filename);
		hFind = FindFirstFileW(filename, &FindFileData);
		FILETIME a = FindFileData.ftLastWriteTime;
		
		hFind = FindFirstFileW(m_SourcePath+L"\\Games\\Files\\1995", &FindFileData);
		FILETIME b = FindFileData.ftLastWriteTime;

		if (b.dwLowDateTime > a.dwLowDateTime) // do dat that folder.

		m_zipFiles++;	// next file please
		m_zipCount++;
	}
	if (!m_cmdLine)
	{
		progDlg.DestroyWindow();
		txt.Format(L"\r\n.DAT contains %d zip files with %d files (roms)", m_zipCount, m_romCount);
		AfxMessageBox(L"Finished!\r\n" + theApp.m_DATPath + " created." + txt, MB_ICONINFORMATION, 0);
	}
}



void CSettingsandTools::OnBnClickedMakeadat()
{
	CFileStuff fileStuff;
	FILE* outFile;
	CString header;
	
	if (theApp.m_collectionPath.GetLength() == 0)
		OnBnClickedSetdoscollectionpathbtn();

	// browse to the output filename
	fileStuff.m_windowTitle = L"Name of file to create...";
	if (fileStuff.UserBrowseSave(&theApp.m_DATPath, L"dat files (*.dat)\0*.dat", L"dat") != TRUE)
return;

// popup the .dat file information GUI

CDATFileInfoDlg dlg;
dlg.m_displayOnly = false;
if (dlg.DoModal() == 2)	// cancel button hit?
return;

m_recursive = dlg.m_recursive;
header = "DOSCenter (\r\n\tName:";
header += dlg.m_datTitleTxt;
header += "\r\n\tDescription: ";
header += dlg.m_datDescTxt;
header += "\r\n\tVersion: ";
header += dlg.m_datVerTxt;
header += "\r\n\tDate: ";
header += dlg.m_dateTxt;
header += "\r\n\tAuthor: ";
header += dlg.m_datAuthorTxt;
header += "\r\n\tHomepage: ";
header += dlg.m_datWebTxt;
header += "\r\n\tComment: ";
header += dlg.m_datCommentTxt;
header += "\r\n)";

// delete the .dat file if it exists, since the creator program uses append mode.
// add a header to it.
// path of files we're scanning
//TODO - make a subroutine of this "writeheader" so we can call it from the command line the same way.
_wfopen_s(&outFile, theApp.m_DATPath, L"w, ccs=UTF-8");
if (outFile != NULL)
{
	header.Replace(L"\r\n", L"\r");
	fwrite(header, sizeof(wchar_t), header.GetLength(), outFile);
	fclose(outFile);
	outFile = NULL;
}
// using current zip folder instead of home folder
makeDATFile(m_SourcePath);

CRegistry regMyReg(CREG_CREATE);  // create registry key if it doesn't exist.
if (regMyReg.Open(L"Software\\DOSCenter", HKEY_CURRENT_USER))
{
	// Now set the new values
	regMyReg[L"masterCollectionFolder"] = theApp.m_collectionPath;
	regMyReg.Close();
}
//	((CButton*)GetDlgItem(IDC_ZIPFOLDERTXT))->SetWindowText("Zip folder: "+m_SourcePath);


}

int CSettingsandTools::zip2dat(CStringW zipfilename)
{
	CStringW datEntry;
	CStringW filename, ext, size, scrc, date, tmp;
	WORD dosDate;
	WORD dosTime;
	CTime time;
	CDOSCenterDlg dc;

	// grab the zip and start collecting each file inside.
	dc.zip2map2(zipfilename);


	// remove the path off the filename if told to on the command line
	if (m_noPathInDat)
	{
		//if (zipfilename.Find('\\') != -1) 
		//zipfilename = zipfilename.Right(zipfilename.GetLength() - (zipfilename.ReverseFind('\\')) -1 );
		zipfilename = theApp.extractFilenameWOPath(zipfilename);
	}

	if (tmpZipList[zipfilename] == 1)
	{
		if (!theApp.m_quietmode)
		{
			AfxMessageBox(L"Duplicate zip filename detected and skipped!\r\n" + zipfilename, MB_ICONEXCLAMATION, 0);
			return 0;
		}
	}
	else
		tmpZipList[zipfilename] = 1;

	datEntry = L"\ngame (\n\tname \"";
	datEntry += zipfilename + L"\"\n";


	theApp.m_zipMap[0].it = theApp.m_zipMap[0].ROMmap.begin();
	while (theApp.m_zipMap[0].it != theApp.m_zipMap[0].ROMmap.end())
	{
		filename = theApp.m_zipMap[0].it->second.name;	// get updated raw rom name from the zip and keep track of it for later.
		tmp = filename;
		ext = dc.extractExtension(tmp);
		size.Format(L"%d", theApp.m_zipMap[0].it->second.size);
		unsigned int crc = theApp.m_zipMap[0].it->second.crc;
		// special patch for some weird shit where sometimes a folder has a non-zero size and this throws off everyone.
		if (((ext.Right(1) == '\\') || (filename.Right(1) == '\\')) && (crc == 0) && (size != '0'))
		{
			// todo: sure would be nice to log these so the zip could be repackaged to fix this.
			size = L"0";
		}

		scrc.Format(L"%08x", crc);
		dosDate = (WORD)(theApp.m_zipMap[0].it->second.date >> 16);
		dosTime = (WORD)theApp.m_zipMap[0].it->second.date & 0xffff;
		CTime time(dosDate, dosTime);
		date = time.Format(L"%Y/%m/%d %H:%M:%S");

		scrc.MakeUpper();

		datEntry += "\tfile ( name ";
		datEntry += filename;
		datEntry += " size ";
		datEntry += size;
		datEntry += " date ";
		datEntry += date;
		datEntry += " crc ";
		datEntry += scrc;

		datEntry += " )\n";


		theApp.m_zipMap[0].it++;
	}


#if 0
	for (int i = 0; i < theApp.m_nFileCount; i++)	// gather all the info for each file in this zip and display it on the right hand window
	{
		filename = theApp.m_tmpzipList[i].fileName;
		theApp.m_zipMap[0].ROMmap[filename].name
		ext = dc.extractExtension(filename);
		size.Format(L"%d", theApp.m_tmpzipList[i].size);
		unsigned int crc = theApp.m_tmpzipList[i].crc;

		// special patch for some weird shit where sometimes a folder has a non-zero size and this throws off everyone.
		if ( ((ext.Right(1) == '\\') || (filename.Right(1) == '\\')) && (crc == 0) && (size != '0') )
		{
			// todo: sure would be nice to log these so the zip could be repackaged to fix this.
			size = L"0";
		}

		scrc.Format(L"%08x", crc);
		dosDate = (WORD)(theApp.m_tmpzipList[i].date >> 16);
		dosTime = (WORD)theApp.m_tmpzipList[i].date & 0xffff;
		CTime time(dosDate, dosTime);
		date =time.Format(L"%Y/%m/%d %H:%M:%S" );

		scrc.MakeUpper();

		datEntry +="\tfile ( name ";
		datEntry +=theApp.m_tmpzipList[i].fileName;
		datEntry+=" size ";
		datEntry +=size;
		datEntry+=" date ";
		datEntry +=date;
		datEntry+=" crc ";
		datEntry +=scrc;

		datEntry +=" )\n";
	}
#endif	
	datEntry +=")\n";
	// huck it into the file
	entry2File(datEntry);
	return theApp.m_nFileCount;
}
void CSettingsandTools::OnBnClickedScratchpadbrowse()
{
	CFileStuff fileStuff;
	// open a "browse to folder" dialog
	if (fileStuff.UserBrowseFolder(theApp.m_tmpWorkingFolder, L"Set temporary working folder.") != TRUE)
		return;
	if (theApp.m_tmpWorkingFolder.Right(1) != L"\\")
		theApp.m_tmpWorkingFolder+=L"\\";

	((CButton*)GetDlgItem(IDC_SCRATCHPATHTXT))->SetWindowText(theApp.m_tmpWorkingFolder);
	
}

void CSettingsandTools::OnBnClickedSetdoscollectionpathbtn()
{
	CFileStuff fileStuff;
	// open a "browse to folder" dialog
	if (fileStuff.UserBrowseFolder(theApp.m_collectionPath, L"Where's the master DOS collection?") != TRUE)
		return;
	if (theApp.m_collectionPath.Right(1) != L"\\")
		theApp.m_collectionPath+=L"\\";

	((CButton*)GetDlgItem(IDC_DOSCOLLECTIONPATHTXT))->SetWindowText(theApp.m_collectionPath);
	
}

void CSettingsandTools::OnBnClickedScoringcfgbtn()
{
	CScoringSetupDlg dlg;
	dlg.DoModal();
	if (theApp.m_reloadIgnore)
		dlg.populateIgnoreList();
}

void CSettingsandTools::OnBnClickedDosboxbrowse()
{
	CFileStuff fileStuff;
	// open a "browse to folder" dialog
	if (fileStuff.UserBrowseFolder(theApp.m_dosboxPath, L"Where is DOSBox?") != TRUE)
		return;
	if (theApp.m_dosboxPath.Right(1) != L"\\")
		theApp.m_dosboxPath+=L"\\";
	
	((CButton*)GetDlgItem(IDC_DOBOXPATHTXT))->SetWindowText(theApp.m_dosboxPath);
}

void CSettingsandTools::OnBnClickedDatfileinfo()
{
	CDATFileInfoDlg dlg;
	dlg.m_displayOnly = true;
	dlg.DoModal();

}
void CSettingsandTools::OnBnClickedMissinglist()	// generate report button
{
	CDOSCenterDlg DC;
	DC.OnBnClickedMissinglist();
}


void CSettingsandTools::OnBnClickedDetailedreports()
{
	
	((CButton*)GetDlgItem(IDC_PERFECTSTOO))->EnableWindow(0);
	((CButton*)GetDlgItem(IDC_PERFECTSTOO))->SetCheck(0);

	if ( ((CButton*)GetDlgItem(IDC_DETAILEDREPORTS))->GetCheck() )
	{	
		theApp.m_detailedReport = true;
		((CButton*)GetDlgItem(IDC_PERFECTSTOO))->EnableWindow(1);
		((CButton*)GetDlgItem(IDC_PERFECTSTOO))->SetCheck(theApp.m_reportPerfects);
	}
	else
		theApp.m_detailedReport = false;
}

void CSettingsandTools::OnBnClickedPerfectstoo()
{
	if ( ((CButton*)GetDlgItem(IDC_PERFECTSTOO))->GetCheck() )
		theApp.m_reportPerfects = true;
	else
		theApp.m_reportPerfects = false;

}


void CSettingsandTools::OnBnClickedHavemisslistcfg()
{
	CHaveMissDlg dlg;
	dlg.DoModal();
}

// purge all databases from memory.  Called typically when loading a new dat file or exiting from DOSCenter altogether.
void CSettingsandTools::deleteKeys()
{
    ROMdata tempROMdata;						// only used for deleting the entire cmap
	POSITION pos;
	CString    nKey;

	if (theApp.m_DATfileLoaded)
	{

		theApp.m_crcMap.clear();

		// delete the elements
		pos = theApp.m_ROMDATEntryName.GetStartPosition();
		while( pos != NULL )
		{
			while (pos != NULL)
			{
				theApp.m_ROMDATEntryName.GetNextAssoc(pos, nKey,tempROMdata);
				theApp.m_ROMDATEntryName.RemoveKey(nKey);
			}

		}
		// RemoveAll deletes the keys
		theApp.m_ROMDATEntryName.RemoveAll();
		
		// delete the elements
		pos = theApp.m_ROMDATEntryCRC.GetStartPosition();
		while( pos != NULL )
		{
			CString    nKey;
			while (pos != NULL)
			{
				theApp.m_ROMDATEntryCRC.GetNextAssoc(pos, nKey,tempROMdata);
				theApp.m_ROMDATEntryCRC.RemoveKey(nKey);
			}

		}
		// RemoveAll deletes the keys
		theApp.m_ROMDATEntryCRC.RemoveAll();
		
		theApp.m_datZipList.clear();
		theApp.m_datZipList.empty();
	}

	// clear the counters
	theApp.m_datfileCount=0;
	theApp.m_datROMCount=0;
}



// open a browse dialog to set the path for the master mdb file
void CSettingsandTools::OnBnClickedLoadMDBfile()
{
	CFileStuff fileStuff;

	LPCWSTR FileFilter = L"dat files (*.mdb)\0*.mdb\0";
	LPCWSTR FileExt = L"mdb";
	fileStuff.m_windowTitle = L"Gimme a mdb file please...";

	

	if (theApp.m_MDBPath.GetLength() == 0)
	{
		if (fileStuff.UserBrowseOpen(&fileStuff.m_sFilePath, FileFilter, FileExt, false))	//true=allow multiple .dat files
		{
			theApp.m_MDBPath = fileStuff.m_sFilePath;
			CRegistry regMyReg(CREG_CREATE);  // create registry key if it doesn't exist.
			if (regMyReg.Open(L"Software\\DOSCenter", HKEY_CURRENT_USER))
			{
				// Now set the new values
				regMyReg[L"MDBFolders"] = theApp.m_MDBPath;
				regMyReg.Close();
			}
		}
	}

	if (theApp.m_MDBPath.GetLength() != 0)
	{
		loadMDB(theApp.m_MDBPath);
		theApp.m_MDBfileLoaded = true;
	}
}



// open and load the mdb file into memory
void CSettingsandTools::loadMDB(CString mdbFile)
{
	ZipInfoMDB tmpZipInfo;
	theApp.m_romCountMDB = 0;
	CProgressDlg progDlg;
	progDlg.Init(PROG_MODE_TIMER, 1000, L"Loading .mdb file...");

	CString sDsn;

	sDsn.Format(_T("ODBC;DRIVER={%s};DSN='';DBQ=%s"), _T("MICROSOFT ACCESS DRIVER (*.mdb)"), mdbFile);
	// Open the database
	m_database.Open(NULL, FALSE, FALSE, sDsn);

	// Allocate the recordset
	CTDCSet m_pSet(&m_database);

	// Build the SQL statement
	CString sSqlString = _T("SELECT * from TDC");

	// Execute the query
	m_pSet.Open(CRecordset::forwardOnly, sSqlString, CRecordset::readOnly);

	// Loop through each record
	while (!m_pSet.IsEOF())
	{
		// Copy each column into a variable
		CStringW zipname = m_pSet.m_longfilename;
		tmpZipInfo.shortFilename = m_pSet.m_83name;
		tmpZipInfo.year = m_pSet.m_year;
		tmpZipInfo.play = m_pSet.m_play;
		tmpZipInfo.setup = m_pSet.m_setup;
		tmpZipInfo.found = m_pSet.m_found;
		tmpZipInfo.ftp = m_pSet.m_FTP;
		tmpZipInfo.notes = m_pSet.m_notes;
		tmpZipInfo.publisher = m_pSet.m_publisher;
		tmpZipInfo.tagsandflags = m_pSet.m_tagsandflags;
		tmpZipInfo.type = m_pSet.m_type;
		tmpZipInfo.index = theApp.m_romCountMDB;

		db2cmap(zipname, tmpZipInfo);

		// goto next record
		m_pSet.MoveNext();

	}
	// Close the database
	m_database.Close();
	progDlg.DestroyWindow();
}

// add a db entry into the internal cmap 
void CSettingsandTools::db2cmap(CStringW zipname, ZipInfoMDB tmpZipInfo)
{
	theApp.m_ZIPDATEntryNameMDB.SetAt(zipname, tmpZipInfo);
	theApp.m_zipListMDB[theApp.m_romCountMDB].filename = zipname;				// add entry to our iterated list too.
	theApp.m_zipListMDB[theApp.m_romCountMDB].status = 0;
	theApp.m_zipListMDB[theApp.m_romCountMDB].year = tmpZipInfo.year;
	theApp.m_zipListMDB[theApp.m_romCountMDB].found = tmpZipInfo.found;
	theApp.m_zipListMDB[theApp.m_romCountMDB].ftp = tmpZipInfo.ftp;
	//theApp.m_zipListMDB[theApp.m_romCountMDB].mobyURL = tmpZipInfo. .m_mobyURL;
	theApp.m_zipListMDB[theApp.m_romCountMDB].notes = tmpZipInfo.notes;
	theApp.m_romCountMDB++;
}

// update an existing record
// if old="" then a new record is generated
// old is the "longfilename" field in the mdb
// entry: zipName2Vars has already been called with "snew" data.
// if inputFlag=-1, then data is taken out of the old entry before being replaced
void CSettingsandTools::updateDBRecord(int inputFlag, CStringW sold, CStringW snew)
{
	CdizInfo fiz;	// file_id.diz structure class
	dizInfo tmpDiz;
	CString sDsn;
	CString tmp;
	fiz.zipName2Vars(snew, 1, tmpDiz);


	// Allocate the recordset
	CTDCSet m_pSet(&m_database);

	// open the database
	m_pSet.GetDefaultConnect(theApp.m_MDBPath);

	sDsn.Format(_T("ODBC;DRIVER={%s};DSN='';DBQ=%s"), _T("MICROSOFT ACCESS DRIVER (*.mdb)"), theApp.m_MDBPath);
	m_database.Open(NULL, FALSE, FALSE, sDsn);
	m_pSet.Open();

	ZipInfoMDB tmpZipInfo;

	// delete a record?
	// sold=entry to delete
	if (inputFlag == DELETE_ENTRY)
	{
		m_pSet.MoveFirst();	// start at the top and find it
		int i = 0;
		while (1)
		{
			tmp = m_pSet.m_longfilename;
			if (tmp == sold)	// found it?
				break;
			m_pSet.MoveNext();
			i++;
		}
		m_pSet.Delete();	// delete the db entry.
		m_pSet.Close();
		return;
	}



	// replacing a record?
	if (sold.GetLength() > 0)
	{
		m_pSet.MoveFirst();	// start at the top and find it
		int i = 0;
		while (1)
		{
			tmp = m_pSet.m_longfilename;
			if (tmp == sold)	// found it?
				break;
			m_pSet.MoveNext();
			i++;
		}
		theApp.m_ZIPDATEntryNameMDB.Lookup(sold, tmpZipInfo);	// get all the data from the DB  TODO:  tmpZipInfo.index should have exactly the one we're updating.

		// most data is already poked into the dizInfo struct already.
		// save any existing info that does not come from the filename itself
		if (inputFlag == -1)
		{
			tmpDiz.shortFilename = m_pSet.m_83name;
			tmpDiz.cmdLine = m_pSet.m_play;
			tmpDiz.setup = m_pSet.m_setup;
			tmpDiz.notes = m_pSet.m_notes;
			if (tmpDiz.mobyURL.IsEmpty())
				tmpDiz.mobyURL = m_pSet.m_mobyURL;
		}
		m_pSet.Delete();	// delete the db entry.

	}
	
	
	m_pSet.AddNew();	// add a new entry
	// tranfer all data over to the database

	m_pSet.m_longfilename = tmpDiz.filename;	// name update, re-use the rest of the data
	m_pSet.m_83name = tmpDiz.shortFilename;
	m_pSet.m_play = tmpDiz.cmdLine;
	m_pSet.m_setup = tmpDiz.setup;
	m_pSet.m_found = TRUE; // tmpDiz.found;
	m_pSet.m_FTP = TRUE; // tmpDiz.ftp;
	m_pSet.m_notes = tmpDiz.notes;
	m_pSet.m_tagsandflags = tmpDiz.flagsTxt;
	m_pSet.m_type = tmpDiz.type;
	m_pSet.m_publisher = tmpDiz.publisher;
	m_pSet.m_genre = tmpDiz.genre;
	m_pSet.m_year = tmpDiz.year;
	m_pSet.m_Chinese = tmpDiz.lang_cn;
	m_pSet.m_Czech = tmpDiz.lang_cz;
	m_pSet.m_Danish = tmpDiz.lang_dk;
	m_pSet.m_Dutch = tmpDiz.lang_nl;
	m_pSet.m_English = tmpDiz.lang_en;
	m_pSet.m_Finnish = tmpDiz.lang_fi;
	m_pSet.m_French = tmpDiz.lang_fr;
	m_pSet.m_German = tmpDiz.lang_de;
	m_pSet.m_Hewbrew = tmpDiz.lang_hw;
	m_pSet.m_Italian = tmpDiz.lang_it;
	m_pSet.m_Japanese = tmpDiz.lang_jp;
	m_pSet.m_Norwegian = tmpDiz.lang_no;
	m_pSet.m_Polish = tmpDiz.lang_pl;
	m_pSet.m_Russian = tmpDiz.lang_ru;
	m_pSet.m_Spanish = tmpDiz.lang_sp;
	m_pSet.m_Korean = tmpDiz.lang_kr;
	m_pSet.m_Hungarian = tmpDiz.lang_hu;
	m_pSet.m_Ukranian = tmpDiz.lang_ua;
	m_pSet.m_Indonesian = tmpDiz.lang_id;
	m_pSet.m_Swedish = tmpDiz.lang_se;
	m_pSet.m_Slovenian = tmpDiz.lang_sl;
	m_pSet.m_Serbian = tmpDiz.lang_sr;
	m_pSet.m_mobyURL = tmpDiz.mobyURL;
	m_pSet.m_Belarusian = tmpDiz.lang_be;
	m_pSet.m_Arabic = tmpDiz.lang_ar;
	m_pSet.m_Greek = tmpDiz.lang_gr;
	m_pSet.m_Latin = tmpDiz.lang_la;
	m_pSet.m_Turkish = tmpDiz.lang_tr;
	m_pSet.m_Icelandic = tmpDiz.lang_is;
	m_pSet.m_Croatian = tmpDiz.lang_hr;
	m_pSet.m_Brazilian = tmpDiz.lang_br;
	m_pSet.m_Estonian = tmpDiz.lang_ee;
	m_pSet.m_Persian = tmpDiz.lang_fa;

	// and write it in
	//UpdateData(FALSE);
	m_pSet.Update();
	m_pSet.Close();

	// update the internal cmap too. TODO likewise, the tmpZipInfo.index already has the goods.
	CStringW zipname = m_pSet.m_longfilename;
	tmpZipInfo.shortFilename = m_pSet.m_83name;
	tmpZipInfo.year = m_pSet.m_year;
	tmpZipInfo.play = m_pSet.m_play;
	tmpZipInfo.setup = m_pSet.m_setup;
	tmpZipInfo.found = m_pSet.m_found;
	tmpZipInfo.ftp = m_pSet.m_FTP;
	tmpZipInfo.notes = m_pSet.m_notes;
	tmpZipInfo.publisher = m_pSet.m_publisher;
	tmpZipInfo.tagsandflags = m_pSet.m_tagsandflags;
	tmpZipInfo.type = m_pSet.m_type;
	tmpZipInfo.index = theApp.m_romCountMDB;
	db2cmap(zipname, tmpZipInfo);

}

// checks that each entry in the database tagged as "ok" actually exists in the archive
void CSettingsandTools::OnBnClickedMdbCheck()
{
	CStringW checkName;
	FILE* inFile;
	CdizInfo fiz;	// file_id.diz structure class
	dizInfo tmpDiz;
	CStringW dataOut;
	ZipInfoMDB tmpZipInfo;
	CSADirRead m_dr;
	unsigned int hash;
	CSADirRead::SAFileVector::const_iterator m_fit; // index thingyabob into file array
	CPopupViewerTxtDlg dlg;

	CProgressDlg progDlg;
	progDlg.Init(PROG_MODE_TIMER, 1000, L"Scanning files...");

	std::map <unsigned int, CStringW > filenameHash;
	std::map <unsigned int, CStringW >::iterator filenameHashIt;



	if (!theApp.m_MDBfileLoaded)
	{
		OnBnClickedLoadMDBfile();
	}
	dataOut = L"--Extra files not in the database--\r\n";

	for (int l = 0; l<theApp.m_romCountMDB; l++)
	{
		// if we know it's missing, no point in checking.
		if (theApp.m_zipListMDB[l].found == false)
		{
			//CStringW f = theApp.m_zipListMDB[l].filename;
			continue;
		}
		fiz.zipName2Vars(theApp.m_zipListMDB[l].filename, false, tmpDiz);
		checkName = theApp.m_collectionPath + tmpDiz.year + L"\\" + theApp.m_zipListMDB[l].filename;
		//progDlg.UpdateText(L"Scanning files...", checkName);
		hash = hashAString((CT2A)checkName, 0);
		if (filenameHash[hash].IsEmpty())
			filenameHash[hash] = checkName;
		else
			dataOut = L"--HASH COLLISION DETECTED--\r\n" + checkName+L"\r\n";
	}
	
	progDlg.UpdateText(L"Checking for files not in the database...", L"");
	// now do the inverse.  Every file in the TDC archive must be in the db.
	//dataOut += L"--Files in TDC that are missing from the database--\r\n";
	// scan and build a listing of all the files we can locate.
	// now we have the path+filename we want to work with.
	// scan all the files in this selected folder.
	m_dr.ClearDirs();
	m_dr.ClearFiles();
	// look in the user folder, recursively
	m_dr.GetDirs(theApp.m_collectionPath, true);
	///////////////////////////////////////////////////
	// get all files in the dirs we found above
	//((CButton*)GetDlgItem(IDC_CURRENT_FILENAME_TXT))->SetWindowText("Reading file list");


	m_dr.GetFiles(L"*.*");

	// sort them by name, ascending
	m_dr.SortFiles(CSADirRead::eSortAlpha, false);
	CSADirRead::SAFileVector &files = m_dr.Files();
	m_fit = files.begin();

	//progDlg.UpdateText(L"Checking for files not in the database...", checkName);


	while ((m_fit) != files.end())
	{

		CSADirRead::SAFileVector &files = m_dr.Files();

		//matches = "";  // clear previous filename matches		
		// ok, somewhere in here are our files.  Get one
		//!checkName = theApp.extractFilenameWOPath((*m_fit).m_sName);
		hash = hashAString((CT2A)(*m_fit).m_sName, 0);

		CStringW q = filenameHash[hash];
		if (q == (*m_fit).m_sName)
		{
			filenameHash[hash] = "";	// we physically have this entry from the database in the archive.  
		}
		else // a file exists that is not in the database
		{
			dataOut += (*m_fit).m_sName + L"\r\n";
			//filenameHash[hash] = ;
		}
		//fiz.zipName2Vars(checkName, true, tmpDiz);
		//if (!theApp.m_ZIPDATEntryNameMDB.Lookup(checkName, tmpZipInfo))
		//{
		//	dataOut += checkName + L"\r\n";
		//}
		

		/*
		found = false;
		for (int l = 0; l<m_romCount; l++)
		{
			// if we know it's missing, no point in checking.
			if (m_zipList[l].found == false)
				continue;

			//fiz.zipName2Vars(m_zipList[l].filename, false);
			CString fn = m_zipList[l].filename;
			if (sourceFilename == fn)
			{
				found = true;
				m_zipList[l].found = false; // burn this one for next time
				break;
			}
		}
		if (!found)
		{
			// did not find this file in the db!
			theApp.m_notfound[theApp.m_nfIdx] = sourceFilename;
			theApp.m_nfIdx++;
		}
		*/
		m_fit++;
	}

	dataOut += L"--Files in database not found in archive--\r\n";

	filenameHashIt = filenameHash.begin();
	while ((filenameHashIt) != filenameHash.end())
	{
		
		//if (filenameHash.at()
		if (filenameHashIt->second != L"")
			dataOut += filenameHashIt->second + L"\r\n"; // this is a filenot found in database.

		filenameHashIt++;
	}




	dlg.m_viewerTxt = dataOut;
	dlg.m_fileNameViewer = L"Missing report";
	dlg.DoModal();
	progDlg.DestroyWindow();
}

// create year based have/miss lists
void CSettingsandTools::makeLists()
{

	CString sYear;
	CStringW outDataHave, outDataMiss;
	CStringW allTitles, byYear;
	FILE* outFileH;
	FILE* outFileM;
	FILE* allOut;
	FILE* byyear;

	CdizInfo fiz;	// file_id.diz structure class

	if (theApp.m_romCountMDB == 0)	// master list hasn't been parsed?
		OnBnClickedLoadMDBfile(); 

	//if (theApp.m_quietmode)
	//	((CButton*)GetDlgItem(IDC_CURRENT_FILENAME_TXT))->SetWindowText("Exporting lists to " + m_collectionPath);// print filename here 

	_wfopen_s(&allOut, theApp.m_dosboxPath + "\\master.txt", L"w, ccs=UTF-8");
	if (allOut != NULL)	// close out master list
	{
		fclose(allOut);
		allOut = NULL;
	}

	_wfopen_s(&byyear, theApp.m_dosboxPath + "\\byyear.txt", L"w, ccs=UTF-8");

	for (int year = 1981; year<2021; year++)
	{
		outDataHave = "";
		outDataMiss = "";
		sYear.Format(L"%d", year);
		_wfopen_s(&outFileH, theApp.m_dosboxPath + "\\" + sYear + "Have.txt", L"w, ccs=UTF-8");
		_wfopen_s(&outFileM, theApp.m_dosboxPath + "\\" + sYear + "Miss.txt", L"w, ccs=UTF-8");
		for (int l = 0; l<theApp.m_romCountMDB; l++) // for every single stinkin file in the db...
		{
			if (theApp.m_zipListMDB[l].year == sYear)
			{
				if ((theApp.m_zipListMDB[l].found == true) && (theApp.m_zipListMDB[l].ftp == true))
					byYear += theApp.m_zipListMDB[l].filename + L"\n"; // longName;
				if (theApp.m_zipListMDB[l].ftp == true)
					outDataHave += theApp.m_zipListMDB[l].filename + L"\n"; // longName;
				else //if (m_zipList[l].ftp == false)
				{
					outDataMiss += theApp.m_zipListMDB[l].filename; // longName;
					outDataMiss += "    ";
					outDataMiss += theApp.m_zipListMDB[l].notes + L"\n";
				}
			}
		}


		if (outFileH != NULL)
		{
			fwrite(outDataHave, sizeof(wchar_t), outDataHave.GetLength(), outFileH);
			fclose(outFileH);
			outFileH = NULL;
		}
		if (outFileM != NULL)
		{
			fwrite(outDataMiss, sizeof(wchar_t), outDataMiss.GetLength(), outFileM);
			fclose(outFileM);
			outFileM = NULL;
		}
	}


	// oddball years - 19xx, 198x, 199x
	outDataHave = "";
	outDataMiss = "";
	_wfopen_s(&outFileH, theApp.m_dosboxPath + "\\19xxHave.txt", L"w, ccs=UTF-8");
	_wfopen_s(&outFileM, theApp.m_dosboxPath + "\\19xxMiss.txt", L"w, ccs=UTF-8");
	for (int l = 0; l<theApp.m_romCountMDB; l++) // for every single stinkin file in the db...
	{
		if (theApp.m_zipListMDB[l].year == "19xx")
		{
			if ((theApp.m_zipListMDB[l].found == true) && (theApp.m_zipListMDB[l].ftp == true))
				byYear += theApp.m_zipListMDB[l].filename + L"\n"; // longName;
			if (theApp.m_zipListMDB[l].ftp == true)
				outDataHave += theApp.m_zipListMDB[l].filename + L"\n"; // longName;
			else //if (m_zipList[l].ftp == false)
			{
				outDataMiss += theApp.m_zipListMDB[l].filename; // longName;
				outDataMiss += "    ";
				outDataMiss += theApp.m_zipListMDB[l].notes + L"\n";
			}
		}
	}

	if (outFileH != NULL)
	{
		fwrite(outDataHave, sizeof(wchar_t), outDataHave.GetLength(), outFileH);
		fclose(outFileH);
		outFileH = NULL;
	}
	if (outFileM != NULL)
	{
		fwrite(outDataMiss, sizeof(wchar_t), outDataMiss.GetLength(), outFileM);
		fclose(outFileM);
		outFileM = NULL;
	}

	outDataHave = "";
	outDataMiss = "";
	_wfopen_s(&outFileH, theApp.m_dosboxPath + "\\199xHave.txt", L"w, ccs=UTF-8");
	_wfopen_s(&outFileM, theApp.m_dosboxPath + "\\199xMiss.txt", L"w, ccs=UTF-8");
	for (int l = 0; l<theApp.m_romCountMDB; l++) // for every single stinkin file in the db...
	{
		if (theApp.m_zipListMDB[l].year == "199x")
		{
			if ((theApp.m_zipListMDB[l].found == true) && (theApp.m_zipListMDB[l].ftp == true))
				byYear += theApp.m_zipListMDB[l].filename + L"\n"; // longName;
			if (theApp.m_zipListMDB[l].ftp == true)
				outDataHave += theApp.m_zipListMDB[l].filename + L"\n"; // longName;
			else //if (m_zipList[l].ftp == false)
			{
				outDataMiss += theApp.m_zipListMDB[l].filename; // longName;
				outDataMiss += "    ";
				outDataMiss += theApp.m_zipListMDB[l].notes + L"\n";
			}
		}
	}

	if (outFileH != NULL)
	{
		fwrite(outDataHave, sizeof(wchar_t), outDataHave.GetLength(), outFileH);
		fclose(outFileH);
		outFileH = NULL;
	}
	if (outFileM != NULL)
	{
		fwrite(outDataMiss, sizeof(wchar_t), outDataMiss.GetLength(), outFileM);
		fclose(outFileM);
		outFileM = NULL;
	}



	outDataHave = "";
	outDataMiss = "";
	_wfopen_s(&outFileH, theApp.m_dosboxPath + "\\198xHave.txt", L"w, ccs=UTF-8");
	_wfopen_s(&outFileM, theApp.m_dosboxPath + "\\198xMiss.txt", L"w, ccs=UTF-8");
	for (int l = 0; l<theApp.m_romCountMDB; l++) // for every single stinkin file in the db...
	{
		if (theApp.m_zipListMDB[l].year == "198x")
		{
			if ((theApp.m_zipListMDB[l].found == true) && (theApp.m_zipListMDB[l].ftp == true))
				byYear += theApp.m_zipListMDB[l].filename + L"\n"; // longName;
			if (theApp.m_zipListMDB[l].ftp == true)
				outDataHave += theApp.m_zipListMDB[l].filename + L"\n"; // longName;
			else //if (m_zipList[l].ftp == false)
			{
				outDataMiss += theApp.m_zipListMDB[l].filename; // longName;
				outDataMiss += "    ";
				outDataMiss += theApp.m_zipListMDB[l].notes + L"\n";
			}
		}
	}

	if (outFileH != NULL)
	{
		fwrite(outDataHave, sizeof(wchar_t), outDataHave.GetLength(), outFileH);
		fclose(outFileH);
		outFileH = NULL;
	}
	if (outFileM != NULL)
	{
		fwrite(outDataMiss, sizeof(wchar_t), outDataMiss.GetLength(), outFileM);
		fclose(outFileM);
		outFileM = NULL;
	}


	if (byyear != NULL)
	{
		fwrite(byYear, sizeof(wchar_t), byYear.GetLength(), byyear);
		fclose(byyear);
		byyear = NULL;
	}


	// master lists
	_wfopen_s(&byyear, theApp.m_dosboxPath + "\\alpha.txt", L"w, ccs=UTF-8");
	_wfopen_s(&allOut, theApp.m_dosboxPath + "\\master.txt", L"w, ccs=UTF-8");
	byYear = "";								// create an alphabetical have list, database should already by alphabetical.
	for (int l = 0; l<theApp.m_romCountMDB; l++) // for every single stinkin file in the db...
	{
		if ((theApp.m_zipListMDB[l].found == true) && (theApp.m_zipListMDB[l].ftp == true))
			byYear += theApp.m_zipListMDB[l].filename + L"\n"; // longName;

		allTitles += theApp.m_zipListMDB[l].filename;
		allTitles += "    ";
		allTitles += theApp.m_zipListMDB[l].notes + L"\n";
	}

	if (allOut != NULL)	// add each entry to the master list
	{
		fwrite(allTitles, sizeof(wchar_t), allTitles.GetLength(), allOut);
	}

	if (byyear != NULL)
	{
		fwrite(byYear, sizeof(wchar_t), byYear.GetLength(), byyear);
		fclose(byyear);
		byyear = NULL;
	}

	if (allOut != NULL)	// close out master list
	{
		fclose(allOut);
		allOut = NULL;
	}

//	if (theApp.m_quietmode)
//		((CButton*)GetDlgItem(IDC_CURRENT_FILENAME_TXT))->SetWindowText("Finished created files.  check " + m_collectionPath);// print filename here

}
/*
* fnv_32a_str - perform a 32 bit Fowler/Noll/Vo FNV-1a hash on a string
*
* input:
*	str	- string to hash
*	hval	- previous hash value or 0 if first call
*
* returns:
*	32 bit hash as a static hash type
*
* NOTE: To use the recommended 32 bit FNV-1a hash, use FNV1_32A_INIT as the
*  	 hval arg on the first call to either fnv_32a_buf() or fnv_32a_str().
*/
unsigned int CSettingsandTools::hashAString(char *str, unsigned int hval)
{
	unsigned char *s = (unsigned char *)str;	/* unsigned string */

	/*
	* FNV-1a hash each octet in the buffer
	*/
	while (*s) {

		/* xor the bottom with the current octet */
		hval ^= *s++;

		/* multiply by the 32 bit FNV magic prime mod 2^32 */
		hval += (hval << 1) + (hval << 4) + (hval << 7) + (hval << 8) + (hval << 24);
	}

	/* return our new hash value */
	return hval;
}

bool CSettingsandTools::is_utf8(const char* string)
{
	if (!string)
		return false;

	const unsigned char * bytes = (const unsigned char *)string;

	while (*bytes != 0x00)
	{
		if (*bytes == 0x3f)
			return true;
		bytes += 1;
	}
	return false;
}


void CSettingsandTools::OnBnClickedAdAssassinBtn()
{
	adAssassinDlg dlg;
	dlg.m_spamAssZipsPath = L"todo..pull last path from registry";
	dlg.DoModal();
}
