// DCDocument.h: interface for the Document related classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCDOCUMENT_H)
#define DCDOCUMENT_H

#if !defined(DCDOCVECTOR_H)
    #include "DCDocVector.h"
#endif

#if !defined(DCKMVECTOR_H)
    #include "DCKMVector.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//***************************************************************************

class CDCDocument  
{
public:
	CDCDocument();
	CDCDocument(CDCDocVector&);
	CDCDocument(CDCDocVector&, CString&);
	virtual ~CDCDocument();

// operations
    inline void getDocVector(const CDCDocVector*& pDocVector) { pDocVector = &m_docVector; };
    CString getClassName() const;
	CString getDocName() const { return m_fileName; };
    void setDocName(LPCTSTR docName) { m_fileName = docName; };
    void setDocName(LPCTSTR className, LPCTSTR posFix);

    // K-Means
    inline void getKMVector(const CDCKMVector*& pKMVector) { pKMVector = &m_kmVector; };
    BOOL normalize(const CDCKMVector* pIDFVector);

    // Clutro Evaluator
    int getClusterID() { return m_clusterID; };
    void setClusterID(int clusterID) { m_clusterID = clusterID; };

protected:
    // K-Means
    BOOL setupKMVector(int size);

    CString m_fileName;                 // original text filename of this document
    CDCDocVector m_docVector;           // vector of frequencies that represents this document
    CDCKMVector m_kmVector;             // K-Means vector

    // Cluto Evaluator
    int m_clusterID;                    // cluster id
};


//***************************************************************************

typedef CTypedPtrArray<CPtrArray, CDCDocument*> CDCDocumentPtrArray;

class CDCDocuments : public CDCDocumentPtrArray
{
public:
    CDCDocuments();
    virtual ~CDCDocuments();
    void cleanup();

// operations
    int getTotalNumWords();
};

#endif
