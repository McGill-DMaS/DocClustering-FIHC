//---------------------------------------------------------------------------
// File:
//      DCDocument.h, DCDocument.cpp
// History:
//      Feb. 23, 2002   Created by Benjamin Fung
//		Mar. 7, 2002	add construction function by Linda: 
//						CDCDocument(CDCDocVector& pNewVector, CString fName)
//---------------------------------------------------------------------------

#include "stdafx.h"

#if !defined (DCDOCUMENT_H)
    #include "DCDocument.h"
#endif

//***************************************************************************
// Class: DCDocument
// Objective: A text document
//***************************************************************************

CDCDocument::CDCDocument()
{

}

CDCDocument::CDCDocument(CDCDocVector& pNewVector)
{
	m_docVector.Copy(pNewVector);
}

CDCDocument::CDCDocument(CDCDocVector& pNewVector, CString& fName)
{
	m_docVector.Copy(pNewVector);
	m_fileName = fName;
}

CDCDocument::~CDCDocument()
{

}

//---------------------------------------------------------------------------
// Return the class name of this document
//---------------------------------------------------------------------------
CString CDCDocument::getClassName() const
{
    int dotPos = m_fileName.Find(_T("."));
    if (dotPos == -1) {
        ASSERT(FALSE);
        return _T("");
    }

    return m_fileName.Left(dotPos);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CDCDocument::setDocName(LPCTSTR className, LPCTSTR posFix)
{
    m_fileName = className;
    m_fileName += _T(".");
    m_fileName += posFix;
}

//---------------------------------------------------------------------------
// Copy the DocVector to the K-Means Vector.
// Then normalize it.
//---------------------------------------------------------------------------
BOOL CDCDocument::normalize(const CDCKMVector* pIDFVector)
{
    int nSize = m_docVector.GetSize();
    if (m_kmVector.GetSize() == 0) {
        if (!setupKMVector(nSize)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }

    if (pIDFVector) {
        if (pIDFVector->GetSize() != nSize) {
            ASSERT(FALSE);
            return FALSE;
        }
    }

    for (int i = 0; i < nSize; ++i) {
        if (pIDFVector)
            m_kmVector[i] = m_docVector[i] * (*pIDFVector)[i];
        else
            m_kmVector[i] = m_docVector[i];
    }

    if (!m_kmVector.normalize(m_fileName)) {
        ASSERT(FALSE);        
        return FALSE;
    }
    
    // reduce memory usage
    m_docVector.SetSize(0);

    /*
    for (int j = 0; j < nSize; ++j) {
        if (m_kmVector[j] != 0.0)
            TRACE("(%d) %f\n", j, m_kmVector[j]);
    }
    */
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCDocument::setupKMVector(int size)
{
    m_kmVector.SetSize(size);
    for (int i = 0; i < size; ++i)
        m_kmVector[i] = 0.0f;

    return TRUE;
}

//***************************************************************************
// Class: DCDocuments
// Objective: An array of documents
//***************************************************************************

CDCDocuments::CDCDocuments()
{

}



CDCDocuments::~CDCDocuments()
{
	RemoveAll();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CDCDocuments::cleanup()
{
    int size = GetSize();
    for (int i = 0; i < size; ++i) {
        delete GetAt(i);
    }
	RemoveAll();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int CDCDocuments::getTotalNumWords()
{
    int nWords = 0;
    CDCDocVector* pDocVector = NULL;
    int size = GetSize();
    for (int i = 0; i < size; ++i) {
        ((CDCDocument*) GetAt(i))->getDocVector(pDocVector);
        nWords += pDocVector->getSum();
    }
    return nWords;
}

