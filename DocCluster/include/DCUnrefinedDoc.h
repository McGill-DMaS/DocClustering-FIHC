// DCUnrefinedDoc.h: interface for the CDCUnrefinedDoc & CDCUnrefinedDocs class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCUNREFINEDDOC_H)
#define DCUNREFINEDDOC_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//***************************************************************************

class CDCUnrefinedDoc  
{
public:
	
	CDCUnrefinedDoc();
	CDCUnrefinedDoc(CString&, CStringArray&);
	virtual ~CDCUnrefinedDoc();

	CString * getName();           
    CStringArray * getWordArray();

private:

	CString m_fileName;       
    CStringArray m_wordArray;

};

//***************************************************************************
typedef CTypedPtrArray<CPtrArray, CDCUnrefinedDoc*> CDCUnrefinedDocPtrArray;

class CDCUnrefinedDocs : public CDCUnrefinedDocPtrArray
{
public:
	
	CDCUnrefinedDocs();
	virtual ~CDCUnrefinedDocs();
	void addUnrefinedDoc(CDCUnrefinedDoc*);
	CDCUnrefinedDoc* getDocAt(int);
	void cleanup();
};


#endif // !defined(DCUNREFINEDDOC_H)