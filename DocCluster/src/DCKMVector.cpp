//---------------------------------------------------------------------------
// File:
//      DCKMVector.h, DCKMVector.cpp
// History:
//      Mar. 19, 2002   Created.
//---------------------------------------------------------------------------

#include "stdafx.h"

#include <math.h>

#if !defined(DCKMVECTOR_H)
    #include "DCKMVector.h"
#endif

//***************************************************************************
// Class: Document Vector
//***************************************************************************

CDCKMVector::CDCKMVector()
{
}

CDCKMVector::~CDCKMVector()
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCKMVector::isEqual(const CDCKMVector* pKMVector)
{
    int size = GetSize();
    if (pKMVector->GetSize() != size) {
        ASSERT(FALSE);
        return FALSE;
    }

    for (int i = 0; i < size; ++i) {
        if ((*this)[i] != (*pKMVector)[i]) {
            //TRACE(_T("(%d) %f <--> %f\n"), i, (*this)[i], (*pKMVector)[i]);
            return FALSE;
        }
    }    
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCKMVector::copyFrom(const CDCKMVector* pKMVector)
{
    int size = GetSize();
    if (pKMVector->GetSize() != size) {
        ASSERT(FALSE);
        return FALSE;
    }

    // copy the centroid
    for (int i = 0; i < size; ++i) {
        (*this)[i] = (*pKMVector)[i];
    }
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCKMVector::normalize(LPCTSTR filename)
{
    FLOAT len = 0.0f;
    if (!length(this, len)) {
        ASSERT(FALSE);
        return FALSE;
    }
    if (len == 0.0f) {
        //ASSERT(FALSE);
        DEBUGPrint(_T("Warning: Document %s has vector length = 0\n"), filename);
        return TRUE;
    }

    int size = GetSize();
    for (int i = 0; i < size; ++i) {
        (*this)[i] /= len;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Add each element in the given vector to this vector.
//---------------------------------------------------------------------------
BOOL CDCKMVector::addUp(const CDCKMVector* pVector)
{
    int nItems = GetSize();
    if (nItems != pVector->GetSize()) {
        ASSERT(FALSE);
        return FALSE;
    }

    for (int i = 0; i < nItems; ++i)
        (*this)[i] += (*pVector)[i];

    return TRUE;
}

//---------------------------------------------------------------------------
// Divide by an integer
//---------------------------------------------------------------------------
BOOL CDCKMVector::divideBy(int n)
{
    if (n == 0) {
        ASSERT(FALSE);
        return FALSE;
    }

    int nItems = GetSize();
    for (int i = 0; i < nItems; ++i)
        (*this)[i] /= n;

    return TRUE;
}

//*******************
// Static Functions *
//*******************

//---------------------------------------------------------------------------
// Caller is responsible for deallocation
//---------------------------------------------------------------------------
CDCKMVector* CDCKMVector::createVector(int nDimensions)
{
    CDCKMVector* pNewKMVector = new CDCKMVector();
    pNewKMVector->SetSize(nDimensions);
    for (int d = 0; d < nDimensions; ++d) {
        pNewKMVector->SetAt(d, 0);
    }
    return pNewKMVector;
}

//---------------------------------------------------------------------------
// Length of a vector
//---------------------------------------------------------------------------
BOOL CDCKMVector::length(const CDCKMVector* pVector, FLOAT& answer)
{
    answer = 0.0f;
    P_ASSERT(pVector);

    int size = pVector->GetSize();
    for (int i = 0; i < size; ++i) {
        answer += (*pVector)[i] * (*pVector)[i];  
    }
    answer = sqrt(answer);
    return TRUE;
}

//---------------------------------------------------------------------------
// Dot product of VectorA and VectorB.
//---------------------------------------------------------------------------
BOOL CDCKMVector::dotProduct(const CDCKMVector* pVectorA, const CDCKMVector* pVectorB, FLOAT& answer)
{
    answer = 0.0f;
    P_ASSERT(pVectorA);
    P_ASSERT(pVectorB);
    
    int sizeA = pVectorA->GetSize();
    int sizeB = pVectorB->GetSize();
    if (sizeA != sizeB) {
        ASSERT(FALSE);
        return FALSE;
    }

    for (int i = 0; i < sizeA; ++i) {
        answer += (*pVectorA)[i] * (*pVectorB)[i];
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Cosine of VectorA and VectorB
//---------------------------------------------------------------------------
BOOL CDCKMVector::cosine(const CDCKMVector* pVectorA, const CDCKMVector* pVectorB, FLOAT& answer)
{
    answer = 0.0f;
    P_ASSERT(pVectorA);
    P_ASSERT(pVectorB);
    
    int sizeA = pVectorA->GetSize();
    int sizeB = pVectorB->GetSize();
    if (sizeA != sizeB) {
        ASSERT(FALSE);
        return FALSE;
    }

    // compute lengths
    FLOAT lengthA = 0.0f;
    FLOAT lengthB = 0.0f;
    if (!length(pVectorA, lengthA)) {
        ASSERT(FALSE);
        return FALSE;
    }
    if (!length(pVectorB, lengthB)) {
        ASSERT(FALSE);
        return FALSE;
    }
    if (lengthA == 0.0f || lengthB == 0.0f) {
        //ASSERT(FALSE);
        answer = 0.0f;
        return TRUE;
    }

    // dot product
    if (!dotProduct(pVectorA, pVectorB, answer)) {
        ASSERT(FALSE);
        return FALSE;
    }

    answer = answer / (lengthA * lengthB);
    return TRUE;
}

//***************************************************************************
// Class: DCKMVectors
//***************************************************************************

CDCKMVectors::CDCKMVectors()
{

}

CDCKMVectors::~CDCKMVectors()
{
	RemoveAll();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CDCKMVectors::cleanup()
{
    int size = GetSize();
    for (int i = 0; i < size; ++i) {
        delete GetAt(i);
    }
	RemoveAll();
}
