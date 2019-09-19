// databaseLookup.cpp : implementation file  Now with actual access database lookup goodness!
// distilled from source provided http://www.codeproject.com/database/codbcaccess.asp
//
#include "stdafx.h"
#include "databaseLookup.h"
#include "errCodes.h"

// Pointer to created database object
//
CdatabaseLookup* g_pDatabaseObj = NULL;



// CdatabaseLookup

IMPLEMENT_DYNAMIC(CdatabaseLookup, CWnd)
CdatabaseLookup::CdatabaseLookup()
{
	m_rowCount = -1;
}

CdatabaseLookup::~CdatabaseLookup()
{
}


BEGIN_MESSAGE_MAP(CdatabaseLookup, CWnd)
END_MESSAGE_MAP()



// CdatabaseLookup message handlers


// open an access .mdb file 
// opens with read-only access.
// make sure you call closeDatabase() when you're finished!
//
BOOL CdatabaseLookup::openDatabase(CString filename)
{
	BOOL status;
	//Configure the datasource which we are using by calling
	//CODBAccess::ConfigDataSource:
	CFileFind fullName;
	status = fullName.FindFile(filename);
	if (status == TRUE)
	{
		fullName.FindNextFile();
		status |= m_db.ConfigDataSource(DATASOURCE_ADD, "TDC", fullName.GetFilePath(), false, "Cobra Database", false);// last true=readonly
		//Connect using selected data source:
		status |= m_db.Connect("TDC");
	}
	return status;
}


// locate the a specific table inside a database
// input: CString = table name, case sensitive
BOOL CdatabaseLookup::locateTable(CString tableName)
{
	BOOL foundTable=FALSE;

	//Get the tables from the open database:
	m_db.QueryOpenTables();

	//Get pointer to it:
	CTables* table;
	m_db.GetTables(table);

	//Loop through tables, looking for the one we want
	for(int h = 0; h < table->GetRecordCount(); h++)
	{
		//Add this one:
		if (tableName == table->m_strTableName)
		{
			foundTable = TRUE;
			m_tableName = tableName;
			break;
		}
		//Move to the next table name:
		table->MoveNext();
	}

	return (foundTable);
}


// locate column based on text name
// input: CString column title
// upon exit, this is now the default table all entries will be read from
// returns column number upon exit, -1 if not found
int CdatabaseLookup::locateColumn(CString columnName)
{
	BOOL status = FALSE;
	int cols;
	CODBCFieldInfo info;

	

	// we've got our table for this chip number.  Open it and get some data (set)
	m_setName="lalala";  // whatever.  this is our friendlyName
	m_db.OpenNewSet(m_setName,"",m_tableName);  // open the table to get the data (set)

	//The CRecordsetEx pointer (to the result set):
	CRecordsetEx *set;

	//Plug in the right set pointer:
	m_db.GetSet(m_setName, set);           //(m_ctlSets.GetItemText(sel, 0), vw.set);
	//Plug in the right set pointer:
	m_db.GetSet(m_setName, m_set);           //(m_ctlSets.GetItemText(sel, 0), vw.set);

	//Move set to beginning:
	if(!set->IsBOF())
		set->MoveFirst();

	//Some temps:
	getColumnCount(cols);	//	long cols = set->GetODBCFieldCount();
	
	//Loop through and get column titles.
	for(int h = 0; h < cols; h++)
	{
		//Add this item:
		set->GetODBCFieldInfo(h, info);
		if (info.m_strName == columnName)
		{
            status = TRUE;
			m_colNum = h;
			break;
		}
		m_colNum=-1;
	}
	
	return m_colNum;
}

// returns data from a particular column, row.  ***Row is 0-based***
// data returned as CString since we don't know what type of data it is
// if not supplied, column to read data from is the last opened column.
BOOL CdatabaseLookup::getRecord(int rowNum, CString &data, int column) 
{

	
	if (column != -1) // no column specified? use last used value
		m_colNum = column;

//	if (rowNum == 0) 
//		return FALSE;



	// requesting data outside our range?
	if (rowNum > m_rowCount)
		return FALSE;

	// are we just looking for data in the next row?  easy!
	if (m_rowNum == (rowNum-1))
		m_set->MoveNext();	// next row

	else
	{
		m_set->MoveFirst();
		for (int i=0; i<rowNum; i++)
			m_set->MoveNext();	// next row
	}
	// now that we're in place, get the data
	m_set->GetFieldValue(m_colNum, data);
	m_rowNum = rowNum;
	return TRUE;
}


// returns data from the current row.  
// if not specified, column is the last used column.
// row is incremented on exit
// data returned as CString since we don't know what type of data it is
// if not supplied, column to read data from is the last opened column.
BOOL CdatabaseLookup::getNextRecord(CString &data, int column) 
{
	//The CRecordsetEx pointer (to the result set):
	CRecordsetEx *set;
	if (column != -1) // no column specified? use last used value
		m_colNum = column;
	
	//Plug in the right set pointer:
	m_db.GetSet(m_setName, set);  
	set->GetFieldValue(m_colNum, data);
	set->MoveNext();	// next row
	return TRUE;
}

// returns a record at the current row, from the specified column
// moves no indexes
BOOL CdatabaseLookup::getDataFromRow(CString &data, int column)
{
	//The CRecordsetEx pointer (to the result set):
	CRecordsetEx *set;
	if (column == -1) // no column specified? use last used value
		column = m_colNum;
		
	//Plug in the right set pointer:
	m_db.GetSet(m_setName, set);  
	set->GetFieldValue(column, data);
	return TRUE;
}

// returns # of entries the currently open column
void CdatabaseLookup::getRecordCount(int &count)	
{
	count=0;
    //The CRecordsetEx pointer (to the result set):
	CRecordsetEx *set;
	//Plug in the right set pointer:
	m_db.GetSet(m_setName, set);  
	
	// now we have to count the number of rows in this column.  Lame!
	while (1)
	{
		if (set->IsEOF() == 0)
			count++;
		else
			break;

		set->MoveNext();
	}

	set->MoveFirst();	// return back to start of table
	m_rowCount = count;
}

// returns the # of columns in an open table
void CdatabaseLookup::getColumnCount(int &cols)	
{
	//The CRecordsetEx pointer (to the result set):
	CRecordsetEx *set;
	//Plug in the right set pointer:
	m_db.GetSet(m_setName, set);  
	cols = set->GetODBCFieldCount();
}

// delete a record
// entrynum = 0 based record#
// there is no column to use, the entire row is deleted.
BOOL CdatabaseLookup::deleteARecord(int entryNum)
{
/*	//The CRecordsetEx pointer (to the result set):
	CRecordsetEx *set;
	
	if (set->CanUpdate() == FALSE)
		return FALSE;

	//Plug in the right set pointer:
	m_db.GetSet(m_setName, set);  

	set->Move(entryNum);
	set->Delete();
*/
	return TRUE;
}

// add a record
// columnName and data
BOOL CdatabaseLookup::addARecord(CString columnName, CString data)
{
	//The CRecordsetEx pointer (to the result set):
	CRecordsetEx *set;
	CODBCFieldInfo info;
	int cols;
	BOOL status;

	//Plug in the right set pointer:
	m_db.GetSet(m_setName, set);  

	if (set->CanUpdate() == FALSE)
		return FALSE;
	if (set->CanAppend() == FALSE)
		return FALSE;

	//Some temps:
	getColumnCount(cols);	//	long cols = set->GetODBCFieldCount();
	
	//Loop through and get column titles.
	for(int h = 0; h < cols; h++)
	{
		//Add this item:
		set->GetODBCFieldInfo(h, info);
		if (info.m_strName == columnName)
		{
            status = TRUE;
			m_colNum = h;
			break;
		}
		return FALSE;
	}
	//info.


	set->AddNew();
	set->Edit();
	set->MoveLast();
	
	return TRUE;
}


// close all tables and disconnect from the database
BOOL CdatabaseLookup::closeDatabase()
{
	// close 'er out
	m_db.CloseAllSets();
	m_db.Disconnect();
	m_colNum = 0;
	return TRUE;
}

// close the currently open table
BOOL CdatabaseLookup::closeTable()
{
	// close the current table out
	m_db.CloseSet(m_setName);
	m_colNum = 0;
	return TRUE;
}

/* ==============================================================================================
 * exGetDataBaseData - Returns a string of data for the desired element from a database
 *
 * Entry:	(database name is global, at the moment only 1 database may be used)
 *          CString  - Table # (aka chip number in our case)
 *          int      - Row # to read
 *          CString  - Column header text
 *          CString* - Data to return 
 *
 * Exit:	CString data
 *			
 * ==============================================================================================
 *
 */__declspec(dllexport) DWORD CALLBACK exGetDataBaseData(CString tableNum, int rowNum, CString colText, CString &returnData)
{
	DWORD	status;

	if (g_pDatabaseObj != NULL)
		status = g_pDatabaseObj->GetDataBaseData(tableNum, rowNum, colText, returnData);
	else
		status = STATUS_FAIL;

	return status;
}




CString CdatabaseLookup::ReadFileText(CString FilePath)
{
	// reads file as text at the given path

	CString sData, sTmp;

	CStdioFile cFile;
	CFileException fileException;

	// try to open the given file
	cFile.Open(FilePath, CFile::modeRead, &fileException);
// cant throw this from the dll	{
//		// create error object
//		CErrorMessage cErr;
//		cErr.m_sMsg.Format("Error attempting to open file \"%s\", error = %u",
//							FilePath, fileException.m_cause);
//		cErr.m_MsgIcon = MB_ICONERROR;
//		throw cErr;
//	}

	// read the file data
	while( cFile.ReadString(sTmp) )
		sData += sTmp + "\n";

	cFile.Close();

	return sData;
}

DWORD CdatabaseLookup::GetDataBaseData(CString tableNum, int rowNum, CString colText, CString &returnData)
{
//	CdatabaseLookup db;
	int entries;
//	int tmpcol;  
//	BOOL found = FALSE;
//	CString txt;
//	DWORD tmp;
//	DWORD min;
	

	// path to database exist?
	if (g_dbPath.GetLength() == 0) 
	{
		// may 9th, 2008 - if the variable for the path in autoexec is missing, just quietly fail.
		//if (warnMsgs) 
		//	AfxMessageBox("Warning!  Missing path+filename for database access.\r\nCheck autoexec.py.", MB_ICONWARNING, 0);
		return STATUS_DATABASE_BAD_PATH;
	}

	// can we open the file?
	if (openDatabase(g_dbPath) == FALSE)
	{
//		if (warnMsgs) 
//			AfxMessageBox("Unable to open database!  Invalid path+filename for database access.\r\nCheck autoexec.py.", MB_ICONWARNING, 0);
		return STATUS_DATABASE_CANNOT_OPEN;
	}

	// does this table exist?
	if (locateTable(tableNum) == FALSE)
	{
		closeDatabase();
//		if (warnMsgs) 
//			AfxMessageBox("Warning!  Unable to find chip# in the database.", MB_ICONWARNING, 0);
		return STATUS_DATABASE_CANNOT_FIND_TABLE;
	}

	// we have a table.  Locate the column we want
	if (locateColumn(colText)== FALSE)
	{
		closeDatabase();
	//	txt = "Warning!  Unable to locate ";
	//	txt+= nameminFreq;
	//	txt+=" column in database.";
	//	AfxMessageBox(txt, MB_ICONWARNING, 0);
		return STATUS_DATABASE_CANNOT_FIND_COLUMN;
	}
	
	// see how many entries (rows) are in this column 
	getRecordCount(entries);		

	if (rowNum > entries)
		return STATUS_DATABASE_CANNOT_FIND_ROW;

	// yay!  we found the data.
	getRecord(rowNum, returnData);

	// close file
	closeDatabase();
	return STATUS_SUCCESS;
}
/*
//open the database
CDatabase  DB;
DB.OpenEx(DB_DNS_NAME,CDatabase::noOdbcDialog);
//tell it you want to change it
DB.BeginTrans();
//open the recordset
CDBDerivedRecordSet  RS(&DB);
//SQL_Statement is your SQL select or just the table name
RS.Open(AFX_BD_USE_DEFAULT_TYPE,SQL_Statement);
RS.AddNew();//add new record
//or
RS.Edit();
//if using edit check the return from RecordSet Update() looking for FALSE
if(!RS.Update())
{
	iError=GetLastError();
	if(!iError)
	{
		//no error, data was same as that already in database
	}
}
DB.CommitTrans();
RS.Close();
BD.Close();

//these may help in debugging
DB.CanTransact();

RS.CanUpdate();

RS.IsBOF();
*/