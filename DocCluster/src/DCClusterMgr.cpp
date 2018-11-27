//---------------------------------------------------------------------------
// File:
//      DCClusterMgr.h, DCClusterMgr.cpp
// History:
//      Feb. 25, 2002   Created by Benjamin Fung
//---------------------------------------------------------------------------

#include "stdafx.h"

#include <MATH.H>
#include <FLOAT.H>

#if !defined(DCCLUSTERMGR_H)
    #include "DCClusterMgr.h"
#endif

#if !defined (DCDOCUMENT_H)
    #include "DCDocument.h"
#endif

#if !defined(DCDEBUG_H)
    #include "DCDebug.h"
#endif

//***************************************************************************
// Class: DCClusterMgr
// Objectives: Perform the clustering stage
//***************************************************************************


CDCClusterMgr::CDCClusterMgr()
{
    if (!initialize())
        ASSERT(FALSE);
}

CDCClusterMgr::~CDCClusterMgr()
{
}

BOOL CDCClusterMgr::initialize()
{
	m_clusterSupport = 0;
    m_pAllDocs = NULL;
    return TRUE;
}

void CDCClusterMgr::cleanup()
{
}

//---------------------------------------------------------------------------
// Calculate the similarity between two clusters.
// Idea is from F-Measure.
//---------------------------------------------------------------------------
BOOL CDCClusterMgr::calculateInterSimilarity(CDCCluster * pCluster1, CDCCluster * pCluster2, FLOAT& interSim)
{
	FLOAT score1 = 0.0f;
	FLOAT score2 = 0.0f;
    UINT sum = 0;
    interSim = 0.0f;

	CDCDocVector occurences1, occurences2;
	CDCDocVector* pTmpOccur;
	pCluster1->getOccurences(pTmpOccur);
	occurences1.SetSize(pTmpOccur->GetSize());
    occurences2.SetSize(pTmpOccur->GetSize());
    if (!pCluster1->computeTreeChildrenOccurences(occurences1)) {
        ASSERT(FALSE);
        return FALSE;
    }
    if (!calClusterScoreV1(&m_clusterWH, &occurences1, pCluster2, score1)) {
        ASSERT(FALSE);
        return FALSE;
    }
    sum = occurences1.getSum();
    if (sum == 0) {
        ASSERT(FALSE);
        score1 = 0.0f;
    }
    else
	    score1 /= sum;  // calculate similarity by normalizing the score

    if (!pCluster2->computeTreeChildrenOccurences(occurences2)) {
        ASSERT(FALSE);
        return FALSE;
    }
    if (!calClusterScoreV1(&m_clusterWH, &occurences2, pCluster1, score2)) {
        ASSERT(FALSE);
        return FALSE;
    }
    sum = occurences2.getSum();
    if (sum == 0) {
        ASSERT(FALSE);
        score2 = 0.0f;
    }
    else
	    score2 /= sum;  // calculate similarity by normalizing the score

/*
    // FMeasure
    if (score1 == 0 && score2 == 0) {
        interSim = 0.0f;
        return TRUE;
    }

    // negative inter-similarity?
    BOOL bNegative = FALSE;
    if (score1 < 0) {
        score1 *= -1.0f;
        bNegative = TRUE;
    }
    if (score2 < 0) {
        score2 *= -1.0f;
        bNegative = TRUE;
    }

    interSim = FLOAT(2 * score1 * score2) / (score1 + score2);
    if (bNegative)
        interSim *= -1.0f;
 */

    // negative inter-similarity?
    BOOL bBothNegative = FALSE;
    if (score1 < 0)
        bBothNegative = TRUE;
    BOOL bNegative = FALSE;
    if (score1 < 0) {
        score1 *= -1.0f;
        bNegative = TRUE;
    }
    if (score2 < 0) {
        score2 *= -1.0f;
        bNegative = TRUE;
    }
   
    // geometric mean
    interSim = pow(DOUBLE(score1 * score2), 0.5);
    if (bNegative)
        interSim *= -1.0f;
    if (bBothNegative)
        interSim *= 2.0f;

    // non negative geo mean
    //interSim = pow(DOUBLE((score1 + 1.0f) * (score2 + 1.0f)), 0.5);

    // arithmetic mean
    //interSim = (score1 + score2) / 2;
	return TRUE;
}

//---------------------------------------------------------------------------
// Calculate the socre of pCluster1 against pCluster2's frequent 1-itemsets.
// It is possible that the returned score is < 0.
//---------------------------------------------------------------------------
BOOL CDCClusterMgr::calculateInterScore(CDCCluster* pCluster1, CDCCluster* pCluster2, FLOAT& interScore)
{
    interScore = 0.0f;    

    // initialize the occurences1 
	CDCDocVector occurences1;	
    CDCDocVector* pTmpOccur = NULL;
	pCluster1->getOccurences(pTmpOccur);
    ASSERT(pTmpOccur);
	occurences1.SetSize(pTmpOccur->GetSize());

    // compute occurences of cluster1 including its children
    if (!pCluster1->computeTreeChildrenOccurences(occurences1)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // caluclate the score against cluster2
    if (!calClusterScoreV1(&m_clusterWH, &occurences1, pCluster2, interScore)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCClusterMgr::makeClusters(const CDCDocuments* pAllDocs,
                                 const CDCFreqItemsets* pGlobalFreqItemsets,
                                 FLOAT clusterSupport)
{
    if (!pAllDocs) {
        ASSERT(FALSE);
        return FALSE;
    }
    if (!pGlobalFreqItemsets || pGlobalFreqItemsets->IsEmpty()) {
        ASSERT(FALSE);
        return FALSE;
    }
    m_pAllDocs = pAllDocs;
	m_clusterSupport = clusterSupport;

    // Construct the cluster warehouse
    DEBUGPrint("*** Adding clusters to warehouse\n");
    if (!m_clusterWH.addClusters(pGlobalFreqItemsets)) {
        ASSERT(FALSE);
        return FALSE;
    }
    DEBUGPrint(_T("%d clusters are constructed\n"), pGlobalFreqItemsets->GetCount());
/*
    // Build potential children net
    DEBUGPrint("*** Building potential children net\n");
    if (!buildPotentialChildrenNet()) {
        ASSERT(FALSE);
        return FALSE;
    }
*/
    // Assign documents to cluster (initial clustering)
    DEBUGPrint("*** Constructing initial clusters\n");
    if (!constructInitialClusters()) {
        ASSERT(FALSE);
        return FALSE;
    }

    // Compute the frequent 1-itemsets for each cluster
    // Maintain the cluster support for each frequent item in the cluster
    DEBUGPrint("*** Computing frequent one itemsets for initial clusters\n");
#pragma message("*** Ben: We shouldn't include the children, but setting it to FALSE would lower the FMeasure???")
    if (!computeFreqOneItemsets(FALSE, clusterSupport)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // Clear all the documents in all the clusters in the Warehouse
    if (!removeAllDocs()) {
        ASSERT(FALSE);
        return FALSE;
    }

    // Assign each document to one cluster based on score
    DEBUGPrint("*** Constructing clusters based on scores\n");
    if (!constructScoreClusters()) {
        ASSERT(FALSE);
        return FALSE;
    }    

    // Re-assignment
    if (FALSE) {
        // Recompute the frequent 1-itemests for each cluster
        DEBUGPrint("*** Computing frequent one itemsets for clusters\n");
        if (!computeFreqOneItemsets(TRUE, clusterSupport)) {
            ASSERT(FALSE);
            return FALSE;
        }

        // Clear all the documents in all the clusters in the Warehouse        
        if (!removeAllDocs()) {
            ASSERT(FALSE);
            return FALSE;
        }

        // Re-assign each document to one cluster based on score
        DEBUGPrint("*** Re-assignment: Constructing clusters based on scores\n");
        if (!constructScoreClusters()) {
            ASSERT(FALSE);
            return FALSE;
        } 
    }

    // Recompute the frequent 1-itemests for each cluster
    DEBUGPrint("*** Computing frequent one itemsets for clusters\n");
    if (!computeFreqOneItemsets(TRUE, clusterSupport)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Build potential children network
//---------------------------------------------------------------------------
BOOL CDCClusterMgr::buildPotentialChildrenNet()
{
	CDCClusters allClusters;
	if(!m_clusterWH.getAllClustersReverseOrder(allClusters)) {
		ASSERT(FALSE);
		return FALSE;	
	}	

    int counter = 0;
	CDCCluster* pCluster = NULL;
    CDCClusterFreqItemset* pCoreItems = NULL;
    POSITION pos = allClusters.GetHeadPosition();
	while (pos != NULL) {
		pCluster = allClusters.GetNext(pos);
        P_ASSERT(pCluster);	
		pCluster->getClusterCoreItems(pCoreItems);

        // find all potential parents
        CDCClusters potentialParents;
		if (!m_clusterWH.findPotentialParents(FALSE, pCoreItems, potentialParents)) {
			ASSERT(FALSE);
			return FALSE;
		}

        // register this cluster as a child
        CDCCluster* pPotentialParent = NULL;
        POSITION parentPos = potentialParents.GetHeadPosition();
        while (parentPos != NULL) {
            pPotentialParent = potentialParents.GetNext(parentPos);
            P_ASSERT(pPotentialParent);
            if (!pPotentialParent->addPotentialChild(pCluster)) {
                ASSERT(FALSE);
                return FALSE;
            }
        }
#ifdef DC_SHOW_PROGRESS
		DEBUGPrint(_T("%c%d\r"), runningChars[counter % NUM_RUNNING_CHARS], counter++);
#endif
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Construct the initial clusters based on the presented frequent items in
// the document
//---------------------------------------------------------------------------
BOOL CDCClusterMgr::constructInitialClusters()
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

#pragma message("*** Ben: Performance can be slightly increased if we cache the coveredClusters in DCDocument")
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
// Construct clusters based on score function
//---------------------------------------------------------------------------
BOOL CDCClusterMgr::constructScoreClusters()
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

        // get all clusters that can cover this doc
        CDCClusters coveredClusters;
        if (!m_clusterWH.findCoveredClusters(&presentItems, coveredClusters)) {
            ASSERT(FALSE);
            presentItems.cleanup();
            return FALSE;
        }
        ASSERT(!coveredClusters.IsEmpty());
        presentItems.cleanup();

        // get the highest score cluster
        CDCCluster* pHighScoreCluster = NULL;
        if (!getHighestScoreCluster(&m_clusterWH, pDocVector, &coveredClusters, pHighScoreCluster)) {
            ASSERT(FALSE);
            return FALSE;
        }
        ASSERT(pHighScoreCluster);

        // assign doc to all the target cluster        
        if (!pHighScoreCluster->addDocument(pDoc)) {
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
// Compute the frequent 1-itemsets for each cluster.
// If bIncludePotentialChildren == TRUE, then include all its potential children;
// otherwise, compute the frequent 1-itemset for each cluster individually.
//---------------------------------------------------------------------------
BOOL CDCClusterMgr::computeFreqOneItemsets(BOOL bIncludePotentialChildren, FLOAT clusterSupport)
{    
    CDCClusters* pAllClusters = NULL;
    if (!m_clusterWH.getAllClusters(pAllClusters)) {
        ASSERT(FALSE);
        return FALSE;
    }
    
    CDCDocVector* pFrequencies = NULL;
    CDCCluster* pCluster = NULL;
    int clusterIdx = 0;
    int nClusters = pAllClusters->GetCount();
    POSITION pos = pAllClusters->GetHeadPosition();
    while (pos != NULL) {
        // get item frequencies in this cluster
        pCluster = pAllClusters->GetNext(pos);
        pCluster->getFrequencies(pFrequencies);

        CDCDocVector domainFrequencies;
        domainFrequencies.Append(*pFrequencies);

        // number of documents in this cluster and its children
        int numDocs = pCluster->getNumDocuments();

        //CDCDebug::printDocVector(domainFrequencies);
        if (bIncludePotentialChildren) {
#if 1
            // core items of this cluster
            CDCClusterFreqItemset* pCoreItems;
            pCluster->getClusterCoreItems(pCoreItems);

            if (!computePotentialChildrenFrequencies(pCoreItems, domainFrequencies, numDocs)) {
#else
            // add up potential children frequencies
            if (!computePotentialChildrenFrequencies(pCluster, domainFrequencies, numDocs)) {
#endif
                ASSERT(FALSE);
                return FALSE;
            }
        }

        // compute the frequent 1-itemsets for this cluster based
        // on this domain frequencies
        if (!pCluster->calFreqOneItemsets(&domainFrequencies, numDocs, clusterSupport)) {
            ASSERT(FALSE);
            return FALSE;
        }           
#ifdef DC_SHOW_PROGRESS
        printf("%c:%d\r", runningChars[(clusterIdx) % NUM_RUNNING_CHARS], clusterIdx++);
#endif
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Compute the potential children frequencies
// input: resultFrequencies should contain the parent node frequencies
// input: numDocs should contains number of documents in the parent node
// output: resultFrequencies will contain the result of adding up all frequencies 
// from all potential children AND the parent.
// output: numDocs will contain the total number documents in parent and children
//---------------------------------------------------------------------------
BOOL CDCClusterMgr::computePotentialChildrenFrequencies(const CDCFreqItemset* pCoreItems, 
                                                        CDCDocVector& resultFrequencies,
                                                        int& numDocs)
{
    ASSERT(pCoreItems);
    // get all the children clusters
    CDCClusters childrenClusters;
    if (!m_clusterWH.findPotentialChildren(pCoreItems, childrenClusters)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // add up all frequencies in these children
    if (!addUpClusterFrequencies(&childrenClusters, resultFrequencies, numDocs)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Compute the potential children frequencies
// input: resultFrequencies should contain the parent node frequencies
// input: numDocs should contains number of documents in the parent node
// output: resultFrequencies will contain the result of adding up all frequencies 
// from all potential children AND the parent.
// output: numDocs will contain the total number documents in parent and children
//---------------------------------------------------------------------------
BOOL CDCClusterMgr::computePotentialChildrenFrequencies(CDCCluster* pCluster, 
                                                        CDCDocVector& resultFrequencies,
                                                        int& numDocs)
{
    ASSERT(pCluster);

    // get all the children clusters
    CDCClusters* pChildrenClusters = NULL;
    pCluster->getPotentialChildren(pChildrenClusters);
    P_ASSERT(pChildrenClusters);

    // add up all frequencies in these children
    if (!addUpClusterFrequencies(pChildrenClusters, resultFrequencies, numDocs)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // go through each child
    CDCCluster* pChildCluster = NULL;
    POSITION pos = pChildrenClusters->GetHeadPosition();
    while (pos != NULL) {
        pChildCluster = pChildrenClusters->GetNext(pos);
        P_ASSERT(pChildCluster);
        if (!computePotentialChildrenFrequencies(pChildCluster, resultFrequencies, numDocs)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Compute the tree children frequencies
// input: resultFrequencies should contain the parent node frequencies
// input: numDocs should contains number of documents in the parent node
// output: resultFrequencies will contain the result of adding up all frequencies 
// from all potential children AND the parent.
// output: numDocs will contain the total number documents in parent and children
//---------------------------------------------------------------------------
BOOL CDCClusterMgr::computeTreeChildrenFrequencies(CDCCluster* pCluster, 
                                                CDCDocVector& resultFrequencies,
                                                int& numDocs)
{
    ASSERT(pCluster);

    // get all the children clusters
    CDCClusters* pChildrenClusters = NULL;
    pCluster->getTreeChildren(pChildrenClusters);
    P_ASSERT(pChildrenClusters);

    // add up all frequencies in these children
    if (!addUpClusterFrequencies(pChildrenClusters, resultFrequencies, numDocs)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // go through each child
    CDCCluster* pChildCluster = NULL;
    POSITION pos = pChildrenClusters->GetHeadPosition();
    while (pos != NULL) {
        pChildCluster = pChildrenClusters->GetNext(pos);
        P_ASSERT(pChildCluster);
        if (!computePotentialChildrenFrequencies(pChildCluster, resultFrequencies, numDocs)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}


//---------------------------------------------------------------------------
// Remove all the documents in the Cluster Warehouse
//---------------------------------------------------------------------------
BOOL CDCClusterMgr::removeAllDocs()
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
// Assign the document to the given clusters.
//---------------------------------------------------------------------------
BOOL CDCClusterMgr::assignDocToClusters(CDCDocument* pDoc, CDCClusters* pClusters)
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
// Add up the clusters' frequencies, store result in resultVector
// Given a list of clusters, add up all of their frequencies.
//---------------------------------------------------------------------------
BOOL CDCClusterMgr::addUpClusterFrequencies(CDCClusters* pClusters, 
                                            CDCDocVector& resultVector,
                                            int& numDocs)
{
    ASSERT(pClusters);
    CDCCluster* pCluster = NULL;
    CDCDocVector* pFrequencies = NULL;
    POSITION pos = pClusters->GetHeadPosition();
    while (pos != NULL) {
        pCluster = pClusters->GetNext(pos);
        ASSERT(pCluster);

        pCluster->getFrequencies(pFrequencies);
        if (!resultVector.addUp(pFrequencies)) {
            ASSERT(FALSE);
            return FALSE;
        }

        numDocs += pCluster->getNumDocuments();
    }    
    return TRUE;
}

//---------------------------------------------------------------------------
// Given a list of potential clusters, get the highest score cluster that
// suits the given DocVector.  Note: DocVector can be either a vector for
// a document or a vector (frequencies) for the whole cluster.
//
// Why does this function need a pointer to the clusterWH? Because we need
// that in the score calcuation function.  See the comments in score function.
//---------------------------------------------------------------------------
BOOL CDCClusterMgr::getHighestScoreCluster(const CDCClusterWH* pClusterWH,
                                           const CDCDocVector* pDocVector,
                                           const CDCClusters* pPotentialClusters, 
                                           CDCCluster*& pTargetCluster)
{
    ASSERT(pClusterWH && pDocVector && pPotentialClusters);
    pTargetCluster = NULL;

    // Scan through each cluster
    FLOAT maxScore = -FLT_MAX;
    FLOAT score = -FLT_MAX;
    CDCCluster* pCluster = NULL;
    POSITION pos = pPotentialClusters->GetHeadPosition();
    while (pos != NULL) {        
        pCluster = pPotentialClusters->GetNext(pos);
        ASSERT(pCluster);

        // compute the score
        if (!calClusterScoreV1(pClusterWH, pDocVector, pCluster, score)) {
            ASSERT(FALSE);
            return FALSE;
        }
        if (score > maxScore) {
            pTargetCluster = pCluster;
            maxScore = score;
        }
    }
    ASSERT(pTargetCluster);
    return TRUE;
}

//---------------------------------------------------------------------------
// Calculate the normalized score
//---------------------------------------------------------------------------
BOOL CDCClusterMgr::calNormalizedScore(const CDCClusterWH* pClusterWH,
                                       const CDCDocVector* pDocVector,
                                       CDCCluster* pCluster,
                                       FLOAT& normScore)
{
    normScore = 0.0f;
    UINT sumOccurences = pDocVector->getSum();
    if (sumOccurences == 0)
        return TRUE;

    FLOAT score = 0.0f;
    if (!calClusterScoreV1(pClusterWH, pDocVector, pCluster, score)) {
        ASSERT(FALSE);
        return FALSE;
    }
    normScore = score / sumOccurences;
    if (normScore > 1.0f) {        
        ASSERT(FALSE);
        normScore = 0.0f;
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Calculate the score of a doc against a cluster.
// Version 1: Score = ClusterFreqItems - ClusterNonFreqItems
//
// Why does this function need a pointer to the clusterWH? Because we need
// to retrieve the GlobalSupport for the non-frequent item from WH.
//
// The reason that we store the GlobalSupport in the WH is just for efficiency
// and completeness.  ClusterMgr does not need to deal with the DocMgr.
//---------------------------------------------------------------------------
BOOL CDCClusterMgr::calClusterScoreV1(const CDCClusterWH* pClusterWH,
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
    FLOAT infreqSup = -1.0f, clusterSup = -1.0f;
    UINT frequency = 0;
    int nItems = pDocVector->GetSize();    
    for (int id = 0; id < nItems; ++id) {
        frequency = (*pDocVector)[id];
        if (frequency == 0)
            continue;

        pFreqItem = coreFreqItemset.getFreqItem(id);
        if (pFreqItem != NULL) {
            // add score --> n(x) * ClusterSupport(x)
            clusterSup = pFreqItem->getClusterSupport();
            if (clusterSup < 0.0f || clusterSup > 1.0f) {
                ASSERT(FALSE);
                return FALSE;
            }
            score += frequency * clusterSup;
        }
        else {
            // deduct score --> n(x') * GlobalSupport(x')
            infreqSup = pClusterWH->getFrequentItemGlobalSupport(id);
            if (infreqSup < 0.0f || infreqSup > 1.0f) {
                ASSERT(FALSE);
                return FALSE;
            }
            score -= frequency * infreqSup;
        }
    }
    return TRUE;
}
