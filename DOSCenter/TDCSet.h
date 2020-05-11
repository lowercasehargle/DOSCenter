// TDCSet.h : Declaration of the CTDCSet

#pragma once
#include <afxdb.h>
// code generated on Monday, August 08, 2011, 2:22 PM

class CTDCSet : public CRecordset
{
public:
	CTDCSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CTDCSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CString	m_83name;
	CString	m_year;
	CString	m_longfilename;
	BOOL		m_FTP;
	BOOL		m_found;
	CStringW	m_publisher;
	CStringW	m_genre;
	CStringW	m_tagsandflags;
	CStringW	m_type;
	CStringW	m_play;
	CStringW    m_setup;
	CStringW	m_notes;
	BOOL	m_English;
	BOOL	m_Spanish;
	BOOL	m_French;
	BOOL	m_Russian;
	BOOL	m_Italian;
	BOOL	m_Chinese;
	BOOL	m_German;
	BOOL	m_Polish;
	BOOL	m_Hewbrew;
	BOOL	m_Finnish;
	BOOL	m_Czech;
	BOOL	m_Norwegian;
	BOOL	m_Japanese;
	BOOL	m_Dutch;
	BOOL	m_Danish;
	BOOL	m_Korean;
	BOOL	m_Hungarian;
	BOOL	m_Ukranian;
	BOOL	m_Swedish;
	BOOL	m_Indonesian;
	BOOL	m_Slovenian;
	BOOL	m_Serbian;
	BOOL	m_Belarusian;
	BOOL	m_Arabic;
	BOOL	m_Greek;
	BOOL	m_Latin;
	BOOL	m_Turkish;
	BOOL	m_Icelandic;
	BOOL	m_Croatian;
	BOOL	m_Brazilian;
	BOOL	m_Estonian;
	BOOL	m_Persian;
	CStringW	m_mobyURL;

// Overrides
	// Wizard generated virtual function overrides
	public:
	virtual CString GetDefaultConnect(CString filename);	// Default connection string

	virtual CString GetDefaultSQL(); 	// default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);	// RFX support


private:
	BOOL m_bDBOpen;
// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};


