// DCDocMgr.h: interface for the CDCDocMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCDOCMGR_H)
#define DCDOCMGR_H

#if !defined (DCDOCUMENT_H)
    #include "DCDocument.h"
#endif

#if !defined (DCDEBUG_H)
	#include "DCDebug.h"
#endif

#if !defined (DCPREPROCESSMGR_H)
	#include "DCPreprocessMgr.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define	NUM_ALLDOCS 2000
//#define NUM_F1 80

class CDCDocMgr  
{
public:
	BOOL preProcess();
	CDCDocMgr();
	virtual ~CDCDocMgr();

    BOOL initialize();
	BOOL getFreqTermFromID(TFreqItemID, CString&);
    void cleanup();

// operations    
	inline void getF1Sets(CDCFreqItemsets*& pF1Sets) { pF1Sets = &m_f1Sets; };
	inline void getF1Tree(CDCWordsBTree*& pF1Tree) { pF1Tree = &m_f1Tree; };
 
	inline void setDocDirectory(LPCTSTR docDir) { m_docsDir = docDir; };
	inline void setMinSupport(double minSupport) { m_minGlobalSupport = minSupport; };
	inline void setStopWordFile(LPCTSTR swFile) { m_stopWordFileName = swFile; };

    inline int getNumDocs() { return m_allDocs.GetSize(); };
    inline void getAllDocs(CDCDocuments*& pAllDocs) {pAllDocs = &m_allDocs;};
	inline void getFreqOneItemsets(CDCFreqItemsets*& pOneItemsets) {pOneItemsets = &m_f1Sets;};    

protected:
    BOOL setFreqOneItemsets();
    BOOL calIDF();

// attributes
	CString m_stopWordFileName;
	CString m_docsDir;
	double m_minGlobalSupport;
	CDCWordsBTree m_f1Tree;
	CDCFreqItemsets m_f1Sets; 
	CDCDocuments m_allDocs;
};

#endif
