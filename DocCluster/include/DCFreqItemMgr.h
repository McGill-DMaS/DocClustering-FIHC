// DCFreqItemMgr.h: interface for the CDCFreqItemMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCFREQITEMMGR_H)
#define DCFREQITEMMGR_H

#if !defined (DCFREQITEM_H)
    #include "DCFreqItem.h"
#endif

#if !defined (DCDOCUMENT_H)
	#include "DCDocument.h"
#endif

#if !defined (DCDOCMGR_H)
	#include "DCDocMgr.h"
#endif

#include "FLOAT.h"

extern char runningChars[];

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDCFreqItemMgr  
{
public:
	CDCFreqItemMgr();
	virtual ~CDCFreqItemMgr();
    BOOL initialize();
    void cleanup();

// operations    
    BOOL mineGlobalFreqItemsets(CDCDocuments* pAllDocs, CDCFreqItemsets* pF1);
    void getGlobalFreqItemsets(CDCFreqItemsets*& pFreqItemsets);

// set up threshold
	void setMinGlobalSupport(FLOAT minGlobalSupport) {m_minSupport = minGlobalSupport;};
	inline FLOAT getMinGlobalSupport() const {return m_minSupport;};

protected:    
    CDCFreqItemsets m_globalFreqItemsets;       // all the global frequent itemsets
	FLOAT m_minSupport;							// the minimum support to find out frequent items
	CDCDocuments* m_pAllDocs;

private:
	CDCFreqItemsets* m_pF1;						// all the global frequent 1-itemsets
	BOOL isInKMinus(POSITION start, POSITION end, CDCFreqItemset* itemset);
	void findInterval(POSITION& start, POSITION& end, TFreqItemID id);
	void locatePosition(CDCFreqItemsets* freqKItemsets);
	BOOL joinKCandidateSets(CDCFreqItemsets* KCandidates);
	BOOL findMinGlobalSupport(CDCFreqItemsets* freqItemsets);
	BOOL pruneKCandidateSets(CDCFreqItemsets* freqKCandidates);
	CDCFreqItemsets* m_pKMinusItemsets;			// The 1-itemset as the standard vector
//	void findFreqItemsets();
	POSITION *m_indexFreqItemset;				// POSITION array for itemsets, to speed up the pruning
	int m_numF1;								// number of frequent 1-itemsets
};

#endif
