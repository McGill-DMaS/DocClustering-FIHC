//---------------------------------------------------------------------------
// File:
//      DCPreprocessMgr.h, DCPreprocessMgr.cpp
// History:
//      Feb. 27, 2002   Created by Linda.
//---------------------------------------------------------------------------

#include "stdafx.h"

#if !defined(DCPREPROCESSMGR_H)
	#include "DCPreprocessMgr.h"
#endif

extern char runningChars[];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCPreprocessMgr::CDCPreprocessMgr(LPCTSTR swFileName, LPCTSTR docDir, double sup)
{
	m_swHandler = new CDCStopwordHandler(swFileName);
	m_swHandler -> createStopwordArray();

	m_docDir = docDir;
	m_minGlobalSupport = sup;
	
	m_wordSum = 0;
	m_fileSum = 0;
	
	m_stmHandler = new CDCStemHandler();
	m_vocTree = new CDCWordsBTree();
	m_unrefinedDocs = new CDCUnrefinedDocs();
	
}


CDCPreprocessMgr::~CDCPreprocessMgr()
{
	delete m_swHandler;

	delete m_stmHandler;
	
	//m_vocTree cleanup() has been done in findGlobalFreqItem()
	delete m_vocTree;	
	
	m_unrefinedDocs -> cleanup();
	delete m_unrefinedDocs;
}

int CDCPreprocessMgr::getDocsSum()
{
	return m_fileSum;
}


// return the number of docs read
int fileID = 0;

int CDCPreprocessMgr::constructVocbTree(char * startDir)
{		
	char fileName[_MAX_PATH];
	sprintf(fileName, "%s\\*", startDir);
	//printf ("Target files are %s\n", fileName);

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile(fileName, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		printf ("Invalid File Handle. Get Last Error reports %d\n", GetLastError ());
		return fileID;
	} 

	BOOL bFinished = false;

	while(!bFinished)
	{
		CString name(FindFileData.cFileName);

		// the file read is not current direcctory . or parent directory ..
		if( (name != ".") && (name != "..") )
		{	
			char fullPath[_MAX_PATH];
			sprintf(fullPath, "%s\\%s", startDir, FindFileData.cFileName);

			// the directory other than . and .. 
			if( GetFileAttributes(fullPath) == FILE_ATTRIBUTE_DIRECTORY )
			{
				constructVocbTree( fullPath );
			}

			else
			{
				//printf ("%s\n", fullPath);
				//insertFileWords2Tree(fullPath, fileD++);

				char * shortName = name.GetBuffer(name.GetLength());
                insertFileWords2Tree(fullPath, shortName, fileID++);
				//insertFileWords2Tree(fullPath, fullPath, fileID++);
				name.ReleaseBuffer(-1);
#ifdef DC_SHOW_PROGRESS				
				printf("%c:%d\r", runningChars[fileID % NUM_RUNNING_CHARS], fileID);
#endif
			}
		}		
		
		if( !FindNextFile(hFind, &FindFileData) )
		{
			if (GetLastError() == ERROR_NO_MORE_FILES) 
			{ 
				bFinished = TRUE; 
	        }
			else
			{
				break;
			}			
		}
	} 		
	
	FindClose(hFind);

	//delete[] fileName;

	return fileID;
}

void CDCPreprocessMgr::insertFileWords2Tree(char * fullName, char * shortName, int fileID)
{
	m_swHandler -> rmStopwords(fullName);

	CStringArray * wordArray = m_swHandler -> getCleanFile();
	m_stmHandler -> stemFile(wordArray);


	CDCUnrefinedDoc *newDoc = new CDCUnrefinedDoc(CString(shortName), *wordArray);
	m_unrefinedDocs -> addUnrefinedDoc(newDoc);

	int len = wordArray -> GetSize();
	m_wordSum += len;

	for (int i = 0; i < len; i ++)
	{
		m_vocTree -> insertNode( TYPE_VOCABULARY_TREE, wordArray -> GetAt(i), fileID);
	}
}


void CDCPreprocessMgr::insertFileWords2Tree(char * dirtyFileName, int fileID)
{
	m_swHandler -> rmStopwords(dirtyFileName);

	CStringArray * wordArray = m_swHandler -> getCleanFile();
	m_stmHandler -> stemFile(wordArray);


	/*  --------------------- output for test: start ---------------------- 
	
	printf("\n--------------- After stemming ----------------\n");
	int size = wordArray->GetSize();

	for(int j =0; j < size; j ++)
	{
		CString S = wordArray -> GetAt(j);
		printf("%s ", S);
	}
	printf("\n\n");
	
	  --------------------- output for test: end -----------------------  */

	CDCUnrefinedDoc *newDoc = new CDCUnrefinedDoc(CString(dirtyFileName), *wordArray);
	m_unrefinedDocs -> addUnrefinedDoc(newDoc);

	int len = wordArray -> GetSize();
	m_wordSum += len;

	for (int i = 0; i < len; i ++)
	{
		m_vocTree -> insertNode( TYPE_VOCABULARY_TREE, wordArray -> GetAt(i), fileID);
	}
}


// tranverse the vocabulary tree in mid-order to find global freq items
//f1Tree = new CDCWordsBTree(): F1 tree
//f1Sets = CDCFreqItemsets: the global frequent itemsets F1 listed in alphabetical order

void CDCPreprocessMgr::findGlobalFreqItem(CDCWordsBTree * f1Tree,
										  CDCFreqItemsets * f1Sets)	
{	
	char * pDir = m_docDir.GetBuffer(m_docDir.GetLength());
	m_fileSum = constructVocbTree(pDir);
	m_docDir.ReleaseBuffer(-1);

	int minTimes = (int)ceil(m_fileSum * m_minGlobalSupport);
	printf("\n# of documents: %d, min_sup = %f\n", m_fileSum, m_minGlobalSupport);
	
	midOrderTraverse(m_vocTree -> getRoot(), minTimes, f1Tree, f1Sets);

	//m_vocTree -> printBTree();
	
	m_vocTree -> cleanup();			// cleanup the nodes of the tree to save memory

	printf("\n");
	
}


// mid-order traverse can guarantee that the resulting frequent items are in 
// alphabetical order

TFreqItemID id = -1;		// id is the ID for m_word in the freqItem tree

void CDCPreprocessMgr::midOrderTraverse(CDCWordNode * node, 
										long minTimes,
										CDCWordsBTree * f1Tree,
										CDCFreqItemsets * f1Sets)
{
	if (node != NULL)
	{
		midOrderTraverse(node -> m_lChild, minTimes, f1Tree, f1Sets);

		if( node -> m_freq >= minTimes)
		{
			f1Tree -> insertNode(TYPE_FREQITEM_TREE, node -> m_word, ++id); 
			addFreqItem(f1Sets, node -> m_freq);			
		}

		midOrderTraverse(node ->m_rChild, minTimes, f1Tree, f1Sets);
	}
	
	m_freqItemCount = id + 1;
}


/* add the frequent item into  globalFreqItem*/
void CDCPreprocessMgr::addFreqItem(CDCFreqItemsets * f1Sets, int times)
{
	CDCFreqItem * fqItem = new CDCFreqItem(id);
	CDCFreqItemset * fqItemset= new CDCFreqItemset();
	fqItemset -> addFreqItem(fqItem);
	fqItemset -> setGlobalSupport(((float)times / m_fileSum));
	f1Sets -> addFreqItemset(fqItemset);
}


// create vectors for all documents
void CDCPreprocessMgr::createDocuments(CDCDocuments * docs, CDCWordsBTree * f1Tree)
{
	int size = m_unrefinedDocs -> GetSize();
	
	CDCUnrefinedDoc * urfDoc;
	for(int i = 0; i < size; i ++)
	{
		urfDoc = m_unrefinedDocs -> GetAt(i);
		createDocument(docs, urfDoc, f1Tree);		
	}
}

// create a vector for a given document
void CDCPreprocessMgr::createDocument(CDCDocuments * docs, 
									  CDCUnrefinedDoc * urfDoc, 
									  CDCWordsBTree * f1Tree)
{	
	CString * dName = urfDoc -> getName();
	CStringArray * dWordArray = urfDoc -> getWordArray();

	CDCDocVector * vec = new CDCDocVector();
	vec -> SetSize(m_freqItemCount, -1);

	int len = dWordArray -> GetSize();
	
	for(int i = 0; i < len; i ++)
	{
		int index = f1Tree -> isNode(dWordArray -> GetAt(i));
		
		if(index != -1)	// the word is in the freqItem Tree
		{
			vec -> SetAt(index, vec -> GetAt(index) + 1 );
		}
	}

	CDCDocument * doc = new CDCDocument(*vec, *dName);
	
	delete vec;
	docs -> Add(doc);
}


void CDCPreprocessMgr::storeCleanDocs()
{

}

void CDCPreprocessMgr::storeDocVectors()
{

}

//---------------------------------------------------------------------------
// preprocess(): do preprocessing, which includes F1 finding, document vector
//				 creating, and F1 search tree creating.
// It is the only method that external can call to invoke preprocessing 
//---------------------------------------------------------------------------
void CDCPreprocessMgr::preprocess(CDCWordsBTree * f1Tree, 
								  CDCFreqItemsets * f1Sets,
								  CDCDocuments * docs)
{	
	findGlobalFreqItem(f1Tree, f1Sets);

	createDocuments(docs, f1Tree);
}


