// TDCSet.h : Implementation of the CTDCSet class



// CTDCSet implementation

// code generated on Monday, August 08, 2011, 2:22 PM

#include "stdafx.h"
#include "TDCSet.h"
#include "DOSCenter.h"

IMPLEMENT_DYNAMIC(CTDCSet, CRecordset)

CTDCSet::CTDCSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_83name = L"";
	m_year = L"";
	m_longfilename = L"";
	m_FTP = FALSE;	// file exists, not verified
	m_found = FALSE;	// file is now checked into the collection
	m_publisher = L"";
	m_genre = L"";
	m_tagsandflags = L"";
	m_type = L"";
	m_play = L"";
	m_setup = L"";
	m_notes = L"";
	m_English = FALSE;
	m_Spanish = FALSE;
	m_French = FALSE;
	m_Russian = FALSE;
	m_Italian = FALSE;
	m_Chinese = FALSE;
	m_German = FALSE;
	m_Polish = FALSE;
	m_Hewbrew = FALSE;
	m_Finnish = FALSE;
	m_Czech = FALSE;
	m_Norwegian = FALSE;
	m_Japanese = FALSE;
	m_Dutch = FALSE;
	m_Danish = FALSE;
	m_Korean = FALSE;
	m_Hungarian =FALSE;
	m_Ukranian = FALSE;
	m_Swedish = FALSE;
	m_Indonesian = FALSE;
	m_Slovenian = FALSE;
	m_Serbian = FALSE;
	m_Belarusian = FALSE;
	m_Arabic = FALSE;
	m_Greek = FALSE;
	m_Latin = FALSE;
	m_Turkish = FALSE;
	m_Icelandic = FALSE;
	m_Croatian = FALSE;
	m_Brazilian = FALSE;
	m_Estonian = FALSE;
	m_Persian = FALSE;
	m_mobyURL = L"";


	m_nFields = 45;	// hey dumbass, don't forget to change this if you alter any of the above!!
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CTDCSet::GetDefaultConnect(CString filename)
{
	CString mdbfile = theApp.extractFilenameWOPath(filename);
	CString path = filename;
	path.Replace(mdbfile, L"");
		
	CString meh = L"ODBC;DSN=MS Access Database;DBQ="+filename+L";DefaultDir="+path+L";DriverId=25;FIL=MS Access;MaxBufferSize=2048;PageTimeout=5;UID=admin;";
	return (meh);
}

CString CTDCSet::GetDefaultSQL()
{
	return _T("[TDC]");
}

void CTDCSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Int() are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Text(pFX, _T("[83name]"), m_83name);
	RFX_Text(pFX, _T("[year]"), m_year);
	RFX_Text(pFX, _T("[long filename]"), m_longfilename);
	RFX_Bool(pFX, _T("[FTP]"), m_FTP);
	RFX_Bool(pFX, _T("[OK]"), m_found);
	RFX_Text(pFX, _T("[publisher]"), m_publisher);
	RFX_Text(pFX, _T("[genre]"), m_genre);
	RFX_Text(pFX, _T("[tags and flags]"), m_tagsandflags);
	RFX_Text(pFX, _T("[type]"), m_type);
	RFX_Text(pFX, _T("[play]"), m_play);
	RFX_Text(pFX, _T("[setup]"), m_setup);
	RFX_Text(pFX, _T("[notes]"), m_notes);
	RFX_Bool(pFX, _T("[English]"), m_English);
	RFX_Bool(pFX, _T("[Spanish]"),m_Spanish);
	RFX_Bool(pFX, _T("[French]"),m_French);
	RFX_Bool(pFX, _T("[Russian]"),m_Russian);
	RFX_Bool(pFX, _T("[Italian]"),m_Italian);
	RFX_Bool(pFX, _T("[Chinese]"),m_Chinese);
	RFX_Bool(pFX, _T("[German]"),m_German);
	RFX_Bool(pFX, _T("[Polish]"),m_Polish);
	RFX_Bool(pFX, _T("[Hewbrew]"),m_Hewbrew);
	RFX_Bool(pFX, _T("[Finnish]"),m_Finnish);
	RFX_Bool(pFX, _T("[Czech]"),m_Czech);
	RFX_Bool(pFX, _T("[Norwegian]"),m_Norwegian);
	RFX_Bool(pFX, _T("[Japanese]"),m_Japanese);
	RFX_Bool(pFX, _T("[Dutch]"),m_Dutch);
	RFX_Bool(pFX, _T("[Danish]"),m_Danish);
	RFX_Bool(pFX, _T("[Korean]"),m_Korean);
	RFX_Bool(pFX, _T("[Hungarian]"),m_Hungarian);
	RFX_Bool(pFX, _T("[Ukranian]"),m_Ukranian);
	RFX_Bool(pFX, _T("[Swedish]"),m_Swedish);
	RFX_Bool(pFX, _T("[Indonesian]"),m_Indonesian);
	RFX_Bool(pFX, _T("[Slovenian]"),m_Slovenian);
	RFX_Bool(pFX, _T("[Serbian]"),m_Serbian);
	RFX_Bool(pFX, _T("[Belarusian]"),m_Belarusian);
	RFX_Bool(pFX, _T("[Arabic]"),m_Arabic);
	RFX_Bool(pFX, _T("[Greek]"),m_Greek);
	RFX_Bool(pFX, _T("[Latin]"),m_Latin);
	RFX_Bool(pFX, _T("[Turkish]"), m_Turkish);
	RFX_Bool(pFX, _T("[Icelandic]"), m_Icelandic);
	RFX_Bool(pFX, _T("[Croatian]"), m_Croatian);
	RFX_Bool(pFX, _T("[Brazilian]"), m_Brazilian);
	RFX_Bool(pFX, _T("[Estonian]"), m_Estonian);
	RFX_Bool(pFX, _T("[Persian]"), m_Persian);
	
	RFX_Text(pFX, _T("[mobyURL]"), m_mobyURL);


}

/////////////////////////////////////////////////////////////////////////////
// CTDCSet diagnostics

#ifdef _DEBUG
void CTDCSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CTDCSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


