//---------------------------------------------------------------------------
// File:
//      DCFreqItem.h, DCFreqItem.cpp
// History:
//      Feb. 23, 2002   Created by Benjamin Fung
//---------------------------------------------------------------------------

#include "stdafx.h"

#if !defined (DCFREQITEM_H)
    #include "DCFreqItem.h"
#endif

#if !defined(DCDOCMGR_H)
    #include "DCDocMgr.h"
#endif

//***************************************************************************
// Class: CDCFreqItemset
// Objective: A frequent itemset, e.g., {100, 101}
//***************************************************************************

CDCFreqItemset::CDCFreqItemset()
{
    m_globalSupport = 0.0f;
	m_nGlobalSupport = 0;
}

CDCFreqItemset::~CDCFreqItemset()
{
    RemoveAll();
}

//---------------------------------------------------------------------------
// Delete each frequent item in this frequent itemset
//---------------------------------------------------------------------------
void CDCFreqItemset::cleanup()
{
	POSITION pos = GetHeadPosition();
	while (pos != NULL) {
		CDCFreqItem* pItem = GetNext(pos);
		ASSERT(pItem);
		if (pItem != NULL)
			delete pItem;
	}
	RemoveAll();
}

//---------------------------------------------------------------------------
// Make a copy of this itemset; caller is responsible to delete it.
//---------------------------------------------------------------------------
BOOL CDCFreqItemset::makeCopy(CDCFreqItemset& itemset)
{
    itemset.cleanup();
    CDCFreqItem* pItem = NULL;
    CDCFreqItem* pCopy = NULL;
	POSITION pos = GetHeadPosition();
	while (pos != NULL) {
		pItem = GetNext(pos);
		P_ASSERT(pItem);
        pItem->makeCopy(pCopy);
        P_ASSERT(pCopy)
        if (!itemset.addFreqItem(pCopy)) {
            ASSERT(FALSE);
            return FALSE;
        }
	}
    return TRUE;
}

//---------------------------------------------------------------------------
// Add a new frequent item into this itemset. 
// No duplicated item and the resultant list is sorted, e.g. {101, 105, 120}
//---------------------------------------------------------------------------
BOOL CDCFreqItemset::addFreqItem(CDCFreqItem* pNewItem)
{
    if (!pNewItem) {
        ASSERT(FALSE);
        return FALSE;
    }
    if (IsEmpty()) {
        return AddTail(pNewItem) != NULL;
    }

    TFreqItemID id;
    TFreqItemID newID = pNewItem->getFreqItemID();
    CDCFreqItem* pFreqItem = NULL;
    POSITION pos = GetTailPosition();
    while (pos != NULL) {
		pFreqItem = GetAt(pos);
        ASSERT(pFreqItem);

        id = pFreqItem->getFreqItemID();
        if (newID == id)
            return TRUE;    // duplicated item
        else if (newID > id) {
			InsertAfter(pos, pNewItem);
			return TRUE;
		}
		else {
			GetPrev(pos);
		}
    }
    return AddHead(pNewItem) != NULL;
}

//---------------------------------------------------------------------------
// Return the frequent item that has the given itemID
//---------------------------------------------------------------------------
CDCFreqItem* CDCFreqItemset::getFreqItem(TFreqItemID itemID) const
{
    CDCFreqItem* pFreqItem = NULL;
    POSITION pos = GetHeadPosition();
    while (pos != NULL) {
        pFreqItem = GetNext(pos);
        if (pFreqItem->getFreqItemID() == itemID)
            return pFreqItem;
    }
    return NULL;
}

//---------------------------------------------------------------------------
// Returns COMPARE_EQUAL if target itemset is same as this itemset
// Returns COMPARE_LARGER if the number of items in target itemset is larger
// then this itemset OR the itemIDs are larger than this itemset;
// Returns COMPARE_SMALLER otherwise.
//---------------------------------------------------------------------------
TCompareResult CDCFreqItemset::compareTo(const CDCFreqItemset* pTargetItemset) const
{
    ASSERT(pTargetItemset);
    int nItems = GetCount();
    int nTargetItems = pTargetItemset->GetCount();
    if (nTargetItems > nItems)
        return COMPARE_LARGER;
    else if (nTargetItems < nItems)
        return COMPARE_SMALLER;

    ASSERT(nItems == nTargetItems);

    // Compare each frequent item's ID
    TFreqItemID id, idTarget;
    CDCFreqItem* pFreqItem = NULL;
    CDCFreqItem* pFreqItemTarget = NULL;
    POSITION pos = GetHeadPosition();
    POSITION posTarget = pTargetItemset->GetHeadPosition();
    while (pos != NULL && posTarget != NULL) {
        pFreqItem = GetNext(pos);
        pFreqItemTarget = pTargetItemset->GetNext(posTarget);
        ASSERT(pFreqItem);
        ASSERT(pFreqItemTarget);

        id = pFreqItem->getFreqItemID();
        idTarget = pFreqItemTarget->getFreqItemID();
        if (idTarget > id)
            return COMPARE_LARGER;
        else if (idTarget < id)
            return COMPARE_SMALLER;
    }
    return COMPARE_EQUAL;
}

//---------------------------------------------------------------------------
// Returns TRUE if the given frequent itemset is a subset of this frequent itemset;
// FALSE otherwise.  Note: Assume both itemsets are sorted.
//---------------------------------------------------------------------------
BOOL CDCFreqItemset::containsAll(const CDCFreqItemset* pTargetItemset) const
{
    if (!pTargetItemset) {
        ASSERT(FALSE);
        return FALSE;
    }

    BOOL bTargetFound = FALSE;
    TFreqItemID idThis = -1;
    TFreqItemID idTarget = -1;   
    POSITION posThis = GetHeadPosition();
    POSITION posTarget = pTargetItemset->GetHeadPosition();
    while (posTarget != NULL) {
        // Check each frequent item ID in target list
        idTarget = pTargetItemset->GetNext(posTarget)->getFreqItemID();
        
        bTargetFound = FALSE;
        while (posThis != NULL) {
            idThis = GetNext(posThis)->getFreqItemID();
            if (idThis > idTarget)
                return FALSE;
            else if (idThis == idTarget) {
                // target found, check the next frequent item
                bTargetFound = TRUE;
                break;
            }

            // otherwise, continue;
            ASSERT(idThis < idTarget);
        }

        if (!bTargetFound)
            return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCFreqItemset::contains(const CDCFreqItem* pTargetItem) const
{
    if (!pTargetItem) {
        ASSERT(FALSE);
        return FALSE;
    }

    TFreqItemID idTarget = pTargetItem->getFreqItemID();
    POSITION pos = GetHeadPosition();
    while (pos != NULL) {
        if (GetNext(pos)->getFreqItemID() == idTarget)
            return TRUE;
    }
    return FALSE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCFreqItemset::contains(TFreqItemID itemID) const
{
    return getFreqItem(itemID) != NULL;
}

//---------------------------------------------------------------------------
// If pDocMgr, use ID to represent the items; otherwise, map the frequent
// word from DocMgr.
//---------------------------------------------------------------------------
CString CDCFreqItemset::toLabel(CDCDocMgr* pDocMgr) const
{
    CString strItemset;
	if (IsEmpty()) {
		strItemset = _T("null");
		return strItemset;
	}    

    CString tmp;
    TFreqItemID id;
	POSITION pos = GetHeadPosition();
	while (pos != NULL) {
        id = GetNext(pos)->getFreqItemID();
        if (pDocMgr) {
            if (!pDocMgr->getFreqTermFromID(id, tmp)) {
                ASSERT(FALSE);
                return _T("error");
            }
            strItemset += tmp + _T(" ");
        }
        else {		    
		    tmp.Format(_T("%d "), id);
		    strItemset += tmp;
        }
	}
    strItemset.TrimRight();
    return strItemset;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CString CDCFreqItemset::toString() const
{
	CString strItemset = _T("{ ");
    strItemset += toLabel(NULL);

    CString tmp;
    tmp.Format(_T(" } with Global Support = %f\n"), getGlobalSupport());
	strItemset += tmp;
	return strItemset;
}

//***************************************************************************
// Class: DCFreqItemsets
// Objective: A set of frequent itemsets, e.g., {{100, 101}, {100}, {106, 109}}
//***************************************************************************

CDCFreqItemsets::CDCFreqItemsets()
{
	
}


CDCFreqItemsets::~CDCFreqItemsets()
{
    RemoveAll();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CDCFreqItemsets::cleanup()
{
    CDCFreqItemset* pFreqItemset = NULL;
	POSITION pos = GetHeadPosition();
	int i = 0;
	while (pos != NULL) {
		pFreqItemset = GetNext(pos);
		ASSERT(pFreqItemset);
		if (pFreqItemset != NULL)
		{
			if (pFreqItemset->GetCount() > 0)
			{
				pFreqItemset->cleanup();
			}
			i++;
	        delete pFreqItemset;
		}
	}
//	DEBUGPrint("In CDCFreqItemsets::cleanup itemset deleted %d\n", i);
	RemoveAll();
}

//---------------------------------------------------------------------------
// Add a new frequent itemset into this list.
// TODO: Need to make sure no duplicated item??? Order should not matter.
//---------------------------------------------------------------------------
BOOL CDCFreqItemsets::addFreqItemset(CDCFreqItemset* pNewItemset)
{
    if (!pNewItemset) {
        ASSERT(FALSE);
        return FALSE;
    }

    // Need to cast to (void*); otherwise, compiler will treat it as Append
    // a list to this list.
    if (CPtrList::AddTail((void*) pNewItemset) == NULL) {
        ASSERT(FALSE);
        return FALSE;
    }

    return TRUE;
}


BOOL CDCFreqItemset::join(CDCFreqItemset *set1, CDCFreqItemset *set2)
{

	if ((!set1) || (!set2) || (set1->IsEmpty()) || (set2->IsEmpty()) || (set1->GetCount() != set2->GetCount()))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	POSITION pos1 = set1->GetHeadPosition();
	while (pos1 != NULL)
	{
		this->addFreqItem(new CDCFreqItem(set1->GetNext(pos1)->getFreqItemID()));
	}
	this->addFreqItem(new CDCFreqItem(set2->GetTail()->getFreqItemID()));

	return TRUE;
}

BOOL CDCFreqItemsets::compactItemsets()
{
	return TRUE;
}

void CDCFreqItemset::incNGlobalSupport()
{
	m_nGlobalSupport++;
}

void CDCFreqItemset::calGlobalSupport(int numAllDocs)
{
	if (numAllDocs <= 0)
	{
		ASSERT(FALSE);
	}
	m_globalSupport = (FLOAT) m_nGlobalSupport / (FLOAT) numAllDocs;
}

BOOL CDCFreqItemset::copyItemset(CDCFreqItemset *itemset)
{
	ASSERT(itemset);

	POSITION pos = itemset->GetHeadPosition();

	while (pos != NULL)
	{
		this->addFreqItem(new CDCFreqItem(itemset->GetNext(pos)->getFreqItemID()));
	}
    this->m_globalSupport = itemset->m_globalSupport;
	return TRUE;
}

BOOL CDCFreqItemset::joinable(CDCFreqItemset *set1, CDCFreqItemset *set2)
{
	if ((!set1) || (!set2) || (set1->IsEmpty()) || (set2->IsEmpty()) || (set1->GetCount() != set2->GetCount()))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	POSITION pos1 = set1->GetHeadPosition();
	POSITION pos2 = set2->GetHeadPosition();

	POSITION tail1 = set1->GetTailPosition();
	POSITION tail2 = set2->GetTailPosition();

	TFreqItemID id1, id2;

	while ((pos1 != tail1) && (pos2 != tail2))
	{
		id1 = set1->GetNext(pos1)->getFreqItemID();
		id2 = set2->GetNext(pos2)->getFreqItemID();

		if (id1 != id2)
		{
			return FALSE;
		}
	}

	return TRUE;
}
