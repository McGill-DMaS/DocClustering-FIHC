//---------------------------------------------------------------------------
// File:
//      DCOutputMgr.h, DCOutputMgr.cpp
// History:
//      Mar. 13, 2002   Created by Benjamin Fung
//---------------------------------------------------------------------------

#include "stdafx.h"

#if !defined(DCOUTPUTMGR_H)
    #include "DCOutputMgr.h"
#endif

#if !defined(DCCLUSTER_H)
    #include "DCCluster.h"
#endif

//***************************************************************************
// Class: DCOutputMgr
// Objectives: Output all the results
//***************************************************************************

CDCOutputMgr::CDCOutputMgr()
{
    m_pClusterMgr = NULL;
    m_labelID = 0;
}

CDCOutputMgr::~CDCOutputMgr()
{
}

BOOL CDCOutputMgr::initialize(TDCAlgoMode algoMode, CDCDocMgr* pDocMgr, CDCClusterMgr* pClusterMgr, CDCClusterMgr2* pClusterMgr2)
{
    if (!pDocMgr || !pClusterMgr)
        return FALSE;

    m_labelID = 0;
    m_algoMode = algoMode;
    m_pDocMgr = pDocMgr;
    m_pClusterMgr = pClusterMgr;
    m_pClusterMgr2 = pClusterMgr2;
    return TRUE;
}

void CDCOutputMgr::cleanup()
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::produceOutput(LPCTSTR outFileName, FLOAT fMeasure, FLOAT overallSim, FLOAT entropyH, FLOAT entropyF)
{
    DEBUGPrint(_T("*** Writing XML file\n"));

    if (!m_pClusterMgr) {
        ASSERT(FALSE);
        return FALSE;
    }

    // get the root of the cluster tree
    CDCClusterWH* pClusterWH = NULL;    
    m_pClusterMgr->getClusterWH(pClusterWH);

    CDCCluster* pRootCluster = NULL;
    pClusterWH->getTreeRoot(pRootCluster);

    /*
    // debug only
    if (!fakeTree(pRootCluster)) {
        ASSERT(FALSE);
        return FALSE;
    }
    */

    if (!writeTreeToXML(outFileName, pRootCluster, fMeasure, overallSim, entropyH, entropyF)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::produceOutput(LPCTSTR outFileName, CDCCluster* pCluster, FLOAT fMeasure, FLOAT overallSim, FLOAT entropyH, FLOAT entropyF)
{
    DEBUGPrint("*** Writing XML file\n");
    ASSERT(pCluster);
    if (!writeTreeToXML(outFileName, pCluster, fMeasure, overallSim, entropyH, entropyF)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Write list of clusters to XML
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::writeClustersToXML(LPCTSTR outFileName, const CDCClusters* pClusters)
{
    ASSERT(pClusters);

    // create an XML file
    CBFXmlDoc m_xmlDoc;
    if (!m_xmlDoc.createXML(outFileName, DC_XML_TAG_ROOT)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // get the root node
    CBFXmlNode rootNode;
    if (!m_xmlDoc.getDocumentNode(&rootNode)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // write the list of clusters
    if (!putClustersList(pClusters, &rootNode)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // save the file
    if (!m_xmlDoc.saveXML(NULL)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Write the cluster tree to XML file.
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::writeTreeToXML(LPCTSTR outFileName, CDCCluster* pRootCluster, FLOAT fMeasure, FLOAT overallSim, FLOAT entropyH, FLOAT entropyF)
{
    // create an XML file
    CBFXmlDoc m_xmlDoc;
    if (!m_xmlDoc.createXML(outFileName, DC_XML_TAG_ROOT)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // get the root node
    CBFXmlNode rootNode;
    if (!m_xmlDoc.getDocumentNode(&rootNode)) {
        ASSERT(FALSE);
        return FALSE;
    }
    if (!rootNode.setAttribute(DC_XML_ATB_FMEASURE, fMeasure)) {
        ASSERT(FALSE);
        return FALSE;
    }
/*
    if (!rootNode.setAttribute(DC_XML_ATB_OVERALLSIM, overallSim)) {
        ASSERT(FALSE);
        return FALSE;
    }
 */
    if (!rootNode.setAttribute(DC_XML_ATB_ENTROPY_HIE, entropyH)) {
        ASSERT(FALSE);
        return FALSE;
    }
    if (!rootNode.setAttribute(DC_XML_ATB_ENTROPY_FLT, entropyF)) {
        ASSERT(FALSE);
        return FALSE;
    }
    if (!rootNode.setAttribute(DC_XML_ATB_NUM_CLUSTERS, LONG(pRootCluster->getNumClustersIncludeTreeChildren()))) {
        ASSERT(FALSE);
        return FALSE;
    }

    // write the tree
    if (!putSubTree(pRootCluster, &rootNode)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // save the file
    if (!m_xmlDoc.saveXML(NULL)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Put the list of clusters
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::putClustersList(const CDCClusters* pClusters, CBFXmlNode* pParentXMLNode)
{
    ASSERT(pClusters && pParentXMLNode);
    
    CDCCluster* pCluster = NULL;
    POSITION pos = pClusters->GetHeadPosition();
    while (pos != NULL) {
        pCluster = pClusters->GetNext(pos);
        ASSERT(pCluster);

        // make a new cluster element
        CBFXmlNode newChildNode;
        if (!makeClusterElement(&newChildNode)) {
            ASSERT(FALSE);
            DEBUGPrint(_T("Failed to make cluster element\n"));
            return FALSE;
        }

        // put all the docuemnts in this cluster
        if (!putDocuments(pCluster, &newChildNode)) {
            ASSERT(FALSE);
            return FALSE;
        }

        // append this subtree to parent
        if (!pParentXMLNode->appendChild(&newChildNode)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Get information from ParentCluster and then write information to
// ParentXMLNode.  Do it recursively for each child.
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::putSubTree(CDCCluster* pParentCluster, CBFXmlNode* pParentXMLNode)
{
    ASSERT(pParentCluster && pParentXMLNode);

    // write all the cluster information
    if (!putClusterElement(pParentCluster, pParentXMLNode))
        return FALSE;

    // get all the children
    CDCClusters* pChildClusters = NULL;
    pParentCluster->getTreeChildren(pChildClusters);

    // do the same step for each child
    CDCCluster* pChildCluster = NULL;
    POSITION pos = pChildClusters->GetHeadPosition();
    while (pos != NULL) {
        pChildCluster = pChildClusters->GetNext(pos);
        ASSERT(pChildCluster);

        // make a new cluster element
        CBFXmlNode newChildNode;
        if (!makeClusterElement(&newChildNode)) {
            ASSERT(FALSE);
            DEBUGPrint(_T("Failed to make cluster element\n"));
            return FALSE;
        }

        // write the child recursively
        if (!putSubTree(pChildCluster, &newChildNode)) {
            ASSERT(FALSE);
            return FALSE;
        }        

        // append this subtree to parent
        if (!pParentXMLNode->appendChild(&newChildNode)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Put the cluster element to the XML node
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::putClusterElement(CDCCluster* pCluster, CBFXmlNode* pXmlNode)
{
    ASSERT(pCluster && pXmlNode);

    // write all attributes in this cluster
    if (!putClusterAttributes(pCluster, pXmlNode)) {
        ASSERT(FALSE);
        return FALSE;
    }

#ifdef _DEBUG_PRT_INFO
    // write the frequent (non-core) items in this cluster
    if (!putClusterFreqItems(pCluster, pXmlNode)) {
        ASSERT(FALSE);
        return FALSE;
    }
#endif

    // write all documents in this cluster
    if (!putDocuments(pCluster, pXmlNode)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Get information form pCluster, and write the attributes to the XML node
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::putClusterAttributes(CDCCluster* pCluster, CBFXmlNode* pXmlNode)
{
    ASSERT(pCluster && pXmlNode);
    ASSERT(m_pDocMgr);

    // get the cluster label
    CDCClusterFreqItemset* pCoreItems = NULL;
    pCluster->getClusterCoreItems(pCoreItems);
    if (!pCoreItems) {
        ASSERT(FALSE);
        return FALSE;
    }

    // get number of children
    CDCClusters* pChildClusters = NULL;
    pCluster->getTreeChildren(pChildClusters);

    // set attributes
    if (m_algoMode == DC_ALGO_MODE_FREQ_ITEMS) {
        if (!pXmlNode->setAttribute(DC_XML_ATB_LABEL, (LPCTSTR) pCoreItems->toLabel(m_pDocMgr))) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    else if (m_algoMode == DC_ALGO_MODE_KMEANS || m_algoMode == DC_ALGO_MODE_CLUTO) {
        if (!pXmlNode->setAttribute(DC_XML_ATB_LABEL, m_labelID++)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    else {
        ASSERT(FALSE);
        return FALSE;
    }
#ifdef _DEBUG_PRT_INFO
    if (!pXmlNode->setAttribute(DC_XML_ATB_GLOBAL_SUPPORT, pCluster->getCoreItemsGlobalSupport())) {
        ASSERT(FALSE);
        return FALSE;
    }
#endif
    if (!pXmlNode->setAttribute(DC_XML_ATB_NUM_CHILDREN, (LONG) pChildClusters->GetCount())) {
        ASSERT(FALSE);
        return FALSE;
    }
    if (!pXmlNode->setAttribute(DC_XML_ATB_NUM_DOCS_WITH_CHILDREN, (LONG) pCluster->getNumDocumentsIncludeTreeChildren())) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Put the frequent items in the given cluster to XML node
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::putClusterFreqItems(CDCCluster* pCluster, CBFXmlNode* pXmlNode)
{
    ASSERT(pCluster && pXmlNode);

    // create freq_items node
    CBFXmlNode freqItemsNode;
    if (!makeFreqItemsElement(&freqItemsNode)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // get all frequent items
    CDCClusterFreqItemset allFreqItems;
    if (!pCluster->getClusterFreqItems(allFreqItems)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // set frequent items attributes
    if (!putClusterFreqItemsAttributes(&allFreqItems, &freqItemsNode)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // create a node for each frequent item
    CDCClusterFreqItem* pFreqItem = NULL;
    POSITION pos = allFreqItems.GetHeadPosition();
    while (pos != NULL) {
        pFreqItem = allFreqItems.GetNext(pos);
        ASSERT(pFreqItem);

        CBFXmlNode freqItemNode;
        if (!makeFreqItemElement(&freqItemNode)) {
            ASSERT(FALSE);
            return FALSE;
        }

        if (!putClusterFreqItem(pFreqItem, &freqItemNode)) {
            ASSERT(FALSE);
            return FALSE;
        }

        if (!freqItemsNode.appendChild(&freqItemNode)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }

    if (!pXmlNode->appendChild(&freqItemsNode)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Put the frequent items in the given cluster to XML node
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::putClusterFreqItemsAttributes(CDCFreqItemset* pFreqItemset, CBFXmlNode* pXmlNode)
{
    ASSERT(pFreqItemset && pXmlNode);

    if (!pXmlNode->setAttribute(DC_XML_ATB_NUM_FREQITEMS, (LONG) pFreqItemset->GetCount())) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Put the frequent item in the given cluster to XML node
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::putClusterFreqItem(CDCClusterFreqItem* pFreqItem, CBFXmlNode* pXmlNode)
{
    if (!putClusterFreqItemAttributes(pFreqItem, pXmlNode)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // get the word from its ID
    CString theWord;
    if (!m_pDocMgr->getFreqTermFromID(pFreqItem->getFreqItemID(), theWord)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // set frequent items
    if (!pXmlNode->setText(theWord)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Get information form pCluster, and write the attributes to the XML node
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::putClusterFreqItemAttributes(CDCClusterFreqItem* pFreqItem, CBFXmlNode* pXmlNode)
{
    ASSERT(pFreqItem && pXmlNode);

    // set attributes
    if (!pXmlNode->setAttribute(DC_XML_ATB_CLUSTER_SUPPORT, pFreqItem->getClusterSupport())) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Put the documents in the given cluster to XML node
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::putDocuments(CDCCluster* pCluster, CBFXmlNode* pXmlNode)
{
    ASSERT(pCluster && pXmlNode);

    CBFXmlNode documentsNode;
    if (!makeDocsElement(&documentsNode)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // get all the documents in this node
    CDCDocuments* pDocs = NULL;
    pCluster->getDocuments(pDocs);

    // make the document nodes
    CDCDocument* pDoc = NULL;
    int nDocs = pDocs->GetSize();
    for (int i = 0; i < nDocs; ++i) {
        pDoc = pDocs->GetAt(i);
        ASSERT(pDoc);

        // make a new document element
        CBFXmlNode newDocNode;
        if (!makeDocElement(&newDocNode)) {
            ASSERT(FALSE);
            return FALSE;
        }

        if (!putDocument(pDoc, &newDocNode)) {
            ASSERT(FALSE);
            return FALSE;
        }

        // append this doc to this documents node
        if (!documentsNode.appendChild(&newDocNode)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }

    // write the documents attributes
    if (!putDocumentsAttributes(pDocs, &documentsNode)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // append all docs to this XML node
    if (!pXmlNode->appendChild(&documentsNode)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::putDocumentsAttributes(CDCDocuments* pDocs, CBFXmlNode* pXmlNode)
{
    ASSERT(pDocs && pXmlNode);

    // set attributes
    if (!pXmlNode->setAttribute(DC_XML_ATB_NUM_DOCS, (LONG) pDocs->GetSize())) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::putDocument(const CDCDocument* pDoc, CBFXmlNode* pXmlNode)
{
    ASSERT(pDoc && pXmlNode);

    // set filename
    if (!pXmlNode->setText(pDoc->getDocName())) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Create a cluster XML element 
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::makeClusterElement(CBFXmlNode* pXmlNode)
{
    ASSERT(pXmlNode);    

    // make a new cluster element
    if (!m_xmlDoc.createElement(DC_XML_TAG_CLUSTER, pXmlNode)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Create a FreqItem XML element
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::makeFreqItemElement(CBFXmlNode* pXmlNode)
{
    ASSERT(pXmlNode);

    // make a new cluster element
    if (!m_xmlDoc.createElement(DC_XML_TAG_FREQITEM, pXmlNode)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Create a FreqItems XML element
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::makeFreqItemsElement(CBFXmlNode* pXmlNode)
{
    ASSERT(pXmlNode);

    // make a new cluster element
    if (!m_xmlDoc.createElement(DC_XML_TAG_FREQITEMS, pXmlNode)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Create a Documents XML element
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::makeDocsElement(CBFXmlNode* pXmlNode)
{
    ASSERT(pXmlNode);

    // make a new cluster element
    if (!m_xmlDoc.createElement(DC_XML_TAG_DOCUMENTS, pXmlNode)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Create a Doc XML element from the given document
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::makeDocElement(CBFXmlNode* pXmlNode)
{
    // make a new cluster element
    if (!m_xmlDoc.createElement(DC_XML_TAG_DOCUMENT, pXmlNode)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Debug only
//---------------------------------------------------------------------------
BOOL CDCOutputMgr::fakeTree(CDCCluster* pRoot)
{
    ASSERT(pRoot);
    CDCClusterWH* pClusterWH = NULL;
    m_pClusterMgr->getClusterWH(pClusterWH);
    CDCClusters* pAllClusters = NULL;
    pClusterWH->getAllClusters(pAllClusters);

    CDCClusters clusters;

    int i = 0;
    CDCCluster* pChild;
    POSITION pos = pAllClusters->GetHeadPosition();
    while (pos != NULL) {
        pChild = pAllClusters->GetNext(pos);
        pRoot->addTreeChild(pChild);
        //DEBUGPrint(_T("%d\n"), i);
        if (i % 7 == 0) {
            //CDCCluster* pCluster = new CDCCluster();    // memory leak, for this funciton is for temporary only
            //pChild->addTreeChild(pCluster);
            //clusters.addCluster(pCluster);
        }
        ++i;
    }
    //clusters.cleanup();
    return TRUE;
}