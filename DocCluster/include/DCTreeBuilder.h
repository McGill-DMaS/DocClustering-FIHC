// DCTreeBuilder.h: interface for the CDCTreeBuilder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCTREEBUILDER_H)
#define DCTREEBUILDER_H

#if !defined(DCCLUSTERMGR_H)
    #include "DCClusterMgr.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDCTreeBuilder  
{
public:
	CDCTreeBuilder();
	virtual ~CDCTreeBuilder();
    BOOL initialize(CDCClusterMgr* pClusterMgr);
    void cleanup();

// operations
    BOOL buildTree();
    BOOL calFreqOneItemsetsUsingTreeChildren();
    BOOL removeEmptyClusters(BOOL bRemoveInternal);
	BOOL pruneTree();
    BOOL pruneChildren();
    BOOL interSimPrune(int kClusters);
    BOOL simBasedPrune(int kClusters);
    BOOL interSimOverPrune(int kClusters);

	static double calculatePessimisticError(double err, double num);

protected:
    BOOL pruneChildren(CDCCluster* pParentCluster, FLOAT minInterSimThreshold, int& nPruned);
    BOOL pruneChildren(CDCCluster* pParentCluster, int& nPruned);
    BOOL mergeChildren(CDCCluster* pParentCluster, BOOL bMerParent, int kClusters, FLOAT minInterSimThreshold, int& mergedSiblingCount, int& mergedParentCount);
	BOOL cleanSimilarity(CDCCluster* pos, CDCCluster** aryPositions1, CDCCluster** aryPositions2, FLOAT* arySims, int length);
	int findClusterIndex(CDCCluster* pos1, CDCCluster** aryPositions1, CDCCluster* pos2, CDCCluster** aryPositions2, int length);
	int findHighestInterSimilarity(const FLOAT* aryInterSimilarities, int length, FLOAT minInterSimThreshold);

// attributes
    CDCClusterMgr* m_pClusterMgr;
};

#endif
