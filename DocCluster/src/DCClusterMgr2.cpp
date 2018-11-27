// DCClusterMgr2.cpp: implementation of the CDCClusterMgr2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "math.h"

#if !defined(DCCLUSTERMGR2_H)
	#include "DCClusterMgr2.h"
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCClusterMgr2::CDCClusterMgr2()
{
	m_clusterSupport = 0;
    m_pAllDocs = NULL;
}

CDCClusterMgr2::~CDCClusterMgr2()
{

}

//---------------------------------------------------------------------------
// Initlialize clusters, reassign documents, and linear pruning;
// After all these are done, the clusters left are put under the tree root in 
// the ClusterWH
//---------------------------------------------------------------------------
BOOL CDCClusterMgr2::produceClusters(const CDCFreqItemsets* pGlobalFreqItemsets, 
									 TDCClusterOrderType orderType,
                                     BOOL descOrAsce)
{
    if (!m_pAllDocs) {
        ASSERT(FALSE);
        return FALSE;
    }
    if (!pGlobalFreqItemsets || pGlobalFreqItemsets->IsEmpty()) {
        ASSERT(FALSE);
        return FALSE;
    }

    // Construct the cluster warehouse
    DEBUGPrint("*** Adding clusters to warehouse\n");
    if (!m_clusterWH.addClusters(pGlobalFreqItemsets)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // Assign documents to cluster (initial clustering)
    DEBUGPrint("*** Initializing clusters\n");
    if (!initializeClusters()) {
        ASSERT(FALSE);
        return FALSE;
    }

	DEBUGPrint("*** Ordering all the initial clusters\n");
	if ( !orderAllClusters(orderType, descOrAsce) ) {
        ASSERT(FALSE);
        return FALSE;
    }	
	
	// Reassign documents to the most suitable cluster
	DEBUGPrint("*** Reassigning documents\n");
	if (!reassignDocuments()) {
        ASSERT(FALSE);
        return FALSE;
    }
	
	// Remove empty clusters
	DEBUGPrint("*** Removing empty clusters\n");
	removeEmptyClusters();

	/*----------------- under testing ------------------
	DEBUGPrint("*** Re-ordering the clusters\n");
	reOrderAllClusters(orderType, descOrAsce);
	----------------- under testing ------------------*/

    // Linear pruning
    DEBUGPrint("*** Linear Pruning\n");
    if (!linearPruneV1()) {
        ASSERT(FALSE);
        return FALSE;
    } 

    return TRUE;
}


//---------------------------------------------------------------------------
// Initialize clusters based on the frequent items in the document
//---------------------------------------------------------------------------
BOOL CDCClusterMgr2::initializeClusters()
{    
    CDCDocument* pDoc = NULL;
    CDCDocVector* pDocVector = NULL;    
    int nDocs = m_pAllDocs->GetSize();
    for (int i = 0; i < nDocs; ++i) {
        pDoc = m_pAllDocs->GetAt(i);
        ASSERT(pDoc);

        pDoc->getDocVector(pDocVector);
        ASSERT(pDocVector);

        // get the appeared items in the document
        CDCFreqItemset presentItems;
        if (!pDocVector->getPresentItems(TRUE, presentItems)) {
            ASSERT(FALSE);
            return FALSE;
        }
        // in case the doc contains no frequent items, this routine still works        

        //CDCDebug::printFreqItemset(&presentItems);
        // get all clusters that can cover this doc
        CDCClusters coveredClusters;
        if (!m_clusterWH.findCoveredClusters(&presentItems, coveredClusters)) {
            ASSERT(FALSE);
            presentItems.cleanup();
            return FALSE;
        }
        presentItems.cleanup();

        // assign doc to all the covered clusters
        if (!assignDocToClusters(pDoc, &coveredClusters)) {
            ASSERT(FALSE);
            return FALSE;
        }
#ifdef DC_SHOW_PROGRESS
        printf("%c:%d\r", runningChars[(i) % NUM_RUNNING_CHARS], i);
#endif
        //CDCDebug::printClusters(&coveredClusters);
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Assign the document to all the given clusters.
//---------------------------------------------------------------------------
BOOL CDCClusterMgr2::assignDocToClusters(CDCDocument* pDoc, CDCClusters* pClusters)
{
    ASSERT(pDoc && pClusters);

    CDCCluster* pCluster = NULL;
    POSITION pos = pClusters->GetHeadPosition();
    while (pos != NULL) {
        pCluster = pClusters->GetNext(pos);
        if (!pCluster) {
            ASSERT(FALSE);
            return FALSE;
        }
        if (!pCluster->addDocument(pDoc)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}


//---------------------------------------------------------------------------
// Sort all the clusters according to the given order type; the ordered cluster
// list will be stored in m_pOrderedClusters
//---------------------------------------------------------------------------
BOOL CDCClusterMgr2::orderAllClusters(TDCClusterOrderType orderType, BOOL descendingOrAscending)
{
	/*
	#define ORDER_TYPE_GLOBAL_SUP 0
	#define ORDER_TYPE_COREITEMS_SIZE 1
	#define ORDER_TYPE_SCORE 2
	#define ORDER_TYPE_INTRA_SIMILARITY 3
	*/

	CDCClusters allClusters;
	if ( !m_clusterWH.getAllClustersReverseOrder(allClusters) ) 
	{
        ASSERT(FALSE);
        return FALSE;
    }

	if(orderType == ORDER_TYPE_SCORE || orderType == ORDER_TYPE_INTRA_SIMILARITY)
	{
		setSelfScoreForAllClusters(&allClusters);
	}

	if( !allClusters.getOrderedClusters(orderType, &m_orderedClusters, descendingOrAscending) )
	{
		ASSERT(FALSE);
		return FALSE;
	}

	/*--------- for test ------------
	POSITION pos1;
	CDCCluster * pCluster;
	CString tmpString1;
	float score;

	for(pos1 = m_orderedClusters.GetHeadPosition(); pos1 != NULL; )
	{
		pCluster = m_orderedClusters.GetNext(pos1);
		tmpString1 = pCluster->toString();
		score = pCluster->getSelfScore();
	}	
	--------------------------------*/
	
	return TRUE;
}

//---------------------------------------------------------------------------
// re-order m_pOrderedClusters after document reassignment
//---------------------------------------------------------------------------
BOOL CDCClusterMgr2::reOrderAllClusters(TDCClusterOrderType orderType, BOOL descOrAsc)
{
	// Re-order is needed if ordering was based on the cluster score
	if(orderType == ORDER_TYPE_SCORE || orderType == ORDER_TYPE_INTRA_SIMILARITY)
	{
		DEBUGPrint("Re-ordering is needed for score & intra-similarity based order\n");

		setSelfScoreForAllClusters(&m_orderedClusters);

		// make a copy of m_orderedClusters in tmpClusters, 
		// and then remove all the pointers from m_orderedClusters
		CDCClusters tmpClusters;
		tmpClusters.AddTail(&m_orderedClusters);
		m_orderedClusters.RemoveAll();

		if( !tmpClusters.getOrderedClusters(orderType, &m_orderedClusters, descOrAsc) )
		{
			tmpClusters.RemoveAll();
			ASSERT(FALSE);
			return FALSE;
		}

		/*--------------- for test 2: start -------------*/
		POSITION pos1;
		CDCCluster * pCluster;
		CString tmpString1;
		float sim;
		
		for(pos1 = m_orderedClusters.GetHeadPosition(); pos1 != NULL; )
		{
			pCluster = m_orderedClusters.GetNext(pos1);
			tmpString1 = pCluster->toString();
			sim = pCluster->calculateIntraSimilairity();
		}
		/*--------------- for test: end -------------*/

		tmpClusters.RemoveAll();		
	}
	
	else
	{
		DEBUGPrint("Re-ordering is not needed\n");
	}
	
	return TRUE;
}


//---------------------------------------------------------------------------
// calculate self score for all the clusters against themselves
//---------------------------------------------------------------------------
BOOL CDCClusterMgr2::setSelfScoreForAllClusters(CDCClusters* pAllClusters)
{
	float score;
	CDCCluster* pCluster;
	CDCDocVector* pOccurences;
	POSITION pos = pAllClusters->GetHeadPosition();
	
	while (pos != NULL) 
	{
		pCluster = pAllClusters->GetNext(pos);
		pCluster->getOccurences(pOccurences);
		
		if( !calClusterScoreV1(&m_clusterWH, pOccurences, pCluster, score) )
		{
			ASSERT(FALSE);
			return FALSE;
		}

		pCluster->setSelfScore(score);
	}

	return TRUE;
}

//---------------------------------------------------------------------------
// Reassign documents to the leftmost cluster that cover them.
// All the clusters have been ordered based on some criteria
//---------------------------------------------------------------------------
BOOL CDCClusterMgr2::reassignDocuments()
{
	// Clear all the documents in all the clusters in the Warehouse
    //DEBUGPrint("*** Removing documents from initial clusters\n");
    if (!removeAllDocsFromWH()) {
        ASSERT(FALSE);
        return FALSE;
    }

	CDCDocument* pDoc = NULL;
    CDCDocVector* pDocVector = NULL;    
    int nDocs = m_pAllDocs->GetSize();
    
	for (int i = 0; i < nDocs; ++i) 
	{
        pDoc = m_pAllDocs->GetAt(i);
        ASSERT(pDoc);

        pDoc->getDocVector(pDocVector);
        ASSERT(pDocVector);

        // get the appeared items in the document
        CDCFreqItemset presentItems;
        if (!pDocVector->getPresentItems(TRUE, presentItems)) {
            ASSERT(FALSE);
            return FALSE;
        }

        if (presentItems.IsEmpty()) {
            // this doc contains no frequent items, add it to dangling array
            if (!m_clusterWH.addDanglingDoc(pDoc)) {
                ASSERT(FALSE);
                presentItems.cleanup();
                return FALSE;
            }
            presentItems.cleanup();
            continue;
        }

		CDCCluster* targetCluster = findLeftmostCoveredCluster(&presentItems);
		presentItems.cleanup();
		// assign doc to the target cluster        
        if (!targetCluster->addDocument(pDoc)) {
            ASSERT(FALSE);
            return FALSE;
        } 
#ifdef DC_SHOW_PROGRESS	
        printf("%c:%d\r", runningChars[(i) % NUM_RUNNING_CHARS], i);
#endif
    }

	return TRUE;
}

//---------------------------------------------------------------------------
// Remove all the documents in the Cluster Warehouse
//---------------------------------------------------------------------------
BOOL CDCClusterMgr2::removeAllDocsFromWH()
{
    CDCClusters* pAllClusters = NULL;
    if (!m_clusterWH.getAllClusters(pAllClusters)) {
        ASSERT(FALSE);
        return FALSE;
    }

    CDCCluster* pCluster = NULL;
    POSITION pos = pAllClusters->GetHeadPosition();
    while (pos != NULL) {
        pCluster = pAllClusters->GetNext(pos);
        pCluster->removeAllDocs();
    }

    m_clusterWH.clearDanglingDocs();
    return TRUE;
}


//---------------------------------------------------------------------------
// find the leftmost cluster in the ordered cluster list that is covered by
// the given frequent itemset
//---------------------------------------------------------------------------
CDCCluster* CDCClusterMgr2::findLeftmostCoveredCluster(CDCFreqItemset* itemset)
{
	CDCCluster* pCluster = NULL;
	CDCCluster* coveredCluster = NULL;

    POSITION pos = m_orderedClusters.GetHeadPosition();
    
	CDCClusterFreqItemset * pCoreItems;

	while (pos != NULL) 
	{
        pCluster = m_orderedClusters.GetNext(pos);
        ASSERT(pCluster);
		
		pCluster->getClusterCoreItems(pCoreItems);

		if(itemset->containsAll(pCoreItems))
		{
			coveredCluster = pCluster;
			break;		      
        }
    }
    
	return coveredCluster;
}

//---------------------------------------------------------------------------
// remove all the empty clusters from m_orderedClusters
//---------------------------------------------------------------------------
void CDCClusterMgr2::removeEmptyClusters()
{			
	printf("Before empty cluster removing, %d clusters\n", m_orderedClusters.GetCount());
	
	CDCCluster* pCluster = NULL;
    POSITION pos1, pos2;

	for(pos1 = m_orderedClusters.GetHeadPosition(); (pos2 = pos1) != NULL; )
	{
        pCluster = m_orderedClusters.GetNext(pos1);
		
		if(pCluster->getNumDocuments() == 0)
		{
			m_orderedClusters.RemoveAt(pos2);
		}
	}
	
	printf("After empty cluster removing, %d clusters\n", m_orderedClusters.GetCount());
}


//---------------------------------------------------------------------------
// Linear pruning; after pruning, all the remaining clusters will be the children
// of m_clusterWH.m_treeRoot 
//---------------------------------------------------------------------------
BOOL CDCClusterMgr2::linearPruneV1()
{
	CDCCluster* pChild = NULL;
	CDCCluster* pParent = NULL;
	CDCDocuments* pDocs = NULL;

	CDCCluster* pRoot = NULL;
	m_clusterWH.getTreeRoot(pRoot);

	CDCCluster* pCluster = NULL;
    POSITION pos1, pos2;

	printf("Before pruning, %d clusters\n", m_orderedClusters.GetCount());

	for(pos1 = m_orderedClusters.GetHeadPosition(); (pos2 = pos1) != NULL; )
	{
		pChild = m_orderedClusters.GetNext(pos1);
        ASSERT(pChild);

		pParent = findParent(pChild, pos1);
		//pParent = findParent(pChild, m_orderedClusters.GetHeadPosition());
		
		// if the cluster has parent, try pruning
		if(pParent != NULL)
		{
			if( pruneOrNot(pParent, pChild) )
			{
				pChild->getDocuments(pDocs);
				pParent->addDocuments(pDocs);
				pChild->removeAllDocs();
				m_orderedClusters.RemoveAt(pos2);
			}
			// if not prune, put the child under the tree root 
			else
			{
				pRoot->addTreeChild(pChild);
				pChild->setTreeParent(pRoot);
			}
		}
		// if has not parent, put it under the tree root 
		else
		{
			pRoot->addTreeChild(pChild);
			pChild->setTreeParent(pRoot);
		}
	}
	printf("After pruning, %d clusters\n", m_orderedClusters.GetCount());

	mergePruning2(m_orderedClusters.GetCount() - 15);

	return TRUE;
}


//---------------------------------------------------------------------------
// In linear pruning, find parent from the head of the cluster list; 
// After pruning, a tree hierarchy will be created and the root
// is m_clusterWH.m_treeRoot 
//---------------------------------------------------------------------------
BOOL CDCClusterMgr2::linearPruneV2()
{
	CDCCluster* pChild = NULL;
	CDCCluster* pParent = NULL;
	CDCDocuments* pDocs = NULL;

	CDCCluster* pRoot = NULL;
	m_clusterWH.getTreeRoot(pRoot);

	CDCCluster* pCluster = NULL;
    POSITION pos1, pos2;

	printf("Before pruning, %d clusters\n", m_orderedClusters.GetCount());

	for(pos1 = m_orderedClusters.GetHeadPosition(); (pos2 = pos1) != NULL; )
	{
		pChild = m_orderedClusters.GetNext(pos1);
        ASSERT(pChild);
	
		pParent = findParent(pChild, m_orderedClusters.GetHeadPosition());

		// if the cluster has parent, try pruning
		if(pParent != NULL)
		{
			if( pruneOrNot(pParent, pChild) )
			{
				pChild->getDocuments(pDocs);
				pParent->addDocuments(pDocs);
				pChild->removeAllDocs();
				m_orderedClusters.RemoveAt(pos2);
			}
		}

		else
		{
			pRoot->addTreeChild(pChild);
			pChild->setTreeParent(pRoot);
		}
	}

	printf("After pruning, %d clusters\n", m_orderedClusters.GetCount());

	constructTree();

	return TRUE;
}


//---------------------------------------------------------------------------
// find parent for each cluster; parent will chosen from the right of the
// given cluster
//---------------------------------------------------------------------------
CDCCluster* CDCClusterMgr2::findParent(CDCCluster* pCurrentCluster, POSITION startPos)
{
	CDCCluster* pPossbileParent = NULL;
    
	CDCClusterFreqItemset *pCoreItems;
	pCurrentCluster->getClusterCoreItems(pCoreItems);
	
	while (startPos != NULL) 
	{
        pPossbileParent = m_orderedClusters.GetNext(startPos);
        ASSERT(pPossbileParent);

		if(pPossbileParent == pCurrentCluster) continue;
		
		CDCClusterFreqItemset *pParentCoreItems;
		pPossbileParent->getClusterCoreItems(pParentCoreItems);

		// it is possible that a cluster has no parent
		if(pCoreItems->containsAll(pParentCoreItems))
		{
			return pPossbileParent;		      
		}
    }

	return NULL;	
}


//---------------------------------------------------------------------------
// check whether pruning is needed for given parent and child
//---------------------------------------------------------------------------
BOOL CDCClusterMgr2::pruneOrNot(CDCCluster* pParent, CDCCluster* pChild)
{
	CDCDocVector* pOccurences = NULL;
	CDCDocVector* pFrequencies = NULL;
	CDCDocVector frequenciesAfter;
	CDCDocVector occurencesAfter;
	
	int nDocs_parent = 0;
	int nDocs_child = 0;
	int nDocs_all = 0;
	FLOAT score = -1;
	double errObserved;

	double errBefore_parent = 0;
	double errBefore_child = 0;
	double errAfter = 0;	
	
	//handle parent at first
	
	pParent->getOccurences(pOccurences);
	pParent->getFrequencies(pFrequencies);

	calClusterScoreV1(&m_clusterWH, pOccurences, pParent, score);
	nDocs_parent = pParent->getNumDocuments();

	errObserved = nDocs_parent * (1 - (score / pOccurences->getSum()));
	errBefore_parent = errObserved + calculatePessimisticError(errObserved, nDocs_parent);

	frequenciesAfter.SetSize(pOccurences->GetSize());
	occurencesAfter.SetSize(pOccurences->GetSize());
	
	frequenciesAfter.addUp(pFrequencies);	// add the parent's frequencies to "after"
	occurencesAfter.addUp(pOccurences);

	// then, handle child

	pChild->getOccurences(pOccurences);
	pChild->getFrequencies(pFrequencies);

	calClusterScoreV1(&m_clusterWH, pOccurences, pChild, score);
	nDocs_child = pChild->getNumDocuments();

	errObserved = nDocs_child * (1 - (score / pOccurences->getSum()));
	errBefore_child = errObserved + calculatePessimisticError(errObserved, nDocs_child);

	frequenciesAfter.addUp(pFrequencies);	// add the child's frequencies to "after"
	occurencesAfter.addUp(pOccurences);

	// take the parent and child together

	nDocs_all= nDocs_parent + nDocs_child;
	
	CDCClusterFreqItemset oldFreqItems; // backup the old frequent items of parent
	if (!pParent->copyClusterFreqItems(oldFreqItems)) 
	{
		ASSERT(FALSE);
        oldFreqItems.cleanup();
        return FALSE;
    }

	pParent->calFreqOneItemsets(&frequenciesAfter, nDocs_all, m_clusterSupport);

	calClusterScoreV1(&m_clusterWH, &occurencesAfter, pParent, score);

	errObserved = nDocs_all * (1- (score / occurencesAfter.getSum()));				
	errAfter = errObserved + calculatePessimisticError(errObserved, nDocs_all);

	if ( (errBefore_parent + errBefore_child ) < errAfter )
	{
		// restore the frequent items for parent
		pParent->setClusterFreqItems(&oldFreqItems);
		return FALSE;	
	}
	
	oldFreqItems.cleanup();
	return TRUE;
}


//---------------------------------------------------------------------------
// calculate the score for the given cluster
//---------------------------------------------------------------------------
BOOL CDCClusterMgr2::calClusterScoreV1(const CDCClusterWH* pClusterWH,
                                      const CDCDocVector* pDocVector,
                                      CDCCluster* pCluster,
                                      FLOAT& score)
{
    ASSERT(pClusterWH && pDocVector && pCluster);
    score = -FLT_MAX;

    // get the cluster's core and frequent items
    CDCClusterFreqItemset coreFreqItemset;
    if (!pCluster->getClusterCoreFreqItems(coreFreqItemset)) {
        ASSERT(FALSE);
        return FALSE;
    }
    //CDCDebug::printFreqItemset(&coreFreqItemset);

    // scan through each frequent item in the document vector
    CDCClusterFreqItem* pFreqItem = NULL;
    score = 0.0f;
    FLOAT infreqSup = 1.0f;
    int frequency = -1;
    int nItems = pDocVector->GetSize();    
    for (int id = 0; id < nItems; ++id) {
        frequency = (*pDocVector)[id];
        if (frequency <= 0)
            continue;

        pFreqItem = coreFreqItemset.getFreqItem(id);
        if (pFreqItem != NULL) {
            // add score --> n(x) * ClusterSupport(x)
            score += frequency * pFreqItem->getClusterSupport();
        }
        else {
            // deduct score --> n(x') * GlobalSupport(x')
            infreqSup = pClusterWH->getFrequentItemGlobalSupport(id);
            if (infreqSup == -1.0f) {
                ASSERT(FALSE);
                return FALSE;
            }
            score -= frequency * infreqSup;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// calculate pessimistic errors
// err: observed errors, num: # of docs
//---------------------------------------------------------------------------
double CDCClusterMgr2::calculatePessimisticError(double err, double num)
{
	static double Val[] = {0, 0.001, 0.005, 0.01, 0.05, 0.10, 0.20, 0.40, 1.00};
	static double Dev[] = {100, 3.09, 2.58, 2.33, 1.65, 1.28, 0.84, 0.25, 0.00};

	static double Coeff = 0;

	double Val0, Pr;

	if (! Coeff)
	{
		//Compute and retain the coefficient value, interpolating from the values in Val and Dev

		int i = 0;
		
		while ( DC_CF > Val[i] )
		{
			i++;
		}

		Coeff = Dev[i - 1] + (Dev[i] - Dev[i-1]) * (DC_CF - Val[i-1]) / (Val[i] - Val[i-1]);
		Coeff = Coeff * Coeff;
	}

	if (err < 1E-6)
	{
		return num * (1 - exp(log(DC_CF) / num));
	}
	else
	{
		if ( err < 0.9999 )
		{
			Val0 = num * (1 - exp(log(DC_CF) / num));
			return Val0 + err * (calculatePessimisticError(1.0, num) - Val0);
		}
		else
		{
			if ( err + 0.5 >= num)
			{
				return 0.67 * (num - err);
			}
			else
			{
				Pr = (err + 0.5 + Coeff / 2 + sqrt(Coeff * ((err + 0.5) * (1- (err + 0.5) / num) + Coeff / 4 ))) / (num + Coeff);
				return (num * Pr - err);
			}
		}
	}
}


//---------------------------------------------------------------------------
// construct tree after pruning
//---------------------------------------------------------------------------
void CDCClusterMgr2::constructTree()
{
	CDCCluster* pChild = NULL;
	CDCCluster* pParent = NULL;

	CDCCluster* pRoot = NULL;
	m_clusterWH.getTreeRoot(pRoot);

	CDCCluster* pCluster = NULL;
    POSITION pos1;

	for(pos1 = m_orderedClusters.GetHeadPosition(); pos1 != NULL; )
	{
		pChild = m_orderedClusters.GetNext(pos1);
        ASSERT(pChild);

		pChild->getTreeParent(pParent);
		if( pParent != NULL) continue;
		
		pParent = findParent(pChild, m_orderedClusters.GetHeadPosition());
		
		if(pParent != NULL)
		{
			pParent->addTreeChild(pChild);
			pChild->setTreeParent(pParent);			
		}
		// if has not parent, put it under the tree root 
		else
		{
			pRoot->addTreeChild(pChild);
			pChild->setTreeParent(pRoot);
		}
	}
}

//---------------------------------------------------------------------------
// clean up
//---------------------------------------------------------------------------
void CDCClusterMgr2::cleanUp()
{

}

//---------------------------------------------------------------------------
// copy from Leo's program
//---------------------------------------------------------------------------
BOOL CDCClusterMgr2::mergePruning2(int runs)
{
	CDCCluster* pRoot;
	CDCClusters* pF1Children;

	CDCCluster* pCluster1;
	CDCCluster* pCluster2;
	POSITION posChildren1;
	POSITION posChildren2;

	m_clusterWH.getTreeRoot(pRoot);
	P_ASSERT(pRoot);

	pRoot->getTreeChildren(pF1Children);
	P_ASSERT(pF1Children);

	int numF1 = pF1Children->GetCount();
	P_ASSERT(numF1);

	FLOAT* aryInterSimilarities = new FLOAT[numF1*(numF1-1)/2];
	CDCCluster** aryPositions1 = new CDCCluster*[numF1*(numF1-1)/2];
	CDCCluster** aryPositions2 = new CDCCluster*[numF1*(numF1-1)/2];

	posChildren1 = pF1Children->GetHeadPosition();
	int count = 0;

	int counter = 0;

	while (posChildren1 != NULL)
	{
#ifdef DC_SHOW_PROGRESS
        printf(_T("%c:Round#:%d\r"), runningChars[counter%NUM_RUNNING_CHARS], counter++);
#endif
		pCluster1 = pF1Children->GetAt(posChildren1);
		if (!pCluster1)
		{
			delete [] aryInterSimilarities;
			delete [] aryPositions1;
			delete [] aryPositions2;
			ASSERT(FALSE);
			return FALSE;
		}

		posChildren2 = posChildren1;
		pF1Children->GetNext(posChildren2);

		while (posChildren2 != NULL)
		{
			pCluster2 = pF1Children->GetAt(posChildren2);
			if (!pCluster2)
			{
				delete [] aryInterSimilarities;
				delete [] aryPositions1;
				delete [] aryPositions2;
				ASSERT(FALSE);
				return FALSE;
			}
			FLOAT interSim = 0;
			if (!calculateInterSimilarity(pCluster1, pCluster2, interSim))
			{
				delete [] aryInterSimilarities;
				delete [] aryPositions1;
				delete [] aryPositions2;
				ASSERT(FALSE);
				return FALSE;
			}
			aryInterSimilarities[count] = interSim;
			aryPositions1[count] = pCluster1;
			aryPositions2[count] = pCluster2;
			count++;
			pF1Children->GetNext(posChildren2);
		}
		pF1Children->GetNext(posChildren1);
	}

	printf("\n");
	for (int i = 0; i < runs; i ++)
	{
#ifdef DC_SHOW_PROGRESS
        printf(_T("%c:Round#:%d\r"), runningChars[i%NUM_RUNNING_CHARS], i);
#endif
		int highIndex = findHighestInterSimilarity(aryInterSimilarities, count);

		if (highIndex == -1)
		{
			break;
		}

		pCluster1 = aryPositions1[highIndex];
		pCluster2 = aryPositions2[highIndex];
		if ((!pCluster1) || (!pCluster2))
		{
			delete [] aryInterSimilarities;
			delete [] aryPositions1;
			delete [] aryPositions2;
			ASSERT(FALSE);
			return FALSE;
		}

		pCluster1->mergeCluster(pCluster2, NULL);
		cleanSimilarity(pCluster2, aryPositions1, aryPositions2, aryInterSimilarities, count);

		posChildren2 = pF1Children->GetHeadPosition();

		while (posChildren2 != NULL)
		{
			pCluster2 = pF1Children->GetAt(posChildren2);
			
			if (pCluster2 == pCluster1)
			{
				pF1Children->GetNext(posChildren2);
				continue;
			}

			if (!pCluster2)
			{
				delete [] aryInterSimilarities;
				delete [] aryPositions1;
				delete [] aryPositions2;
				ASSERT(FALSE);
				return FALSE;
			}

			int index = findClusterIndex(pCluster1, aryPositions1, pCluster2, aryPositions2, count);

			if (index == -1)
			{
				delete [] aryInterSimilarities;
				delete [] aryPositions1;
				delete [] aryPositions2;
				ASSERT(FALSE);
				return FALSE;
			}

			FLOAT interSim = 0;
			if (!calculateInterSimilarity(pCluster1, pCluster2, interSim))
			{
				delete [] aryInterSimilarities;
				delete [] aryPositions1;
				delete [] aryPositions2;
				ASSERT(FALSE);
				return FALSE;
			}
			aryInterSimilarities[index] = interSim;
			pF1Children->GetNext(posChildren2);
		}

	}
	delete [] aryInterSimilarities;
	delete [] aryPositions1;
	delete [] aryPositions2;

	return TRUE;
}


//---------------------------------------------------------------------------
// copy from Leo's program
// Calculate the similarity between two clusters.
// Idea is from F-Measure.
//---------------------------------------------------------------------------
BOOL CDCClusterMgr2::calculateInterSimilarity(CDCCluster * pCluster1, CDCCluster * pCluster2, FLOAT & interSim)
{
	FLOAT score1 = 0;
	FLOAT score2 = 0;

	CDCDocVector occurences1, occurences2;
	CDCDocVector* pTmpOccur;

	pCluster1->getOccurences(pTmpOccur);

	occurences1.SetSize(pTmpOccur->GetSize());
    occurences2.SetSize(pTmpOccur->GetSize());

	pCluster1->computeTreeChildrenOccurences(occurences1);
	this->calClusterScoreV1(&m_clusterWH, &occurences1, pCluster2, score1);
	score1 /= occurences1.getSum(); //calculate similarity

	pCluster2->computeTreeChildrenOccurences(occurences2);
	this->calClusterScoreV1(&m_clusterWH, &occurences2, pCluster1, score2);
	score2 /= occurences2.getSum(); //calculate similarity

	if ((score1 <= 0) || (score2 <= 0))
	{
		interSim = 0;
	}
	else
	{
		interSim = (2*score1*score2) / (score1+score2);
	}

	return TRUE;

}

int CDCClusterMgr2::findHighestInterSimilarity(const FLOAT *aryInterSimilarities, const int length)
{
	P_ASSERT(aryInterSimilarities);

	FLOAT highSim = 0;
	int highIndex = -1;

	for (int i = 1; i < length; i ++)
	{
		if (aryInterSimilarities[i] > highSim)
		{
			highSim = aryInterSimilarities[i];
			highIndex = i;
		}
	}

	return highIndex;
}

int CDCClusterMgr2::findClusterIndex(CDCCluster* pos1, CDCCluster** aryPositions1, CDCCluster* pos2, CDCCluster** aryPositions2, const int length)
{
	for (int i = 0; i < length; i ++)
	{
		if ((pos1 == aryPositions1[i]) && (pos2 == aryPositions2[i]))
		{
			return i;
		}
		if ((pos1 == aryPositions2[i]) && (pos2 == aryPositions1[i]))
		{
			return i;
		}
	}

	return -1;
}

BOOL CDCClusterMgr2::cleanSimilarity(CDCCluster* pos, CDCCluster **aryPositions1, CDCCluster**aryPositions2, FLOAT* arySims, const int length)
{
	for (int i = 0; i < length; i++)
	{
		if (aryPositions1[i] == pos)
		{
			arySims[i] = 0;
		}

		if (aryPositions2[i] == pos)
		{
			arySims[i] = 0;
		}
	}

	return TRUE;
}