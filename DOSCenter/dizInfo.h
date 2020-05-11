#pragma once

struct dizInfo
{
	CStringW filename;		// full filename
	CString name;			// just the name
	CString shortFilename;	// 8.3
	CString type;			// file extension
	CString year;			// 
	CString publisher;
	CString genre;
	CString version;
	CString languageTxt;
	CString flagsTxt;
	CString notes;
	BOOL	lang_en;
	BOOL	lang_sp;
	BOOL	lang_de;
	BOOL	lang_hw;
	BOOL	lang_cn;
	BOOL	lang_it;
	BOOL	lang_fr;
	BOOL	lang_ru;
	BOOL	lang_jp;
	BOOL	lang_fi;
	BOOL	lang_cz;
	BOOL	lang_pl;
	BOOL	lang_no;
	BOOL	lang_nl;
	BOOL	lang_dk;
	BOOL	lang_hu;
	BOOL	lang_kr;
	BOOL	lang_ua;
	BOOL	lang_pt;
	BOOL	lang_se;
	BOOL	lang_id;
	BOOL	lang_sl;
	BOOL	lang_sr;
	BOOL	lang_be;
	BOOL	lang_ar;
	BOOL	lang_gr;
	BOOL	lang_la;
	BOOL	lang_tr;
	BOOL	lang_is;
	BOOL	lang_hr;
	BOOL	lang_br;
	BOOL	lang_ee;
	BOOL	lang_fa;
	BOOL	lang_multi;
	
	BOOL	kgd;		// known good dump
	BOOL	sw;
	BOOL	swr;
	BOOL	fw;
	BOOL	hack;
	BOOL	fixed;
	BOOL	alt;
	BOOL	overdump;
	BOOL	baddump;
	BOOL	translated;
	BOOL	dc;
	

	// things that can only be determined by looking at the .dat file
	CString cmdLine;
	CString setup;	// command line to setup (if exists)
	BOOL	tzipped;
	BOOL	found;	// have/miss
	BOOL	ftp;

	CString mobyURL;

	dizInfo()
	{
	filename="";		// full filename
	name="";			// just the name
	shortFilename="";	// 8.3
	type="";			// file extension
	year="";			// 
	publisher="";
	genre="";
	version="";
	languageTxt="";
	flagsTxt="";
	notes="";

	lang_hw=FALSE;
	lang_cn=FALSE;
	lang_en=FALSE;
	lang_sp=FALSE;
	lang_de=FALSE;
	lang_ru=FALSE;
	lang_it=FALSE;
	lang_fr=FALSE;
	lang_jp=FALSE;
	lang_fi=FALSE;
	lang_cz=FALSE;
	lang_pl=FALSE;
	lang_no=FALSE;
	lang_nl=FALSE;
	lang_dk=FALSE;
	lang_hu=FALSE;
	lang_kr=FALSE;
	lang_ua=FALSE;
	lang_pt=FALSE;
	lang_se=FALSE;
	lang_id=FALSE;
	lang_sl=FALSE;
	lang_sr=FALSE;
	lang_be=FALSE;
	lang_ar=FALSE;
	lang_gr=FALSE;
	lang_la=FALSE;
	lang_is = FALSE;
	lang_hr = FALSE;
	lang_br = FALSE;
	lang_ee = FALSE;
	lang_multi=FALSE;
	
	kgd=FALSE;		// known good dump
	sw=FALSE;
	swr=FALSE;
	hack=FALSE;
	fixed=FALSE;
	alt=FALSE;
	overdump=FALSE;
	baddump=FALSE;
	translated=FALSE;
	dc=FALSE;
	

	// things that can only be determined by looking at the .dat file
	cmdLine="";
	tzipped=FALSE;
	found=FALSE;	// have/miss
	ftp=FALSE;

	mobyURL="";
	}
};

class CdizInfo : public CWnd
{
	DECLARE_DYNAMIC(CdizInfo)
public:
	// CdizInfo
	CdizInfo(void);
	~CdizInfo(void);
	bool zipName2Vars(CStringW filename, bool silent, dizInfo &m_dizInfo);
private:
	bool setLangTags(CStringW &dizname, CString &language, bool silent, dizInfo &m_dizInfo);
	bool setFlags(CStringW &dizname, CString &flags, bool silent, dizInfo &m_dizInfo);
	bool getFlagNum(CString flag, CString &num);
	CStringW filename2gameName(CStringW &filename, bool makeFilename);
	CStringW gameName2fileName(CStringW &filename, bool makeFilename);
	CString extractExtension(CStringW &filename);
protected:
	DECLARE_MESSAGE_MAP()
};
