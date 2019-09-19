//By: George Polouse

// Tables.h: interface for the CTables class.
//

#if !defined(AFX_TABLES_H__B429EFC4_93B3_11D2_BD3E_204C4F4F5020__INCLUDED_)
#define AFX_TABLES_H__B429EFC4_93B3_11D2_BD3E_204C4F4F5020__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTables - results from ::SQLTables()
#include "RecordsetEx.h"

class CTables : public CRecordsetEx
{
public:
	CTables(CDatabase* pDatabase);
	DECLARE_DYNAMIC(CTables)

// Field Data
	CString m_strTableQualifier;
	CString m_strTableOwner;
	CString m_strTableName;
	CString m_strTableType;
	CString m_strRemarks;

// Operations
	BOOL Open(LPCSTR pszTableQualifier = NULL,
			 LPCSTR pszTableOwner = NULL,
			 LPCSTR pszTableName = NULL,
			 LPCSTR pszColumnName = NULL,
			 UINT   nOpenType = dynaset);
// Overrides
	virtual CString GetDefaultConnect();
	virtual CString GetDefaultSQL();
	virtual void DoFieldExchange(CFieldExchange*);

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

#endif // !defined(AFX_TABLES_H__B429EFC4_93B3_11D2_BD3E_204C4F4F5020__INCLUDED_)