// adAssassinDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "adAssassinDlg.h"
#include "afxdialogex.h"
#include "DOSCenterDlg.h"
#include "fileStuff.h"
#include "SettingsandTools.h"
#include "SpamPopupViewer.h"
#include <ziparchive.h>


// adAssassinDlg dialog

IMPLEMENT_DYNAMIC(adAssassinDlg, CDialog)

adAssassinDlg::adAssassinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(adAssassinDlg::IDD, pParent)
{

}

adAssassinDlg::~adAssassinDlg()
{
}

void adAssassinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(adAssassinDlg, CDialog)
	ON_BN_CLICKED(IDC_SPAMASS_LOADBTN, &adAssassinDlg::OnBnClickedSpamassLoadbtn)
	ON_BN_CLICKED(IDC_SPAMASS_TRAINSPAM_BTN, &adAssassinDlg::OnBnClickedSpamassTrainspamBtn)
	ON_BN_CLICKED(IDC_SPAMASS_TRAINHAM_BTN, &adAssassinDlg::OnBnClickedSpamassTrainhamBtn)
	ON_BN_CLICKED(IDC_SPAMASSBROWSEBUTTON4, &adAssassinDlg::OnBnClickedSpamassbrowsebutton4)
	ON_BN_CLICKED(IDC_SPAMASS_GO, &adAssassinDlg::OnBnClickedSpamassGo)
END_MESSAGE_MAP()


// adAssassinDlg message handlers
BOOL adAssassinDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
#if 0
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
#endif
	// TODO: Add extra initialization here
	((CButton*)GetDlgItem(IDC_SPAMASSTEXT))->SetWindowText(L"Database not loaded");
	((CButton*)GetDlgItem(IDC_SPAMASS_ZIPPATH))->SetWindowText(m_spamAssZipsPath);
	m_spamTokenPath = L"c:\\projects\\bbsassassin\\spam.tok";	// FIXME
	m_hamTokenPath = L"c:\\projects\\bbsassassin\\ham.tok";

	return TRUE;  // return TRUE  unless you set the focus to a control
}
// training needs to first load the existing database to update it.
// 
void adAssassinDlg::doSpamTraining(void)
{

	CSADirRead::SAFileVector::const_iterator m_fit; // index thingyabob into file array
	CSADirRead m_dr;
	CString filename;
	FILE* inFile;
	CString filedata;

	CString spamFilesPath;
	CFileStuff fileAccess;

	//if (fileAccess.UserBrowseFolder(spamFilesPath, L"Folder of SPAM training files...") != TRUE)
	//	spamFilesPath=L"";

	//if (spamFilesPath.Right(1) != L"\\")
	//	spamFilesPath+=L"\\";
	spamFilesPath = L"f:\\spam\\";

	//if (spamFilesPath.GetLength() > 2)
	//	((CButton*)GetDlgItem(IDC_ZIPFILE_PATH))->SetWindowText(m_collectionPath); 

	// scan and build a listing of all the files we can locate.
	// now we have the path+filename we want to work with.
	m_dr.ClearDirs();
	m_dr.ClearFiles();
	// look in the user folder, recursively
	m_dr.GetDirs(spamFilesPath, true);
	// get all files in the dirs we found above
	m_dr.GetFiles(L"*.*");
	// sort them by name, ascending
	m_dr.SortFiles(CSADirRead::eSortAlpha, false);
	CSADirRead::SAFileVector &files = m_dr.Files();
	m_fit = files.begin();

	// for each spammy file
	while ((m_fit) != files.end())
	{
		CSADirRead::SAFileVector &files = m_dr.Files();
		filename = (*m_fit).m_sName;

		// load the file in a buffer
		inFile = fopen((CT2A)filename, "rb");
		if (inFile == NULL) // any error?
			int g = 0;
		fseek(inFile, 0, SEEK_END); // seek to end of file
		int size = ftell(inFile); // get current file pointer
		fseek(inFile, 0, SEEK_SET); // seek back to beginning of file
		char* buffer = new char[size + 1];
		memset(buffer, 0, size + 1);
		fread(buffer, size, 1, inFile);
		fclose(inFile);
		addToSpamToken(buffer);
		delete[]buffer;
		m_fit++;
	}

	saveTokenfile(true);
}

void adAssassinDlg::doHamTraining(void)
{

	CSADirRead::SAFileVector::const_iterator m_fit; // index thingyabob into file array
	CSADirRead m_dr;
	CString filename;
	FILE* inFile;
	CString filedata;
	CString spamFilesPath;
	CFileStuff fileAccess;

	//if (fileAccess.UserBrowseFolder(spamFilesPath, L"Folder of HAM training files...") != TRUE)
	//	spamFilesPath=L"";

	//if (spamFilesPath.Right(1) != L"\\")
	//	spamFilesPath+=L"\\";

	spamFilesPath = L"f:\\ham\\";

	//if (spamFilesPath.GetLength() > 2)
	//	((CButton*)GetDlgItem(IDC_ZIPFILE_PATH))->SetWindowText(m_collectionPath); 

	// scan and build a listing of all the files we can locate.
	// now we have the path+filename we want to work with.
	m_dr.ClearDirs();
	m_dr.ClearFiles();
	// look in the user folder, recursively
	m_dr.GetDirs(spamFilesPath, true);
	// get all files in the dirs we found above
	m_dr.GetFiles(L"*.*");
	// sort them by name, ascending
	m_dr.SortFiles(CSADirRead::eSortAlpha, false);
	CSADirRead::SAFileVector &files = m_dr.Files();
	m_fit = files.begin();

	// for each hammy file
	while ((m_fit) != files.end())
	{
		CSADirRead::SAFileVector &files = m_dr.Files();
		filename = (*m_fit).m_sName;

		// load the file in a buffer
		inFile = fopen((CT2A)filename, "rb");
		if (inFile == NULL) // any error?
			int g = 0;
		fseek(inFile, 0, SEEK_END); // seek to end of file
		int size = ftell(inFile); // get current file pointer
		fseek(inFile, 0, SEEK_SET); // seek back to beginning of file
		char* buffer = new char[size + 1];
		memset(buffer, 0, size + 1);
		fread(buffer, size, 1, inFile);
		fclose(inFile);
		addToHamToken(buffer);
		delete[]buffer;
		m_fit++;
	}

	saveTokenfile(false);
}

void adAssassinDlg::addToHamToken(char* buffer)
{
	unsigned int hash;
	char* token;
	int totaltokens = 0;
	CSettingsandTools CnT;

	token = strtok(buffer, " \t\r\n\x1b");
	while (token != NULL)
	{
		int size = strlen(token);

		if (size > 3) // arbitrary length
		{
			bool ok = true;
			// lowercase and/or reject chars < 32
			for (int i = 0; i < size; i++)
			{
				if ((token[i] & 0x7f) < 0x1a) // allow ESC chars
				{
					ok = false;
					break;
				}
				token[i] = tolower(token[i]);
			}
			if (ok)
			{
				// hash the token so we can store it as well as count how many times it comes up.
				hash = CnT.hashAString(token, 0);
				if (m_hamhashes.count(hash) == 0)
				{
					m_hamhashes[hash].word = token;	// every time we see this token, keep track of it- this turns in the weight later on.
					m_hamhashes[hash].count = 0;
				}
				m_hamhashes[hash].count++;
			}
		}

		token = strtok(NULL, " \t\r\n\x1b"); // next token please
	} // while
}

void adAssassinDlg::addToSpamToken(char* buffer)
{
	CSettingsandTools CnT;
	unsigned int hash;
	char* token;
	int totaltokens = 0;

	token = strtok(buffer, " \t\r\n\x1b");
	while (token != NULL)
	{
		int size = strlen(token);

		if (size > 3) // arbitrary length
		{
			bool ok = true;
			// lowercase and/or reject chars < 32
			for (int i = 0; i < size; i++)
			{
				if ((token[i] & 0x7f) < 0x1a) // allow ESC chars
				{
					ok = false;
					break;
				}
				token[i] = tolower(token[i]);
			}
			if (ok)
			{
				// hash the token so we can store it as well as count how many times it comes up.
				hash = CnT.hashAString(token, 0);
				if (m_spamhashes.count(hash) == 0)
				{
					m_spamhashes[hash].word = token;	// every time we see this token, keep track of it- this turns in the weight later on.
					m_spamhashes[hash].count = 0;
				}
				m_spamhashes[hash].count++;
			}
		}

		token = strtok(NULL, " \t\r\n\x1b"); // next token please
	} // while

}

void adAssassinDlg::saveTokenfile(bool spam)
{
	if (spam)
		saveSpamTokenfile();
	else
		saveHamTokenfile();
}

// writes the current map of tokens to the ham token file
// 
void adAssassinDlg::saveHamTokenfile(void)
{
	FILE* outFile;
	outFile = fopen((CT2A)m_hamTokenPath, "wb");
	if (outFile != NULL)
	{
		ham_it = m_hamhashes.begin();
		while (ham_it != m_hamhashes.end())
		{
			string ag = m_hamhashes[ham_it->first].word;
			// save every word or only words that appear more than once?  If we don't save every word it'll likely not grow from training to training.
			if (1) //m_hamhashes[ham_it->first].count > 1)
			{
				char s_count[5] = { '0', '0', '0', '0', ' ' };
				int valu = m_hamhashes[ham_it->first].count;
				for (int i = 3; i >= 0; i--)
				{
					int digit = valu & 0xf;
					if (digit > 9)
						digit += 7;
					s_count[i] = 0x30 + digit;
					valu = (valu >> 4);
					if (valu == 0)
						break;
				}
				fwrite(s_count, sizeof(char), 5, outFile);
				fwrite(&ag[0], sizeof(char), strlen(&ag[0]), outFile);
				fwrite("\r", sizeof(char), 1, outFile);
			}
			ham_it++;
		}
	}
	fclose(outFile);
	outFile = NULL;
}


// writes the current map of tokens to the spam/ham token file
void adAssassinDlg::saveSpamTokenfile(void)
{
	FILE* outFile;
	outFile = fopen((CT2A)m_spamTokenPath, "wb");

	if (outFile != NULL)
	{
		spam_it = m_spamhashes.begin();
		while (spam_it != m_spamhashes.end())
		{
			string ag = m_spamhashes[spam_it->first].word;
			if (m_spamhashes[spam_it->first].count > 1)
			{
				char s_count[5] = { '0', '0', '0', '0', ' ' };
				int valu = m_spamhashes[spam_it->first].count;
				for (int i = 3; i >= 0; i--)
				{
					int digit = valu & 0xf;
					if (digit > 9)
						digit += 7;
					s_count[i] = 0x30 + digit;
					valu = (valu >> 4);
					if (valu == 0)
						break;
				}

				fwrite(s_count, sizeof(char), 5, outFile);
				fwrite(&ag[0], sizeof(char), strlen(&ag[0]), outFile);
				fwrite("\r", sizeof(char), 1, outFile);
			}
			spam_it++;
		}
	}

	fclose(outFile);
	outFile = NULL;
}

void adAssassinDlg::loadTokenfile(bool spam)
{
	if (spam)
		loadSpamTokenFile();
	else
		loadHamTokenFile();
}

void adAssassinDlg::loadSpamTokenFile()
{
	// load and tokenize and cmap-ize spam and ham words
	FILE* inFile;
	CFileStuff fileAccess;
	CSettingsandTools CnT;
	int curPos;
	CString filedata;
	char* token;
	unsigned int hash;

	// load the file in a buffer
	inFile = fopen((CT2A)m_spamTokenPath, "rb");

	if (inFile == NULL) // any error?
	{
		AfxMessageBox(L"You don't appear to have any SPAM token files.", MB_ICONWARNING, 0);
		return;
	}
	fseek(inFile, 0, SEEK_END); // seek to end of file
	int size = ftell(inFile); // get current file pointer
	fseek(inFile, 0, SEEK_SET); // seek back to beginning of file
	char* buffer = new char[size + 1];
	memset(buffer, 0, size + 1);
	fread(buffer, size, 1, inFile);
	fclose(inFile);

	// tok files have a format:
	// 0000 name\r
	// where 0000 is a hex number of the # of times that words appeared in the training file.  the higher the number the more spammy it is.

	token = strtok(buffer, " \r");
	while (token != NULL)
	{
		// first token is count
		int size = strlen(token);
		if (size != 4)
		{
			// somthing is not right
		}
		int count = atoi(token);
		token = strtok(NULL, "\r");

		{
			hash = CnT.hashAString(token, 0);
			// get the count here
			if (m_spamhashes.count(hash) == 0)
			{
				m_spamhashes[hash].word = token;
				m_spamhashes[hash].count = count;
			}
		}
		token = strtok(NULL, " \r");
	} // while
	delete[]buffer;

}

// load ham token database into a map.
// ok this one could be merged with spamloader
void adAssassinDlg::loadHamTokenFile()
{
	// load and tokenize and cmap-ize spam and ham words
	FILE* inFile;
	CFileStuff fileAccess;
	CString filedata;
	char* token;
	unsigned int hash;
	CSettingsandTools CnT;

	inFile = fopen((CT2A)m_hamTokenPath, "rb");

	// load the file in a buffer
	if (inFile == NULL) // any error?
	{
		AfxMessageBox(L"You don't appear to have any HAM token files.", MB_ICONWARNING, 0);
		return;
	}
	fseek(inFile, 0, SEEK_END); // seek to end of file
	int size = ftell(inFile); // get current file pointer
	fseek(inFile, 0, SEEK_SET); // seek back to beginning of file
	char* buffer = new char[size + 1];
	memset(buffer, 0, size + 1);
	fread(buffer, size, 1, inFile);
	fclose(inFile);

	// tok files have a format:
	// 0000 name\r
	// where 0000 is a hex number of the # of times that words appeared in the training file.  the higher the number the more spammy it is.

	token = strtok(buffer, " \r");
	while (token != NULL)
	{
		// first token is count
		int size = strlen(token);
		if (size != 4)
		{
			// somthing is not right
		}
		int count = atoi(token);
		token = strtok(NULL, "\r");

		{
			hash = CnT.hashAString(token, 0);
			// get the count here
			if (m_hamhashes.count(hash) == 0)
			{
				m_hamhashes[hash].word = token;
				m_hamhashes[hash].count = count;
			}
		}
		token = strtok(NULL, " \r");
	} // while
	delete[]buffer;

}


void adAssassinDlg::OnBnClickedSpamassLoadbtn()
{
	loadTokenfile(true);	// move to starup
	loadTokenfile(false);
	((CButton*)GetDlgItem(IDC_SPAMASSTEXT))->SetWindowText(L"Database loaded");
}


void adAssassinDlg::OnBnClickedSpamassTrainspamBtn()
{
	doSpamTraining();
}


void adAssassinDlg::OnBnClickedSpamassTrainhamBtn()
{
	doHamTraining();
}


void adAssassinDlg::OnBnClickedSpamassbrowsebutton4()
{
	CFileStuff fileStuff;
	// open a "browse to folder" dialog
	if (fileStuff.UserBrowseFolder(m_spamAssZipsPath, L"Folder of zip files to scan..") != TRUE)
		return;
	if (m_spamAssZipsPath.Right(1) != L"\\")
		m_spamAssZipsPath += L"\\";

	((CButton*)GetDlgItem(IDC_SPAMASS_ZIPPATH))->SetWindowText(m_spamAssZipsPath);
	CZipArchive zip;
}

// additional ideas: check if title has any clues for words.  
// eg, if the publisher is "coolco" and a token comes up as "coolco" in a readme, chances are quite good that is not spam
// 
// allow 3 letter tokens.  Too many spammy words are being ignored.  BBS, Gig, bps
//
// if a spam token (bbs) is also in a ham token list, don't just null both, but use the weight to decide who wins.
// this might help for "call my cool bbs" vs "call our support bbs"
//
// lists of numbers?  lots of false positives were simply lists of text numbers.  maybe a monitor for those, like the ascii/binary check.
//
// maybe some known not-spam file size+crc database.  epic.ans is a prime candidate.  When training/feedback add to ham option to record crcSize too.
//


// scan zips for target files (files < 50k and not .exe or .com) and see how spammy they are.
#define MAX_SPAM_FILE_SIZE (50 * 1024) // add adjuster to dialog?
void adAssassinDlg::OnBnClickedSpamassGo()
{
	CZipArchive zip;
	CSpamPopupViewer dlg;
	CDOSCenterDlg dc;
	int results;
	FILE* inFile;
	unsigned int size;
	CString fileData;


	// let's test a file
	CSADirRead::SAFileVector::const_iterator m_fit; // index thingyabob into file array
	CSADirRead m_dr;
	CStringW filename, zipfilename, targetfilename;

	// scan and build a listing of all the files we can locate.
	// now we have the path+filename we want to work with.
	m_dr.ClearDirs();
	m_dr.ClearFiles();
	// look in the user folder, recursively
	m_dr.GetDirs(m_spamAssZipsPath, true);
	// get all files in the dirs we found above
	m_dr.GetFiles(L"*.zip");
	// sort them by name, ascending
	m_dr.SortFiles(CSADirRead::eSortAlpha, false);
	CSADirRead::SAFileVector &files = m_dr.Files();
	m_fit = files.begin();

	// for each zip file
	while ((m_fit) != files.end())
	{
		CSADirRead::SAFileVector &files = m_dr.Files();
		zipfilename = (*m_fit).m_sName;
		((CButton*)GetDlgItem(IDC_ADASSNAMETXT))->SetWindowText(theApp.extractFilenameWOPath(zipfilename));
		
		// huck zip contents into a map
		dc.zip2map2(zipfilename);

		// now examine every *+^&*( file!		
		theApp.m_zipMap[0].it = theApp.m_zipMap[0].ROMmap.begin();
		while (theApp.m_zipMap[0].it != theApp.m_zipMap[0].ROMmap.end())
		{
			if (theApp.m_zipMap[0].it->second.size <= MAX_SPAM_FILE_SIZE) // I think I need a min size here too.  nothing less than 10 bytes?
			{
				filename = theApp.m_zipMap[0].it->second.name;
				targetfilename = theApp.m_tmpWorkingFolder + L"\\tmpfile.xyz";
				dc.extractFileFromZip(zipfilename, filename, targetfilename);
				results = spamOrHam(targetfilename);

				if (results ==1) // display this file in the popup viewer to confirm
				{
					//dc.hexViewAFile(targetfilename, filename);
					inFile = fopen((CT2A)targetfilename, "rb");
					fseek(inFile, 0, SEEK_END); // seek to end of file
					size = ftell(inFile); // get current file pointer
					fseek(inFile, 0, SEEK_SET); // seek back to beginning of file

					char* buffer = new char[size + 1];
					memset(buffer, 0, size + 1);


					if (inFile != NULL)
						fread(buffer, size, 1, inFile);

					// close input file
					if (inFile != NULL)
						fclose(inFile);

					fileData = buffer;

					dlg.m_viewerTxt = fileData;
					dlg.m_fileNameViewer = filename;
					dlg.DoModal();

					// process results from dialog.
					if (dlg.m_dlgResults == 1)	// yes this was spam.  
					{
						addToSpamToken(buffer);
						saveTokenfile(true);
					}
					if (dlg.m_dlgResults == 2)	// nope, not spam
					{
						addToHamToken(buffer);
						saveTokenfile(false);
					}
					delete[]buffer;

					if (dlg.m_dlgResults == 3)	// uncle!
					{
						break;
					}

				}
				
				if (results == 3)	// failure extracting the file from the zip- typically due to old compression methods.
				{
					AfxMessageBox(zipfilename+ L"\r\nUnable to open file "+filename, MB_ICONWARNING, 0);
				}


			}
			theApp.m_zipMap[0].it++;
		} // files in zip
		m_fit++;

	} // zip files

	((CButton*)GetDlgItem(IDC_ADASSNAMETXT))->SetWindowText(L"Done!!");
}


// returns 1 = spam, 0 = ham, 3 = error
//
int adAssassinDlg::spamOrHam(CString filename)
{
	FILE* inFile;
	char* token;
	unsigned int hash;
	CSettingsandTools CnT;
	int fileIsSpam = 0;
	int hamWeight, spamWeight;
	unsigned int spamHits = 0, hamHits = 0, numTokens = 0;
	bool isAscii = true;	// a flag to help weed out binary files with text in them.
//	CString txt, txt2, outMsg;
//	outMsg = L"";


	// load the file in a buffer
	inFile = fopen((CT2A)filename, "rb");
	if (inFile == NULL) // any error?
	{
		// AfxMessageBox(L"Unable to open file "+filename, MB_ICONWARNING, 0);
		return 3;
	}
	fseek(inFile, 0, SEEK_END); // seek to end of file
	int size = ftell(inFile); // get current file pointer
	fseek(inFile, 0, SEEK_SET); // seek back to beginning of file
	char* buffer = new char[size + 1];
	memset(buffer, 0, size + 1);
	fread(buffer, size, 1, inFile);
	fclose(inFile);

	// check for all ASCII characters.  popup the correct viewer depending on data.
	for (long i = 0; i<size; i++)
		if ((unsigned char)buffer[i] < 9)	// ascii
		{
			isAscii = false;
			break;
		}



	// refresh all counters
	spamHits = 0;
	hamHits = 0;
	numTokens = 0;
	spamWeight = 0;
	hamWeight = 0;

	token = strtok(buffer, " \t\r\n\x1b");
	while (token != NULL)
	{
		int size = strlen(token);
		if (size > 3) // arbitrary length
		{
			numTokens++;
			for (int i = 0; i < size; i++)
				token[i] = tolower(token[i]);
			// do I need this?
			if (1) //(isNonAscii(resToken))
			{
				// hash this token to see if it's unique.
				hash = CnT.hashAString(token, 0);

				bool isHam = (bool)(m_hamhashes.count(hash) != 0);
				bool isSpam = (bool)(m_spamhashes.count(hash) != 0);

				// if a token is in both databases, skip it as a neutral word.  otherwise increment as needed
				if (!isHam && isSpam)
				{
					spamHits++;
					spamWeight += m_spamhashes[hash].count;
				}
				if (isHam && !isSpam)
				{
					hamHits++;
					hamWeight += m_hamhashes[hash].count;
				}

			}
		}
		token = strtok(NULL, " \t\r\n\x1b");
	} // while

	// stats for this file
//	outMsg += L"\r\n" + filename;
//	txt.Format(L" :total Tokens: %d  ", numTokens);
	
	
	if (isAscii)
	{
		if (spamWeight > hamWeight) // could do some better math here!
			fileIsSpam = 1;
	}

//		outMsg += "-->SPAM\r\n";
//	if (spamWeight < hamWeight)
//		outMsg += "-->ham\r\n";
//
//	outMsg += txt;
//	txt.Format(L"spamHits %d weight: %d\r\n", spamHits, spamWeight);
//	outMsg += txt;
//	txt.Format(L"hamHits %d, weight: %d\r\n", hamHits, hamWeight);
//	outMsg += txt;


	//AfxMessageBox(outMsg, MB_ICONWARNING, 0);
	delete[]buffer;
	return fileIsSpam;
}