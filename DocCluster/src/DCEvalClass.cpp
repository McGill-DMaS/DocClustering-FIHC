// DCEvalClass.cpp: implementation of the CDCEvalClass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#if !defined(DCEVALCLASS_H)
    #include "DCEvalClass.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CDCEvalClass::CDCEvalClass(LPCTSTR className, int nItems)
{
    m_nItems = nItems;
    m_className = className;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CDCEvalClass::~CDCEvalClass()
{

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CDCEvalClasses::CDCEvalClasses()
{
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CDCEvalClasses::~CDCEvalClasses()
{
    cleanup();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CDCEvalClasses::cleanup()
{
	int nDocs = GetSize();
    for (int i = 0; i < nDocs; ++i) {
		CDCEvalClass* pItem = (*this)[i];
		ASSERT(pItem);
		if (pItem != NULL)
			delete pItem;
	}
	RemoveAll();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCEvalClasses::incEvalClass(LPCTSTR targetClassName)
{
    CDCEvalClass* pItem = findEvalClass(targetClassName);
    if (!pItem) {
        pItem = new CDCEvalClass(targetClassName, 0);
        if (!pItem)
            return FALSE;

        Add(pItem);
    }

    pItem->incNItems();
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CDCEvalClass* CDCEvalClasses::findEvalClass(LPCTSTR targetClassName)
{
    CDCEvalClass* pItem = NULL;
	int nDocs = GetSize();
    for (int i = 0; i < nDocs; ++i) {
		pItem = (*this)[i];
		P_ASSERT(pItem);
        if (pItem->getClassName() == targetClassName)            
            return pItem;
	}
    return NULL;
}

//---------------------------------------------------------------------------
// Get number of documents
//---------------------------------------------------------------------------
int CDCEvalClasses::getNumDocs()
{
    int nDocs = 0;
	int nItems = GetSize();
    CDCEvalClass* pItem = NULL;
    for (int i = 0; i < nItems; ++i) {
		pItem = (*this)[i];
		ASSERT(pItem);
        nDocs += pItem->getNItems();
	}
    return nDocs;
}
