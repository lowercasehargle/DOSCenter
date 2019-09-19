#pragma once


// CSimilar
#define MAX_LCS 256	/*Maximum size of the longest common sequence. You might wish to change it*/

class CSimilar : public CWnd
{
	DECLARE_DYNAMIC(CSimilar)

public:
	CSimilar();
	virtual ~CSimilar();

protected:
	DECLARE_MESSAGE_MAP()

public:
	float simil( char *str1, char *str2);
	char *LCS( char *str1, char *str2);
	void swap( unsigned **first, unsigned **second);

};


