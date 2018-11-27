//---------------------------------------------------------------------------
// File:
//      DCWordNode.h, DCWordNode.cpp
// History:
//      Feb. 27, 2002   Created by Linda.
//---------------------------------------------------------------------------

#include "stdafx.h"

#if !defined(DCWORDNODE_H)
	#include "DCWordNode.h"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCWordNode::CDCWordNode(CString& a, int file_or_word_id)
{
	m_word = a;
	m_balance = 0;
	m_freq = 1;		// for Vocabulary tree: only if this word occurs in 
					// vocabulary will it be added into the BTree. 
					// So in constructor, m_freq = 1; for FreqItem tree, 
					// m_freq is no use.

	m_file_or_word_id = file_or_word_id;
	m_lChild = NULL;
	m_rChild = NULL;
}


CDCWordNode::CDCWordNode()
{
	m_lChild = NULL;
	m_rChild = NULL;
}


CDCWordNode::~CDCWordNode()
{

}
