// DCKMVector.h: interface for the CDCKMVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCKMVECTOR_H)
#define DCKMVECTOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//***************************************************************************

class CDCKMVector : public CDCFloatArray
{
public:
	CDCKMVector();
	virtual ~CDCKMVector();

// operations
    BOOL isEqual(const CDCKMVector* pKMVector);
    BOOL copyFrom(const CDCKMVector* pKMVector);
    BOOL normalize(LPCTSTR filename);
    BOOL addUp(const CDCKMVector* pVector);
    BOOL divideBy(int n);

    // static
    static CDCKMVector* createVector(int nDimensions);
    static BOOL length(const CDCKMVector* pVector, FLOAT& answer);
    static BOOL dotProduct(const CDCKMVector* pVectorA, const CDCKMVector* pVectorB, FLOAT& answer);
    static BOOL cosine(const CDCKMVector* pVectorA, const CDCKMVector* pVectorB, FLOAT& answer);

protected:
};


//***************************************************************************

typedef CTypedPtrArray<CPtrArray, CDCKMVector*> CDCKMVectorPtrArray;

class CDCKMVectors : public CDCKMVectorPtrArray
{
public:
    CDCKMVectors();
    virtual ~CDCKMVectors();
    void cleanup();
};

#endif
