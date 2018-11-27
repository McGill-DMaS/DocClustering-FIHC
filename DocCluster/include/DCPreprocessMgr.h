// DCPreprocessMgr.h: interface for the CDCPreprocessMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCPREPROCESSMGR_H)
#define DCPREPROCESSMGR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DCWordsBTree.h"
#include "DCStopwordHandler.h"
#include "DCStemHandler.h"
#include "DCFreqItem.h"
#include "DCDocVector.h"
#include "DCDocument.h"
#include "DCUnrefinedDoc.h"

#include <math.h>
#include <afxtempl.h>


class CDCPreprocessMgr  
{
public:
	
	//CDCPreprocessMgr(float, char*);
	CDCPreprocessMgr(LPCTSTR, LPCTSTR, double);
	virtual ~CDCPreprocessMgr();
	void preprocess(CDCWordsBTree*, CDCFreqItemsets*, CDCDocuments*);
	int getDocsSum();
	
private:

	CString m_docDir;
	//CStringArray m_fileNameList;
	double m_minGlobalSupport;
	int m_wordSum;
	int m_fileSum;
	int m_freqItemCount;	

	CDCStopwordHandler * m_swHandler;	// stop word handler
	CDCStemHandler * m_stmHandler;		// stemming handler
	CDCWordsBTree * m_vocTree;			// vocabulary tree
	CDCUnrefinedDocs * m_unrefinedDocs;	// the stemmed documents contianing both 
										// frequent words and infrequent words	
		
	//@todo, in constructVocbTree(), read all the files in a better way 
	int constructVocbTree(char *);
	void insertFileWords2Tree(char*, int);	// depreciated
	void insertFileWords2Tree(char *, char *, int);
	void midOrderTraverse(CDCWordNode*, long, CDCWordsBTree*, CDCFreqItemsets*);	
	void addFreqItem(CDCFreqItemsets*, int);
	void createDocument(CDCDocuments*, CDCUnrefinedDoc*, CDCWordsBTree*);

	void findGlobalFreqItem(CDCWordsBTree*, CDCFreqItemsets*);
	void createDocuments(CDCDocuments*, CDCWordsBTree*);
	
	// @todo: needed when data is large
	char * m_cleanDocPath;				// the path to store the clean files
	char * m_vectorFileName;			// the file to store the doc vector
	void storeCleanDocs();
	void storeDocVectors();
};

#endif // !defined(DCPREPROCESSMGR_H)
