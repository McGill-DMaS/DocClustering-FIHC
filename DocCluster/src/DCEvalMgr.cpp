// DCEvalMgr.cpp: implementation of the CDCEvalMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#if !defined(DCEVALMGR_H)
    #include "DCEvalMgr.h"
#endif

#if !defined(DCCLUSTERWH_H)
    #include "DCClusterWH.h"
#endif

#include <MATH.H>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCEvalMgr::CDCEvalMgr()
{
}

CDCEvalMgr::~CDCEvalMgr()
{

}

BOOL CDCEvalMgr::initialize(TDCAlgoMode algoMode, CDCDocMgr* pDocMgr, CDCClusterMgr* pClusterMgr)
{
    P_ASSERT(pDocMgr);
    m_pDocMgr = pDocMgr;
    m_pClusterMgr = pClusterMgr;
    m_algoMode = algoMode;
	return TRUE;
}

void CDCEvalMgr::cleanup()
{
}

//************
// F-Measure *
//************

//---------------------------------------------------------------------------
// Evaluate F-Measure
//---------------------------------------------------------------------------
BOOL CDCEvalMgr::evalFMeasure(CDCCluster* pRoot, FLOAT& fm)
{
    fm = 0.0f;
    CDCEvalClasses evalClasses;
    if (!findAllClasses(pRoot, evalClasses)) {
        ASSERT(FALSE);
        return FALSE;
    }
    int nDocs = evalClasses.getNumDocs();
    if (nDocs == 0)
        return TRUE;

    // go through each class
    CDCEvalClass* pEvalClass = NULL;
    int nClasses = evalClasses.GetSize();
    for (int i = 0; i < nClasses; ++i) {
        pEvalClass = evalClasses[i];
        ASSERT(pEvalClass);        

        // go through each child
        FLOAT maxFM = -1.0f;
        CDCCluster* pBestCluster = NULL;
        CDCClusters* pChildrenClusters = NULL;
        pRoot->getTreeChildren(pChildrenClusters);
        CDCCluster* pChild = NULL;
        POSITION pos = pChildrenClusters->GetHeadPosition();
        while (pos != NULL) {
            pChild = pChildrenClusters->GetNext(pos);
            P_ASSERT(pChild);

            // find the maximum F-Measure within this child branch
            FLOAT childFM = -1.0f;
            CDCCluster* pBestChildCluster = NULL;
            if (!findMaxFMeasure(pChild, pEvalClass, childFM, pBestChildCluster)) {
                ASSERT(FALSE);
                return FALSE;
            }

            if (childFM > maxFM) {
                maxFM = childFM;
                pBestCluster = pBestChildCluster;
            }
        }

        // FMeasure of this class
        fm += (FLOAT(pEvalClass->getNItems()) / nDocs) * maxFM;

        // Evaluate Cluster Frequent Items
        if (!evalCFI(pEvalClass, pBestCluster)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Find the maximum FMeasure from the cluster
//---------------------------------------------------------------------------
BOOL CDCEvalMgr::findMaxFMeasure(CDCCluster* pParent, CDCEvalClass* pEvalClass, FLOAT& maxFM, CDCCluster*& pBestCluster)
{
    FLOAT fm = 0.0f;
    if (!pParent->computeFMeasure(pEvalClass, fm)) {
        ASSERT(FALSE);
        return FALSE;
    }
    if (fm > maxFM) {
        maxFM = fm;
        pBestCluster = pParent;
    }

    // go through each child    
    CDCClusters* pChildrenClusters = NULL;
    pParent->getTreeChildren(pChildrenClusters);
    CDCCluster* pChild = NULL;
    POSITION pos = pChildrenClusters->GetHeadPosition();
    while (pos != NULL) {
        pChild = pChildrenClusters->GetNext(pos);
        P_ASSERT(pChild);

        if (!findMaxFMeasure(pChild, pEvalClass, maxFM, pBestCluster)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Count all the classes
//---------------------------------------------------------------------------
BOOL CDCEvalMgr::findAllClasses(CDCCluster* pRoot, CDCEvalClasses& evalClasses)
{
    CDCClusters* pChildrenClusters = NULL;
    pRoot->getTreeChildren(pChildrenClusters);

    int nDocs;
    CString className;
    CDCCluster* pChild = NULL;
    POSITION pos = pChildrenClusters->GetHeadPosition();
    while (pos != NULL) {
        pChild = pChildrenClusters->GetNext(pos);
        P_ASSERT(pChild);

        // number of documents in the cluster
        nDocs = pChild->getNumDocumentsIncludeTreeChildren();
        if (nDocs != 0) {
            CDCDocuments docs;
            pChild->getDocumentsIncludeTreeChildren(docs);

            // go through each document and increment the corresponding counter
            for (int i = 0; i < nDocs; ++i) {
                className = docs[i]->getClassName();
                if (!evalClasses.incEvalClass(className)) {
                    ASSERT(FALSE);
                    return FALSE;
                }
            }
        }
    }
    return TRUE;
}

//*********************
// Overall Similarity *
//*********************

//---------------------------------------------------------------------------
// Evaluate clusters using Overall Similarity.
// Weighted ||c||^2
//---------------------------------------------------------------------------
BOOL CDCEvalMgr::evalOverallSimilarity(CDCCluster* pRoot, FLOAT& overallSim)
{
    overallSim = 0.0f;
    P_ASSERT(pRoot);
    int nTotalDocs = m_pDocMgr->getNumDocs();

    CDCClusters* pChildrenClusters = NULL;
    pRoot->getTreeChildren(pChildrenClusters);

    // go through child cluster
    int nDocs = 0;
    FLOAT intraSim = 0.0f;
    CDCCluster* pChildCluster = NULL;
    POSITION pos = pChildrenClusters->GetHeadPosition();
    while (pos != NULL) {
        pChildCluster = pChildrenClusters->GetNext(pos);
        P_ASSERT(pChildCluster);

        // compute intra-cluster similarity
        if (!pChildCluster->computeIntraSimilarity(intraSim)) {
            ASSERT(FALSE);
            return FALSE;
        }

        // get number of documents in this cluster
        nDocs = pChildCluster->getNumDocumentsIncludeTreeChildren();
        overallSim += (FLOAT(nDocs) / nTotalDocs) * intraSim;
    }
    return TRUE;
}

//**********
// Entropy *
//**********

//---------------------------------------------------------------------------
// Evaluate entropy
//---------------------------------------------------------------------------
BOOL CDCEvalMgr::evalEntropyHierarchical(CDCCluster* pRoot, FLOAT& totalEntropy)
{
    int nTotalDocs = pRoot->getNumDocumentsIncludeTreeChildren();
    if (nTotalDocs == 0)
        return TRUE;

    CDCClusters* pChildrenClusters = NULL;
    pRoot->getTreeChildren(pChildrenClusters);

    // go through child cluster
    CDCCluster* pChildCluster = NULL;
    POSITION pos = pChildrenClusters->GetHeadPosition();
    while (pos != NULL) {
        pChildCluster = pChildrenClusters->GetNext(pos);
        P_ASSERT(pChildCluster);

        // number of documents in the cluster
        int nDocs = pChildCluster->getNumDocumentsIncludeTreeChildren();
        if (nDocs != 0) {
            CDCDocuments docs;
            pChildCluster->getDocumentsIncludeTreeChildren(docs);

            // go through each document and increment the corresponding counter
            CString className;
            CDCEvalClasses evalClasses;
            for (int i = 0; i < nDocs; ++i) {
                className = docs[i]->getClassName();
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

            totalEntropy += (FLOAT(nDocs) / nTotalDocs) * entropy;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Evaluate entropy
//---------------------------------------------------------------------------
BOOL CDCEvalMgr::evalEntropyFlat(CDCCluster* pRoot, FLOAT& totalEntropy)
{
    totalEntropy = 0.0f;
    P_ASSERT(pRoot);
    int nTotalDocs = pRoot->getNumDocumentsIncludeTreeChildren();
    if (nTotalDocs == 0)
        return TRUE;

    CDCClusters* pChildrenClusters = NULL;
    pRoot->getTreeChildren(pChildrenClusters);

    // go through child cluster
    CDCCluster* pChildCluster = NULL;
    POSITION pos = pChildrenClusters->GetHeadPosition();
    while (pos != NULL) {
        pChildCluster = pChildrenClusters->GetNext(pos);
        P_ASSERT(pChildCluster);

        // compute entropy
        if (!pChildCluster->computeEntropy(nTotalDocs, totalEntropy)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

//************************
// Cluster Freqent Items *
//************************

#define DC_EVAL_CFI_THRESHOLD 0.25f

//---------------------------------------------------------------------------
// Evaluate Cluster Frequent Items
//---------------------------------------------------------------------------
BOOL CDCEvalMgr::evalCFI(CDCEvalClass* pEvalClass, CDCCluster* pTargetCluster)
{  
    DEBUGPrint(_T("** Class: %s **\n"), pEvalClass->getClassName());

    DEBUGPrint(_T("Cluster Label:\n"));
    CDCClusterFreqItemset coreItems;
    if (!pTargetCluster->getClusterCoreFreqItems(coreItems))
        return FALSE;

    CDCClusterFreqItem* pFreqItem = NULL;
    POSITION pos = coreItems.GetHeadPosition();
    while (pos != NULL) {
        pFreqItem = coreItems.GetNext(pos);
        
        // get the word from its ID
        CString theWord;
        if (!m_pDocMgr->getFreqTermFromID(pFreqItem->getFreqItemID(), theWord)) {
            DEBUGPrint(_T("Error in getFreqTermFromID\n"));
            ASSERT(FALSE);
            return FALSE;
        }
        DEBUGPrint(_T("%s\t%f\n"), theWord, 1.0f);
    }

    DEBUGPrint(_T("Cluster Frequent Items:\n"));
    CDCDocVector sumVector;
    CDCDocVector* pFrequencies = NULL;
    pTargetCluster->getFrequencies(pFrequencies);
    int nDocs = pTargetCluster->getNumDocuments();    
    sumVector.Append(*pFrequencies);
    if (!m_pClusterMgr->computeTreeChildrenFrequencies(pTargetCluster, sumVector, nDocs)) {
        DEBUGPrint(_T("Error in computeTreeChildrenFrequencies\n"));
        ASSERT(FALSE);        
        return FALSE;
    }
    if (nDocs == 0)
        return TRUE;
    
    int minNumDocs = ceil(nDocs * DC_EVAL_CFI_THRESHOLD);    
    int nFrequencies = sumVector.GetSize();
    for (int i = 0; i < nFrequencies; ++i) {
        if (sumVector[i] >= minNumDocs) {  
            // this item passes the threshold

            // get the word from its ID
            CString theWord;
            if (!m_pDocMgr->getFreqTermFromID(i, theWord)) {
                DEBUGPrint(_T("Error in getFreqTermFromID\n"));
                ASSERT(FALSE);
                return FALSE;
            }
            DEBUGPrint(_T("%s\t%f\n"), theWord, FLOAT(sumVector[i]) / nDocs);
        }

    }
    return TRUE;
}
