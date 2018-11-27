// DCController.h: interface for the CDCController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCCONTROLLER_H)
#define DCCONTROLLER_H

#if !defined(DCDOCMGR_H)
    #include "DCDocMgr.h"
#endif

#if !defined(DCFREQITEMMGR_H)
    #include "DCFreqItemMgr.h"
#endif

#if !defined(DCCLUSTERMGR_H)
    #include "DCClusterMgr.h"
#endif

#if !defined(DCTREEBUILDER_H)
    #include "DCTreeBuilder.h"
#endif

#if !defined(DCEVALMGR_H)
    #include "DCEvalMgr.h"
#endif

#if !defined(DCOUTPUTMGR_H)
    #include "DCOutputMgr.h"
#endif

#if !defined(DCDEBUG_H)
	#include "DCDebug.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDCController  
{
public:
	CDCController();
	virtual ~CDCController();
    BOOL initialize(TDCAlgoMode algoMode);
    void cleanup();

// operations
    BOOL runFreqItems(FLOAT globalSupport, FLOAT clusterSupport, int kClusters, LPCTSTR inputDir);
    BOOL runKMeans(int kClusters, LPCTSTR inputDir);
    BOOL runCluto(LPCTSTR solFile, LPCTSTR classFile);

protected:
    CString makeOutFilePath(LPCTSTR dir, LPCTSTR fileNamePrefix, FLOAT globalSupport, FLOAT clusterSupport, int kClusters, FLOAT fm, FLOAT entropyF);
    BOOL writeVectorsFile(CDCDocuments* pAllDocs);    

// attributes
    CDCDocMgr m_docMgr;
    CDCFreqItemMgr m_freqItemMgr;
    CDCClusterMgr m_clusterMgr;
    CDCClusterMgr2 m_clusterMgr2;
    CDCTreeBuilder m_treeBuilder;
    CDCEvalMgr m_evalMgr;
    CDCOutputMgr m_outputMgr;    
};

#endif
