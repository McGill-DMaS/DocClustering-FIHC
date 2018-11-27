// DCClusterMgr.h: interface for the CDCClusterMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCCLUSTERMGR_H)
#define DCCLUSTERMGR_H

#if !defined (DCFREQITEM_H)
    #include "DCFreqItem.h"
#endif

#if !defined(DCCLUSTERWH_H)
    #include "DCClusterWH.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDCClusterMgr  
{
public:
	CDCClusterMgr();
	virtual ~CDCClusterMgr();
    BOOL initialize();
    void cleanup();

// operations
    BOOL calculateInterSimilarity(CDCCluster*, CDCCluster*, FLOAT&);
    BOOL calculateInterScore(CDCCluster* pCluster1, CDCCluster* pCluster2, FLOAT& interScore);

    BOOL makeClusters(const CDCDocuments* pAllDocs, const CDCFreqItemsets* pGlobalFreqItemsets, FLOAT clusterSupport);
    inline void getClusterWH(CDCClusterWH*& pClusterWH) { pClusterWH = &m_clusterWH; };
	inline FLOAT getClusterSupport() { return m_clusterSupport; };

    // dangling document means a document that does not belong to any cluster.
    inline void getDanglingDocs(CDCDocuments*& pDocs) { m_clusterWH.getDanglingDocs(pDocs); };

    BOOL computeTreeChildrenFrequencies(CDCCluster* pCluster, CDCDocVector& resultFrequencies, int& numDocs);

// static operations
    static BOOL assignDocToClusters(CDCDocument* pDoc, CDCClusters* pClusters);
    static BOOL addUpClusterFrequencies(CDCClusters* pClusters, CDCDocVector& resultVector, int& numDocs);
    static BOOL getHighestScoreCluster(const CDCClusterWH* pClusterWH,
                                       const CDCDocVector* pDocVector,
                                       const CDCClusters* pPotentialClusters, 
                                       CDCCluster*& pTargetCluster);
    static BOOL calNormalizedScore(const CDCClusterWH* pClusterWH,
                                   const CDCDocVector* pDocVector,
                                   CDCCluster* pCluster,
                                   FLOAT& normScore);
    static BOOL calClusterScoreV1(const CDCClusterWH* pClusterWH,
                                  const CDCDocVector* pDocVector,
                                  CDCCluster* pCluster,
                                  FLOAT& score);

protected:
// operations
    BOOL buildPotentialChildrenNet();
    BOOL constructInitialClusters();
    BOOL constructScoreClusters();
    BOOL computeFreqOneItemsets(BOOL bIncludePotentialChildren, FLOAT clusterSupport);
    BOOL computePotentialChildrenFrequencies(const CDCFreqItemset* pCoreItems, CDCDocVector& resultFrequencies, int& numDocs);
    BOOL computePotentialChildrenFrequencies(CDCCluster* pCluster, CDCDocVector& resultFrequencies, int& numDocs);
    BOOL removeAllDocs();

// attributes
    CDCClusterWH m_clusterWH;
    const CDCDocuments* m_pAllDocs;
	FLOAT m_clusterSupport;
};

#endif
