// DCKMeansMgr.h: interface for the CDCKMeansMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCKMEANSMGR_H)
#define DCKMEANSMGR_H

#if !defined (DCDOCUMENT_H)
    #include "DCDocument.h"
#endif

#if !defined(DCCLUSTER_H)
    #include "DCCluster.h"
#endif

#if !defined(DCDOCVECTOR_H)
    #include "DCDocVector.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDCKMeansMgr  
{
public:
	CDCKMeansMgr();
	virtual ~CDCKMeansMgr();
    BOOL initialize();
    void cleanup();

// operations
    BOOL makeClusters(int kClusters, CDCDocuments* pAllDocs);
    void getRoot(CDCCluster*& pCluster) { pCluster = &m_root; };

protected:
    BOOL normalizeKMVectors(CDCDocuments* pDocs);
    BOOL calIDF(CDCDocuments* pDocs);
    BOOL setupIDF(int size);
    BOOL constructInitialClusters(int kClusters, CDCDocuments* pAllDocs, CDCClusters* pClusters);
    BOOL isCentroidChanged(const CDCClusters* pClusters, CDCKMVectors* pCentroids);
    BOOL removeDocsFromClusters(CDCClusters* pClusters);
    BOOL assignDocsToClusters(CDCClusters* pClusters, CDCDocuments* pAllDocs);
    BOOL findClosestCluster(CDCClusters* pClusters, CDCDocument* pDoc, CDCCluster*& pClosestCluster);
    BOOL copyCentroids(CDCKMVectors* pCentroids, CDCClusters* pClusters);
    BOOL computeCentroids(CDCClusters* pClusters);

// attributes
    int m_nDimensions;
    CDCCluster m_root;
    CDCKMVector m_idf;
};

#endif
