// DCEvalClass.h: interface for the CDCEvalClass class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCEVALCLASS_H)
#define DCEVALCLASS_H

#if !defined(DCEVALCLASS_H)
    #include "DCEvalClass.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//***************************************************************************

class CDCEvalClass  
{
public:
	CDCEvalClass(LPCTSTR className, int nItems);
	virtual ~CDCEvalClass();

// operations
    CString getClassName() { return m_className; };
    int getNItems() { return m_nItems; };
    void incNItems() { ++m_nItems; };

protected:
// parameters
    CString m_className;
    int m_nItems;
};

//***************************************************************************

typedef CTypedPtrArray<CPtrArray, CDCEvalClass*> CDCEvalClassPtrArray;

class CDCEvalClasses : public CDCEvalClassPtrArray
{
public:
    CDCEvalClasses();
	virtual ~CDCEvalClasses();
    void cleanup();

// operations
    BOOL incEvalClass(LPCTSTR targetClassName);
    CDCEvalClass* findEvalClass(LPCTSTR targetClassName);
    int getNumDocs();
};

#endif // !defined(DCEVALCLASS_H)
