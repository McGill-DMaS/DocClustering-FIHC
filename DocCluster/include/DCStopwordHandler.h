// DCStopwordHandler.h: interface for the CDCStopwordHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCSTOPWORDHANDLER_H)
#define DCSTOPWORDHANDLER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define LETTER_NUM 26

class CDCStopwordHandler  
{

public:
	
	CDCStopwordHandler(LPCTSTR);
	virtual ~CDCStopwordHandler();
	
	int createStopwordArray();
	void rmStopwords(char *);
	CStringArray * getCleanFile();

private:
	
	CString m_stopwordFilePath;
	CStringArray m_cleanFile;
	CStringArray m_stopwordArray;
	int m_beginPosition[LETTER_NUM];
	int m_stopwordNum;
	
	void initialize();
	BOOL isValidLetter(char);
	
	int readFileStream(LPCTSTR, char *, int);
	BOOL isStopword(CString);
	int bisectSearch(CStringArray&, CString, int, int);

};

#endif // !defined(DCSTOPWORDHANDLER_H)
