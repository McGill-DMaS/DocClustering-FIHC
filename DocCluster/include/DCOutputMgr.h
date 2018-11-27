// DCOutput.h: interface for the CDCOutputMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCOUTPUTMGR_H)
#define DCOUTPUTMGR_H

#if !defined(DCDOCMGR_H)
    #include "DCDOCMgr.h"
#endif

#if !defined(DCCLUSTERMGR_H)
    #include "DCClusterMgr.h"
#endif

#if !defined(DCCLUSTERMGR2_H)
    #include "DCClusterMgr2.h"
#endif

#if !defined(DCCLUSTER_H)
    #include "DCCluster.h"
#endif

#if !defined(BFXMLPSER_H)
	#include "BFXmlPser.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDCOutputMgr
{
public:
	CDCOutputMgr();
	virtual ~CDCOutputMgr();
    BOOL initialize(TDCAlgoMode algoMode, CDCDocMgr* pDocMgr, CDCClusterMgr* pClusterMgr, CDCClusterMgr2* pClusterMgr2);
    void cleanup();

// operations
    BOOL produceOutput(LPCTSTR outFileName, FLOAT fMeasure, FLOAT overallSim, FLOAT entropyH, FLOAT entropyF);
    BOOL produceOutput(LPCTSTR outFileName, CDCCluster* pCluster, FLOAT fMeasure, FLOAT overallSim, FLOAT entropyH, FLOAT entropyF);

protected:
// operations
    BOOL writeTreeToXML(LPCTSTR outFileName, CDCCluster* pRootCluster, FLOAT fMeasure, FLOAT overallSim, FLOAT entropyH, FLOAT entropyF);
    BOOL putSubTree(CDCCluster* pParentCluster, CBFXmlNode* pParentXMLNode);

    BOOL writeClustersToXML(LPCTSTR outFileName, const CDCClusters* pClusters);
    BOOL putClustersList(const CDCClusters* pClusters, CBFXmlNode* pParentXMLNode);

    BOOL putClusterElement(CDCCluster* pCluster, CBFXmlNode* pXmlNode);
    BOOL putClusterAttributes(CDCCluster* pCluster, CBFXmlNode* pXmlNode);

    BOOL putClusterFreqItems(CDCCluster* pCluster, CBFXmlNode* pXmlNode);   
    BOOL putClusterFreqItemsAttributes(CDCFreqItemset* pFreqItemset, CBFXmlNode* pXmlNode);

    BOOL putClusterFreqItem(CDCClusterFreqItem* pFreqItem, CBFXmlNode* pXmlNode);
    BOOL putClusterFreqItemAttributes(CDCClusterFreqItem* pFreqItem, CBFXmlNode* pXmlNode);

    BOOL putDocuments(CDCCluster* pCluster, CBFXmlNode* pXmlNode);
    BOOL putDocumentsAttributes(CDCDocuments* pDocs, CBFXmlNode* pXmlNode);
    BOOL putDocument(const CDCDocument* pDoc, CBFXmlNode* pXmlNode);

    BOOL makeClusterElement(CBFXmlNode* pXmlNode);
    BOOL makeFreqItemElement(CBFXmlNode* pXmlNode);
    BOOL makeFreqItemsElement(CBFXmlNode* pXmlNode);
    BOOL makeDocsElement(CBFXmlNode* pXmlNode);
    BOOL makeDocElement(CBFXmlNode* pXmlNode);    

    // debug
    BOOL fakeTree(CDCCluster* pRoot);

// attributes
    CBFXmlDoc m_xmlDoc;
    CDCDocMgr* m_pDocMgr;
    CDCClusterMgr* m_pClusterMgr;
    CDCClusterMgr2* m_pClusterMgr2;
    TDCAlgoMode m_algoMode;
    LONG m_labelID;
};

#endif
