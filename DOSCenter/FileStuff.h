#pragma once
#include "DOSCenter.h"

class CFileStuff
{
public:
	CFileStuff(void);
	~CFileStuff(void);


bool loadFile(FILE* pinputFile, CString &fileData);

int fIndex;
fpos_t m_filePos;

	CStringW m_sFilePath;
	BOOL UserBrowseOpen(CString *pPath, LPCTSTR Filter, LPCTSTR DefaultExt,bool multi=false);
	BOOL UserBrowseSave(CString *pPath, LPCTSTR Filter, LPCTSTR DefaultExt);
	BOOL UserBrowseFolder(CString &folderpath, LPCWSTR szCaption, HWND hOwner = NULL);
	int getROMDesc(FILE* pinputFile, CStringW &romDesc, CStringW keyword=GAMEID, int filePos=-1);
	LPCWSTR m_windowTitle;
	//BOOL DeleteDirectory(const TCHAR* sPath);
	BOOL DeleteDirectory(CONST CString& sFolder);
	VOID MakeWritable(CONST CString& filename);
	bool getDATHeader(FILE* pinputFile, CString &header);
	bool setTimestamp(CString filepath, unsigned int timestamp);
	BOOL setTimestampFolder(CString filepath, unsigned int timestamp);
	unsigned long long getTimestamp(CStringW filename);
protected:
	BOOL IsDots(const TCHAR* str);
};
