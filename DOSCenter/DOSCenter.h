
// DOSCenter2.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include <afxtempl.h>	// needed for cmap
#include "SortListCtrl.h"
#include <map>
#include <unordered_map>
#include "utf8.h"
#include "afxcmn.h"


// ID marker for a game entry in the .dat file
#define GAMEID L"game ("

// how about an enum here eh?
// zip names list (left side window)
#define COLORCOL 0
#define FILENAMECOL 1
#define NEWFILENAMECOL 2
#define SCORECOL 3
#define MATCHCOL 4
#define MISSCOL 5
#define UNKCOL 6


// these are inverted values as calculated here: http://www.drpeterjones.com/colorcalc/
#define COLOR_HIDE L"0xffffffff"
#define COLOR_RED L"0x0000ff"
#define COLOR_RED_IGNORED L"0x9999ff"
#define COLOR_GREEN_PERFECT L"0x009900"
#define COLOR_GREEN2 L"0x004000"			// CRC match, but filename is wrong
#define COLOR_GREEN3 L"0x00c0c0"
#define COLOR_GREEN4 L"0x00b0b0"
#define COLOR_BLACK L"0x000000"
#define COLOR_YELLOW L"0x4d7cc9" 
#define COLOR_BLUE   L"0xA0522D"
#define COLOR_LGREEN L"0x009999"
#define COLOR_PURPLE L"0xff00ff"
#define COLOR_BROWN_PURPLE L"0xff66AA"
#define COLOR_BROWN	L"0x006699"
#define COLOR_GRAY L"0x999999"
#define COLOR_PBROWN L"0x042690" // tzip color reddish brown
#define COLOR_WTF  L"a070f0"
#define COLOR_TZIPPED 0x770000
#define ALTERNATE_CRC L"0x80000000"		// flag that this ROM has multiple CRCs


#define TZIPPED 0x40000000


// CDOSCenterApp:
// See DOSCenter.cpp for the implementation of this class
//
//#define NEW_STATUS_COLLISION 0x100
#define COLLISION_COUNT_START_BIT 36

typedef struct _NEWROMINFO	// each file (ROM) inside the zip or a .dat entry
{
	CStringW name;			// ROM filename
	unsigned int size;		// size of file
	unsigned int date;		// upper 16=date, lower 16=time
	unsigned int status;	// bitmask info, see below
	unsigned int crc;		// 
	unsigned int datPtr;	// pointer back to .dat file entry#
//	unsigned int datOff;	// and the actual "file (" entry
	unsigned int collsionCount;
} NEWROMINFO;

// master structure for the .dat file.  
typedef struct _DATFILESTRUC
{
	CString filename;			// top level filename
	unsigned int count;			// # of files inside it
	unsigned int totalBytes;	// total number of bytes of the files (uncompressed)
	std::unordered_map<unsigned long long, unsigned int> CRCMap;	// storage pointer back to the .dat entry
	std::unordered_map<unsigned long long, unsigned int>::const_iterator it;	// an iterator
} DATFILESTRUC;



typedef struct _ROMENTRY	// each file (ROM) inside the zip or a .dat entry
{
	CStringW name;			// filename
	unsigned int fnamehash; // hash of filename (only used in collisions)
	unsigned int size;		// size 
	unsigned int crc;		// this rom's CRC 
	unsigned int date;		// upper 16=date, lower 16=time
	unsigned int status;	// bitmask info, see below
	CStringW correctFilename; // .dat entry filename
//	bool alternate;			// if set then
//	unsigned int crc2;		// this is valid (for pklited or lzexed file)
} datEntry;

// status bitfields
#define DATENTRY_STATUS_IGNORE 1	// this ROM is in the user's ignore list


typedef struct _ZIPMERGE
{
	CString filename;		// top level filename
	int		fileCount;		// # of files inside it
	std::map<CStringW, _ROMENTRY> ROMmap;	// storage, make the filename the key
	std::map<CStringW, _ROMENTRY>::const_iterator it;	// an iterator
} ZIPMERGE;


typedef struct _ZIPMAP // replace the above with this one.
{
	CString filename;		// top level filename
	int		fileCount;		// # of files inside it
	int		zipDatPtr;		// pointer back to the .dat entry
	std::map<CStringW, _ROMENTRY> ROMmap;	// storage, make the filename the key
	std::map<CStringW, _ROMENTRY>::const_iterator it;	// an iterator
} ZIPMAP;


typedef struct _BESTHITS
{ // hey dumbass.  if you change any of these, make sure you change sortResults() too!
	CString filename;	// filename of ZIP file
	unsigned int	score;		//  overall DAT score (CRC matches / count of files in the dat) + datsize count / bytematchcount;
	float   zipScore;	// % of matches in this zip
	unsigned int		perfects;	// # of matching filename+CRC's found
	unsigned int		zipCount;	// # of files inside this zip
	unsigned int		crcMatch;	// # of CRC matches but filename mismatches
	//unsigned int		dupes;		// # of CRC matches that are also in other .dat entries (not useful, but not damaging to the score)
	unsigned int		datCount;	// # of files from the dat for this top ten hit
	unsigned int		zipPos;		// CList position where the data was found
	unsigned int		datSizeCount;	// number of bytes in this entry in the .dat
	unsigned int		byteMatchCount; // number of bytes in this zip which match an entry in the .dat
} BESTHITS;


typedef struct _BESTHITS2
{
//	CString filename;	// filename of ZIP file
	unsigned int	score;		//  overall DAT score (CRC matches / count of files in the dat) + datsize count / bytematchcount;
//	float   zipScore;	// % of matches in this zip
	//unsigned int		perfects;	// # of matching filename+CRC's found
//	unsigned int		zipCount;	// # of files inside this zip
//	unsigned int		crcMatch;	// # of CRC matches but filename mismatches
	//unsigned int		dupes;		// # of CRC matches that are also in other .dat entries (not useful, but not damaging to the score)
	unsigned int		datCount;	// # of files from the dat for this top ten hit
	unsigned int		datSizeCount;	// number of bytes in this entry in the .dat
	unsigned int		matchCount; //
	unsigned int		byteMatchCount; // number of bytes in this zip which match an entry in the .dat
} BESTHITS2;


typedef struct _IGNOREFILE
{
	CString filename;
	CString ext;
	unsigned int crc;
} IGNOREFILE;
/*
typedef struct _FILESTRUC	// each .dat file entry, contains all the files inside the zip
{
	CString filename;		// top level filename from .dat
	int		fileCount;		// # of files inside entry
	std::map<unsigned int, _ROMENTRY> ROMmap;	// storage for crc, name of file and size
	std::map<unsigned int, _ROMENTRY>::const_iterator it;	// an iterator
	int statusFlags;
	CString realFilename;	// actual zip file that is the closest match to this .dat entry
	int collisionPtr;
} FILESTRUC;
*/

// CDOSCenterApp:
// See DOSCenter2.cpp for the implementation of this class
//
// rom data structure (each file inside a zip gets filled in here)
struct ROMdata
{
	CStringW fileName;	// filename.ext
	unsigned int size;	// file's size in bytes
	unsigned int crc;	// file's CRC as pulled from .dat file/zip header
	unsigned int date;	// file's date
	unsigned int collision;		// collision detected in CMAP
	unsigned int ptr2zipFileArray; // pointer to this rom's main .zip filename in the dat file
	int status;			// misc flags-n-stuff
	CStringW newFileName; // when a filename inside a zip is wrong, and the dat thinks it should be this
};


// Info about each ZIP file as its pulled in from the .dat file.  We also keep some data here during scanning
struct ZIPdata
{
	CStringW filename;	// zip filename 
	int fileCount;		// number of files declared in this zip(dat entry)
	int status;			// status (perfect match, missing some stuff, extra, whatever) - filled in after a file scan.
	unsigned int numBytes; // total number of bytes contained in this zip across all files.
	std::map<unsigned int, _ROMENTRY> ROMsMap;	// storage for each rom's crc, name of file and size
	std::map<unsigned int, _ROMENTRY>::const_iterator it;	// an iterator to be able to scan through this zip's list of roms

};

/////////////// database stuff
// Info about each ZIP file as its pulled in from the .mdb file.  We also keep some data here during scanning
struct ZIPdataMDB
{
	CString filename;	// zip filename 
	CString year;
	int fileCount;		// number of files declared in this zip
	int status;			// status (perfect match, missing some stuff, extra, whatever)
	bool found;
	bool ftp;
	CString mobyURL;
	CString notes;
};
// storage element for each line from the database
// data is stored AT the longFilename's position in the cmap
struct ZipInfoMDB
{
	//CString longFilename;	
	CString year;
	CString shortFilename;
	BOOL	found;
	BOOL	ftp;
	CString	publisher;
	CString	genre;
	CString	tagsandflags;
	CString	type;
	CString	play;
	CString setup;
	CString	notes;
	unsigned int index;
};



class CDOSCenterApp : public CWinAppEx
{
public:
	CDOSCenterApp();
	CString m_DATPath;
	CString m_MDBPath;
// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation
	unsigned int m_datfileCount;	// for info only, # of zips inthe loaded .dat file
	unsigned int m_datROMCount;		// " " " # of roms in the zips in the dat loaded
	unsigned int m_romCountMDB;		// number of zips in mdb
	bool m_DATfileLoaded;
	bool m_MDBfileLoaded;
	bool m_autoLoadDat;
	bool m_reloadIgnore;
	bool m_ignoreOn;
	bool m_recursiveScan;
	unsigned int m_minFilesize;
	CString m_currentlyTaggedZipFile;
	CSortListCtrl m_zipDetailsList;
	CSortListCtrl m_leftSideList;
	CSortListCtrl m_leftSideDATList;

	CString m_collectionPath;		// folder where main collection exists (ie, where .dat file is built off of)
	CString m_tmpWorkingFolder;
	CString m_dosboxPath;
	CString g_appPath;
	CString m_defaultMovePath;
	_IGNOREFILE m_ignoreThese[400];


	CString m_datName;
	CString m_datDescription;
	CString m_datHomepage;
	CString m_datdate;
	CString m_datVersion;
	CString m_datComment;
	CString m_datAuthor;

	DATFILESTRUC m_datMap;		// the main dat file map (global)
	ZIPMAP* m_zipMap;

	int m_ignoreTheseCount;
	CString m_ignoreFileData;
	int m_minScoreForAutoRename;
	bool	m_detailedReport;
	bool	m_ScanCRCFilename;
	bool	m_reportPerfects;
	bool m_datParsed;
	bool m_quietmode;
	bool m_processMode;
	bool m_commitMode;
	CString m_mySitesDat;
	CString m_fxpDlPath;
	
	ULONGLONG m_selectedYears;
	bool m_zipIsFoldered;

//	void makeNewKey(CString &ROMFilenameKey);
	
// here we store each ROM from the .dat file.  entries are stored as a key of romname_crc to make them unique
// if there are duplicates (like gwbasic.exe_987341 would show up multiple times across zip files), then each key is appended with ?1, ?2, etc to make them unique
	//    key      whatever  thing stored  whatever
	CMap< CString, LPCWSTR,   ROMdata,      ROMdata& > m_ROMDATEntryName;
	CMap< CString, LPCWSTR,   ROMdata,      ROMdata& > m_ROMDATEntryCRC;

	// list of zipfiles loaded in during the .dat parsing process
	// this guy has a map inside the structure inside the map that we're storing!
	// data stored here is zip filename and number of roms contained therein.
	std::unordered_map <unsigned int, ZIPdata > m_datZipList;
	std::unordered_map <unsigned int, ZIPdata >::iterator m_datZipListit;
	int m_nFileCount;
	
	// a globlal, but temporary storage item
	std::map <unsigned int, ROMdata > m_tmpzipList; // temporary storage for pulling things out of the cmap when looking inside a zip
	// this is another attempt at the above
	CMap< CString, LPCWSTR,	  int,      int& > m_ROMsInZip;	

	// would this work instead?
	// need to set ROMdata but have the filename drive the key/lookup so it can be found quickly.
	// clearing the map may take longer than simply overwriting existing contents and looping
	// through trying to find the element.
	//std::map <std::string, int > m_ROMsInZip2; // temporary storage for pulling things out of the cmap when looking inside a zip



	///////////////////////////////////// database stuff ///////////////////////////

	// list of zipfiles loaded in during the .dat parsing process
	// this guy has a map inside the structure inside the map that we're storing!
	// data stored here is zip filename and number of roms contained therein.
	std::map <unsigned int, ZIPdataMDB > m_zipListMDB;
	std::map <unsigned int, ZIPdataMDB >::iterator m_zipListitMDB;

	// CMAP for storing zip names
	//    key  whatever  thing stored  whatever
	CMap< CStringW, LPCWSTR, ZipInfoMDB, ZipInfoMDB& > m_ZIPDATEntryNameMDB;

	CStringW extractFilenameWOPath(CStringW filename);
	DECLARE_MESSAGE_MAP()
};

extern CDOSCenterApp theApp;
