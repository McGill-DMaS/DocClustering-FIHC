// DCEvalMgr.h: interface for the CDCEvalMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCEVALMGR_H)
#define DCEVALMGR_H

#if !defined(DCDOCMGR_H)
    #include "DCDocMgr.h"
#endif

#if !defined(DCCLUSTERMGR_H)
    #include "DCClusterMgr.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//***************************************************************************

class CDCEvalMgr  
{
public:
	CDCEvalMgr();
	virtual ~CDCEvalMgr();
    BOOL initialize(TDCAlgoMode algoMode, CDCDocMgr* pDocMgr, CDCClusterMgr* pClusterMgr);
    void cleanup();

// operations
    BOOL evalFMeasure(CDCCluster* pRoot, FLOAT& fm);
    BOOL evalOverallSimilarity(CDCCluster* pRoot, FLOAT& overallSim);
    BOOL evalEntropyHierarchical(CDCCluster* pRoot, FLOAT& totalEntropy);
    BOOL evalEntropyFlat(CDCCluster* pRoot, FLOAT& totalEntropy);

protected:
    BOOL findAllClasses(CDCCluster* pRoot, CDCEvalClasses& evalClasses);
    BOOL findMaxFMeasure(CDCCluster* pParent, CDCEvalClass* pEvalClass, FLOAT& maxFM, CDCCluster*& pBestCluster);
    BOOL evalCFI(CDCEvalClass* pEvalClass, CDCCluster* pTargetCluster);

// attributes
    CDCDocMgr* m_pDocMgr;
    CDCClusterMgr* m_pClusterMgr;
    TDCAlgoMode m_algoMode;
};

#endif
