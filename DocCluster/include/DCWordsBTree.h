// DCWordsBTree.h: interface for the CDCWordsBTree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCWORDSBTREE_H)
#define DCWORDSBTREE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DCWordNode.h"

#define TYPE_VOCABULARY_TREE 0
#define TYPE_FREQITEM_TREE 1

class CDCWordsBTree  
{
public:

	CDCWordsBTree();
	virtual ~CDCWordsBTree();
	void insertNode(unsigned, CString&, int);
	int isNode(CString&);
	CString * idToWord(int);
	CDCWordNode * getRoot();
	void cleanup();
	void printBTree();
	
private:

	CDCWordNode * root;

	CDCWordNode * simpleInsert_CommonWord(CString&, int, CDCWordNode*, CDCWordNode*&, CDCWordNode*&);
	CDCWordNode * simpleInsert_FreqWord(CString&, int, CDCWordNode*, CDCWordNode*&, CDCWordNode*&);

	void updateBalance(CString&, 
						CDCWordNode*, 
						CDCWordNode*, 
						CDCWordNode*, 
						CDCWordNode*, 
						CDCWordNode*);

	void rearrangeLeftSubTree(CDCWordNode*, 
								CDCWordNode*, 
								CDCWordNode*);
	
	void rearrangeRightSubTree(CDCWordNode*, 
								CDCWordNode*, 
								CDCWordNode*);

	void cleanupSubTree(CDCWordNode*&);

	void printSubTreeInOrder(CDCWordNode*);
};

#endif // !defined(DCWORDSBTREE_H)
