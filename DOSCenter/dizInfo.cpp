// dizInfo.cpp : implementation file
//

#include "stdafx.h"
#include "DOSCenter.h"
#include "dizInfo.h"


// CdizInfo

IMPLEMENT_DYNAMIC(CdizInfo, CWnd)
CdizInfo::CdizInfo()
{
}

CdizInfo::~CdizInfo()
{
}


BEGIN_MESSAGE_MAP(CdizInfo, CWnd)
END_MESSAGE_MAP()



// CdizInfo message handlers


bool CdizInfo::setLangTags(CStringW &dizname, CString &language, bool silent, dizInfo &m_dizInfo)
{
	// languages
	// using the language table here: http://www.lingoes.net/en/translator/langcode.htm
	int pos;
	CString tmp;
	bool found=true;
	while (found)
	{
		found=false;
		if (dizname.Find(L"(Hu)") != -1)
		{
			language +=L"Hungarian, ";
			dizname.Replace(L"(Hu)",L"");
			m_dizInfo.lang_hu =1;
			found=true;
		}

		if (dizname.Find(L"(Kr)") != -1)
		{
			language +=L"Korean, ";
			dizname.Replace(L"(Kr)",L"");
			m_dizInfo.lang_kr =1;
			found=true;
		}
	
		if (dizname.Find(L"(Id)") != -1)
		{
			language +=L"Indonesian, ";
			dizname.Replace(L"(Id)",L"");
			m_dizInfo.lang_id =1;
			found=true;
		}
		
		if (dizname.Find(L"(Il)") != -1)
		{
			language +=L"Hebrew, ";
			dizname.Replace(L"(Il)",L"");
			m_dizInfo.lang_hw =1;
			found=true;
		}

		if (dizname.Find(L"(Zh)") != -1)
		{
			language +=L"Chinese, ";
			dizname.Replace(L"(Zh)",L"");
			m_dizInfo.lang_cn = 1;
			found=true;
		}

		if (dizname.Find(L"(De)") != -1)
		{
			language +=L"German, ";
			dizname.Replace(L"(De)",L"");
			m_dizInfo.lang_de = 1;
			found=true;
		}

		if (dizname.Find(L"(Is)") != -1)
		{
			language +="Icelandic, ";
			dizname.Replace(L"(Is)",L"");
			m_dizInfo.lang_is = 1;
			found=true;
		}

		if (dizname.Find(L"(Es)") != -1)
		{
			language +=L"Spanish, ";
			dizname.Replace(L"(Es)",L"");
			m_dizInfo.lang_sp = 1;
			found=true;
		}

		if (dizname.Find(L"(It)") != -1)
		{
			language +=L"Italian, ";
			dizname.Replace(L"(It)",L"");
			m_dizInfo.lang_it = 1;
			found=true;
		}

		if (dizname.Find(L"(Fr)") != -1)
		{
			language +=L"French, ";
			dizname.Replace(L"(Fr)",L"");
			m_dizInfo.lang_fr = 1;
			found=true;
		}

		if (dizname.Find(L"(Ru)") != -1)
		{
			language +=L"Russian, ";
			dizname.Replace(L"(Ru)",L"");
			m_dizInfo.lang_ru = 1;
			found=true;
		}

		if (dizname.Find(L"(Jp)") != -1)
		{
			language +=L"Japanese, ";
			dizname.Replace(L"(Jp)",L"");
			m_dizInfo.lang_jp = 1;
			found=true;
		}

		if (dizname.Find(L"(Fi)") != -1)
		{
			language +=L"Finnish, ";
			dizname.Replace(L"(Fi)",L"");
			m_dizInfo.lang_fi = 1;
			found=true;
		}
		
		if (dizname.Find(L"(Sl)") != -1)
		{
			language +=L"Slovenian, ";
			dizname.Replace(L"(Sl)",L"");
			m_dizInfo.lang_sl = 1;
			found=true;
		}

		if (dizname.Find(L"(Sr)") != -1)
		{
			language +=L"Serbian, ";
			dizname.Replace(L"(Sr)",L"");
			m_dizInfo.lang_sr = 1;
			found=true;
		}

		if (dizname.Find(L"(Cz)") != -1)
		{
			language +=L"Czech, ";
			dizname.Replace(L"(Cz)",L"");
			m_dizInfo.lang_cz = 1;
			found=true;
		}

		if (dizname.Find(L"(Pl)") != -1)
		{
			language +=L"Polish, ";
			dizname.Replace(L"(Pl)",L"");
			m_dizInfo.lang_pl = 1;
			found=true;
		}

		if (dizname.Find(L"(No)") != -1)
		{
			language +=L"Norwegian, ";
			dizname.Replace(L"(No)",L"");
			m_dizInfo.lang_no = 1;
			found=true;
		}

		if (dizname.Find(L"(Nl)") != -1)
		{
			language +=L"Dutch, ";
			dizname.Replace(L"(Nl)",L"");
			m_dizInfo.lang_nl = 1;
			found=true;
		}

		if (dizname.Find(L"(En)") != -1)
		{
			language +=L"English, ";
			dizname.Replace(L"(En)",L"");
			m_dizInfo.lang_en = 1;
			found=true;
		}

		if (dizname.Find(L"(Da)") != -1)
		{
			language +=L"Danish, ";
			dizname.Replace(L"(Da)",L"");
			m_dizInfo.lang_dk = 1;
			found=true;
		}

		if (dizname.Find(L"(Ua)") != -1)
		{
			language +=L"Ukranian, ";
			dizname.Replace(L"(Ua)",L"");
			m_dizInfo.lang_ua = 1;
			found=true;
		}

		if (dizname.Find(L"(Pt)") != -1)
		{
			language +=L"Portuguese, ";
			dizname.Replace(L"(Pt)",L"");
			m_dizInfo.lang_ua = 1;
			found=true;
		}

		if (dizname.Find(L"(Se)") != -1)
		{
			language +=L"Swedish, ";
			dizname.Replace(L"(Se)",L"");
			m_dizInfo.lang_se = 1;
			found=true;
		}
	
		if (dizname.Find(L"(Be)") != -1)
		{
			language +=L"Belarusian, ";
			dizname.Replace(L"(Be)",L"");
			m_dizInfo.lang_be = 1;
			found=true;
		}

		if (dizname.Find(L"(Ar)") != -1)
		{
			language +=L"Arabic, ";
			dizname.Replace(L"(Ar)",L"");
			m_dizInfo.lang_ar = 1;
			found=true;
		}

		if (dizname.Find(L"(Gr)") != -1)
		{
			language +=L"Greek, ";
			dizname.Replace(L"(Gr)",L"");
			m_dizInfo.lang_gr = 1;
			found=true;
		}
		if (dizname.Find(L"(La)") != -1)
		{
			language +=L"Latin, ";
			dizname.Replace(L"(La)",L"");
			m_dizInfo.lang_la = 1;
			found=true;
		}
		if (dizname.Find(L"(Tr)") != -1)
		{
			language += L"Turkish, ";
			dizname.Replace(L"(Tr)", L"");
			m_dizInfo.lang_tr = 1;
			found = true;
		}
		if (dizname.Find(L"(Hr)") != -1)
		{
			language += L"Croatian, ";
			dizname.Replace(L"(Hr)", L"");
			m_dizInfo.lang_hr = 1;
			found = true;
		}

		if (dizname.Find(L"(Br)") != -1)
		{
			language += L"Brazilian, ";
			dizname.Replace(L"(Br)", L"");
			m_dizInfo.lang_br = 1;
			found = true;
		}

		if (dizname.Find(L"(Ee)") != -1)
		{
			language += L"Estonian, ";
			dizname.Replace(L"(Ee)", L"");
			m_dizInfo.lang_ee = 1;
			found = true;
		}

		// depricated tags
		if ( (dizname.Find(L"(Dk)") != -1) || (dizname.Find(L"(Sp)") != -1) || (dizname.Find(L"(Cn)") != -1))
		{
			AfxMessageBox(L"Depricated language detected."+dizname, MB_ICONEXCLAMATION, 0);
			return false;
		}


	} // while

	pos = dizname.Find(L"(");	// look for any uncaught languages
	if ((pos != -1) && (!silent))
	{
		if (dizname.Mid(pos+3,1) == L")")
		{
			AfxMessageBox(L"Unknown language detected."+dizname, MB_ICONEXCLAMATION, 0);
			return false;
		}
	}

	pos = dizname.Find(L"(Multi-");
	if (pos != -1)
	{
		// verify there is a number after this keyword
		int val=0;
		tmp = dizname.Mid(pos+8,1);
		swscanf_s(tmp, L"%x", &val);
		if (val >0)
		{
			language +=L"Multiple Languages supported";
			tmp = dizname.Mid(pos, 9);
			dizname.Replace(tmp, L"");
			m_dizInfo.lang_multi = 1;
		}
	}


	language.TrimRight(L", ");
	return true;
}

// input [a1] [o1] [h2] etc
// output cstring number only 
bool CdizInfo::getFlagNum(CString flag, CString &num)
{
	int val;
	flag.Replace(L"[",L"");
	flag.Replace(L"]",L"");
	num=flag.Mid(1,flag.GetLength());
	// check if we have a number
	swscanf_s(num,L"%x",&val);
	if (val != 0)
        return true;
	else
	{
		num="0";
		return false;
	}
}
// alts and hacks
// [a1], [h1], [o1] etc.
bool CdizInfo::setFlags(CString &dizname, CString &flags, bool silent, dizInfo &m_dizInfo)
{
	int pos, endpos;
	CString tmp, tmp2, num;

	pos = dizname.Find(L"[a");
	if (pos != -1)
	{
		endpos = dizname.Find(L"]",pos);
		tmp = dizname.Mid(pos, (endpos-pos)+1); 
		if ((!getFlagNum(tmp, num) && (!silent)))
				AfxMessageBox(L"Please rename "+dizname, MB_ICONEXCLAMATION, 0);
		//if ( (dizname.Mid(pos+3,1) !="]")  && (!silent) )
		//	AfxMessageBox("Please rename "+dizname, MB_ICONEXCLAMATION, 0);
		flags+=L"Alternate version "+num+L", ";
        //tmp="[a"+tmp+"]";
		dizname.Replace(tmp,L"");
		m_dizInfo.alt = 1;
	}

/*	pos = dizname.Find(L"[h");
	if (pos != -1)
	{
		tmp=dizname.Mid(pos+2,1);
		if ( (dizname.Mid(pos+3,1) !=L"]") && (!silent) )
			AfxMessageBox(L"Please rename "+dizname, MB_ICONEXCLAMATION, 0);
		flags+=L"Hacked/graffiti version "+tmp+L", ";
        tmp=L"[h"+tmp+L"]";
		dizname.Replace(tmp,L"");
		m_dizInfo.hack = 1;
	}
	*/
	pos = dizname.Find(L"[h");
	if (pos != -1)
	{
		endpos = dizname.Find(L"]", pos);
		tmp = dizname.Mid(pos, (endpos - pos) + 1);
		if ((!getFlagNum(tmp, num) && (!silent)))
			AfxMessageBox(L"Please rename " + dizname, MB_ICONEXCLAMATION, 0);
		flags += L"Hacked/graffiti version " + num + L", ";
		dizname.Replace(tmp, L"");
		m_dizInfo.hack = 1;
	}

	pos = dizname.Find(L"[o");
	if (pos != -1)
	{
		endpos = dizname.Find(L"]", pos);
		tmp = dizname.Mid(pos, (endpos - pos) + 1);
		if ((!getFlagNum(tmp, num) && (!silent)))
			AfxMessageBox(L"Please rename " + dizname, MB_ICONEXCLAMATION, 0);
		flags += L"Overdump version " + num + L", ";
		dizname.Replace(tmp, L"");
		m_dizInfo.overdump = 1;
	}

	pos = dizname.Find(L"[f");
	if (pos != -1)
	{
		endpos = dizname.Find(L"]", pos);
		tmp = dizname.Mid(pos, (endpos - pos) + 1);
		if ((!getFlagNum(tmp, num) && (!silent)))
			AfxMessageBox(L"Please rename " + dizname, MB_ICONEXCLAMATION, 0);
		flags += L"Fixed/patched version " + num + L", ";
		dizname.Replace(tmp, L"");
		m_dizInfo.fixed = 1;
	}

	pos = dizname.Find(L"[b");
	if (pos != -1)
	{
		endpos = dizname.Find(L"]", pos);
		tmp = dizname.Mid(pos, (endpos - pos) + 1);
		if ((!getFlagNum(tmp, num) && (!silent)))
			AfxMessageBox(L"Please rename " + dizname, MB_ICONEXCLAMATION, 0);
		flags += L"Bad version " + tmp + L" (possibly best dump available), ";
		dizname.Replace(tmp, L"");
		m_dizInfo.baddump = 1;
	}


	// translations
	pos = dizname.Find(L"[tr");
	if (pos != -1)
	{
		tmp2=L"";
		tmp=dizname.Mid(pos+4,2);
		if ( (dizname.Mid(pos+6,1) !=L"]")  && (!silent) )
			AfxMessageBox(L"Please rename "+dizname, MB_ICONEXCLAMATION, 0);
		if (tmp == L"En")
			tmp2=L"English";
		if (tmp == L"Es")
			tmp2=L"Spanish";
		if (tmp ==L"Ru")
			tmp2=L"Russian";
		if (tmp == L"De")
			tmp2=L"German";
		if (tmp == L"It")
			tmp2=L"Italian";
		if (tmp == L"Fr")
			tmp2=L"French";
		if (tmp == L"No")
			tmp2=L"Norwegian";
		if (tmp == L"Cz")
			tmp2=L"Czech";
		if (tmp == L"Kr")
			tmp2=L"Korean";
		if (tmp == L"Fi")
			tmp2=L"Finnish";

		if ( (tmp2.GetLength()==0)  && (!silent) )
			AfxMessageBox(L"WTF is this? "+dizname, MB_ICONEXCLAMATION, 0);

		flags+=L"Translated (hacked) to "+tmp2+L", ";
        tmp=L"[tr "+tmp+L"]";
		dizname.Replace(tmp,L"");
		m_dizInfo.translated = 1;	//fixme - add language flags here too?

	}

	flags.TrimRight(L", ");
	return true;
}


// convert filename into diz file
bool CdizInfo::zipName2Vars(CStringW filename, bool silent, dizInfo &m_dizInfo)
{
	bool status = true;
	int pos,pos2;
	unsigned int tmpNum=0;
	CString name, flags, version, year, publisher, genre, dizname, language, tmp, tmp2, notes, extension;
	dizname = filename;	// copy filename

	// clear all fields
	m_dizInfo.publisher = "";
	m_dizInfo.genre = "";
	m_dizInfo.year = "";
	m_dizInfo.type = "";
	m_dizInfo.lang_cn = FALSE;
	m_dizInfo.lang_cz =  FALSE;
	m_dizInfo.lang_de =  FALSE;
	m_dizInfo.lang_dk =  FALSE;
	m_dizInfo.lang_en =  FALSE;
	m_dizInfo.lang_fi =  FALSE;
	m_dizInfo.lang_fr =  FALSE;
	m_dizInfo.lang_hw =  FALSE;
	m_dizInfo.lang_it =  FALSE;
	m_dizInfo.lang_jp =  FALSE;
	m_dizInfo.lang_nl =  FALSE;
	m_dizInfo.lang_no =  FALSE;
	m_dizInfo.lang_pl =  FALSE;
	m_dizInfo.lang_ru =  FALSE;
	m_dizInfo.lang_sp =  FALSE;
	m_dizInfo.lang_hu =  FALSE;
	m_dizInfo.lang_kr =  FALSE;
	m_dizInfo.lang_ua =  FALSE;  
	m_dizInfo.lang_pt =  FALSE;  
	m_dizInfo.lang_se = FALSE;
	m_dizInfo.lang_id = FALSE;
	m_dizInfo.lang_sl = FALSE;
	m_dizInfo.lang_sr = FALSE;
	m_dizInfo.lang_be = FALSE;
	m_dizInfo.lang_ar = FALSE;
	m_dizInfo.lang_gr = FALSE;
	m_dizInfo.lang_la = FALSE;
	m_dizInfo.lang_tr = FALSE;

	m_dizInfo.filename = dizname;
	m_dizInfo.type = L"zip";	// default unless overridden later

	// check for compatibility - must have one and only one )(
	pos = dizname.Find(L")(");
	if (pos == -1)
	{
		status = false;
		if (!silent)
			AfxMessageBox(L"WTF is this? "+filename, MB_ICONEXCLAMATION, 0);
		//return status;
	}


	extension = extractExtension(dizname);


	// extract all known things
	if (dizname.Find(L"[!]") != -1)
	{
		flags +=L"Known good dump!, ";
		dizname.Replace(L"[!]",L"");
		dizname.TrimRight();
		m_dizInfo.kgd = 1;
	}

	if (dizname.Find(L"[SW]") != -1)
	{
		flags +=L"Shareware, ";
		dizname.Replace(L"[SW]",L"");
		m_dizInfo.sw = 1;
	}

	if (dizname.Find(L"[FW]") != -1)
	{
		flags +=L"Freeware, ";
		dizname.Replace(L"[FW]",L"");
		m_dizInfo.fw = 1;
	}

	if (dizname.Find(L"[SWR]") != -1)
	{
		flags +=L"Registered shareware, ";
		dizname.Replace(L"[SWR]",L"");
		m_dizInfo.swr = 1;
	}

	if (dizname.Find(L"[DC]") != -1)
	{
		flags +=L"DOS Conversion, ";
		dizname.Replace(L"[DC]",L"");
		m_dizInfo.dc = 1;
	}

	// update language flags
	setLangTags(dizname, language, silent, m_dizInfo);

	setFlags(dizname, flags, silent, m_dizInfo);
	flags.TrimRight(L", ");

	


	// year + publisher
	pos = dizname.Find(L")(");
	int key=pos;

	if (key == 0xffffffff)
		goto out;
	// back up 4 spaces this is also where the end of the name exists
	int nameEnd = pos-4;
	year=dizname.Mid(nameEnd,4);
	
	swscanf_s(year, L"%d", &tmpNum);
	if (tmpNum == 0)
	{
		if (!silent)
			AfxMessageBox(L"WTF is this? " + filename, MB_ICONEXCLAMATION, 0);
	}



	m_dizInfo.year = year;
	pos+=2; // skip over ')('
	pos2=dizname.Find('[',pos); // from )( find right ']' to get pub
	if (pos2 == -1)
	{
		if (!silent)
			AfxMessageBox(L"WTF is this? "+filename, MB_ICONEXCLAMATION, 0);
		// no [ so we're likely sitting on the publisher info now.
		tmp2 = dizname.Mid(pos, dizname.GetLength() - pos - 1);

	}
	else
	{
		pos2 -= 2; // back up by 2 since there was a " ["
		tmp2 = dizname.Mid(pos, pos2 - (pos));
	}
	publisher+=tmp2;
	m_dizInfo.publisher = tmp2;
	pos+=publisher.GetLength();
		//pos++;	// now we're sitting just past the ) from the publisher section
	



	// by now all tags and flags should be removed.
	
	// genre, if it exists
	// attempt to find a [ ], but only start looking after our key location, the ")("
	pos = dizname.Find(L"[", key);
	if (pos != -1)
	{
		// closing bracket?
		pos2=dizname.Find(L"]",pos);
		if (pos2 != -1)
		{
			pos++;
			genre = dizname.Mid(pos, (pos2-pos));
			// remove it from the name
			tmp =L"["+genre+L"]";
			dizname.Replace(tmp, L"");
		}

	}

	if (genre.IsEmpty())
	{
		status = false;
		if (!silent)
			AfxMessageBox(L"WTF is this? "+filename, MB_ICONEXCLAMATION, 0);
	}


	// remove year+pub
	tmp2=L"("+tmp2+L")";
	dizname.Replace(tmp2, L"");
	tmp=L"("+year+L")";
	dizname.Replace(tmp,L"");


	// fixme: this gets tripped up by games like "coolGame ]I["
/*	
	tmp=(dizname.Right(dizname.GetLength() -  dizname.ReverseFind('[')));
	if (tmp != dizname)
	{
		dizname.Replace(tmp,"");
		tmp.TrimRight("]");
		tmp.TrimLeft("[");
		dizname.Trim();
		genre+=tmp;
		m_dizInfo.genre = tmp;
	}
*/


	// anything to the right of the title's end point is notes, since we've already pulled the genre out if it exists.
	notes = dizname.Right(dizname.GetLength()-nameEnd);
	
	// remove notes
	dizname.Replace(notes,L"");
	
	notes.Trim();
	dizname.Trim();


	if (status == false)	// did not find )( keyword.  publisher and year may have a space between them or one or more of them may be missing
	{
		// can we find a 4 digit thing inside ()'s?
		pos = dizname.Find(L"(");
		if (pos == -1)
			goto out;
		if (dizname.Mid(pos+5,1) != L")")
			goto out;
		year=dizname.Mid(pos+1,4);
		m_dizInfo.year=year;
		// pull year out
		tmp2=L"("+year+L")";
		dizname.Replace(tmp2, L"");

			// look for a publisher
	//		pos = dizname.Find("(");
	//		if (pos == -1)
	//			goto out;
	}
out:
	//	dizname.Trim();

		// version or release

		pos = dizname.Find(L" v");
		if (pos != -1)
		{
			dizname+=L" ";
			tmp=dizname.Mid(pos+2, dizname.Find(L" ",pos+1) - pos-2);
            float num;
			swscanf_s(tmp, L"%f", &num);
			
			if (num > 0)
			{
				version +=tmp;
				m_dizInfo.version = tmp;
				tmp=L" v"+tmp;
				dizname.Replace(tmp,L"");
			}
			
			
		}
		

		pos = dizname.Find(L" r");
		if (pos != -1)
		{
			tmp=dizname.Right(dizname.GetLength() - (pos+2));
            float num;
			swscanf_s(tmp, L"%f", &num);
			if (num > 0)
			{
				version +=tmp;
				m_dizInfo.version = tmp;
				tmp=L" r"+tmp;
				dizname.Replace(tmp,L"");
			}
			
		}
		

	//	pos = dizname.Find("[");
	//	if ( (pos != -1)  && (!silent) )
	//	{
	//		AfxMessageBox("WTF is this? "+filename, MB_ICONEXCLAMATION, 0);
	//		status = false;
	//	}

		if (version.GetLength() ==0)
		{
			version = L"1.0";
			m_dizInfo.version = L"1.0";
		}

		if (language.GetLength() ==0)
		{
			language = L"English";
			m_dizInfo.lang_en = 1;
		}


		// restore filename to game title
		filename2gameName(dizname, false);


		//m_dizInfo.name = dizname;
		m_dizInfo.languageTxt = language;
		m_dizInfo.flagsTxt = flags;
		m_dizInfo.notes = notes;
		m_dizInfo.genre = genre;
		m_dizInfo.name = dizname; //+" ("+m_dizInfo.year+")("+m_dizInfo.publisher+") ["+m_dizInfo.genre+"]";
		m_dizInfo.type = extension;

	// was that the only )(
	tmp = dizname;
	tmp.Insert(pos+1,L" ");
	
	pos = tmp.Find(L")(");
	if (pos != -1)
	{
		status = false;
		if (!silent)
			AfxMessageBox(L"WTF is this? "+filename, MB_ICONEXCLAMATION, 0);
	}

		
		return status;
	// exit point

}
CString CdizInfo::extractExtension(CStringW &filename)
{
	CString ext = filename.Right(filename.GetLength() - filename.ReverseFind('.'));
	if ( (ext.IsEmpty()) || (ext.GetLength() > 4))
		return L"";
	filename = filename.Left(filename.GetLength() - ext.GetLength());
	if (filename == L"")
	{
		filename = ext;
		ext = L"";
	}
	ext.TrimLeft(L".");
	return ext;
}

// convert things like ,The to the beginning of the game title, add colons where necessary.  
// optionally restore :'s from -'s.  don't use this if you're making/checking a filename as : is an invalid character.
CStringW CdizInfo::filename2gameName(CStringW &filename, bool makeFilename)
{
	int pos;
	CStringW tmp;

	filename.Trim();
	pos = filename.Find(L", The");
	if (pos != -1)
	{
		filename.Replace(L", The", L"");
		filename.Insert(0, L"The ");
	}

	pos = filename.Find(L", Die");
	if (pos != -1)
	{
		filename.Replace(L", Die", L"");
		filename.Insert(0, L"Die ");
	}

	pos = filename.Find(L", Das");
	if (pos != -1)
	{
		filename.Replace(L", Das", L"");
		filename.Insert(0, L"Das ");
	}

	pos = filename.Find(L", Der");
	if (pos != -1)
	{
		filename.Replace(L", Der", L"");
		filename.Insert(0, L"Der ");
	}

	pos = filename.Find(L", De");
	if (pos != -1)
	{
		filename.Replace(L", De", L"");
		filename.Insert(0, L"De ");
	}

	pos = filename.Find(L", La");
	if (pos != -1)
	{
		filename.Replace(L", La", L"");
		filename.Insert(0, L"La ");
	}

	pos = filename.Find(L", Les");
	if (pos != -1)
	{
		filename.Replace(L", Les", L"");
		filename.Insert(0, L"Les ");
	}

	pos = filename.Find(L", Le");
	if (pos != -1)
	{
		filename.Replace(L", Le", L"");
		filename.Insert(0, L"Le ");
	}

	pos = filename.Find(L", A");
	if (pos != -1)
	{
		filename.Replace(L", A", L"");
		filename.Insert(0, L"A ");
	}

	if (!makeFilename)
	{
		pos = 0;
		// try to fix colons - do this last
		pos = filename.Find(L"- ");
		while (pos != -1)
		{
			if (pos > 2)
			{
				tmp = filename.GetAt(pos - 1);
				if (tmp != L" ")
				{
					tmp = filename.Left(pos);

					filename.Replace(tmp, L"");
					filename.TrimLeft(L"-");
					tmp.Append(L":");
					filename.Insert(0, tmp);
				}
			}
			pos = filename.Find(L"- ", pos + 1);
		}
	}
	return L"";
}

// the inverse of the above.  input of "the game" converts to "game, the"
// "the game: the subtitle" converts to "game, the- the subtitle"
CStringW CdizInfo::gameName2fileName(CStringW &filename, bool makeFilename)
{
	// todo - this hasn't actually been written yet!
	int pos;
	CStringW tmp;

	filename.Trim();
	pos = filename.Find(L", The");
	if (pos != -1)
	{
		filename.Replace(L", The", L"");
		filename.Insert(0, L"The ");
	}

	pos = filename.Find(L", Die");
	if (pos != -1)
	{
		filename.Replace(L", Die", L"");
		filename.Insert(0, L"Die ");
	}

	pos = filename.Find(L", Das");
	if (pos != -1)
	{
		filename.Replace(L", Das", L"");
		filename.Insert(0, L"Das ");
	}

	pos = filename.Find(L", Der");
	if (pos != -1)
	{
		filename.Replace(L", Der", L"");
		filename.Insert(0, L"Der ");
	}

	pos = filename.Find(L", De");
	if (pos != -1)
	{
		filename.Replace(L", De", L"");
		filename.Insert(0, L"De ");
	}

	pos = filename.Find(L", La");
	if (pos != -1)
	{
		filename.Replace(L", La", L"");
		filename.Insert(0, L"La ");
	}

	pos = filename.Find(L", Les");
	if (pos != -1)
	{
		filename.Replace(L", Les", L"");
		filename.Insert(0, L"Les ");
	}

	pos = filename.Find(L", Le");
	if (pos != -1)
	{
		filename.Replace(L", Le", L"");
		filename.Insert(0, L"Le ");
	}

	pos = filename.Find(L", A");
	if (pos != -1)
	{
		filename.Replace(L", A", L"");
		filename.Insert(0, L"A ");
	}

	if (!makeFilename)
	{
		pos = 0;
		// try to fix colons - do this last
		pos = filename.Find(L"- ");
		while (pos != -1)
		{
			if (pos > 2)
			{
				tmp = filename.GetAt(pos - 1);
				if (tmp != L" ")
				{
					tmp = filename.Left(pos);

					filename.Replace(tmp, L"");
					filename.TrimLeft(L"-");
					tmp.Append(L":");
					filename.Insert(0, tmp);
				}
			}
			pos = filename.Find(L"- ", pos + 1);
		}
	}
	return L"";
}
