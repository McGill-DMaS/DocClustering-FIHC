// DCClusterWH.h: interface for the DCCluster Warehouse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCCLUSTERWH_H)
#define DCCLUSTERWH_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(DCCLUSTER_H)
    #include "DCCluster.h"
#endif

#if !defined (DCFREQITEM_H)
    #include "DCFreqItem.h"
#endif

#if !defined (DCDOCUMENT_H)
    #include "DCDocument.h"
#endif

extern char runningChars[];

//***************************************************************************

typedef CTypedPtrMap<CMapWordToPtr, TFreqItemID, CDCClusters*> CDCItemIDToClustersPtrMap;

class CDCClusterWH
{
public:
    CDCClusterWH();
    virtual ~CDCClusterWH();
    
// operations
    BOOL addClusters(const CDCFreqItemsets* pFreqItemsets);
    BOOL addCluster(const CDCFreqItemset* pFreqItemset);
    BOOL getAllClusters(CDCClusters*& pAllClusters);
    BOOL getAllClustersReverseOrder(CDCClusters& allClusters);
    BOOL getOneItemClusters(CDCClusters& oneItemClusters);
    BOOL getTreeRoot(CDCCluster*& pRoot) { pRoot = &m_treeRoot; return TRUE; };

    FLOAT getFrequentItemGlobalSupport(int itemID) const;

    BOOL findCoveredClusters(CDCFreqItemset* pFreqItemset, CDCClusters& clusters) const;
    BOOL findPotentialParents(BOOL bIncludeGreatGrandParents, CDCFreqItemset* pFreqItemset, CDCClusters& clusters) const;
    BOOL findPotentialChildren(const CDCFreqItemset* pFreqItemset, CDCClusters& clusters) const;

    inline BOOL addDanglingDoc(CDCDocument* pDoc) { return m_treeRoot.addDocument(pDoc); };
    inline void getDanglingDocs(CDCDocuments*& pDocs) { m_treeRoot.getDocuments(pDocs); };
    inline void clearDanglingDocs() { m_treeRoot.removeAllDocs(); };

protected:
    CDCItemIDToClustersPtrMap m_clustersMap;
    CDCClusters m_allClusters;      // all clusters in this warehouse
    CDCCluster m_treeRoot;          // this is the root of the tree
};

#endif
