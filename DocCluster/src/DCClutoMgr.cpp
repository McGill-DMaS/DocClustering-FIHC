// DCClutoMgr.cpp: implementation of the CDCClutoMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#if !defined(DCCLUTOMGR_H)
    #include "DCClutoMgr.h"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCClutoMgr::CDCClutoMgr()
{
}

CDCClutoMgr::~CDCClutoMgr()
{
    m_documents.cleanup();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCClutoMgr::makeClusters(LPCTSTR solFile, LPCTSTR classFile)
{
    if (!createDocuments(solFile, classFile)) {
        ASSERT(FALSE);
        return FALSE;
    }

    if (!createClusters()) {
        ASSERT(FALSE);
        return FALSE;
    }

    if (!assignDocsToClusters()) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCClutoMgr::createDocuments(LPCTSTR solFile, LPCTSTR classFile)
{
    m_documents.cleanup();
    if (!readSolutionFile(solFile)) {
        ASSERT(FALSE);
        return FALSE;
    }

    if (!readClassFile(classFile)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCClutoMgr::createClusters()
{    
    CDCDocument* pDoc = NULL;
    int nDocs = m_documents.GetSize();
    if (nDocs == 0)
        return TRUE;

    CDCClusters* pClusters = NULL;
    m_root.getTreeChildren(pClusters);

    // how many clusers are required?    
    int nClusters = -1;
    for (int d = 0; d < nDocs; ++d) {
        pDoc = m_documents.GetAt(d);
        if (pDoc->getClusterID() > nClusters) {
            nClusters = pDoc->getClusterID();
        }
    }
    ++nClusters;

    // create clusters
    for (int c = 0; c < nClusters; ++c) {
        if (!pClusters->AddTail(new CDCCluster())) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCClutoMgr::assignDocsToClusters()
{
    CDCClusters* pClusters = NULL;
    m_root.getTreeChildren(pClusters);

    // assign each document
    int nDocs = m_documents.GetSize();
    int clusterID = -1;
    POSITION pos = NULL;
    CDCCluster* pCluster = NULL;
    CDCDocument* pDoc = NULL;
    for (int d = 0; d < nDocs; ++d) {
        pDoc = m_documents.GetAt(d);
        clusterID = pDoc->getClusterID();
        if (clusterID < 0) {
            m_root.addDocument(pDoc);
            continue;
        }

        pos = pClusters->FindIndex(clusterID);
        P_ASSERT(pos);

        pCluster = pClusters->GetAt(pos);
        P_ASSERT(pCluster);

        if (!pCluster->addDocument(pDoc)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Read solution file and create documents with cluster ID
//---------------------------------------------------------------------------
BOOL CDCClutoMgr::readSolutionFile(LPCTSTR solFileName)
{
    CStdioFile solFile;
    if (!solFile.Open(solFileName, CFile::modeRead)) {
        ASSERT(FALSE);
        return FALSE;
    }

    CString str;
    CDCDocument* pDoc = NULL;
    while (solFile.ReadString(str)) {
        pDoc = new CDCDocument();
        P_ASSERT(pDoc);

        pDoc->setClusterID(StrToInt((LPCTSTR) str));
        m_documents.Add(pDoc);
    }
    solFile.Close();
    return TRUE;
}

//---------------------------------------------------------------------------
// Read class file and update documents with filename
//---------------------------------------------------------------------------
BOOL CDCClutoMgr::readClassFile(LPCTSTR classFileName)
{
    CStdioFile classFile;
    if (!classFile.Open(classFileName, CFile::modeRead)) {
        ASSERT(FALSE);
        return FALSE;
    }
    
    int fileUID = 0;
    CString className;
    TCHAR* uidStr = NULL;
    CDCDocument* pDoc = NULL;
    while (classFile.ReadString(className)) {
        pDoc = m_documents.GetAt(fileUID);
        P_ASSERT(pDoc);

        uidStr = IntToStr(fileUID);
        pDoc->setDocName((LPCTSTR) className, uidStr);

        delete [] uidStr;
        uidStr = NULL;
        ++fileUID;
    }
    classFile.Close();
    return TRUE;
}
