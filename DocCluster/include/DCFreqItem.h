// DCFreqItem.h: interface for the Frequent Items relates classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCFREQITEM_H)
#define DCFREQITEM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//***************************************************************************
// A frequent item, represented by an ID, e.g., 100

typedef int TFreqItemID;

class CDCFreqItem
{
public:
    CDCFreqItem(TFreqItemID itemID) : m_freqItemID(itemID) {};
    virtual ~CDCFreqItem() {};
    virtual void makeCopy(CDCFreqItem*& pCopy) { pCopy = new CDCFreqItem(m_freqItemID); };
    inline TFreqItemID getFreqItemID() const { return m_freqItemID; };
	
protected:
    TFreqItemID m_freqItemID;
};

class CDCDocMgr;

//***************************************************************************
// Each frequent item is represented by an unsigned integer.
// So, a frequent itemset is just an array of unsigned integers. e.g., {100, 101}

typedef CTypedPtrList<CPtrList, CDCFreqItem*> CDCFreqItemPtrList;

class CDCFreqItemset : public CDCFreqItemPtrList
{
public:
	CDCFreqItemset();
	virtual ~CDCFreqItemset();
    virtual void cleanup();

// operations
    BOOL makeCopy(CDCFreqItemset& itemset);

    BOOL addFreqItem(CDCFreqItem* pNewItem);
    inline TFreqItemID getFreqItemID(int idx) const { return GetAt(FindIndex(idx))->getFreqItemID(); };
    CDCFreqItem* getFreqItem(TFreqItemID itemID) const;

    TCompareResult compareTo(const CDCFreqItemset* pTargetItemset) const;
    BOOL containsAll(const CDCFreqItemset* pTargetItemset) const;
    BOOL contains(const CDCFreqItem* pTargetItem) const;
    BOOL contains(TFreqItemID itemID) const;

    inline FLOAT getGlobalSupport() const { return m_globalSupport; };
	inline void setGlobalSupport(FLOAT support) {m_globalSupport = support; };

    // output
    CString toLabel(CDCDocMgr* pDocMgr) const;
	CString toString() const;
	
    // join function for Apriori
	BOOL join(CDCFreqItemset* set1, CDCFreqItemset* set2);

	static BOOL joinable(CDCFreqItemset *set1, CDCFreqItemset *set2);
	BOOL copyItemset(CDCFreqItemset* itemset);
	void calGlobalSupport(int numAllDocs);
	void incNGlobalSupport();

protected:
    FLOAT m_globalSupport;  // global support of this frequent itemset
                            // (for now, we only need the global support for frequent 1-itemset,
                            //  but there is no harm if you specify all global support in here.)
private:
	int m_nGlobalSupport;
};


//***************************************************************************
// CDCFreqItemsets is a list of CDCFreqItemset. e.g., {{100}, {100, 101}, {106, 109}}

typedef CTypedPtrList<CPtrList, CDCFreqItemset*> CDCFreqItemsetPtrList;

class CDCFreqItemsets : public CDCFreqItemsetPtrList
{
public:
	BOOL compactItemsets();
    CDCFreqItemsets();
	virtual ~CDCFreqItemsets();
    void cleanup();

// operations
    BOOL addFreqItemset(CDCFreqItemset* pNewItemset);

};

#endif
