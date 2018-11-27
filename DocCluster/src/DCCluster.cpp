//---------------------------------------------------------------------------
// File:
//      DCDocCluster.h, DCDocCluster.cpp
// History:
//      Feb. 23, 2002   Created by Benjamin Fung
//---------------------------------------------------------------------------

#include "stdafx.h"

#include <math.h>

#if !defined(DCCLUSTER_H)
    #include "DCCluster.h"
#endif

#if !defined(DCDEBUG_H)
    #include "DCDebug.h"
#endif

#if !defined(DCEVALCLASS_H)
    #include "DCEvalClass.h"
#endif

//***************************************************************************
// Class: DCClusterFreqItem
// Objective: A frequent itemset that is specialized for cluster usage
//***************************************************************************

CDCClusterFreqItem::CDCClusterFreqItem(TFreqItemID itemID, FLOAT clusterSupport) 
    : CDCFreqItem(itemID), m_clusterSupport(clusterSupport)
{
}

CDCClusterFreqItem::~CDCClusterFreqItem()
{
}

//---------------------------------------------------------------------------
// Make a copy this item.
//---------------------------------------------------------------------------
void CDCClusterFreqItem::makeCopy(CDCClusterFreqItem*& pCopy) 
{ 
    pCopy = new CDCClusterFreqItem(m_freqItemID, m_clusterSupport); 
};

//***************************************************************************
// Class: DCClusterFreqItemset
//***************************************************************************

//---------------------------------------------------------------------------
// Make a copy of this itemset; caller is responsible to delete it.
//---------------------------------------------------------------------------
BOOL CDCClusterFreqItemset::makeCopy(CDCClusterFreqItemset& itemset)
{
    itemset.cleanup();
    CDCClusterFreqItem* pItem = NULL;
    CDCClusterFreqItem* pCopy = NULL;
	POSITION pos = GetHeadPosition();
	while (pos != NULL) {
		pItem = GetNext(pos);
		P_ASSERT(pItem);
        pItem->makeCopy(pCopy);
        P_ASSERT(pCopy);
        if (!itemset.addFreqItem(pCopy)) {
            ASSERT(FALSE);
            return FALSE;
        }
	}
    return TRUE;
}

//---------------------------------------------------------------------------
// Reset the cluster support to 0 for every item
//---------------------------------------------------------------------------
BOOL CDCClusterFreqItemset::resetClusterSupports()
{
    CDCClusterFreqItem* pItem = NULL;
	POSITION pos = GetHeadPosition();
	while (pos != NULL) {
		pItem = GetNext(pos);
		P_ASSERT(pItem);
        pItem->setClusterSupport(0.0f);
	}
    return TRUE;
}

//***************************************************************************
// Class: DCClusters
// Objective: A list of clusters
//***************************************************************************
CDCClusters::CDCClusters()
{

}

CDCClusters::~CDCClusters()
{
    RemoveAll();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CDCClusters::cleanup()
{
    POSITION pos = GetHeadPosition();
    while (pos != NULL) {
        delete GetNext(pos);
    }
    RemoveAll();
}

//---------------------------------------------------------------------------
// Add a list of new clusters to this collection.  The resultant clusters
// are sorted in ascending order
//---------------------------------------------------------------------------
BOOL CDCClusters::addClusters(CDCClusters* pNewClusters)
{
    if (!pNewClusters)
        return FALSE;

    CDCCluster* pCluster = NULL;
    POSITION pos = pNewClusters->GetHeadPosition();
    while (pos != NULL) {
        pCluster = pNewClusters->GetNext(pos);
        ASSERT(pCluster);

        if (!addCluster(pCluster)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Add the new cluster to this collection.  Given the current data structure
// is a list, it should look like:
// {{100}, {100, 101}, {100, 102}, {100, 334, 500}, {100, 120, 130, 202}}
//---------------------------------------------------------------------------
BOOL CDCClusters::addCluster(CDCCluster* pNewCluster)
{
    ASSERT(pNewCluster);
    if (IsEmpty()) {
        return AddTail(pNewCluster) != NULL;
    }

    // Properties of the new cluster
    CDCClusterFreqItemset* pNewCoreItems = NULL;
    pNewCluster->getClusterCoreItems(pNewCoreItems);    
    int numNewCoreItems = pNewCluster->getNumCoreItems();
    
    INT numCoreItems = -1;
    CDCCluster* pCluster = NULL;
    CDCClusterFreqItemset* pCoreItems = NULL;
    POSITION pos = GetTailPosition();
    while (pos != NULL) {
        //pCluster = GetPrev(pos);
        pCluster = GetAt(pos);
        ASSERT(pCluster);
        
        // number of core items of the current cluster
        numCoreItems = pCluster->getNumCoreItems();

        if (numCoreItems > numNewCoreItems) {
            // not found yet, keep going.
            GetPrev(pos);
            continue;
        }
        else if (numCoreItems < numNewCoreItems) {
            // insert new cluster After the current cluster
            InsertAfter(pos, pNewCluster);
            return TRUE;
        }
        else {
            // same number of items, need to compare item by item
            ASSERT(numCoreItems == numNewCoreItems);
            pCluster->getClusterCoreItems(pCoreItems);
            TCompareResult res = pCoreItems->compareTo(pNewCoreItems);
            if (res == COMPARE_SMALLER) {
                // not found yet, keep going.
                GetPrev(pos);
                continue;
            }
            else if (res == COMPARE_LARGER) {
                // insert new cluster After the current cluster
                InsertAfter(pos, pNewCluster);
                return TRUE;
            }
            else {
                // duplciated frequent itemset
                ASSERT(res == COMPARE_EQUAL);
                ASSERT(FALSE);
                GetPrev(pos);
                return FALSE;
            }
        }
    }
    return AddHead(pNewCluster) != NULL;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCClusters::removeCluster(CDCCluster *cluster)
{
	POSITION pos = this->GetHeadPosition();

	while (pos != NULL)
	{
		if (cluster == this->GetAt(pos))
		{
			this->RemoveAt(pos);
			return TRUE;
		}
		this->GetNext(pos);
	}

	return FALSE;
}

//---------------------------------------------------------------------------
// Find the clusters that has core item which is a subset of pFreqItemset
// No specific order in the resultant pClusters.
//---------------------------------------------------------------------------
BOOL CDCClusters::findSubSetClusters(const CDCFreqItemset* pFreqItemset, CDCClusters& clusters) const
{
    ASSERT(pFreqItemset);

    int nFreqItems = pFreqItemset->GetCount();
    CDCCluster* pCluster = NULL;
    CDCClusterFreqItemset* pCoreItems = NULL;
    POSITION pos = GetHeadPosition();
    while (pos != NULL) {
        pCluster = GetNext(pos);
        if (!pCluster) {
            ASSERT(FALSE);
            return FALSE;
        }

        if (pCluster->getNumCoreItems() > nFreqItems)
            return TRUE;

        // the frequent itemset is a superset of the core itemset of the current cluster        
        pCluster->getClusterCoreItems(pCoreItems);
        ASSERT(pCoreItems);
        if (pFreqItemset->containsAll(pCoreItems))
            clusters.AddTail(pCluster);
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Find the clusters that has core item which is a superset of pFreqItemset
// No specific order in the resultant pClusters.
//---------------------------------------------------------------------------
BOOL CDCClusters::findSuperSetClusters(const CDCFreqItemset* pFreqItemset, CDCClusters& clusters) const
{
    ASSERT(pFreqItemset);

    int nFreqItems = pFreqItemset->GetCount();
    CDCCluster* pCluster = NULL;
    CDCClusterFreqItemset* pCoreItems = NULL;
    POSITION pos = GetHeadPosition();
    while (pos != NULL) {
        pCluster = GetNext(pos);
        if (!pCluster) {
            ASSERT(FALSE);
            return FALSE;
        }

        if (pCluster->getNumCoreItems() < nFreqItems)
            continue;

        // the core itemset of the current cluster is a superset of the frequent itemset
        pCluster->getClusterCoreItems(pCoreItems);
        ASSERT(pCoreItems);
        if (pCoreItems->containsAll(pFreqItemset))
            clusters.AddTail(pCluster);
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Retain the itemset that has k items in it. (copied from MSDN)
//---------------------------------------------------------------------------
BOOL CDCClusters::retainKItemsets(int k) 
{
    CDCCluster* pCluster = NULL;
    POSITION pos1, pos2;
    for (pos1 = GetHeadPosition(); (pos2 = pos1) != NULL;) {
        pCluster = GetNext(pos1);
        if (!pCluster) {
            ASSERT(FALSE);
            return FALSE;
        }
        if (pCluster->getNumCoreItems() != k)
           RemoveAt(pos2);
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Retain the itemset that has k items in it. (copied from MSDN)
//---------------------------------------------------------------------------
BOOL CDCClusters::removeLargerThanKItemsets(int k) 
{
    CDCCluster* pCluster = NULL;
    POSITION pos1, pos2;
    for (pos1 = GetHeadPosition(); (pos2 = pos1) != NULL;) {
        pCluster = GetNext(pos1);
        if (!pCluster) {
            ASSERT(FALSE);
            return FALSE;
        }
        if (pCluster->getNumCoreItems() > k)
           RemoveAt(pos2);
    }
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int CDCClusters::getNumDocuments()
{
	POSITION pos = this->GetHeadPosition();
	CDCCluster* pCluster = NULL;
	int num = 0;

	while (pos != NULL)
	{
		pCluster = this->GetNext(pos);
		num += pCluster->getNumDocuments();
	}

	return num;
}

//---------------------------------------------------------------------------
// Sort all the clusters according to the given order type;
// parameter: 
//		(1) pOrderedClusters, the pointer to the clusters after ordering
//		(2) descendingOrAscending, whether in descending or ascending order
//				true: in descending order
//				flase: in ascending order
//---------------------------------------------------------------------------
BOOL CDCClusters::getOrderedClusters(TDCClusterOrderType orderType, 
									 CDCClusters* pOrderedClusters,
									 BOOL descendingOrAscending)
{
	CDCCluster* pCluster = NULL;
    
	POSITION pos = GetHeadPosition();
    while(pos != NULL) 
	{
        pCluster = GetNext(pos);
        ASSERT(pCluster);

		POSITION insertPos = findInsertPosition(orderType, pOrderedClusters, pCluster, descendingOrAscending);

		if(insertPos != NULL)
		{
			pOrderedClusters->InsertBefore(insertPos, pCluster);
		}

		else
		{
			pOrderedClusters->AddTail(pCluster);
		}		
    }

    return TRUE;	
}


//---------------------------------------------------------------------------
// Find the position in the pOrderedClusters to insert pTargetCluster
// parameter (descendingOrAscending): whether in descending or ascending order
//				true: in descending order
//				flase: in ascending order
//---------------------------------------------------------------------------
POSITION CDCClusters::findInsertPosition(TDCClusterOrderType orderType,
									     CDCClusters* pOrderedClusters, 
									     CDCCluster* pTargetCluster,
									     BOOL descendingOrAscending)
{
	/*
	#define ORDER_TYPE_GLOBAL_SUP 0
	#define ORDER_TYPE_COREITEMS_SIZE 1
	#define ORDER_TYPE_SCORE 2
	#define ORDER_TYPE_INTRA_SIMILARITY 3
	*/

	if(orderType == ORDER_TYPE_GLOBAL_SUP)
	{
		return findInsertPos_GlbSup(pOrderedClusters, pTargetCluster, descendingOrAscending);
	}
	
	if(orderType == ORDER_TYPE_COREITEMS_SIZE)
	{
		return findInsertPos_CoreItemSize(pOrderedClusters, pTargetCluster, descendingOrAscending);
	}

	if(orderType == ORDER_TYPE_SCORE)
	{
		return findInsertPos_Score(pOrderedClusters, pTargetCluster, descendingOrAscending);
	}

	if(orderType == ORDER_TYPE_INTRA_SIMILARITY)
	{
		return findInsertPos_IntraSimilarity(pOrderedClusters, pTargetCluster, descendingOrAscending);
	}
	
	else
	{
		printf("Invlaid type for ordering");
		return NULL;
	}
}


POSITION CDCClusters::findInsertPos_GlbSup(CDCClusters* pOrderedClusters, 
										   CDCCluster* pTargetCluster,
										   BOOL descendingOrAscending)
{
	float targetSup = pTargetCluster->getCoreItemsGlobalSupport();
	float tempSup = 0;
	
	CDCCluster* pCluster = NULL;
    
	POSITION pos = pOrderedClusters->GetHeadPosition();
    while(pos != NULL) 
	{
		pCluster = GetAt(pos);
        ASSERT(pCluster);

		tempSup = pCluster->getCoreItemsGlobalSupport();
		
		if(    ( descendingOrAscending && (targetSup > tempSup)) 
			|| (!descendingOrAscending && (targetSup < tempSup)) )
		{
			break;
		}
  
		else
		{
			GetNext(pos);
		}
	}

	return pos;
}


POSITION CDCClusters::findInsertPos_CoreItemSize(CDCClusters* pOrderedClusters, 
										  		 CDCCluster* pTargetCluster,
												 BOOL descendingOrAscending)
{
	int targetCoreItemNum = pTargetCluster->getNumCoreItems();
	int tempCoreItemNum = 0;
	
	CDCCluster* pCluster = NULL;
    
	POSITION pos = pOrderedClusters->GetHeadPosition();
    while(pos != NULL) 
	{
		pCluster = GetAt(pos);
        ASSERT(pCluster);

		tempCoreItemNum = pCluster->getNumCoreItems();
		
		if(    ( descendingOrAscending && (targetCoreItemNum > tempCoreItemNum)) 
			|| (!descendingOrAscending && (targetCoreItemNum < tempCoreItemNum)) )
		{
			break;
		}

		else
		{
			GetNext(pos);
		}
	}

	return pos;
}


POSITION CDCClusters::findInsertPos_Score(CDCClusters* pOrderedClusters, 
										  CDCCluster* pTargetCluster,
										  BOOL descendingOrAscending)
{
	float targetScore = pTargetCluster->getSelfScore();
	float tempScore = 0;
	
	CDCCluster* pCluster = NULL;
    
	POSITION pos = pOrderedClusters->GetHeadPosition();
    while(pos != NULL) 
	{
		pCluster = GetAt(pos);
        ASSERT(pCluster);

		tempScore = pCluster->getSelfScore();

		if(    ( descendingOrAscending && (targetScore > tempScore)) 
			|| (!descendingOrAscending && (targetScore < tempScore)) )
		{
			break;
		}

		else
		{
			GetNext(pos);
		}
	}

	return pos;
}


POSITION CDCClusters::findInsertPos_IntraSimilarity(CDCClusters* pOrderedClusters, 
													CDCCluster* pTargetCluster,
													BOOL descendingOrAscending)
{
	
	float targetSim = pTargetCluster->calculateIntraSimilairity();
	float tempSim = 0;
	
	CDCCluster* pCluster = NULL;
    
	POSITION pos = pOrderedClusters->GetHeadPosition();
    while(pos != NULL)
	{
		pCluster = GetAt(pos);
        ASSERT(pCluster);

		tempSim = pCluster->calculateIntraSimilairity();
		
		if(    ( descendingOrAscending && (targetSim > tempSim)) 
			|| (!descendingOrAscending && (targetSim < tempSim)) )
		{
			break;
		}
		else
		{
			GetNext(pos);
		}
	}

	return pos;
	
}

//***************************************************************************
// Class: DCCluster
// Objective: Cluster of documents
//***************************************************************************

CDCCluster::CDCCluster(const CDCFreqItemset* pCoreItems)
{
    ASSERT(pCoreItems && !pCoreItems->IsEmpty());
    m_pTreeParent = NULL;

    // Construct a "cluster core item" correspond to each given core item
    CDCFreqItem* pCoreItem = NULL;
    POSITION pos = pCoreItems->GetHeadPosition();
    while (pos != NULL) {
        pCoreItem = pCoreItems->GetNext(pos);
        ASSERT(pCoreItem);

        if (!m_coreItems.addFreqItem(new CDCClusterFreqItem(pCoreItem->getFreqItemID(), 0.0f)))
            ASSERT(FALSE);
    }

    // the global support of this core itemset
    m_coreItems.setGlobalSupport(pCoreItems->getGlobalSupport());

    // Cache the number of core items (for efficiency)
    // This is unlikley to change within lifetime of this instance.
    // However, if core items are updated, remember to update this counter.
    m_numCoreItems = m_coreItems.GetCount();  
	m_status = UNTOUCHED;
}

CDCCluster::~CDCCluster()
{    
    m_coreItems.cleanup();
    m_freqItems.cleanup();
    m_documents.RemoveAll();
    m_frequencies.RemoveAll();
    m_occurences.RemoveAll();
    m_potentialChildren.RemoveAll();
    m_treeChildren.RemoveAll();
}

//---------------------------------------------------------------------------
// Add a document to this cluster
//---------------------------------------------------------------------------
BOOL CDCCluster::addDocument(CDCDocument* pDocument)
{
    CDCDocVector* pDocVector = NULL;
    pDocument->getDocVector(pDocVector);
    if (!pDocVector) {
        ASSERT(FALSE);
        return FALSE;
    }

    // update the frequencies in this cluster
    if (!updateFrequencies(pDocVector)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // update the occurences in this cluster
    if (!updateOccurences(pDocVector)) {
        ASSERT(FALSE);
        return FALSE;
    }

    m_documents.Add(pDocument);    
    return TRUE;
}

//---------------------------------------------------------------------------
// Add some documents to this cluster
//---------------------------------------------------------------------------
BOOL CDCCluster::addDocuments(CDCDocuments* pDocuments)
{
	int nDocs = pDocuments->GetSize();
	for(int i = 0; i < nDocs; i ++) {
		if(!addDocument(pDocuments->GetAt(i))) {
			ASSERT(FALSE);
			return FALSE;
		}
	}
    return TRUE;
}

//---------------------------------------------------------------------------
// Remove all documents in this cluster
//---------------------------------------------------------------------------
void CDCCluster::removeAllDocs()
{ 
    m_documents.RemoveAll(); 
    if (!setupFrequencies(m_frequencies.GetSize())) {
        // this will reset the frequencies, but size unchanged.
        ASSERT(FALSE);
    }
    if (!setupOccurences(m_occurences.GetSize())) {
        // this will reset the occurences, but size unchanged.
        ASSERT(FALSE);
    }
}

//---------------------------------------------------------------------------
// Get all documents inlude tree children
//---------------------------------------------------------------------------
BOOL CDCCluster::getDocumentsIncludeTreeChildren(CDCDocuments& allDocs)
{
    allDocs.Append(m_documents);    
    CDCCluster* pChild = NULL;
    POSITION pos = m_treeChildren.GetHeadPosition();
    while (pos != NULL) {
        pChild = m_treeChildren.GetNext(pos);
        P_ASSERT(pChild);
        if (!pChild->getDocumentsIncludeTreeChildren(allDocs)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Get total number of documents including its tree children
//---------------------------------------------------------------------------
int CDCCluster::getNumDocumentsIncludeTreeChildren(const CDCCluster* pParentCluster)
{
    P_ASSERT(pParentCluster);
    int nTotal = pParentCluster->getNumDocuments();    
    CDCCluster* pChild = NULL;
    POSITION pos = pParentCluster->m_treeChildren.GetHeadPosition();
    while (pos != NULL) {
        pChild = pParentCluster->m_treeChildren.GetNext(pos);
        P_ASSERT(pChild);
        nTotal += getNumDocumentsIncludeTreeChildren(pChild);
    }
    return nTotal;
}

//---------------------------------------------------------------------------
// Get total number of clusters including its tree children
//---------------------------------------------------------------------------
int CDCCluster::getNumClustersIncludeTreeChildren(const CDCCluster* pParentCluster)
{
    P_ASSERT(pParentCluster);
    int nTotal = pParentCluster->m_treeChildren.GetCount();
    CDCCluster* pChild = NULL;
    POSITION pos = pParentCluster->m_treeChildren.GetHeadPosition();
    while (pos != NULL) {
        pChild = pParentCluster->m_treeChildren.GetNext(pos);
        P_ASSERT(pChild);
        nTotal += getNumClustersIncludeTreeChildren(pChild);
    }
    return nTotal;
}

//---------------------------------------------------------------------------
// Get the itemID of the first core item
//---------------------------------------------------------------------------
TFreqItemID CDCCluster::getFirstCoreItemID()
{
    return m_coreItems.getFreqItemID(0);
}

//---------------------------------------------------------------------------
// Get all the core and frequent 1-itemsets of this cluster
//---------------------------------------------------------------------------
BOOL CDCCluster::getClusterCoreFreqItems(CDCClusterFreqItemset& allItems)
{
    allItems.RemoveAll();
    allItems.AddTail(&m_coreItems);
    allItems.AddTail(&m_freqItems);
    return TRUE;
}

//---------------------------------------------------------------------------
// Get all the cluster frequent items
//---------------------------------------------------------------------------
BOOL CDCCluster::getClusterFreqItems(CDCClusterFreqItemset& freqItems)
{
    freqItems.RemoveAll();
    freqItems.AddTail(&m_freqItems);
    return TRUE;
}

//---------------------------------------------------------------------------
// Perform a deep copy on the frequent items
// Caller is responsible to delete it.
//---------------------------------------------------------------------------
BOOL CDCCluster::copyClusterFreqItems(CDCClusterFreqItemset& freqItems)
{
    return m_freqItems.makeCopy(freqItems);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CDCCluster::setClusterFreqItems(CDCClusterFreqItemset* pFreqItems)
{
	m_freqItems.cleanup();
	m_freqItems.AddTail(pFreqItems);
}

//---------------------------------------------------------------------------
// Calculate frequent one itemsets for this cluster based on the given domain
// frequencies and threshold.  In case the cluster is empty, pDomainFrequencies
// will be an array of zeros, so this function has no effect.
//---------------------------------------------------------------------------
BOOL CDCCluster::calFreqOneItemsets(const CDCDocVector* pDomainFrequencies, int nDocs, FLOAT clusterThreshold)
{
    //CDCDebug::printCluster(this);
    //CDCDebug::printDocVector(*pDomainFrequencies);
    
    m_freqItems.cleanup();
#pragma message("*** Ben: Fixing this bug lowers F-Measure???")
/*
    if (!m_coreItems.resetClusterSupports()) {
        ASSERT(FALSE);
        return FALSE;
    }
 */
    if (nDocs == 0)
        return TRUE;

    FLOAT clusterSupport = 0.0f;
    int minNumDocs = ceil(nDocs * clusterThreshold);    
    int nFrequencies = pDomainFrequencies->GetSize();
    for (int i = 0; i < nFrequencies; ++i) {
        if ((*pDomainFrequencies)[i] >= minNumDocs) {  
            // this item passes the threshold

            CDCClusterFreqItem* pFreqItem = m_coreItems.getFreqItem(i);
            if (pFreqItem) {
                // this item is a core item
                ASSERT((*pDomainFrequencies)[i] == nDocs);
                pFreqItem->setClusterSupport(1.0f);
            }
            else {
                // add to frequent itemset            
                clusterSupport = FLOAT((*pDomainFrequencies)[i]) / nDocs;
                if (clusterSupport < 0.0f || clusterSupport > 1.0f) {
                    ASSERT(FALSE);
                    return FALSE;
                }
                if (!m_freqItems.addFreqItem(new CDCClusterFreqItem(i, clusterSupport))) {
                    ASSERT(FALSE);
                    return FALSE;
                }
            }
        }
    }
    //CDCDebug::printFreqItemset(&m_freqItems);
    return TRUE;
}

//---------------------------------------------------------------------------
// Compute the centroid
//---------------------------------------------------------------------------
BOOL CDCCluster::computeCentroid(int nDimensions)
{
    // reset the centroid
    if (!setupCentroid(nDimensions)) {
        ASSERT(FALSE);
        return FALSE;
    }

    int nDocs = m_documents.GetSize();
    if (nDocs == 0)
        return TRUE;

    // add up the vector    
    CDCDocument* pDoc = NULL;
    CDCKMVector* pKMVector = NULL;
    for (int d = 0; d < nDocs; ++d) {
        pDoc = m_documents.GetAt(d);
        P_ASSERT(pDoc);
        pDoc->getKMVector(pKMVector);
        P_ASSERT(pKMVector);

        if (!m_centroid.addUp(pKMVector)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }

    // divide by number of documents
    if (!m_centroid.divideBy(nDocs)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Compute the FMeasure of this cluster given an evaluation class
//---------------------------------------------------------------------------
BOOL CDCCluster::computeFMeasure(CDCEvalClass* pEvalClass, FLOAT& fm)
{
    fm = 0.0f;
    CDCDocuments allDocs;
    if (!getDocumentsIncludeTreeChildren(allDocs)) {
        ASSERT(FALSE);
        return FALSE;
    }
    
    int nMatch = 0;
    CString targetClassName = pEvalClass->getClassName();
    CDCDocument* pDoc = NULL;    
    int nDocs = allDocs.GetSize();
    for (int i = 0; i < nDocs; ++i) {
        pDoc = allDocs.GetAt(i);
        if (targetClassName == pDoc->getClassName()) {
            ++nMatch;
        }
    }  

    if (nDocs == 0 || pEvalClass->getNItems() == 0)
        return TRUE;

    FLOAT recall = FLOAT(nMatch) / pEvalClass->getNItems();
    FLOAT precision = FLOAT(nMatch) / nDocs;
    FLOAT recallPrec = recall + precision;
    if (recallPrec == 0.0f)
        return TRUE;

    fm = (2 * recall * precision) / (recallPrec);
    return TRUE;
}

//---------------------------------------------------------------------------
// Compute intra cluster similarity, ||c||^2
//---------------------------------------------------------------------------
BOOL CDCCluster::computeIntraSimilarity(FLOAT& intraSim)
{
    intraSim = 0.0f;
    CDCDocuments allDocs;
    if (!getDocumentsIncludeTreeChildren(allDocs)) {
        ASSERT(FALSE);
        return FALSE;
    }

    int nDocs = allDocs.GetSize();
    if (nDocs == 0)
        return TRUE;

    // get the dimension    
    CDCKMVector* pKMVector = NULL;
    allDocs[0]->getKMVector(pKMVector);
    int nDimensions = pKMVector->GetSize();   

    // compute centroid of this cluster    
    CDCKMVector centroid;    
    centroid.SetSize(nDimensions);
    CDCDocument* pDoc = NULL;
    for (int i = 0; i < nDocs; ++i) {
        pDoc = allDocs.GetAt(i);
        P_ASSERT(pDoc);
        pDoc->getKMVector(pKMVector);
        P_ASSERT(pKMVector);

        if (!centroid.addUp(pKMVector)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }

    // divide by number of documents
    if (!centroid.divideBy(nDocs)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // compute c.c
    if (!CDCKMVector::dotProduct(&centroid, &centroid, intraSim)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Compute entropy
//---------------------------------------------------------------------------
BOOL CDCCluster::computeEntropy(int nTotalDocs, FLOAT& totalEntropy)
{
    // number of documents in the cluster
    int nDocs = getNumDocuments();
    if (nDocs > 0) {
        // go through each document and increment the corresponding counter
        CString className;
        CDCEvalClasses evalClasses;
        for (int i = 0; i < nDocs; ++i) {
            className = m_documents[i]->getClassName();
            if (!evalClasses.incEvalClass(className)) {
                ASSERT(FALSE);
                return FALSE;
            }
        }       

        // calculate probability that a member of cluster j belongs to class i
        FLOAT entropy = 0.0f, p = 0.0f;
        int nEvalClasses = evalClasses.GetSize();
        for (int j = 0; j < nEvalClasses; ++j) {
            p = FLOAT(evalClasses[j]->getNItems()) / nDocs;
            entropy += p * log(p) * -1;
        }

        // total entropy
        totalEntropy += (FLOAT(nDocs) / nTotalDocs) * entropy;
    }

    // go through child cluster
    CDCCluster* pChildCluster = NULL;
    POSITION pos = m_treeChildren.GetHeadPosition();
    while (pos != NULL) {
        pChildCluster = m_treeChildren.GetNext(pos);
        P_ASSERT(pChildCluster);

        if (!pChildCluster->computeEntropy(nTotalDocs, totalEntropy)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Setup the frequencies array
//---------------------------------------------------------------------------
BOOL CDCCluster::setupFrequencies(int size)
{
    m_frequencies.SetSize(size);
    for (int i = 0; i < size; ++i)
        m_frequencies[i] = 0;

    return TRUE;
}

//---------------------------------------------------------------------------
// Setup the occurences array
//---------------------------------------------------------------------------
BOOL CDCCluster::setupOccurences(int size)
{
    m_occurences.SetSize(size);
    for (int i = 0; i < size; ++i)
        m_occurences[i] = 0;

    return TRUE;
}

//---------------------------------------------------------------------------
// Setup the centroid array
//---------------------------------------------------------------------------
BOOL CDCCluster::setupCentroid(int size)
{
    m_centroid.SetSize(size);
    for (int i = 0; i < size; ++i)
        m_centroid[i] = 0;

    return TRUE;
}

//---------------------------------------------------------------------------
// Update the # of documents in this cluster contains this frequen item 
// (for computing frequent 1-itemsets)
//---------------------------------------------------------------------------
BOOL CDCCluster::updateFrequencies(const CDCDocVector* pDocVector)
{
    ASSERT(pDocVector);
    if (m_frequencies.GetSize() == 0) {
        if (!setupFrequencies(pDocVector->GetSize())) {
            ASSERT(FALSE);
            return FALSE;
        }
    }

    CDCFreqItemset presentItems;
    if (!pDocVector->getPresentItems(FALSE, presentItems)) {
        ASSERT(FALSE);
        return FALSE;
    }

    CDCFreqItem* pFreqItem = NULL;
    POSITION pos = presentItems.GetHeadPosition();
    while (pos != NULL) {
        pFreqItem = presentItems.GetNext(pos);
        ASSERT(pFreqItem);

        // For each present item, update the frequencies by one.
        // Even though an item appears 10 times in a doc, it is counted as 1.
        ++m_frequencies[pFreqItem->getFreqItemID()];
    }
    presentItems.cleanup();
    return TRUE;
}

//---------------------------------------------------------------------------
// Update the # of occurences of this frequent item in this cluster
//---------------------------------------------------------------------------
BOOL CDCCluster::updateOccurences(const CDCDocVector* pDocVector)
{
    ASSERT(pDocVector);
    if (m_occurences.GetSize() == 0) {
        if (!setupOccurences(pDocVector->GetSize())) {
            ASSERT(FALSE);
            return FALSE;
        }
    }

    if (!m_occurences.addUp(pDocVector)) {
        ASSERT(FALSE);
        return FALSE;
    }        
    return TRUE;
}

//---------------------------------------------------------------------------
// Add up the children occurences with the occurences in this cluster
// output: resultOccurences will contain the result of adding up all occurences 
// from all tree children AND the parent.
//---------------------------------------------------------------------------
BOOL CDCCluster::computeTreeChildrenOccurences(CDCDocVector& resultOccurences)
{
    if (!resultOccurences.addUp(&m_occurences)) {
        ASSERT(FALSE);
        return FALSE;
    }

    CDCCluster* pChildCluster = NULL;
    POSITION pos = m_treeChildren.GetHeadPosition();
    while (pos != NULL) {
        pChildCluster = m_treeChildren.GetNext(pos);
        ASSERT(pChildCluster);

        if (!pChildCluster->computeTreeChildrenOccurences(resultOccurences)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Add up the children frequencies with the frequency in this cluster
// output: resultFrequencies will contain the result of adding up all frequencies 
// from all tree children AND the parent.
//---------------------------------------------------------------------------
BOOL CDCCluster::computeTreeChildrenFrequencies(CDCDocVector& resultFrequencies)
{
    if (!resultFrequencies.addUp(&m_frequencies)) {
        ASSERT(FALSE);
        return FALSE;
    }

    CDCCluster* pChildCluster = NULL;
    POSITION pos = m_treeChildren.GetHeadPosition();
    while (pos != NULL) {
        pChildCluster = m_treeChildren.GetNext(pos);
        ASSERT(pChildCluster);

        if (!pChildCluster->computeTreeChildrenFrequencies(resultFrequencies)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int CDCCluster::pruneAllChildren()
{
    int nPrune = 0;
	int countDoc = 0;
	CDCDocuments* pDocuments = NULL;
	CDCCluster* pChildCluster = NULL;
	POSITION pos = m_treeChildren.GetHeadPosition();
	while (pos != NULL) {
		pChildCluster = m_treeChildren.GetNext(pos);
		pChildCluster->getDocuments(pDocuments);
		if (!pDocuments) {
			ASSERT(FALSE);
			return -1;
		}

		countDoc = pDocuments->GetSize();
		for (int i = 0; i < countDoc; i ++) {
			this->addDocument(pDocuments->GetAt(i));
		}

		pChildCluster->clearTreeParent();
		pChildCluster->removeAllDocs();
		pChildCluster->setStatus(PRUNED);
        ++nPrune;
	}
	m_treeChildren.RemoveAll();
	return nPrune;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCCluster::touchAllChildren()
{
	if (m_treeChildren.GetCount() == 0)
	    return FALSE;

	POSITION pos = m_treeChildren.GetHeadPosition();

	while (pos != NULL)
	{
		m_treeChildren.GetNext(pos)->setStatus(TOUCHED);
	}

	return TRUE;
}

//---------------------------------------------------------------------------
// Merge the given cluster to this cluster with its children
//---------------------------------------------------------------------------
BOOL CDCCluster::mergeCluster(CDCCluster* pCluster, CDCClusters* pAllClusters)
{
	ASSERT(pCluster);
	CDCDocuments* pDocuments = NULL;

    // move children under this cluster
	CDCClusters* pChildren = NULL;
	pCluster->getTreeChildren(pChildren);
	CDCCluster* pChild = NULL;
	POSITION posChild = pChildren->GetHeadPosition();
	while (posChild != NULL) {
		pChild = pChildren->GetNext(posChild);		
        P_ASSERT(pChild);
                
        if (!addTreeChild(pChild)) {
            ASSERT(FALSE);
            return FALSE;
        }
        pChild->setTreeParent(this);
	}
	pChildren->RemoveAll();
    pChildren = NULL;

    // move documents to this cluster    
	pCluster->getDocuments(pDocuments);
    int nDocs = pDocuments->GetSize();
	for (int i = 0; i < nDocs; ++i) {
        if (!addDocument(pDocuments->GetAt(i))) {
            ASSERT(FALSE);
            return FALSE;
        }
	}

    // remove the cluster from its parent
	CDCCluster* pParent = NULL;
	pCluster->getTreeParent(pParent);
	pParent->getTreeChildren(pChildren);
    POSITION remPos = pChildren->Find(pCluster);
    if (remPos)
        pChildren->RemoveAt(remPos);

    pCluster->clearTreeParent();

    // remove from the given list
    if (pAllClusters) {
        POSITION remPos = pAllClusters->Find(pCluster);
        if (remPos)
            pAllClusters->RemoveAt(remPos);
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Move all documents in the given cluster and its children to this cluster
//---------------------------------------------------------------------------
BOOL CDCCluster::mergeClusterPruneChildren(CDCCluster* pCluster, CDCClusters* pAllClusters)
{
    // move documents to this cluster
    CDCDocuments* pDocuments = NULL;
	pCluster->getDocuments(pDocuments);
    int nDocs = pDocuments->GetSize();
	for (int i = 0; i < nDocs; ++i) {
        if (!addDocument(pDocuments->GetAt(i))) {
            ASSERT(FALSE);
            return FALSE;
        }
	}
    
    // move children's documents to this cluster
	CDCClusters* pChildren = NULL;
	pCluster->getTreeChildren(pChildren);
	CDCCluster* pChild = NULL;
	POSITION posChild = pChildren->GetHeadPosition();
	while (posChild != NULL) {
		pChild = pChildren->GetNext(posChild);		
        P_ASSERT(pChild);

        if (!mergeClusterPruneChildren(pChild, pAllClusters)) {
            ASSERT(FALSE);
            return FALSE;
        }
	}
	pChildren->RemoveAll();
    pCluster->clearTreeParent();

    // remove from the given list
    if (pAllClusters) {
        POSITION remPos = pAllClusters->Find(pCluster);
        if (remPos)
            pAllClusters->RemoveAt(remPos);
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Convert to string
//---------------------------------------------------------------------------
CString CDCCluster::toString() const
{
    CString tmp;
	CString str = "** Cluster: ";
    str += m_coreItems.toString();
    str += "Addition frequent items: " + m_freqItems.toString();
    //str += "Frequencies: " + m_frequencies.toString();
    tmp.Format("# of docs = %d\n", m_documents.GetSize());
    str += tmp + "\n";
	return str;
}