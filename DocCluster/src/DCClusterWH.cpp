//---------------------------------------------------------------------------
// File:
//      DCDocClusterWH.h, DCDocClusterWH.cpp
// History:
//      Feb. 23, 2002   Created by Benjamin Fung
//---------------------------------------------------------------------------

#include "stdafx.h"

#if !defined(DCCLUSTERWH_H)
    #include "DCClusterWH.h"
#endif

#if !defined(DCDEBUG_H)
    #include "DCDebug.h"
#endif

//***************************************************************************
// Class: DCClusterWH
// Objective: The cluster warehouse.
//***************************************************************************

CDCClusterWH::CDCClusterWH()
{

}

CDCClusterWH::~CDCClusterWH()
{
	TFreqItemID id;
	CDCClusters* pClusters = NULL;
	POSITION pos = m_clustersMap.GetStartPosition();
	while (pos != NULL) {
		m_clustersMap.GetNextAssoc(pos, id, pClusters);
        pClusters->cleanup();
		delete pClusters;
	}
	m_clustersMap.RemoveAll();
    m_allClusters.RemoveAll();
}

//---------------------------------------------------------------------------
// Add a collection of clusters to the warehouse based on the given frequent itemsets.
//---------------------------------------------------------------------------
BOOL CDCClusterWH::addClusters(const CDCFreqItemsets* pFreqItemsets)
{
    // Scan through each frequent itemset
    CDCFreqItemset* pFreqItemset = NULL;
	POSITION pos = pFreqItemsets->GetHeadPosition();
	while (pos != NULL) {
        pFreqItemset = pFreqItemsets->GetNext(pos);
		ASSERT(pFreqItemset);

        if (!addCluster(pFreqItemset)) {
            ASSERT(FALSE);
            return FALSE;
        }
	}
    return TRUE;
}

//---------------------------------------------------------------------------
// Add a cluster to the warehouse.
//---------------------------------------------------------------------------
BOOL CDCClusterWH::addCluster(const CDCFreqItemset* pFreqItemset)
{
    // Make a new cluster
    ASSERT(pFreqItemset);
    CDCCluster* pNewCluster = new CDCCluster(pFreqItemset);
    if (!pNewCluster)
        return FALSE;

    // Find the appropriate place for this new cluster in the map
    TFreqItemID firstCoreID = pNewCluster->getFirstCoreItemID();

    CDCClusters* pDestClusters = NULL;
    if (m_clustersMap.Lookup(firstCoreID, pDestClusters) == 0) {
        // No such group of cluster exists, so create one.
        pDestClusters = new CDCClusters();
        if (!pDestClusters)
            return FALSE;

        m_clustersMap.SetAt(firstCoreID, pDestClusters);
    }

    // Add the new cluster to this collection
    return pDestClusters->addCluster(pNewCluster);
}

//---------------------------------------------------------------------------
// Get all clusters in this warehouse; in asecending order
// IMPORTANT: Do not modify the returned clusters; other parts of the program
// rely on this list of clusters.
//---------------------------------------------------------------------------
BOOL CDCClusterWH::getAllClusters(CDCClusters*& pAllClusters)
{
    ASSERT(!pAllClusters);
    if (m_allClusters.IsEmpty()) {   
	    TFreqItemID id;
	    CDCClusters* pClusters = NULL;
	    POSITION pos = m_clustersMap.GetStartPosition();
	    while (pos != NULL) {
		    m_clustersMap.GetNextAssoc(pos, id, pClusters);
            if (!pClusters) {
                ASSERT(FALSE);
                return FALSE;
            }
            if (!m_allClusters.addClusters(pClusters)) {
                ASSERT(FALSE);
                return FALSE;
            }
	    }
        //CDCDebug::printClusters(&m_allClusters);
    }
    pAllClusters = &m_allClusters;    
    return TRUE;
}

//---------------------------------------------------------------------------
// Get all clusters in reverse order.
//---------------------------------------------------------------------------
BOOL CDCClusterWH::getAllClustersReverseOrder(CDCClusters& allClusters)
{
    CDCClusters* pAllClusters = NULL;
    if (!getAllClusters(pAllClusters)) {
        ASSERT(FALSE);
        return FALSE;
    }

    CDCCluster* pCluster = NULL;
    POSITION pos = pAllClusters->GetTailPosition();
    while (pos != NULL) {
        pCluster = pAllClusters->GetPrev(pos);
        ASSERT(pCluster);

        if (allClusters.AddTail(pCluster) == NULL) {
            ASSERT(FALSE);
            return FALSE;
        }
    }    
    //CDCDebug::printClusters(&allClusters);
    return TRUE;
}

//---------------------------------------------------------------------------
// Get all the clusters with one core item
//---------------------------------------------------------------------------
BOOL CDCClusterWH::getOneItemClusters(CDCClusters& oneItemClusters)
{
    CDCClusters* pAllClusters = NULL;
    if (!getAllClusters(pAllClusters)) {
        ASSERT(FALSE);
        return FALSE;
    }

    CDCClusterFreqItemset* pCoreItems = NULL;
    CDCCluster* pCluster = NULL;
    POSITION pos = pAllClusters->GetHeadPosition();
    while (pos != NULL) {
        pCluster = pAllClusters->GetNext(pos);
        ASSERT(pCluster);

        pCluster->getClusterCoreItems(pCoreItems);
        ASSERT(pCoreItems);

        if (pCoreItems->GetCount() == 1) {
            if (oneItemClusters.AddTail(pCluster) == NULL) {
                ASSERT(FALSE);
                return FALSE;
            }
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Get the global support of the given frequent item
//---------------------------------------------------------------------------
FLOAT CDCClusterWH::getFrequentItemGlobalSupport(int itemID) const
{
    CDCClusters* pClusters = NULL;
    if (m_clustersMap.Lookup(itemID, pClusters) == 0) {
        ASSERT(FALSE);
        return -1.0f;
    }

    if (pClusters->IsEmpty()) {
        ASSERT(FALSE);
        return -1.0f;
    }

    CDCCluster* pCluster = pClusters->GetHead();
    ASSERT(pCluster);

    if (pCluster->getFirstCoreItemID() != itemID) {
        // error in warehouse construction phase.
        ASSERT(FALSE);
        return -1.0f;
    }

    return pCluster->getCoreItemsGlobalSupport();
}

//---------------------------------------------------------------------------
// Get the clusters that can cover the given frequent itemset.
// The resultant pClusters have no specific order.
// Note: mainly used for document assginment stage.
//---------------------------------------------------------------------------
BOOL CDCClusterWH::findCoveredClusters(CDCFreqItemset* pFreqItemset, CDCClusters& clusters) const
{
    if (!pFreqItemset) {
        ASSERT(FALSE);
        return FALSE;
    }
    clusters.RemoveAll();

    // make a shallow copy
    CDCFreqItemset itemsetCopy;
    itemsetCopy.AddTail(pFreqItemset);

    // Remove the first frequent item one by one
    TFreqItemID firstItemID = -1;
    CDCClusters* pTargetClusters = NULL;    
    while (!itemsetCopy.IsEmpty()) {
        // Find the corresponding group of clusters based on the first itemID        
        firstItemID = itemsetCopy.getFreqItemID(0);
        if (m_clustersMap.Lookup(firstItemID, pTargetClusters) == 0) {
            // Must be some error in addCluster phase
            ASSERT(FALSE);
            itemsetCopy.RemoveAll();
            clusters.RemoveAll();
            return FALSE;
        }

        // Find the clusters that can cover the current frequent itemset in pTargetClusters
        if (!pTargetClusters->findSubSetClusters(&itemsetCopy, clusters)) {
            ASSERT(FALSE);
            itemsetCopy.RemoveAll();
            clusters.RemoveAll();
            return FALSE;
        }
        
        itemsetCopy.RemoveHead();
    }

    itemsetCopy.RemoveAll();
    return TRUE;
}

//---------------------------------------------------------------------------
// Get the potential parent cluster of the given frequent itemset.
// The resultant pClusters have no specific order.
// Note: mainly used for the tree construction stage.
//---------------------------------------------------------------------------
BOOL CDCClusterWH::findPotentialParents(BOOL bIncludeGreatGrandParents, CDCFreqItemset* pFreqItemset, CDCClusters& clusters) const
{
    // find all ancestors
    if (!findCoveredClusters(pFreqItemset, clusters)) {
        ASSERT(FALSE);
        return FALSE;
    }
    //CDCDebug::printClusters(&clusters);
    
    if (bIncludeGreatGrandParents)
        return clusters.removeLargerThanKItemsets(pFreqItemset->GetCount() - 1);
    else {
        // filter out the grandparents or great... grandparents, keep the parents
        return clusters.retainKItemsets(pFreqItemset->GetCount() - 1);
    }
}

//---------------------------------------------------------------------------
// Get the potential children cluster of the given frquent itemset.
// The resultant pClusters have no specific order.
//---------------------------------------------------------------------------
BOOL CDCClusterWH::findPotentialChildren(const CDCFreqItemset* pFreqItemset, CDCClusters& clusters) const
{
    if (!pFreqItemset) {
        ASSERT(FALSE);
        return FALSE;
    }
    ASSERT(!pFreqItemset->IsEmpty());
    clusters.RemoveAll();

    // Find the corresponding group of clusters based on the first itemID        
    CDCClusters* pTargetClusters = NULL;
    TFreqItemID firstItemID = pFreqItemset->getFreqItemID(0);
    if (m_clustersMap.Lookup(firstItemID, pTargetClusters) == 0) {
        // Must be some error in addCluster phase
        ASSERT(FALSE);
        return FALSE;
    }
#pragma message("Ben: major bug -- the result is incomplete")
    return pTargetClusters->findSuperSetClusters(pFreqItemset, clusters);
}