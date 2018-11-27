// DCWordNode.h: interface for the CDCVocabularyNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCWORDNODE_H)
#define DCWORDNODE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDCWordNode  
{
public:
	
	//CDCWordNode(CString&);	
	CDCWordNode(CString&, int); 
	CDCWordNode();
	virtual ~CDCWordNode();

	CString m_word;
	int m_balance;
	
	int m_freq;		// for vocabulary tree, it represents frequency;
						// for frequent item tree, it is of no use

	int m_file_or_word_id;	// for vocabulary tree, it represents the ID of latest 
							// file that contains this m_word; 
							// for frequent item tree, it is the ID of m_word.
	
	CDCWordNode * m_lChild;
	CDCWordNode * m_rChild;
};

#endif // !defined(DCWORDNODE_H)
