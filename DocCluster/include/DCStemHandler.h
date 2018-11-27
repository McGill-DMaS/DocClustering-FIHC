// DCStemHandler.h: interface for the CDCStemHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCSTEMHANDLER_H)
#define DCSTEMHANDLER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDCStemHandler
{
public:

	CDCStemHandler();
	virtual ~CDCStemHandler();
	void stemFile(CStringArray *);

private:
	int cons(int i);
	int m();
	int vowelinstem();
	int doublec(int j);
	int cvc(int i);
	int ends(char * s);
	void setto(char * s);
	void r(char * s);
	void step1ab();
	void step1c();
	void step2();
	void step3();
	void step4();
	void step5();
	int stem(char * p, int i, int j);
	void stemWord(CString *);	
};


#endif // !defined(DCSTEMHANDLER_H)
