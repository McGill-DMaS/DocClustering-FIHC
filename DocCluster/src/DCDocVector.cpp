//---------------------------------------------------------------------------
// File:
//      DCDocVector.h, DCDocVector.cpp
// History:
//      Feb. 23, 2002   Created.
//---------------------------------------------------------------------------

#include "stdafx.h"

#include <math.h>

#if !defined(DCDOCVECTOR_H)
    #include "DCDocVector.h"
#endif

//***************************************************************************
// Class: Document Vector
// Objective: A vector to represent a document.  The vector contains frequency
// of each frequent 1-itemset in F1(D).  The index number becomes the UID of
// the frequent item.  For example:
// 0   1   2   3    4
// d   e   b   c    a
// Suppose all a, b, c, d, e are frequent items.  b's UID is 2.
// These UIDs will be used to construct the frequent itemset, which is part
// of the clusters in the later stage.
//***************************************************************************

CDCDocVector::CDCDocVector()
{
    m_pOneItemsets = NULL;
}

CDCDocVector::~CDCDocVector()
{

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCDocVector::convertToIDF(const CDCKMVector* pIDF)
{
    int size = GetSize();
    if (pIDF->GetSize() != size) {
        ASSERT(FALSE);
        return FALSE;
    }
    for (int i = 0; i < size; ++i)
        (*this)[i] = ceil((*this)[i] * (*pIDF)[i]);

    return TRUE;
}

//---------------------------------------------------------------------------
// Get all the frequent items that presents in this document.
// Caller is responsible to call cleanup() for presentItems.
//---------------------------------------------------------------------------
/*
	It might be modified to contain some file operations.
*/
BOOL CDCDocVector::getPresentItems(BOOL bItemID, CDCFreqItemset& presentItems) const
{
    CDCFreqItem* pNewItem = NULL;
    int nItems = GetSize();
    for (int i = 0; i < nItems; ++i) {
        if (GetAt(i) > 0) {
            // This frequent item appears in this document
            if (bItemID)
                pNewItem = new CDCFreqItem(getFreqItemID(i));
            else
                pNewItem = new CDCFreqItem(i);


            if (!pNewItem)
                return FALSE;

            if (!presentItems.addFreqItem(pNewItem)) {
                ASSERT(FALSE);
                return FALSE;
            }
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Given a vector UID, returns the corresponding Frequent Item ID
//---------------------------------------------------------------------------
TFreqItemID CDCDocVector::getFreqItemID(int vectorID) const
{
    if (!m_pOneItemsets) {
        ASSERT(FALSE);
        return FALSE;
    }

    POSITION pos = m_pOneItemsets->FindIndex(vectorID);
    if (!pos) {
        ASSERT(FALSE);
        return -1;
    }
    return m_pOneItemsets->GetAt(pos)->getFreqItemID(0);    
}

//---------------------------------------------------------------------------
// Add each element in the given vector to this vector.
//---------------------------------------------------------------------------
BOOL CDCDocVector::addUp(const CDCDocVector* pVector)
{
    int nItems = GetSize();
    if (nItems != pVector->GetSize()) {
        if (nItems == 0 || pVector->GetSize() == 0)
            return TRUE;

        ASSERT(FALSE);
        return FALSE;
    }

    for (int i = 0; i < nItems; ++i)
        (*this)[i] += (*pVector)[i];

    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CString CDCDocVector::toString() const
{
    CString str = "[ ";
    CString tmp;
	int count = GetSize();
	for (int i = 0; i < count; i ++) {
        tmp.Format("%d ", GetAt(i));
        str += tmp;
	}
	str += "]\n";
    return str;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
UINT CDCDocVector::getSum() const
{
	int count = this->GetSize();
	UINT sum = 0;

	for (int i = 0; i < count; i ++)
	{
		sum += this->GetAt(i);
	}

	return sum;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCDocVector::deduce(const CDCDocVector *pVector)
{
    int nItems = GetSize();
    if (nItems != pVector->GetSize()) {
        ASSERT(FALSE);
        return FALSE;
    }

    for (int i = 0; i < nItems; ++i)
        (*this)[i] -= (*pVector)[i];

    return TRUE;

}

//***************************************************************************
// Class: DCDocVectors
// Objective: An array of document vectors
//***************************************************************************

CDCDocVectors::CDCDocVectors()
{

}

CDCDocVectors::~CDCDocVectors()
{
	RemoveAll();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CDCDocVectors::cleanup()
{
    int size = GetSize();
    for (int i = 0; i < size; ++i) {
        delete GetAt(i);
    }
	RemoveAll();
}
