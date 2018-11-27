// DCClusterMgr2.h: interface for the CDCClusterMgr2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCCLUSTERMGR2_H)
#define DCCLUSTERMGR2_H

#if !defined (DCFREQITEM_H)
    #include "DCFreqItem.h"
#endif

#if !defined(DCCLUSTERWH_H)
    #include "DCClusterWH.h"
#endif

#if !defined(DCDEBUG_H)
	#include "DCDebug.h"
#endif

#include "float.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDCClusterMgr2  
{
public:
	
	CDCClusterMgr2();
	virtual ~CDCClusterMgr2();
	inline void setAllDocs(CDCDocuments* docs) { m_pAllDocs = docs; };
	inline void setClusterSupport(FLOAT support) { m_clusterSupport = support; };
	inline void getClusterWH(CDCClusterWH*& pClusterWH) { pClusterWH = &m_clusterWH; };
	BOOL produceClusters(const CDCFreqItemsets*, TDCClusterOrderType orderType, BOOL descOrAsc);
	void cleanUp();

protected:
	
	CDCClusterWH m_clusterWH;
    CDCDocuments* m_pAllDocs;
	FLOAT m_clusterSupport;			
	CDCClusters m_orderedClusters;

	BOOL initializeClusters();
	BOOL assignDocToClusters(CDCDocument* pDoc, CDCClusters* pClusters);
	BOOL orderAllClusters(TDCClusterOrderType, BOOL);
	BOOL reOrderAllClusters(TDCClusterOrderType, BOOL);
	BOOL reassignDocuments();
	BOOL removeAllDocsFromWH();
	CDCCluster* findLeftmostCoveredCluster(CDCFreqItemset*);
	void removeEmptyClusters();
	BOOL linearPruneV1();
	BOOL linearPruneV2();
	void constructTree();
	BOOL mergePruning2(int runs);
	CDCCluster* findParent(CDCCluster*, POSITION);
	BOOL pruneOrNot(CDCCluster*, CDCCluster*);
	BOOL calClusterScoreV1(const CDCClusterWH*, const CDCDocVector*, CDCCluster* , FLOAT&);
	double calculatePessimisticError(double, double);
	BOOL setSelfScoreForAllClusters(CDCClusters*);

	// copy from Leo
	BOOL calculateInterSimilarity(CDCCluster * pCluster1, CDCCluster * pCluster2, FLOAT & interSim);
	BOOL cleanSimilarity(CDCCluster* pos, CDCCluster **aryPositions1, CDCCluster**aryPositions2, FLOAT* arySims, const int length);
	int findClusterIndex(CDCCluster* pos1, CDCCluster** aryPositions1, CDCCluster* pos2, CDCCluster** aryPositions2, const int length);
	int findHighestInterSimilarity(const FLOAT *aryInterSimilarities, const int length);
};

#endif // !defined(DCCLUSTERMGR2_H)
