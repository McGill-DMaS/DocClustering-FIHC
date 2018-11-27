//---------------------------------------------------------------------------
// File:
//      DCDocMgr.h, DCDocMgr.cpp
// History:
//      Feb. 25, 2002   Created.
//---------------------------------------------------------------------------

#include "stdafx.h"

#include <math.h>

#if !defined(DCDOCMGR_H)
    #include "DCDocMgr.h"
#endif

#if !defined(DCKMVECTOR_H)
    #include "DCKMVector.h"
#endif

//***************************************************************************
// Class: Document Manager
// Objectives:
//      - Read all text files, trim stop words, stemming
//      - Write result to separate files
//      - Count frequencies
//      - Maintain a collection of Documents for later access
//***************************************************************************

CDCDocMgr::CDCDocMgr()
{
	if (!initialize())
	{
		ASSERT(FALSE);
	}
}


CDCDocMgr::~CDCDocMgr()
{
	m_f1Tree.cleanup();
	//delete m_f1Tree;

	m_f1Sets.cleanup();
	//delete m_globalFreqItem; 
	
	m_allDocs.cleanup();
	//delete m_allDocs;
}

BOOL CDCDocMgr::initialize()
{
	//For Debug Usage, simulated data set
	/*
	for (int i = 0; i < NUM_ALLDOCS; i ++)
	{
		CDCDocument* document = new CDCDocument();
		for (int j = 0; j < NUM_F1; j ++)
		{
			if (rand() < 0.35 * RAND_MAX)
			{
				document->m_docVector.Add(1);
			}
			else
			{
				document->m_docVector.Add(0);
			}
		}

	//		CDCDebug::printDocVector(document->m_docVector);
		m_allDocs.Add(document);
	}

	*/
    return TRUE;
}

void CDCDocMgr::cleanup()
{
}

//---------------------------------------------------------------------------
// Preprocessing step
//---------------------------------------------------------------------------
BOOL CDCDocMgr::preProcess()
{
	CDCPreprocessMgr * preMgr = new CDCPreprocessMgr(m_stopWordFileName, m_docsDir, m_minGlobalSupport);
	
	preMgr -> preprocess(&m_f1Tree, &m_f1Sets, &m_allDocs);
#ifndef DC_DOCVECTOR_CONVERTOR
    if (!calIDF()) {
        ASSERT(FALSE);
        return FALSE;
    }
#endif
    if (!setFreqOneItemsets()) {
        ASSERT(FALSE);
        return FALSE;
    }

	/* ----------------output for testing: start--------------*/
#ifdef _DEBUG_PRT_INFO
	printf(_T("Frequent one itemsets:\n"));
	m_f1Tree.printBTree();
	printf(_T("\n"));
#endif

	/*
	printf("\nF1 sets:\n");
	CDCFreqItemset * pFreqItemset = NULL;
	POSITION pos = m_f1Sets.GetHeadPosition();
	while (pos != NULL) {
		pFreqItemset = m_f1Sets.GetNext(pos);        
		TFreqItemID id = pFreqItemset -> getFreqItemID(0);
		printf("%d ", id);
	}
	printf("\n\n");
	*/

	/*
	int size = m_allDocs.GetSize();
	printf("%d files processed!\n", size);
	for(int i = 0; i < size; i ++)
	{
		CDCDocument * doc = m_allDocs.GetAt(i);

		CString * docName;
		doc -> getDocName(docName);

		char * name = docName -> GetBuffer(docName -> GetLength());
		printf("  Vector of %s: ", name);
		docName -> ReleaseBuffer(-1);

		CDCDocVector * vec;
		doc -> getDocVector(vec);

		int len = vec ->GetSize();
		for(int i = 0; i < len; i ++)
		{
			printf("%d ", vec -> GetAt(i));
		}

		printf("\n");
	}

	CString * dest = NULL;
	for(int i = 0; i < 10; i ++)
	{
		if(getFreqTermFromID(i, dest))
		{
			char * term = dest -> GetBuffer(dest -> GetLength());
			printf("ID %d : Term %s\n", i, term);
			dest -> ReleaseBuffer(-1);

		}
		else
		{	
			printf("Cannot find term for ID %d\n", i);
		}
	}

  */


	/* ----------------output for testing: end--------------*/

	delete preMgr;

	return TRUE;
}


//---------------------------------------------------------------------------
// Set frequent 1-itemsets to each doc vector
//---------------------------------------------------------------------------
BOOL CDCDocMgr::setFreqOneItemsets()
{
    CDCDocVector* pDocVector = NULL;
    int nDocs = m_allDocs.GetSize();
    for (int i = 0; i < nDocs; ++i) {
        m_allDocs.GetAt(i)->getDocVector(pDocVector);
        ASSERT(pDocVector);
        pDocVector->setFreqOneItemsets(&m_f1Sets);
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// search the F1 tree for the frequent term with the given ID.
// If the term is found, return true; otherwise, false  
//---------------------------------------------------------------------------
BOOL CDCDocMgr::getFreqTermFromID(TFreqItemID id, CString& destStr)
{	
    CString* pStr = m_f1Tree.idToWord(id);
    if (!pStr)
        return FALSE;

	destStr = *pStr;
	return TRUE;
}

//---------------------------------------------------------------------------
// Convert the document vectors to IDF vectors
//---------------------------------------------------------------------------
BOOL CDCDocMgr::calIDF()
{
    int nDocs = m_allDocs.GetSize();
    if (nDocs <= 0)
        return TRUE;

    // get dimensions
    CDCDocVector* pDocVector = NULL;
    m_allDocs.GetAt(0)->getDocVector(pDocVector);
    P_ASSERT(pDocVector);
    int nDimensions = pDocVector->GetSize();

    // initialize idf vector
    CDCKMVector idf;
    idf.SetSize(nDimensions);
    for (int i = 0; i < nDimensions; ++i)
        idf[i] = 0.0f;
    
    // calculate term frequencies
    CDCDocument* pDoc = NULL;
    for (int d = 0; d < nDocs; ++d) {
        pDoc = m_allDocs.GetAt(d);
        ASSERT(pDoc);
        pDoc->getDocVector(pDocVector);
        ASSERT(pDocVector);

        for (int t = 0; t < nDimensions; ++t) {
            if ((*pDocVector)[t] > 0)
                ++idf[t];
        }
    }

    // calculate IDF
    FLOAT log2 = log10(2);
    FLOAT logNDocs = log10(nDocs) / log2;
    for (int t = 0; t < nDimensions; ++t) {
        idf[t] = logNDocs - log10(idf[t] + 1) / log2;
    }

    // convert the doc vector to IDF vector
    for (int j = 0; j < nDocs; ++j) {
        pDoc = m_allDocs.GetAt(j);
        ASSERT(pDoc);
        pDoc->getDocVector(pDocVector);
        ASSERT(pDocVector);
        if (!pDocVector->convertToIDF(&idf)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    return TRUE;
}