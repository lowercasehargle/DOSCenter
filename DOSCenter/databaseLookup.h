#pragma once

#include "ODBCAccess.h"


/*********************************************************************************************
	CLASS DLL EXPORT PROTOTYPES
 *********************************************************************************************/

// Return Board ID parameters
__declspec(dllexport) DWORD CALLBACK exGetDataBaseData(CString tableNum, int rowNum, CString colText, CString &returnData);


// CdatabaseLookup

class CdatabaseLookup : public CWnd
{
	DECLARE_DYNAMIC(CdatabaseLookup)

public:
	CdatabaseLookup();
	virtual ~CdatabaseLookup();

	//You only need one ODBCAccess variable to manage your data source:
	CODBCAccess m_db;

	CString m_setName;
	CString m_tableName;
	int m_colNum;	// tracker for current open column
	int m_rowNum;	// " " row
	int m_rowCount;

	CString	g_dbPath;


	// opens an access .mdb file 
	BOOL openDatabase(CString filename);
	// locate the a specific table inside a database
	// input: CString = table name, case sensitive
	BOOL locateTable(CString tableName);
	// locate column based on text name
	// input: CString column title
	// upon exit, this is now the default table all entries will be read from
	int locateColumn(CString columnName);
	// returns data from a particular column, row
	// data returned as CString since we don't know what type of data it is
	BOOL getRecord(int rowNum, CString &data, int column=-1); 
	BOOL getNextRecord(CString &data, int column=-1);
	BOOL getDataFromRow(CString &data, int column=-1);
	// returns # of entries in a particular column
	// override to use currently open column if not specified.
	void getRecordCount(int &count);	// int columnNum
	// returns the # of columns in an open table
	void getColumnCount(int &cols);	
	BOOL deleteARecord(int entryNum);
	BOOL addARecord(CString columnName, CString data);
	BOOL closeTable();
	BOOL closeDatabase();

	DWORD GetDataBaseData(CString tableNum, int rowNum, CString colText, CString &returnData);
	CString ReadFileText(CString FilePath);


protected:
	DECLARE_MESSAGE_MAP()
	
	//The CRecordsetEx pointer (to the result set):
	CRecordsetEx *m_set;

};


