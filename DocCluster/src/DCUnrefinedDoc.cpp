//---------------------------------------------------------------------------
// File:
//      DCUnrefinedDoc.h, DCUnrefinedDoc.cpp
// History:
//      Mar. 7, 2002   Created by Linda
//---------------------------------------------------------------------------

#include "stdafx.h"

#if !defined(DCUNREFINEDDOC_H)
    #include "DCUnrefinedDoc.h"
#endif


//***************************************************************************
// Class: CDCUnrefinedDoc
//***************************************************************************


/* Construction & Destruction */

CDCUnrefinedDoc::CDCUnrefinedDoc()
{

}

CDCUnrefinedDoc::CDCUnrefinedDoc(CString& fName, CStringArray& fArray)
{
	m_fileName = fName;           
    m_wordArray.Copy(fArray);
}

CDCUnrefinedDoc::~CDCUnrefinedDoc()
{
	/*
	for(int i = 0; i < m_wordArray.GetSize(); i ++)
	{
		CString * a = &m_wordArray.GetAt(i);
		delete a;
	}
	*/

	m_wordArray.RemoveAll();
}

CString * CDCUnrefinedDoc::getName()
{
	return &m_fileName;
}

CStringArray * CDCUnrefinedDoc::getWordArray()
{
	return &m_wordArray;
}



//***************************************************************************
// Class: CDCUnrefinedDocs, an array of CDCUnrefinedDoc
//***************************************************************************
CDCUnrefinedDocs::CDCUnrefinedDocs()
{

}

CDCUnrefinedDocs::~CDCUnrefinedDocs()
{
	RemoveAll();
}


void CDCUnrefinedDocs::addUnrefinedDoc(CDCUnrefinedDoc* pNewDoc)
{
	if(pNewDoc != NULL)
	{
		this->Add( pNewDoc );
	}
}

CDCUnrefinedDoc* CDCUnrefinedDocs::getDocAt(int index)
{
	return (CDCUnrefinedDoc*)CPtrArray::GetAt(index);
}

void CDCUnrefinedDocs::cleanup()
{
	int size = GetSize();
    for (int i = 0; i < size; i++) {
        delete GetAt(i);
    }

	RemoveAll();
}

/*
void CDCDocuments::cleanup()
{
    int size = GetSize();
    for (int i = 0; i < size; ++i) {
        delete GetAt(i);
    }
	RemoveAll();
}

*/