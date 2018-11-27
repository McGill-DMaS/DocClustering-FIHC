// DCClutoMgr.h: interface for the CDCClutoMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCCLUTOMGR_H)
#define DCCLUTOMGR_H

#if !defined(DCCLUSTER_H)
    #include "DCCluster.h"
#endif

#if !defined (DCDOCUMENT_H)
    #include "DCDocument.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDCClutoMgr  
{
public:
	CDCClutoMgr();
	virtual ~CDCClutoMgr();

// operations
    BOOL makeClusters(LPCTSTR solFile, LPCTSTR classFile);
    void getRoot(CDCCluster*& pCluster) { pCluster = &m_root; };

protected:
    BOOL createDocuments(LPCTSTR solFile, LPCTSTR classFile);
    BOOL createClusters();
    BOOL assignDocsToClusters();
    BOOL readSolutionFile(LPCTSTR solFileName);
    BOOL readClassFile(LPCTSTR classFileName);

// attributes
    CDCDocuments m_documents;
    CDCCluster m_root;
};

#endif
