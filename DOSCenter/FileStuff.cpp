#include "StdAfx.h"
#include ".\filestuff.h"
#include <locale>
//#include <codecvt>
#include <string>
#include <fstream>
#include <cstdlib>

#define bufSize 256

CFileStuff::CFileStuff(void)
{
}

CFileStuff::~CFileStuff(void)
{
}

BOOL CFileStuff::UserBrowseOpen(CString *pPath, LPCWSTR Filter, LPCWSTR DefaultExt,bool multi)
{

	OPENFILENAME	ofn;

	LPTSTR lpPathBuff = pPath->GetBuffer(MAX_PATH);

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = Filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrDefExt = DefaultExt;
	ofn.lpstrFile = lpPathBuff;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrCustomFilter = NULL;
	ofn.lpstrTitle = m_windowTitle;  // window title
	ofn.Flags = OFN_HIDEREADONLY+OFN_EXPLORER;
	if (multi)
		ofn.Flags |=OFN_ALLOWMULTISELECT;
	


	// display file open/save dialog
	BOOL result;
	result = GetOpenFileName(&ofn);

	pPath->ReleaseBuffer();

	return result;
}

BOOL CFileStuff::UserBrowseSave(CString *pPath, LPCTSTR Filter, LPCTSTR DefaultExt)
{

	OPENFILENAME	ofn;

	LPTSTR lpPathBuff = pPath->GetBuffer(MAX_PATH);

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = Filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrDefExt = DefaultExt;
	ofn.lpstrFile = lpPathBuff;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrCustomFilter = NULL;
	ofn.lpstrTitle = m_windowTitle;  // window title
	ofn.Flags = OFN_HIDEREADONLY+OFN_EXPLORER;
	
	// display file open/save dialog
	BOOL result;
	result = GetSaveFileName(&ofn);

	pPath->ReleaseBuffer();

	return result;
}

// part of user browse folder below
int CALLBACK BrowseCallbackProc(HWND hwnd,
    UINT uMsg,LPARAM lParam,LPARAM lpData)
{
	extern CString srcPathBrowse;

	// Look for BFFM_INITIALIZED so we know our dialog is available
	if(uMsg == BFFM_INITIALIZED)
    {

		SendMessage(hwnd, BFFM_SETSELECTION,
			TRUE, (LPARAM)(LPCTSTR)(srcPathBrowse)); 
    }
    return 0;
}
// this is a pass-thru between userBrowseFolder and the callback above.
CStringW srcPathBrowse = L"c:\\";	// default to something safe.


BOOL CFileStuff::UserBrowseFolder(CStringW &folderpath, LPCWSTR szCaption, HWND hOwner)
{
	BOOL retVal = false;
	
	// if we've already got a file path, lets start the browse there.
	extern CString srcPathBrowse;
	if (folderpath.GetLength() > 0)
		srcPathBrowse = folderpath;

	// The BROWSEINFO struct tells the shell 
	// how it should display the dialog.
	BROWSEINFO bi;
	memset(&bi, 0, sizeof(bi));

	bi.ulFlags   = BIF_USENEWUI | BIF_VALIDATE;
	bi.hwndOwner = hOwner;
	bi.lpszTitle = szCaption;
	
	// Set the callback function
    bi.lpfn = BrowseCallbackProc;

	// must call this if using BIF_USENEWUI
	::OleInitialize(NULL);

	// Show the dialog and get the itemIDList for the selected folder.
	LPITEMIDLIST pIDL = ::SHBrowseForFolder(&bi);

	if(pIDL != NULL)
	{
		// Create a buffer to store the path, then get the path.
		TCHAR buffer[_MAX_PATH] = {'\0'};
		// bufer = L"Test String";
		if(::SHGetPathFromIDListW(pIDL, buffer) != 0)
		{
			// Set the string value.
			folderpath = buffer;
			retVal = true;
		}		

		// free the item id list
		CoTaskMemFree(pIDL);
	}

	::OleUninitialize();

	return retVal;
}

	

// returns false when EOF reached
bool CFileStuff::loadFile(FILE* pinputFile, CString &fileData)
{
	bool eof = false;	// reached end of file?
	char buffer[200];
	bool flag=false;
	fileData = "";
	CString tmp;

	while (1)
	{
	if (fgets(buffer,sizeof(buffer),pinputFile) != NULL)
			{	
				tmp=buffer;
				fileData +=tmp;
			}
	else
		{
			eof = true;	// flag EOF
			break;  // exit while loop on read error
		}
	}
	return !eof;
}

bool CFileStuff::getDATHeader(FILE* pinputFile, CStringW &header)
{
	/*
	CStringW Line;
	bool flag=false;
	wchar_t LineOfChars[bufSize];
	header="";

	while (1)
	{
		fgetws(LineOfChars, bufSize, pinputFile);
		Line.Empty();
		Line.Append(LineOfChars);
		if (Line.IsEmpty()) // EOF reached!?
		{
			header ="";
			break;
		}

		if (Line.Find(GAMEID) == -1 )
            header+=Line;
		else
			break;
	}

	header.Trim();
	if (header.GetLength() > 0)
		return true;
		*/
	return false;
}


// returns false when EOF reached
// returns file position of the start of the rom, -1 for end of file.
// if a file position is supplied, we seek to that starting point before starting to search for our keyword
int CFileStuff::getROMDesc(FILE* pinputFile, CStringW &romDesc, CStringW keyword, int filePos)
{
	/*
	CStringW Line;
	bool flag=false;
	romDesc = "";
	wchar_t LineOfChars[bufSize];

	if (filePos != -1)
		fseek(pinputFile, filePos, SEEK_SET);

	while (1)
	{
		if (fgetws(LineOfChars, bufSize, pinputFile) == NULL)
		{
			m_filePos = -1;
			break;
		}
		Line=LineOfChars;

		if (!flag)
		{
			if (Line.Find(keyword) > -1 )
			{
				fgetpos (pinputFile,&m_filePos); // save file pos incase we need it for backing up
				m_filePos -=Line.GetLength();
				flag = true;
			}	
		}
		else
		{
			romDesc.Append(Line);  // append this line to buffer
			if (Line.Left(2) == L")\n") // found end of game descriptor? 
				break;
		}
	}		
*/
	return (int)m_filePos;
}


VOID CFileStuff::MakeWritable(CONST CString& filename)
{
  DWORD dwAttrs = ::GetFileAttributes(filename);
  if (dwAttrs==INVALID_FILE_ATTRIBUTES) return;

  if (dwAttrs & FILE_ATTRIBUTE_READONLY)
  {
    ::SetFileAttributes(filename,
    dwAttrs & (~FILE_ATTRIBUTE_READONLY));
  }
}

BOOL CFileStuff::DeleteDirectory(CONST CString& sFolder)
{
  CFileFind   ff;
  CString     sCurFile;
  BOOL bMore = ff.FindFile(sFolder + _T("\\*.*"));

  // Empty the folder, before removing it
  while (bMore)
  {
    bMore = ff.FindNextFile();
    if (ff.IsDirectory())
    {
      if (!ff.IsDots())
        DeleteDirectory(ff.GetFilePath());
    }
    else
    {
      sCurFile = ff.GetFilePath();
      MakeWritable(sCurFile);

      if (!::DeleteFile(sCurFile))
      {
        //LogLastError(); // just a placeholder - recover whichever way you want
        return FALSE;
      }
    }
  }

  // RemoveDirectory fails without this one!  CFileFind locks file system resources.
  ff.Close();

  if(! ::RemoveDirectory(sFolder))
  {
    //LogLastError();
    return FALSE;
  }
  return TRUE;
}

/*
bool CFileStuff::DeleteDirectory(LPCTSTR lpszDir, bool noRecycleBin)
{
    int len = _tcslen(lpszDir);
    TCHAR* pszFrom = new TCHAR[len+4]; //4 to handle wide char
    //_tcscpy(pszFrom, lpszDir); //todo:remove warning//;//convet wchar to char*
    wcscpy_s (pszFrom, len+2, lpszDir);
    pszFrom[len] = 0;
    pszFrom[len+1] = 0;

    SHFILEOPSTRUCT fileop;
    fileop.hwnd   = NULL;    // no status display
    fileop.wFunc  = FO_DELETE;  // delete operation
    fileop.pFrom  = pszFrom;  // source file name as double null terminated string
    fileop.pTo    = NULL;    // no destination needed
    fileop.fFlags = FOF_NOCONFIRMATION|FOF_SILENT;  // do not prompt the user

    if(!noRecycleBin)
        fileop.fFlags |= FOF_ALLOWUNDO;

    fileop.fAnyOperationsAborted = FALSE;
    fileop.lpszProgressTitle     = NULL;
    fileop.hNameMappings         = NULL;

    int ret = SHFileOperation(&fileop); //SHFileOperation returns zero if successful; otherwise nonzero 
    delete [] pszFrom;  
    return (0 == ret);
}
*/
/*	
BOOL CFileStuff::DeleteDirectory(const TCHAR* sPath)
{
	HANDLE hFind; // file handle
	WIN32_FIND_DATA FindFileData;

	TCHAR DirPath[MAX_PATH];
	TCHAR FileName[MAX_PATH];

	_tcscpy(DirPath,sPath);
	_tcscat(DirPath,_T("\\"));
	_tcscpy(FileName,sPath);
	_tcscat(FileName,_T("\\*")); // searching all files

	hFind = FindFirstFile(FileName, &FindFileData); // find the first file
	if( hFind != INVALID_HANDLE_VALUE )
	{
		do
		{
			if( IsDots(FindFileData.cFileName) )
				continue;
			_tcscpy(FileName + _tcslen(DirPath), FindFileData.cFileName);
			if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				// we have found a directory, recurse
				FindClose(hFind); // closing file handle
				if( !DeleteDirectory(FileName) )
					break; // directory couldn't be deleted
			}
			else
			{
				//if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
				//_chmod(FileName, _S_IWRITE); // change read-only file mode
				if( !DeleteFile(FileName) )
					break; // file couldn't be deleted
			}
		} while( FindNextFile(hFind,&FindFileData) );
	}
	FindClose(hFind); // closing file handle
	return RemoveDirectory(sPath); // remove the empty (maybe not) directory
}
*/
/*
BOOL CFileStuff::DeleteDirectory(const TCHAR* sPath) 
{
   HANDLE hFind;    // file handle
   WIN32_FIND_DATA FindFileData;
   CFileStatus fileStatus;

   TCHAR DirPath[MAX_PATH];
   TCHAR FileName[MAX_PATH];

   wcscpy_s(DirPath,sPath);
   wcscat_s(DirPath,L"\\*");    // searching all files
   wcscpy_s(FileName,sPath);
   wcscat_s(FileName,L"\\");

   // find the first file
   hFind = FindFirstFile(DirPath,&FindFileData);
   if(hFind == INVALID_HANDLE_VALUE) return FALSE;
   wcscpy_s(DirPath,FileName);

   bool bSearch = true;
   while(bSearch) {    // until we find an entry
      if(FindNextFile(hFind,&FindFileData)) {
         if(IsDots(FindFileData.cFileName)) continue;
         wcscat_s(FileName,FindFileData.cFileName);
         if((FindFileData.dwFileAttributes &
            FILE_ATTRIBUTE_DIRECTORY)) {

            // we have found a directory, recurse
            if(!DeleteDirectory(FileName)) {
                FindClose(hFind);
                return FALSE;    // directory couldn't be deleted
            }
            // remove the empty directory
            RemoveDirectory(FileName);
             wcscpy_s(FileName,DirPath);
         }
         else {
            if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
			{
               // change read-only file mode
             //     _chmod(FileName, _S_IWRITE);
				//read information
				CFile::GetStatus(FileName,fileStatus);
				fileStatus.m_attribute = 0;
				//save
				CFile::SetStatus(FileName,fileStatus);
			}             
				  if(!DeleteFile(FileName)) 
				  {    // delete the file
                    FindClose(hFind);
                    return FALSE;
               }
               wcscpy_s(FileName,DirPath);
         }
      }
      else {
         // no more files there
         if(GetLastError() == ERROR_NO_MORE_FILES)
         bSearch = false;
         else {
            // some error occurred; close the handle and return FALSE
               FindClose(hFind);
               return FALSE;
         }

      }

   }
   FindClose(hFind);                  // close the file handle

   return RemoveDirectory(sPath);     // remove the empty directory
}
*/
BOOL CFileStuff::IsDots(const TCHAR* str) 
{
   if(_tcscmp(str,L".") && _tcscmp(str,L"..")) return FALSE;
   return TRUE;
}

// get a file/folder timestamp.
unsigned long long CFileStuff::getTimestamp(CStringW filename)
{
	CFileStatus fileStatus;
	unsigned long long timestamp;
	SYSTEMTIME t;
	FILETIME ftCreate;

	CFile::GetStatus(filename, fileStatus);
	fileStatus.m_mtime.GetAsSystemTime(t);
	SystemTimeToFileTime(&t, &ftCreate);
	timestamp = (unsigned long long)ftCreate.dwHighDateTime << 32;
	timestamp += ftCreate.dwLowDateTime;
	return timestamp;
}

// set a given timestamp to a file
bool CFileStuff::setTimestamp(CString filepath, unsigned int timestamp)
{
//	FILETIME dateTime;
//	SYSTEMTIME sysTime;
	CFileStatus fileStatus;
	WORD dosDate;
	WORD dosTime;
	dosDate = (WORD)(timestamp >> 16);
	dosTime = (WORD)timestamp & 0xffff;	

	// does the date+time stamp need to be updated?
	//read information
	CFile::GetStatus(filepath, fileStatus);
	// convert 
	// if (datetime != tempZIPdata.ROMsMap[i].date);
	//change last access date
	CTime lastAccess = fileStatus.m_atime;
	//modify (new date/time : 10:10:10, 1989-12-12)
	//lastAccess = CTime(1989,12,12,10,10,10);
	lastAccess = CTime(dosDate, dosTime); 
	//m_mtime must always be changed
	fileStatus.m_mtime = lastAccess;
	//save
	CFile::SetStatus(filepath, fileStatus);
	return true;
}

BOOL CFileStuff::setTimestampFolder(CString filepath, unsigned int timestamp)
{
    FILETIME ft;
    SYSTEMTIME st;
    BOOL f;
	HANDLE hFile;

	WORD dosDate;
	WORD dosTime;
	dosDate = (WORD)(timestamp >> 16);
	dosTime = (WORD)timestamp & 0xffff;	

    hFile = CreateFile((LPCTSTR)filepath,      // name of the write
                       GENERIC_WRITE,          // open for writing
                       0,                      // do not share
                       NULL,                   // default security
                       OPEN_EXISTING,          // create new file only
                       FILE_FLAG_BACKUP_SEMANTICS, //FILE_ATTRIBUTE_NORMAL,  // normal file
                       NULL);                  // no attr. template

// Convert CTime to FILETIME
CTime time(dosDate, dosTime);
time.GetAsSystemTime(st);
::SystemTimeToFileTime(&st, &ft);

    f = SetFileTime(hFile,           // Sets last-write time of the file 
        (LPFILETIME) NULL,           // to the converted current system time 
        (LPFILETIME) NULL, 
        &ft);    
	CloseHandle(hFile);
    return f;
}