// DCDocVector.h: interface for the CDCDocVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCDOCVECTOR_H)
#define DCDOCVECTOR_H

#if !defined (DCFREQITEM_H)
    #include "DCFreqItem.h"
#endif

#if !defined(DCKMVECTOR_H)
    #include "DCKMVector.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//***************************************************************************

class CDCDocVector : public CUIntArray
{
public:
	CDCDocVector();
	virtual ~CDCDocVector();

// operations
    inline void setFreqOneItemsets(const CDCFreqItemsets* pItemsets) { m_pOneItemsets = pItemsets; };
    BOOL convertToIDF(const CDCKMVector* pIDF);
    BOOL getPresentItems(BOOL bItemID, CDCFreqItemset& presentItems) const;
    TFreqItemID getFreqItemID(int vectorID) const;
    BOOL addUp(const CDCDocVector* pVector);    
	UINT getSum() const;
    BOOL deduce(const CDCDocVector * pVector);

    // debug
    CString toString() const;

protected:
    //LONG m_offset;                          // specify the offset position of this vector in DC_VECTOR_FILENAME
    const CDCFreqItemsets* m_pOneItemsets;  // pointer to the frequent one itemset
};


//***************************************************************************

typedef CTypedPtrArray<CPtrArray, CDCDocVector*> CDCDocVectorPtrArray;

class CDCDocVectors : public CDCDocVectorPtrArray
{
public:
    CDCDocVectors();
    virtual ~CDCDocVectors();
    void cleanup();
};

#endif
