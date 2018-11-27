// DCCluster.h: interface for the DCCluster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCCLUSTER_H)
#define DCCLUSTER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined (DCDOCUMENT_H)
    #include "DCDocument.h"
#endif

#if !defined(DCDOCVECTOR_H)
    #include "DCDocVector.h"
#endif

#if !defined (DCFREQITEM_H)
    #include "DCFreqItem.h"
#endif

#if !defined(DCEVALCLASS_H)
    #include "DCEvalClass.h"
#endif

enum TDCClusterOrderType {
    ORDER_TYPE_GLOBAL_SUP = 100,
    ORDER_TYPE_COREITEMS_SIZE,
    ORDER_TYPE_SCORE,
    ORDER_TYPE_INTRA_SIMILARITY
};

//***************************************************************************

typedef enum cluster_status {UNTOUCHED, TOUCHED, PRUNED} CLUSTER_STATUS;

class CDCClusterFreqItem : public CDCFreqItem
{
public:
    CDCClusterFreqItem(TFreqItemID itemID, FLOAT clusterSupport);
    virtual ~CDCClusterFreqItem();

// operations
    virtual void makeCopy(CDCClusterFreqItem*& pCopy);
    inline FLOAT getClusterSupport() const { return m_clusterSupport; };
    inline void setClusterSupport(FLOAT clusterSupport) { m_clusterSupport = clusterSupport; };

protected:
    FLOAT m_clusterSupport;         // Cluster support of this frequent item
};

//***************************************************************************

class CDCClusterFreqItemset : public CDCFreqItemset
{
public:
    CDCClusterFreqItemset() {};
    virtual ~CDCClusterFreqItemset() {};
    BOOL makeCopy(CDCClusterFreqItemset& itemset);
    BOOL resetClusterSupports();
    inline CDCClusterFreqItem* GetNext(POSITION& pos) const { return (CDCClusterFreqItem*) CDCFreqItemset::GetNext(pos); };
    inline CDCClusterFreqItem* getFreqItem(TFreqItemID itemID) const { return (CDCClusterFreqItem*) CDCFreqItemset::getFreqItem(itemID); };
};

class CDCCluster;

//***************************************************************************
// A collection of clusters.  We may improve the efficieny of this class by
// making it as a tree, instead of a list.

typedef CTypedPtrList<CPtrList, CDCCluster*> CDCClusterPtrList;

class CDCClusters : public CDCClusterPtrList
{
public:
    CDCClusters();
    virtual ~CDCClusters();
    void cleanup();

// operations
    BOOL addClusters(CDCClusters* pNewClusters);
    BOOL addCluster(CDCCluster* pNewCluster);
    BOOL removeCluster(CDCCluster* cluster);
    BOOL findSubSetClusters(const CDCFreqItemset* pFreqItemset, CDCClusters& clusters) const;
    BOOL findSuperSetClusters(const CDCFreqItemset* pFreqItemset, CDCClusters& clusters) const;    
    BOOL retainKItemsets(int k);
    BOOL removeLargerThanKItemsets(int k);
	int getNumDocuments();
	BOOL getOrderedClusters(TDCClusterOrderType, CDCClusters*, BOOL);

protected:
    // linear clustering
	POSITION findInsertPosition(TDCClusterOrderType, CDCClusters*, CDCCluster*, BOOL);
	POSITION findInsertPos_GlbSup(CDCClusters*, CDCCluster*, BOOL);
	POSITION findInsertPos_CoreItemSize(CDCClusters*, CDCCluster*, BOOL);
	POSITION findInsertPos_Score(CDCClusters*, CDCCluster*, BOOL);
	POSITION findInsertPos_IntraSimilarity(CDCClusters*, CDCCluster*, BOOL);
};


//***************************************************************************

class CDCCluster  
{
public:

    CDCCluster() {m_status = UNTOUCHED; m_selfScore = 0;};
	CDCCluster(const CDCFreqItemset* pCoreItems);
	virtual ~CDCCluster();
    
// operations
    // documents
    BOOL addDocument(CDCDocument* pDocument);
    BOOL addDocuments(CDCDocuments* pDocuments);
    void removeAllDocs();
    BOOL getDocumentsIncludeTreeChildren(CDCDocuments& allDocs);
    inline void getDocuments(const CDCDocuments*& pDocs) { pDocs = &m_documents; };    
    inline int getNumDocuments() const { return m_documents.GetSize(); };    
    int getNumDocumentsIncludeTreeChildren() { return getNumDocumentsIncludeTreeChildren(this); };
    static int getNumDocumentsIncludeTreeChildren(const CDCCluster* pParentCluster);

    // cluster
    int getNumClustersIncludeTreeChildren() { return getNumClustersIncludeTreeChildren(this); };
    static int getNumClustersIncludeTreeChildren(const CDCCluster* pParentCluster);

    // frequencies
    inline void getFrequencies(const CDCDocVector*& pFrequencies) { pFrequencies = &m_frequencies; };

	//occuriencies
	inline void getOccurences(const CDCDocVector*& pOccurences) {pOccurences = & m_occurences; };
    
	// core items
    TFreqItemID getFirstCoreItemID();
    inline int getNumCoreItems() { return m_numCoreItems; };

    inline void getClusterCoreItems(CDCClusterFreqItemset*& pCoreItems) { pCoreItems = &m_coreItems; };
    inline FLOAT getCoreItemsGlobalSupport() const { return m_coreItems.getGlobalSupport(); };

    // core items and frequent items
    BOOL getClusterCoreFreqItems(CDCClusterFreqItemset& allItems);
    BOOL getClusterFreqItems(CDCClusterFreqItemset& freqItems);
    BOOL copyClusterFreqItems(CDCClusterFreqItemset& freqItems);
	void setClusterFreqItems(CDCClusterFreqItemset* pFreqItems);

    // frequent itemsets
    BOOL calFreqOneItemsets(const CDCDocVector* pDomainFrequencies, int numDocs, FLOAT clusterThreshold);

    // tree
    inline void setTreeParent(CDCCluster* pCluster) { ASSERT(pCluster); m_pTreeParent = pCluster; };
    inline void clearTreeParent() { m_pTreeParent = NULL; };
	inline void getTreeParent(CDCCluster*& pParent) {pParent = m_pTreeParent;}
    inline BOOL addTreeChild(CDCCluster* pCluster) { return m_treeChildren.addCluster(pCluster); };
    inline void getTreeChildren(CDCClusters*& pClusters) { pClusters = &m_treeChildren; };
    inline int getNumTreeChildren() { return m_treeChildren.GetCount(); };
    BOOL computeTreeChildrenOccurences(CDCDocVector& resultOccurences);
    BOOL computeTreeChildrenFrequencies(CDCDocVector& resultFrequencies);

    // potential children
    inline void getPotentialChildren(CDCClusters*& pChildren) { pChildren = &m_potentialChildren; };
    inline BOOL addPotentialChild(CDCCluster* pCluster) { return m_potentialChildren.addCluster(pCluster); };

    // debug
    CString toString() const;

	inline CLUSTER_STATUS getStatus() {return m_status;} const
	inline void setStatus(CLUSTER_STATUS status) {m_status = status; };
//	double getPessimisticError();
//	double getObservedError(CDCClusterWH * pClusterWH);
//	double getObservedErrorAfter(CDCClusterWH * pClusterWH);
	BOOL touchAllChildren();
	int pruneAllChildren();
    BOOL mergeCluster(CDCCluster* pCluster, CDCClusters* pAllClusters);
    BOOL mergeClusterPruneChildren(CDCCluster* pCluster, CDCClusters* pAllClusters);

    // K-Means
    inline void getCentroid(CDCKMVector*& pCentroid) { pCentroid = &m_centroid; };
    BOOL computeCentroid(int nDimensions);

    // Evaluation
    BOOL computeFMeasure(CDCEvalClass* pEvalClass, FLOAT& fm);
    BOOL computeIntraSimilarity(FLOAT& intraSim);
    BOOL computeEntropy(int nTotalDocs, FLOAT& totalEntropy);

	// linear clustering
    inline FLOAT calculateIntraSimilairity() { return (m_selfScore / m_occurences.getSum()); };
	inline float getSelfScore() { return m_selfScore; };
	inline void setSelfScore(FLOAT score) { m_selfScore = score; };

protected:
    BOOL setupFrequencies(int size);
    BOOL setupOccurences(int size);
    BOOL setupCentroid(int size);
    BOOL updateFrequencies(const CDCDocVector* pDocVector);
    BOOL updateOccurences(const CDCDocVector* pDocVector);

// attributes
    int m_numCoreItems;                 // Cache it for faster access
    CDCClusterFreqItemset m_coreItems;  // These frequent items must have 100% support
                                        // Every document in this cluster must contain
                                        // all of these core items.
    CDCClusterFreqItemset m_freqItems;  // These are the frequent items in this cluster,
                                        // but some document may not have these items.
    CDCDocuments m_documents;           // All documents in this cluster.
    CDCDocVector m_frequencies;         // # of documents in this cluster contains this frequen item (for computing frequent 1-itemsets)
    CDCDocVector m_occurences;          // # of occurences of this frequent item in this cluster
    
    CDCClusters m_treeChildren;         // list of all children clusters
    CDCCluster* m_pTreeParent;          // the parent cluster
    CDCClusters m_potentialChildren;    // list of all potential children clusters

	CLUSTER_STATUS m_status;			// Status of the cluster for pruning
	FLOAT m_selfScore;					// the score of the cluster against itself

    // K-Means
    CDCKMVector m_centroid;             // centroid of the clusters
};

#endif
