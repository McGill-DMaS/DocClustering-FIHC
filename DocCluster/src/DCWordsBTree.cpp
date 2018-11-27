//---------------------------------------------------------------------------
// File:
//      DCWordsBTree.h, DCWordsBTree.cpp
// History:
//      Feb. 27, 2002   Created by Linda.
//---------------------------------------------------------------------------


#include "stdafx.h"

#if !defined(DCWORDSBTREE_H)
	#include "DCWordsBTree.h"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCWordsBTree::CDCWordsBTree()
{
	root = NULL;
}

CDCWordsBTree::~CDCWordsBTree()
{
	if( root!= NULL) cleanup();
}

CDCWordNode * CDCWordsBTree::getRoot()
{
	return root;
}


//////////////////////////////////////////////////////////////////////////////
// variable f_or_w_id:
//		for vocabulary tree, it is the latest file ID that contains this word
//		for FreqItem tree, it is the id assigned to different freqItem 
// variable currentFileID:
//		for vocabulary tree, it is the ID of current file being processed 
//		for FreqItem tree, it is of no use 
//////////////////////////////////////////////////////////////////////////////
void CDCWordsBTree::insertNode(unsigned type, CString& a, int f_or_w_id)
{
	if(root == NULL)				// if the root is null, create a node for the 
									// input word, and set it as tree root
	{
		CDCWordNode * newNode = new CDCWordNode(a, f_or_w_id);
		root = newNode;
		return;
	}
	
	CDCWordNode * superRoot = new CDCWordNode(); // used as the superRoot of the real root
	superRoot -> m_lChild = root;

	CDCWordNode * possibleNode;		// in the whole process of insertion, this pointer
									// always points to the node that probably needs
									// rearrangement after insertion

	CDCWordNode * possibleParent;	// the parent of posible node
	
	CDCWordNode * newNode;

	
	if(type == TYPE_VOCABULARY_TREE)
	{
		newNode = simpleInsert_CommonWord(a, f_or_w_id, superRoot, 
										possibleNode, possibleParent);
	}
	
	else if(type == TYPE_FREQITEM_TREE)
	{
		newNode = simpleInsert_FreqWord(a, f_or_w_id, superRoot, 
										possibleNode, possibleParent);
	}

	else{
		printf("Invalid parameter: type!\n");
		delete superRoot;
		return;
	}

	if(newNode != NULL)
	{
		CDCWordNode * startNode;
	
		if(a < possibleNode -> m_word)
		{
			startNode = possibleNode -> m_lChild;
		}

		else {
			startNode = possibleNode -> m_rChild;
		}
	
		updateBalance(a, possibleParent, possibleNode, startNode, newNode, superRoot);
	}	

	delete superRoot;	
}

	
CDCWordNode * CDCWordsBTree::simpleInsert_CommonWord(CString& a,
													 int currentFileID,
													 CDCWordNode *sr, 
													 CDCWordNode * &pn, 
													 CDCWordNode * &pp)
{
	//sr: the pointer to the superRoot
	//pn: the pointer to possible node
	//pp: the pointer to the parent of possible node
	
	CDCWordNode * currentNode, * currentChild;
	
	pn = currentNode = root;
	pp = sr;

	if(a == currentNode -> m_word)		// the word to insert already in the tree
	{
		if(currentFileID != (currentNode -> m_file_or_word_id))
		{
			(currentNode -> m_freq) ++;
			currentNode -> m_file_or_word_id = currentFileID;
		}
		return NULL;
	}

	if(a < currentNode -> m_word) {
		currentChild = currentNode -> m_lChild;
	}

	else {
		currentChild = currentNode -> m_rChild;
	}

	while(currentChild != NULL)			// find a suitable position to insert word a
	{
		if(currentChild -> m_balance != 0)
		{
			pn = currentChild;
			pp = currentNode;			
		}

		currentNode = currentChild;

		if(a == currentNode -> m_word) {
			if(currentFileID != (currentNode -> m_file_or_word_id))
			{
				(currentNode -> m_freq) ++;
				currentNode -> m_file_or_word_id = currentFileID;
			}
			return NULL;
		}

		if(a < currentNode -> m_word) {
			currentChild = currentNode -> m_lChild;
		}

		else {
			currentChild = currentNode -> m_rChild;
		}
	}

	CDCWordNode * newNode = new CDCWordNode(a, currentFileID);
	
	if(a < currentNode -> m_word) {
		currentNode -> m_lChild = newNode;
	}

	else {
		currentNode -> m_rChild = newNode;
	}

	return newNode;
	
}



CDCWordNode * CDCWordsBTree::simpleInsert_FreqWord(CString& a,
												   int wordID,
												   CDCWordNode *sr, 
												   CDCWordNode * &pn, 
												   CDCWordNode * &pp)
{
	CDCWordNode * currentNode, * currentChild;
	
	pn = currentNode = root;
	pp = sr;

	// When constructing FreqItem tree, every FreqItem node will be inserted only once.
	// So it is impossible that the word to be inserted is the same as the word in the existing node

	if(a < currentNode -> m_word) {
		currentChild = currentNode -> m_lChild;
	}

	else {
		currentChild = currentNode -> m_rChild;
	}

	while(currentChild != NULL)			// find a suitable position to insert word a
	{
		if(currentChild -> m_balance != 0)
		{
			pn = currentChild;
			pp = currentNode;			
		}

		currentNode = currentChild;

		if(a < currentNode -> m_word) {
			currentChild = currentNode -> m_lChild;
		}

		else {
			currentChild = currentNode -> m_rChild;
		}
	}

	CDCWordNode * newNode = new CDCWordNode(a, wordID);
	
	if(a < currentNode -> m_word) {
		currentNode -> m_lChild = newNode;
	}

	else {
		currentNode -> m_rChild = newNode;
	}

	return newNode;
}


void CDCWordsBTree::updateBalance(CString& a, 
								   CDCWordNode *pp, 
								   CDCWordNode *pn, 
								   CDCWordNode *start, 
								   CDCWordNode *end, 
								   CDCWordNode *sr)
{
	//pp: parent of possible node
	//pn: possible node
	//start: start node
	//end: end node
	//sr: super root
	
	CDCWordNode * currentNode;
	
	currentNode = start;

	while(currentNode != end)
	{
		if(a < currentNode -> m_word)
		{
			currentNode -> m_balance = -1;
			currentNode = currentNode -> m_lChild;
		}

		else
		{
			currentNode -> m_balance = 1;
			currentNode = currentNode -> m_rChild;
		}
	}

	int aux;
	if(a < pn -> m_word) aux = -1;
	else aux = 1;

	if(pn -> m_balance == 0)
	{
		pn -> m_balance = aux;
		root = sr -> m_lChild;
		return;
	}

	if(pn -> m_balance == - aux)
	{
		pn -> m_balance = 0;
		root = sr -> m_lChild;
		return;
	}

	if(aux == -1)
	{
		rearrangeLeftSubTree(start, pp, pn);
	}

	else
	{
		rearrangeRightSubTree(start, pp, pn);
	}

	root = sr -> m_lChild;
}


void CDCWordsBTree::rearrangeLeftSubTree(CDCWordNode *start, 
										 CDCWordNode *pp, 
										 CDCWordNode *pn)
{
	
	CDCWordNode *p, *p_l, *p_r;
	
	if(start -> m_balance == -1)
	{
		if(pp->m_lChild == pn)
		{
			pp ->m_lChild = start;
		}
		else
		{
			pp->m_rChild = start;
		}

		p = start->m_rChild;
		start->m_rChild = pn;
		pn->m_lChild = p;
		pn->m_balance = 0;
		start->m_balance = 0;			
	}

	else
	{
		p = start->m_rChild;
		p_r = p->m_rChild;
		p_l = p->m_lChild;

		if(pp->m_lChild == pn)
		{
			pp->m_lChild = p;
		}
		else 
		{
			pp->m_rChild = p;
		}

		p->m_lChild = start;
		p->m_rChild = pn;
		start->m_rChild = p_l;
		pn->m_lChild = p_r;
		start->m_balance = 0;
		pn->m_balance = 0;

		if(p->m_balance < 0)
		{
			pn->m_balance = 1;
		}

		if(p->m_balance > 0)
		{
			start->m_balance = -1;
		}

		p->m_balance = 0;
	}

}
	

void CDCWordsBTree::rearrangeRightSubTree(CDCWordNode *start, 
										  CDCWordNode *pp, 
										  CDCWordNode *pn)
{
	
	CDCWordNode *p, *p_l, *p_r;
	
	if(start -> m_balance == 1)
	{
		if(pp->m_lChild == pn)
		{
			pp ->m_lChild = start;
		}
		else
		{
			pp->m_rChild = start;
		}

		p = start->m_lChild;
		start->m_lChild = pn;
		pn->m_rChild = p;
		pn->m_balance = 0;
		start->m_balance = 0;			
	}

	else
	{
		p = start->m_lChild;
		p_r = p->m_rChild;
		p_l = p->m_lChild;

		if(pp->m_lChild == pn)
		{
			pp->m_lChild = p;
		}
		else 
		{
			pp->m_rChild = p;
		}

		p->m_rChild = start;
		p->m_lChild = pn;
		start->m_lChild = p_r;
		pn->m_rChild = p_l;
		start->m_balance = 0;
		pn->m_balance = 0;

		if(p->m_balance > 0)
		{
			pn->m_balance = -1;
		}

		if(p->m_balance < 0)
		{
			start->m_balance = 1;
		}

		p->m_balance = 0;
	}

}


// return m_file_or_word_id, as the given string's id
int CDCWordsBTree::isNode(CString& a)
{
	CDCWordNode * pNode = root;

	while(pNode != NULL)
	{
		if( pNode -> m_word == a)
		{
			return (int)pNode -> m_file_or_word_id;
		}

		if( a < pNode -> m_word )
		{
			pNode = pNode -> m_lChild;
		}
		else
		{
			pNode = pNode ->m_rChild;
		}
	}
	
	// if the given string not in the tree, return -1
	return -1;
}


void CDCWordsBTree::cleanupSubTree(CDCWordNode * &subTreeRoot)
{	
	if (subTreeRoot == NULL)
		return;

	cleanupSubTree(subTreeRoot->m_lChild);
	cleanupSubTree(subTreeRoot->m_rChild);
	delete subTreeRoot;
	subTreeRoot = NULL;
}


void CDCWordsBTree::cleanup()
{
	cleanupSubTree(root);
}

void CDCWordsBTree::printSubTreeInOrder(CDCWordNode * subTreeRoot)
{
	if (subTreeRoot != NULL)
	{
		printSubTreeInOrder(subTreeRoot -> m_lChild);

		//int id = subTreeRoot -> m_file_or_word_id;
		//CString * str = &(subTreeRoot -> m_word);
		//char * c = str -> GetBuffer(str -> GetLength());
		//printf("%s,%d ", c, id);

		printf("(%s,%d) ", subTreeRoot -> m_word, 
							subTreeRoot -> m_file_or_word_id);
		
		//str -> ReleaseBuffer(-1);
		
		printSubTreeInOrder(subTreeRoot -> m_rChild);
	}
}

void CDCWordsBTree::printBTree()
{
	printSubTreeInOrder(root);
}


CString * CDCWordsBTree::idToWord(int id)
{
	CDCWordNode * pNode = root;

	while(pNode != NULL)
	{
		if( pNode -> m_file_or_word_id == id )
		{
			return &(pNode -> m_word);
		}

		if( id < pNode -> m_file_or_word_id )
		{
			pNode = pNode -> m_lChild;
		}
		else
		{
			pNode = pNode ->m_rChild;
		}
	}
	
	// if the given string not in the tree, return -1
	return NULL;


}