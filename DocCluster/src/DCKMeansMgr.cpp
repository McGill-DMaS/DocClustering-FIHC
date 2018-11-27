// DCKMeansMgr.cpp: implementation of the CDCKMeansMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <math.h>

#if !defined(DCKMEANSMGR_H)
    #include "DCKMeansMgr.h"
#endif

#if !defined(DCKMVECTOR_H)
    #include "DCKMVector.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCKMeansMgr::CDCKMeansMgr()
{
    if (!initialize())
        ASSERT(FALSE);
}

CDCKMeansMgr::~CDCKMeansMgr()
{

}

BOOL CDCKMeansMgr::initialize()
{
    m_nDimensions = 0;
	return TRUE;
}

void CDCKMeansMgr::cleanup()
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCKMeansMgr::makeClusters(int kClusters, CDCDocuments* pAllDocs)
{	    
    CDCClusters* pClusters = NULL;
    m_root.getTreeChildren(pClusters);

    P_ASSERT(pAllDocs);
    P_ASSERT(pClusters);
    if (kClusters <= 0) {
        ASSERT(FALSE);
        return FALSE;
    }

    int nDocs = pAllDocs->GetSize();
    if (nDocs <= 0) {
        ASSERT(FALSE);
        return FALSE;
    }

    // get dimension size
    CDCDocVector* pFirstVector = NULL;
    pAllDocs->GetAt(0)->getDocVector(pFirstVector);
    m_nDimensions = pFirstVector->GetSize();
    if (m_nDimensions <= 0) {
        ASSERT(FALSE);
        return FALSE;
    }

    // normailize all document vectors
    DEBUGPrint(_T("*** Normalizing vectors\n"));
    if (!normalizeKMVectors(pAllDocs)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // construct initial clusters
    DEBUGPrint(_T("*** Constructing initial clusters\n"));
    if (!constructInitialClusters(kClusters, pAllDocs, pClusters)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // compute their centroids
    DEBUGPrint(_T("*** Computing centroids for initial clusters\n"));
    if (!computeCentroids(pClusters)) {
        ASSERT(FALSE);
        pClusters->cleanup();
        return FALSE;
    }

    // initialize previous centroids for later comparison
    CDCKMVectors prevCentroids;
    for (int c = 0; c < kClusters; ++c) {
        prevCentroids.Add(CDCKMVector::createVector(m_nDimensions));
    }

    // reassign the documents to clusters until the centroids converge
    DEBUGPrint(_T("*** K-Means iterations\n"));
    int loopCounter = 0;
    while (isCentroidChanged(pClusters, &prevCentroids)) {
        if (!removeDocsFromClusters(pClusters)) {
            ASSERT(FALSE);
            prevCentroids.cleanup();
            pClusters->cleanup();
            return FALSE;
        }

        //DEBUGPrint(_T("*** Assigning documents to clusters\r"));
        if (!assignDocsToClusters(pClusters, pAllDocs)) {
            ASSERT(FALSE);
            prevCentroids.cleanup();
            pClusters->cleanup();
            return FALSE;
        }

        if (!copyCentroids(&prevCentroids, pClusters)) {
            ASSERT(FALSE);
            prevCentroids.cleanup();
            pClusters->cleanup();
            return FALSE;
        }

        //DEBUGPrint(_T("*** Recomputing clusters' centroids\r"));
        if (!computeCentroids(pClusters)) {
            ASSERT(FALSE);
            prevCentroids.cleanup();
            pClusters->cleanup();
            return FALSE;
        }
#ifdef DC_SHOW_PROGRESS
        DEBUGPrint(_T("K-Means iteration #%d\r"), loopCounter++);
#endif
    }
    DEBUGPrint(_T("\nAll iterations done\n"));
    prevCentroids.cleanup();
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCKMeansMgr::normalizeKMVectors(CDCDocuments* pDocs)
{
    if (!calIDF(pDocs)) {
        ASSERT(FALSE);
        return FALSE;
    }

    CDCDocument* pDoc = NULL;
    CDCKMVector* pKMVector = NULL;
    int nDocs = pDocs->GetSize();
    for (int i = 0; i < nDocs; ++i) {
        pDoc = pDocs->GetAt(i);
        P_ASSERT(pDoc);
        if (!pDoc->normalize(&m_idf)) {
        //if (!pDoc->normalize(NULL)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCKMeansMgr::calIDF(CDCDocuments* pDocs)
{
    if (m_idf.GetSize() == 0) {
        if (!setupIDF(m_nDimensions)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }    
    
    // calculate term frequencies
    CDCDocument* pDoc = NULL;
    CDCDocVector* pDocVector = NULL;
    int nDocs = pDocs->GetSize();    
    for (int i = 0; i < nDocs; ++i) {
        pDoc = pDocs->GetAt(i);
        P_ASSERT(pDoc);
        pDoc->getDocVector(pDocVector);
        P_ASSERT(pDocVector);

        for (int t = 0; t < m_nDimensions; ++t) {
            if ((*pDocVector)[t] > 0)
                ++m_idf[t];
        }
    }

    // calculate IDF
    FLOAT log2 = log10(2);
    FLOAT logNDocs = log10(nDocs) / log2;
    for (int t = 0; t < m_nDimensions; ++t) {
        m_idf[t] = logNDocs - log10(m_idf[t] + 1) / log2;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCKMeansMgr::setupIDF(int size)
{
    m_idf.SetSize(size);
    for (int i = 0; i < size; ++i)
        m_idf[i] = 0.0f;
    return TRUE;
}

//---------------------------------------------------------------------------
// Caller is responsible to deallocate the clusters.
//---------------------------------------------------------------------------
BOOL CDCKMeansMgr::constructInitialClusters(int kClusters, CDCDocuments* pAllDocs, CDCClusters* pClusters)
{
    P_ASSERT(pAllDocs);
    P_ASSERT(pClusters);

    for (int c = 0; c < kClusters; ++c) {
        pClusters->AddTail(new CDCCluster());
    }

    CDCDocument* pDoc = NULL;
    int nDocs = pAllDocs->GetSize();
    for (int i = 0; i < nDocs; ++i) {
        pDoc = pAllDocs->GetAt(i);
        P_ASSERT(pDoc);

        // Find the appriopriate cluster to add
        POSITION pos = pClusters->FindIndex(i % kClusters);
        CDCCluster* pCluster = pClusters->GetAt(pos);
        P_ASSERT(pCluster);
        if (!pCluster->addDocument(pDoc)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Returns TRUE if at least one of the centroids is changed.
//---------------------------------------------------------------------------
BOOL CDCKMeansMgr::isCentroidChanged(const CDCClusters* pClusters, CDCKMVectors* pCentroids)
{
    int nCentroids = pCentroids->GetSize();
    int nClusters = pClusters->GetCount();
    if (nCentroids != nClusters) {
        ASSERT(FALSE);
        return FALSE;
    }

    int c = 0;
    CDCCluster* pCluster = NULL;
    CDCKMVector* pClusterCentroid = NULL;
    CDCKMVector* pCentroid = NULL;
    POSITION pos = pClusters->GetHeadPosition();
    while (pos != NULL) {
        // get cluster's centroid
        pCluster = pClusters->GetNext(pos);
        P_ASSERT(pCluster);
        pCluster->getCentroid(pClusterCentroid);
        P_ASSERT(pClusterCentroid);

        // the KM vector
        pCentroid = pCentroids->GetAt(c++);
        P_ASSERT(pCentroid);

        if (!pClusterCentroid->isEqual(pCentroid))
            return TRUE;
    }
    return FALSE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCKMeansMgr::removeDocsFromClusters(CDCClusters* pClusters)
{
    CDCCluster* pCluster = NULL;
    POSITION pos = pClusters->GetHeadPosition();
    while (pos != NULL) {
        pCluster = pClusters->GetNext(pos);
        P_ASSERT(pCluster);

        pCluster->removeAllDocs();
    }

    m_root.removeAllDocs();
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCKMeansMgr::assignDocsToClusters(CDCClusters* pClusters, CDCDocuments* pAllDocs)
{    
    CDCDocument* pDoc = NULL;
    int nDocs = pAllDocs->GetSize();
    for (int i = 0; i < nDocs; ++i) {
        pDoc = pAllDocs->GetAt(i);
        P_ASSERT(pDoc);

        CDCCluster* pClosestCluster = NULL;
        if (!findClosestCluster(pClusters, pDoc, pClosestCluster)) {
            ASSERT(FALSE);
            return FALSE;
        }
        if (!pClosestCluster) {            
            if (!m_root.addDocument(pDoc)) {
                ASSERT(FALSE);
                return FALSE;
            }
            continue;
        }

        if (!pClosestCluster->addDocument(pDoc)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCKMeansMgr::findClosestCluster(CDCClusters* pClusters, CDCDocument* pDoc, CDCCluster*& pClosestCluster)
{
    P_ASSERT(pDoc);
    CDCKMVector* pKMVector = NULL;
    pDoc->getKMVector(pKMVector);
    P_ASSERT(pKMVector);

    FLOAT cos = 0.0f;
    FLOAT closest = 0.0f;
    CDCCluster* pCluster = NULL;
    CDCKMVector* pCentroid = NULL;
    POSITION pos = pClusters->GetHeadPosition();
    while (pos != NULL) {
        pCluster = pClusters->GetNext(pos);
        P_ASSERT(pCluster);

        pCluster->getCentroid(pCentroid);
        if (!CDCKMVector::cosine(pKMVector, pCentroid, cos)) {
            ASSERT(FALSE);
            return FALSE;
        }

        if (cos > closest) {
            closest = cos;
            pClosestCluster = pCluster;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCKMeansMgr::copyCentroids(CDCKMVectors* pCentroids, CDCClusters* pClusters)
{
    int nCentroids = pCentroids->GetSize();
    int nClusters = pClusters->GetCount();
    if (nCentroids != nClusters) {
        ASSERT(FALSE);
        return FALSE;
    }

    int c = 0;
    CDCCluster* pCluster = NULL;
    CDCKMVector* pClusterCentroid = NULL;
    CDCKMVector* pDestCentroid = NULL;
    POSITION pos = pClusters->GetHeadPosition();
    while (pos != NULL) {
        // get cluster's centroid
        pCluster = pClusters->GetNext(pos);
        P_ASSERT(pCluster);
        pCluster->getCentroid(pClusterCentroid);
        P_ASSERT(pClusterCentroid);

        // the document KM vector
        pDestCentroid = pCentroids->GetAt(c++);
        P_ASSERT(pDestCentroid);

        if (!pDestCentroid->copyFrom(pClusterCentroid)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Recompute the centroids of each cluster
//---------------------------------------------------------------------------
BOOL CDCKMeansMgr::computeCentroids(CDCClusters* pClusters)
{
    CDCCluster* pCluster = NULL;
    POSITION pos = pClusters->GetHeadPosition();
    while (pos != NULL) {
        pCluster = pClusters->GetNext(pos);
        P_ASSERT(pCluster);

        if (!pCluster->computeCentroid(m_nDimensions)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}
