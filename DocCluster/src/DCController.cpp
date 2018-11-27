// DCController.cpp: implementation of the CDCController class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#if !defined(DCCONTROLLER_H)
    #include "DCController.h"
#endif

#if !defined(DCKMEANSMGR_H)
    #include "DCKMeansMgr.h"
#endif

#if !defined(DCCLUTOMGR_H)
    #include "DCClutoMgr.h"
#endif

#if !defined(BFFILEHELPER_H)
	#include "BFFileHelper.h"
#endif

char runningChars [] = {'-', '\\', '|', '/'};
CDCDebug* debugObject = new CDCDebug();

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCController::CDCController()
{
}

CDCController::~CDCController()
{
    cleanup();
}

BOOL CDCController::initialize(TDCAlgoMode algoMode)
{
    if (!m_treeBuilder.initialize(&m_clusterMgr))
        return FALSE;

    if (!m_evalMgr.initialize(algoMode, &m_docMgr, &m_clusterMgr)) 
        return FALSE;

    if (!m_outputMgr.initialize(algoMode, &m_docMgr, &m_clusterMgr, &m_clusterMgr2))
        return FALSE;

	return TRUE;
}

void CDCController::cleanup()
{
    delete debugObject;
    debugObject = NULL;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCController::runFreqItems(FLOAT globalSupport, FLOAT clusterSupport, int kClusters, LPCTSTR inputDir)
{	
    // Document Manager reads all the files 
    CString docSetDirPath = inputDir;
    docSetDirPath += DC_DOCUMENTS_DIRECTORY;

    CString stopWordFilePath = inputDir;
    stopWordFilePath += DC_STOPWORDS_FILENAME;

	m_docMgr.setDocDirectory((LPCTSTR) docSetDirPath);    
	m_docMgr.setStopWordFile(DC_STOPWORDS_FILENAME);

	m_docMgr.setMinSupport(globalSupport);
	m_docMgr.preProcess();
    
    CDCDocuments* pAllDocs = NULL;
    m_docMgr.getAllDocs(pAllDocs);
    ASSERT(pAllDocs);

	CDCFreqItemsets * pF1 = NULL;
	m_docMgr.getF1Sets(pF1);
	ASSERT(pF1);

    DEBUGPrint(_T("********\n"));
    DEBUGPrint(_T("* FIHC *\n"));
    DEBUGPrint(_T("********\n"));
	CDCDebug::printTime();

    // Frequent Item Manager mines the frequent itemset (Apriori)    
    m_freqItemMgr.setMinGlobalSupport(globalSupport);
    if (!m_freqItemMgr.mineGlobalFreqItemsets(pAllDocs, pF1)) {
        ASSERT(FALSE);
        return FALSE;
    }

    CDCFreqItemsets* pGlobalFreqItemsets = NULL;
    m_freqItemMgr.getGlobalFreqItemsets(pGlobalFreqItemsets);
    ASSERT(pGlobalFreqItemsets);

#ifdef _DEBUG_PRT_INFO
    DEBUGPrint("Global frequent itemsets\n");
    debugObject->printFreqItemsets(*pGlobalFreqItemsets);
#endif

    // Cluster Manager builds the clusters of documents    
    CDCDebug::printTime();
    if (TRUE) {
        // tree based clustering
        if (!m_clusterMgr.makeClusters(pAllDocs, pGlobalFreqItemsets, clusterSupport)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    else {
        // linear clustering
	    m_clusterMgr2.setAllDocs(pAllDocs);
	    m_clusterMgr2.setClusterSupport(clusterSupport);
	    TDCClusterOrderType orderType = ORDER_TYPE_GLOBAL_SUP;
	    BOOL descOrAsc = TRUE;
	    if(!m_clusterMgr2.produceClusters(pGlobalFreqItemsets, orderType, descOrAsc)) {
		    ASSERT(FALSE);
		    return FALSE;
	    }
    }    

    // Tree Builder constructs the topical tree
    CDCDebug::printTime();
    if (!m_treeBuilder.buildTree()) {
        ASSERT(FALSE);
        return FALSE;
    }    

    // Remove empty clusters
    CDCDebug::printTime();
    if (!m_treeBuilder.removeEmptyClusters(FALSE)) {
        ASSERT(FALSE);
        return FALSE;
    }

/*
    // Pruning clusters from tree
	if (!m_treeBuilder.pruneTree()) {
		ASSERT(FALSE);
		return FALSE;
	}
 */    
    if (TRUE) {
        // prune children based on inter-cluster similarity with parent
        if (!m_treeBuilder.pruneChildren()) {
            ASSERT(FALSE);
            return FALSE;
        }
/*
        // Remove empty clusters
        if (!m_treeBuilder.removeEmptyClusters(TRUE)) {
            ASSERT(FALSE);
            return FALSE;
        }
 */
        // inter-cluster similarity based pruning
        if (!m_treeBuilder.interSimPrune(kClusters)) {
            ASSERT(FALSE);
            return FALSE;
        }
/*
        // similarity based pruning
		if (!m_treeBuilder.simBasedPrune(kClusters)) {
			ASSERT(FALSE);
			return FALSE;
		}
 */
        // score based pruning
        if (!m_treeBuilder.interSimOverPrune(kClusters)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    else {
        // score based pruning
        if (!m_treeBuilder.interSimOverPrune(kClusters)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }    

    // Evaluate clusters using overall similairty
    CDCDebug::printTime();
    CDCClusterWH* pClusterWH = NULL;
    m_clusterMgr.getClusterWH(pClusterWH);
    CDCCluster* pRoot = NULL;
    if (!pClusterWH->getTreeRoot(pRoot)) {
        ASSERT(FALSE);
        return FALSE;
    }

    // Evaluate clusters using FMeasure
    FLOAT fMeasure = 0.0f;
    if (!m_evalMgr.evalFMeasure(pRoot, fMeasure)) {
        ASSERT(FALSE);
        return FALSE;
    }
    DEBUGPrint(_T("F-Measure = %f\n"), fMeasure);

    // Evaluate overall similarity
    FLOAT overallSim = 0.0f;
    if (!m_evalMgr.evalOverallSimilarity(pRoot, overallSim)) {
        ASSERT(FALSE);
        return FALSE;
    }
    //DEBUGPrint(_T("Overall similarity = %f\n"), overallSim);

    // Evaluate clusters using entropy
    FLOAT entropyH = 0.0f;
    if (!m_evalMgr.evalEntropyHierarchical(pRoot, entropyH)) {
        ASSERT(FALSE);
        return FALSE;
    }
    //DEBUGPrint(_T("Entropy (Hierarchical) = %f\n"), entropyH);

    FLOAT entropyF = 0.0f;
    if (!m_evalMgr.evalEntropyFlat(pRoot, entropyF)) {
        ASSERT(FALSE);
        return FALSE;
    }
    DEBUGPrint(_T("Entropy (Flat) = %f\n"), entropyF);

    // Output Manager organizes and displays the results to user
    CString inputDirStr = inputDir;
    CString prefixStr;
    int slashPos = inputDirStr.ReverseFind(TCHAR('\\'));
    if (slashPos == -1)
        prefixStr = DC_XML_FREQITEM_FILENAME;
    else
        prefixStr = inputDirStr.Mid(slashPos + 1);

    CString outFilePath = makeOutFilePath(_T(".."), (LPCTSTR) prefixStr, globalSupport, clusterSupport, kClusters, fMeasure, entropyF);
    if (!m_outputMgr.produceOutput((LPCTSTR) outFilePath, fMeasure, overallSim, entropyH, entropyF)) {
        ASSERT(FALSE);
        return FALSE;
    }
    DEBUGPrint(_T("Output file: %s\n"), outFilePath);
    CDCDebug::printTime();
    return TRUE;
}

//---------------------------------------------------------------------------
// Run the K-Means algorithm
//---------------------------------------------------------------------------
BOOL CDCController::runKMeans(int kClusters, LPCTSTR inputDir)
{
    // Document Manager reads all the files 
    CString docSetDirPath = inputDir;
    docSetDirPath += DC_DOCUMENTS_DIRECTORY;

    CString stopWordFilePath = inputDir;
    stopWordFilePath += DC_STOPWORDS_FILENAME;

	m_docMgr.setDocDirectory((LPCTSTR) docSetDirPath);    
	m_docMgr.setStopWordFile(DC_STOPWORDS_FILENAME);

	m_docMgr.setMinSupport(0.0f);
	m_docMgr.preProcess();
    
    CDCDocuments* pAllDocs = NULL;
    m_docMgr.getAllDocs(pAllDocs);
    ASSERT(pAllDocs);

#ifdef DC_DOCVECTOR_CONVERTOR
    if (!writeVectorsFile(pAllDocs)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
#endif

    // Run the K-Means algorithm
    DEBUGPrint("***********\n");
    DEBUGPrint("* K-Means *\n");
    DEBUGPrint("***********\n");
    CDCDebug::printTime();

    CDCKMeansMgr kMeansMgr;
    if (!kMeansMgr.makeClusters(kClusters, pAllDocs)) {
        ASSERT(FALSE);
        return FALSE;
    }
    CDCDebug::printTime();

    CDCCluster* pRoot = NULL;
    kMeansMgr.getRoot(pRoot);
    CDCClusters* pChildren = NULL;
    pRoot->getTreeChildren(pChildren);

    // Evaluate clusters using FMeasure
    FLOAT fMeasure = 0.0f;
    if (!m_evalMgr.evalFMeasure(pRoot, fMeasure)) {
        ASSERT(FALSE);
        pChildren->cleanup();
        return FALSE;
    }
    DEBUGPrint(_T("F-Measure = %f\n"), fMeasure);

    // Evaluate clusters using overall similairty
    FLOAT overallSim = 0.0f;
    if (!m_evalMgr.evalOverallSimilarity(pRoot, overallSim)) {
        ASSERT(FALSE);
        pChildren->cleanup();
        return FALSE;
    }
    //DEBUGPrint(_T("Overall similarity = %f\n"), overallSim);

    // Evaluate clusters using entropy
    FLOAT entropyH = 0.0f;
    if (!m_evalMgr.evalEntropyHierarchical(pRoot, entropyH)) {
        ASSERT(FALSE);
        pChildren->cleanup();
        return FALSE;
    }
    DEBUGPrint(_T("Entropy (Hierarchical) = %f\n"), entropyH);

    FLOAT entropyF = 0.0f;
    if (!m_evalMgr.evalEntropyFlat(pRoot, entropyF)) {
        ASSERT(FALSE);
        pChildren->cleanup();
        return FALSE;
    }
    DEBUGPrint(_T("Entropy (Flat) = %f\n"), entropyF);

    // Output Manager organizes and displays the results to user
    CString inputDirStr = inputDir;
    CString prefixStr = DC_XML_KMEANS_FILENAME;
    int slashPos = inputDirStr.ReverseFind(TCHAR('\\'));
    if (slashPos != -1)
        prefixStr += inputDirStr.Mid(slashPos + 1);

    CString outFilePath = makeOutFilePath(_T(".."), (LPCTSTR) prefixStr, 0.0f, 0.0f, kClusters, fMeasure, entropyF);
    if (!m_outputMgr.produceOutput((LPCTSTR) outFilePath, pRoot, fMeasure, overallSim, entropyH, entropyF)) {
        ASSERT(FALSE);
        pChildren->cleanup();
        return FALSE;
    }
    DEBUGPrint(_T("Output file: %s\n"), outFilePath);
    CDCDebug::printTime();
    pChildren->cleanup();
    return TRUE;
}

//---------------------------------------------------------------------------
// Run the Cluto evaluator
//---------------------------------------------------------------------------
BOOL CDCController::runCluto(LPCTSTR solFile, LPCTSTR classFile)
{
    DEBUGPrint("*******************\n");
    DEBUGPrint("* Cluto Evaluator *\n");
    DEBUGPrint("*******************\n");

    CDCClutoMgr clutoMgr;
    if (!clutoMgr.makeClusters(solFile, classFile)) {
        ASSERT(FALSE);
        return FALSE;
    }

    CDCCluster* pRoot = NULL;
    clutoMgr.getRoot(pRoot);
    CDCClusters* pChildren = NULL;
    pRoot->getTreeChildren(pChildren);

    // Evaluate clusters using FMeasure
    FLOAT fMeasure = 0.0f;
    if (!m_evalMgr.evalFMeasure(pRoot, fMeasure)) {
        ASSERT(FALSE);
        pChildren->cleanup();
        return FALSE;
    }
    DEBUGPrint(_T("F-Measure = %f\n"), fMeasure);

    // Evaluate clusters using overall similairty
    FLOAT overallSim = 0.0f;
    if (!m_evalMgr.evalOverallSimilarity(pRoot, overallSim)) {
        ASSERT(FALSE);
        pChildren->cleanup();
        return FALSE;
    }
    //DEBUGPrint(_T("Overall similarity = %f\n"), overallSim);

    // Evaluate clusters using entropy
    FLOAT entropyH = 0.0f;
    if (!m_evalMgr.evalEntropyHierarchical(pRoot, entropyH)) {
        ASSERT(FALSE);
        pChildren->cleanup();
        return FALSE;
    }
    DEBUGPrint(_T("Entropy (Hierarchical) = %f\n"), entropyH);

    FLOAT entropyF = 0.0f;
    if (!m_evalMgr.evalEntropyFlat(pRoot, entropyF)) {
        ASSERT(FALSE);
        pChildren->cleanup();
        return FALSE;
    }
    DEBUGPrint(_T("Entropy (Flat) = %f\n"), entropyF);

    // get number of clusters
    CString drive, dir, fname, ext;
    CBFFileHelper::splitPath(solFile, drive, dir, fname, ext);
    ext = ext.Mid(1);
    int kClusters = StrToInt((LPCTSTR) ext);

    // Output Manager organizes and displays the results to user
    CString outFilePath = makeOutFilePath(drive + dir, DC_XML_CLUTO_FILENAME, 0.0f, 0.0f, kClusters, fMeasure, entropyF);
    if (!m_outputMgr.produceOutput((LPCTSTR) outFilePath, pRoot, fMeasure, overallSim, entropyH, entropyF)) {
        ASSERT(FALSE);
        pChildren->cleanup();
        return FALSE;
    }
    DEBUGPrint(_T("Output file: %s\n"), outFilePath);
    pChildren->cleanup();
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CString CDCController::makeOutFilePath(LPCTSTR dir, LPCTSTR fileNamePrefix, FLOAT globalSupport, FLOAT clusterSupport, int kClusters, FLOAT fm, FLOAT entropyF)
{
    time_t ltime;
    time(&ltime);

    CString outFilePath = dir;
    if (outFilePath.Right(1) != _T("\\") && outFilePath.Right(1) != _T("/"))
        outFilePath += _T("\\");
    outFilePath += fileNamePrefix;

    CString gsStr;
    int gsSupport = (int) (globalSupport * 100.5f);
    if (gsSupport >= 100)
        gsStr.Format(_T("%d"), gsSupport);
    else if (gsSupport >= 10)
        gsStr.Format(_T("0%d"), gsSupport);
    else
        gsStr.Format(_T("00%d"), gsSupport);
    gsStr = gsStr.Left(3);

    CString csStr;
    int csSupport = (int) (clusterSupport * 100.5f);
    if (csSupport >= 100)
        csStr.Format(_T("%d"), csSupport);
    else if (csSupport >= 10)
        csStr.Format(_T("0%d"), csSupport);
    else
        csStr.Format(_T("00%d"), csSupport);
    csStr = csStr.Left(3);

    CString kcStr;
    if (kClusters >= 100)
        kcStr.Format(_T("%d"), kClusters);
    else if (kClusters >= 10)
        kcStr.Format(_T("0%d"), kClusters);
    else
        kcStr.Format(_T("00%d"), kClusters);
    kcStr = kcStr.Left(3);

    CString fmStr;
    fmStr.Format(_T("_%f"), fm);

    CString entropyFStr;
    entropyFStr.Format(_T("_%f"), entropyF);

    CString timeStr;
    timeStr.Format(_T("_%ld"), ltime);

    outFilePath += gsStr + csStr + kcStr + fmStr + entropyFStr + DC_XML_EXT;
    return outFilePath;
}

//---------------------------------------------------------------------------
// Write vectors to the file
//---------------------------------------------------------------------------
BOOL CDCController::writeVectorsFile(CDCDocuments* pAllDocs)
{
    int nDocs = pAllDocs->GetSize();
    if (nDocs == 0)
        return TRUE;

    CString vectorFileName = _T("vectors.mat");
    CString classFileName = _T("vectors.mat.rclass");
    CString labelFileName = _T("vectors.mat.clabel");
    CStdioFile vectorFile, classFile, labelFile;
    if (!vectorFile.Open(vectorFileName, CFile::modeCreate | CFile::modeWrite)) {
        ASSERT(FALSE);
        return FALSE;
    }    
    if (!classFile.Open(classFileName, CFile::modeCreate | CFile::modeWrite)) {
        ASSERT(FALSE);
        vectorFile.Close();
        return FALSE;
    }    

    // get number of nonzero entries
    int nonZeroEntry = 0;
    CDCDocument* pDoc = NULL;
    CDCDocVector* pDocVector = NULL;
    for (int i = 0; i < nDocs; ++i) {
        DEBUGPrint(_T("%d\r"), i);
        pDoc = pAllDocs->GetAt(i);
        pDoc->getDocVector(pDocVector);
        int vectorSize = pDocVector->GetSize();

        for (int v = 0; v < vectorSize; ++v) {
            if ((*pDocVector)[v] > 0) {
                ++nonZeroEntry;
            }
        }
    }    

    // # of docs, # of distinct words, # of nonzero entries
    CString outStr;    
    ((CDCDocument*) pAllDocs->GetAt(0))->getDocVector(pDocVector);
    int vectorSize = pDocVector->GetSize();
    outStr.Format(_T("%d %d %d\n"), nDocs, vectorSize, nonZeroEntry);
    vectorFile.WriteString(outStr);
    
    for (i = 0; i < nDocs; ++i) {
        DEBUGPrint(_T("%d\r"), i);
        pDoc = pAllDocs->GetAt(i);
        pDoc->getDocVector(pDocVector);
        vectorSize = pDocVector->GetSize();

        outStr.Empty();
        for (int v = 0; v < vectorSize; ++v) {
            if ((*pDocVector)[v] > 0) {
                CString temp;
                temp.Format(_T(" %d %d"), v + 1, (*pDocVector)[v]);
                outStr += temp;
            }
        }
        vectorFile.WriteString(outStr + _T("\n"));
        classFile.WriteString(pDoc->getClassName() + _T("\n"));
    }
    classFile.Close();
    vectorFile.Close();

    // write label file
    if (!labelFile.Open(labelFileName, CFile::modeCreate | CFile::modeWrite)) {
        ASSERT(FALSE);        
        return FALSE;
    }   
    CString labelStr;
    for (int v = 0; v < vectorSize; ++v) {
        if (!m_docMgr.getFreqTermFromID(v, labelStr)) {
            ASSERT(FALSE);
            return FALSE;
        }
        labelFile.WriteString(labelStr + _T("\n"));
    }
    labelFile.Close();
    return TRUE;
}
