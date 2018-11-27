//---------------------------------------------------------------------------
// File:
//      DCFreqItemMgr.h, DCFreqItemMgr.cpp
// History:
//      Feb. 25, 2002   Created.
//---------------------------------------------------------------------------

#include "stdafx.h"

#if !defined(DCFREQITEMMGR_H)
    #include "DCFreqItemMgr.h"
#endif

#if !defined(DCDEBUG_H)
	#include "DCDebug.h"
#endif

#if !defined(DCCONTROLLER_H)
	#include "DCController.h"
#endif

//extern 	CDCDebug* debugObject;

//***************************************************************************
// Class: Frequent Item Manager
// Objectives:
//      - Perform Apriori to mine all global frequent itemsets
//      - Maintain an array of GlobelFreqItems which keeps track the global
//        supports for all the frequent itemests.
//***************************************************************************

CDCFreqItemMgr::CDCFreqItemMgr()
{
	m_pKMinusItemsets = new CDCFreqItemsets();
	m_indexFreqItemset = NULL;
}

CDCFreqItemMgr::~CDCFreqItemMgr()
{
//	DEBUGPrint("CDCFreqItemMgr::~CDCFreqItemMgr\n");

	if (m_indexFreqItemset != NULL)
	{
		delete [] m_indexFreqItemset;
	}

	if (m_pKMinusItemsets->GetCount() > 0)
		m_pKMinusItemsets->cleanup();

	m_pKMinusItemsets->RemoveAll();
	delete m_pKMinusItemsets;

	if (m_globalFreqItemsets.GetCount() > 0)
	{
		m_globalFreqItemsets.cleanup();
	}

//	m_pKMinusItemsets->RemoveAll();
	m_globalFreqItemsets.RemoveAll();
//	DEBUGPrint("CDCFreqItemMgr::~CDCFreqItemMgr Done\n");
}

BOOL CDCFreqItemMgr::initialize()
{
	m_pAllDocs = NULL;
    return TRUE;
}

void CDCFreqItemMgr::cleanup()
{
}

//---------------------------------------------------------------------------
// Mine all global frequent itemsets, e.g., {{101}, {100, 101}, {100}... }
// Results are stored in m_globalFreqItemsets.
// The itemsets can be in any order.  However, the items in the itemsets
// should be sorted in ascending order.
//---------------------------------------------------------------------------
BOOL CDCFreqItemMgr::mineGlobalFreqItemsets(CDCDocuments* pAllDocs, CDCFreqItemsets* pF1)
{
    DEBUGPrint("*** Computing global frequent itemsets using Apriori\n");
	if (!pAllDocs)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_pAllDocs = pAllDocs;

	CDCFreqItemsets* freqKItemsets = new CDCFreqItemsets();

	this->m_pF1 = pF1;

	POSITION p1 = pF1->GetHeadPosition();
	CDCFreqItemset* p_f1Itemset = NULL;

	while (p1 != NULL)
	{
		p_f1Itemset = new CDCFreqItemset();
		p_f1Itemset->copyItemset(pF1->GetAt(p1));
		m_pKMinusItemsets->addFreqItemset(p_f1Itemset);
		p_f1Itemset = new CDCFreqItemset();
		p_f1Itemset->copyItemset(pF1->GetAt(p1));
		m_globalFreqItemsets.addFreqItemset(p_f1Itemset);
		pF1->GetNext(p1);
	}

	m_numF1 = pF1->GetCount();

	m_indexFreqItemset = new POSITION[m_numF1];

	int count = 2;

	if (m_pKMinusItemsets->IsEmpty())
	{
		ASSERT(FALSE);
		return FALSE;
	}
	
	long counter = 0;
	do
	{
		counter = 0;
		DEBUGPrint(_T("F%d size [%d]\n"), count-1, m_pKMinusItemsets->GetCount());

		if (!joinKCandidateSets(freqKItemsets))
		{
			ASSERT(FALSE);
			return FALSE;
		};
		//DEBUGPrint("After Joining, F%d-Candidates size [%d]\n", count, freqKItemsets->GetCount());

		locatePosition(m_pKMinusItemsets);
		
        //DEBUGPrint("Before Pruning...\n");
        //DEBUGPrint("\nKMinus Itemsets ----->\n");
        //CDCDebug::printFreqItemsets(*m_pKMinusItemsets);
        //DEBUGPrint("\nK Itemsets ----->\n");
        //CDCDebug::printFreqItemsets(*freqKItemsets);

		this->pruneKCandidateSets(freqKItemsets);

		//DEBUGPrint("After Pruning, F%d-Candidates size [%d]\n", count, freqKItemsets->GetCount());		
        //DEBUGPrint("\nK Itemsets ----->\n");
        //CDCDebug::printFreqItemsets(*freqKItemsets);	        
        //DEBUGPrint("Finding support...\n");
		if (!this->findMinGlobalSupport(freqKItemsets))
		{
			ASSERT(FALSE);
			return FALSE;
		}

		//DEBUGPrint("After support finding, F%d size [%d]\n", count, freqKItemsets->GetCount());
		if (freqKItemsets->GetCount() == 0)
		{
			break;
		}

		POSITION temp = freqKItemsets->GetHeadPosition();
		CDCFreqItemset* itemset = NULL;
		
		m_pKMinusItemsets->cleanup();

		while (temp != NULL)
		{
			itemset = freqKItemsets->GetNext(temp);
			ASSERT(itemset);

			CDCFreqItemset* tempItemset = new CDCFreqItemset();
			tempItemset->copyItemset(itemset);
			m_pKMinusItemsets->addFreqItemset(tempItemset);

			//ToDo: Should copy the itemset to the m_globalFreqItemsets
            //tempItemset = new CDCFreqItemset();
            //tempItemset->copyItemset(itemset);
			m_globalFreqItemsets.addFreqItemset(itemset);
		}

        freqKItemsets->RemoveAll();
		count ++;
	}
	while (count < m_numF1);

	//freqKItemsets->cleanup();
	//freqKItemsets->RemoveAll();
	delete freqKItemsets;
    return TRUE;
}

//---------------------------------------------------------------------------
// Get all global frequent itemsets.
//---------------------------------------------------------------------------
void CDCFreqItemMgr::getGlobalFreqItemsets(CDCFreqItemsets*& pFreqItemsets)
{
    pFreqItemsets = &m_globalFreqItemsets;    
}


BOOL CDCFreqItemMgr::pruneKCandidateSets(CDCFreqItemsets* freqKCandidates)
{

	if (!freqKCandidates)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	POSITION pos = freqKCandidates->GetHeadPosition();
	POSITION tail, posCandidate, temp = NULL;

	POSITION start, end;

	CDCFreqItemset *freqTemp = new CDCFreqItemset();
	CDCFreqItemset *freqCandidateSet = NULL;
	TFreqItemID id;

	long counter;

	while (pos != NULL)
	{
		counter = 0;

		freqCandidateSet = freqKCandidates->GetAt(pos);
		ASSERT(freqCandidateSet);

		posCandidate = freqCandidateSet->GetHeadPosition();
		freqCandidateSet->GetNext(posCandidate);
		id = freqCandidateSet->GetAt(posCandidate)->getFreqItemID();

		findInterval(start, end, id);

		while (posCandidate != NULL)
		{
			freqTemp->addFreqItem(freqCandidateSet->GetNext(posCandidate));
		}

		if (!isInKMinus(start, end, freqTemp))
		{
			temp = pos;			
		}
		else
		{

			posCandidate = freqCandidateSet->GetHeadPosition();
			id = freqCandidateSet->GetAt(posCandidate)->getFreqItemID();
			findInterval(start, end, id);

			tail = freqCandidateSet->GetTailPosition();
			freqCandidateSet->GetNext(posCandidate);

			while (posCandidate != tail)
			{
				freqTemp->RemoveAll();
				POSITION tempPOS = freqCandidateSet->GetHeadPosition();
				while (tempPOS != NULL)
				{
					if (tempPOS != posCandidate)
					{
						freqTemp->addFreqItem(freqCandidateSet->GetAt(tempPOS));
					}
					freqCandidateSet->GetNext(tempPOS);
				}
				if (!isInKMinus(start, end, freqTemp))
				{
					temp = pos;
					break;
				}
				freqCandidateSet->GetNext(posCandidate);
			}
		}

 		freqTemp->RemoveAll();
		freqKCandidates->GetNext(pos);
		if (temp != NULL)
		{
			CDCFreqItemset* itemset = freqKCandidates->GetAt(temp);
			if (!itemset)
			{
				ASSERT(FALSE);
				return FALSE;
			}
			
			itemset->cleanup();
			delete itemset;

			freqKCandidates->RemoveAt(temp);
			temp = NULL;
#ifdef DC_SHOW_PROGRESS
			//printf("deleted %c:%d\r", runningChars[(counter++) % NUM_RUNNING_CHARS], counter);
#endif

		}
	}

	freqTemp->RemoveAll();
	delete freqTemp;

	return TRUE;
}

BOOL CDCFreqItemMgr::findMinGlobalSupport(CDCFreqItemsets *freqItemsets)
{

	if (!freqItemsets)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	POSITION posItemset = NULL;
	CDCFreqItemset *itemset = NULL;
	POSITION posItems = NULL;
	int numDocs = m_pAllDocs->GetSize();
	int n;
	TFreqItemID id;

	CDCDocVector* pDocVector = NULL;

	long counter;
	for (int i = 0; i < numDocs; i ++)
	{
		counter = 0;
		m_pAllDocs->GetAt(i)->getDocVector(pDocVector);

		n = pDocVector->GetSize();

		posItemset = freqItemsets->GetHeadPosition();
#ifdef DC_SHOW_PROGRESS
		printf("%c:%d\r", runningChars[(i) % NUM_RUNNING_CHARS], i);
#endif
		
		while (posItemset != NULL)
		{
			itemset = freqItemsets->GetAt(posItemset);
			posItems = itemset->GetHeadPosition();

			while (posItems != NULL)
			{

				id = itemset->GetAt(posItems)->getFreqItemID();

				if (pDocVector->GetAt(id) <= 0)	//didn't contain this item
				{
					break;		
				}
				itemset->GetNext(posItems);
			}

			if (posItems == NULL)	//contain this itemset
			{
				itemset->incNGlobalSupport();
//				DEBUGPrint("*Increment Global Support:");
//				CDCDebug::printFreqItemset(itemset);
				
			}
			if (posItemset != NULL)
				freqItemsets->GetNext(posItemset);
		}
	}

	posItemset = freqItemsets->GetHeadPosition();
	
	POSITION temp = NULL;
	
	int j = 0;
	while (posItemset != NULL)
	{
		itemset = freqItemsets->GetAt(posItemset);
		itemset->calGlobalSupport(numDocs);
		if (itemset->getGlobalSupport() < m_minSupport)
		{
			temp = posItemset;
		}
		freqItemsets->GetNext(posItemset);
		if (temp != NULL)
		{
			CDCFreqItemset* tempSet = freqItemsets->GetAt(temp);
			j++;
			freqItemsets->RemoveAt(temp);
			tempSet->cleanup();
			delete tempSet;
			temp = NULL;
		}
	}

	//DEBUGPrint("In findMinGlobalSupport itemset deleted : %d\n", j);
	return TRUE;
}

BOOL CDCFreqItemMgr::joinKCandidateSets(CDCFreqItemsets *KCandidates)
{
	POSITION pos1 = m_pKMinusItemsets->GetHeadPosition();

	POSITION pos2;

	CDCFreqItemset* freqItemset1 = NULL;
	CDCFreqItemset* freqItemset2 = NULL;

	long counter = 0;
//	DEBUGPrint("Joining... round[%d]\n", count);
	while (pos1 != NULL)
	{

		freqItemset1 = m_pKMinusItemsets->GetNext(pos1);
		
		pos2 = pos1;

		while (pos2 != NULL)
		{
			freqItemset2 = m_pKMinusItemsets->GetNext(pos2);
			
			if (CDCFreqItemset::joinable(freqItemset1, freqItemset2))
			{
				CDCFreqItemset* freqCandidateSet = new CDCFreqItemset();
				if (freqCandidateSet->join(freqItemset1, freqItemset2))
				{
#ifdef DC_SHOW_PROGRESS
					printf("%c:%d\r", runningChars[(counter++) % NUM_RUNNING_CHARS], counter);
#endif
					if (!KCandidates->addFreqItemset(freqCandidateSet))
					{
						ASSERT(FALSE);
						return FALSE;
					}
				}
			}
		}
	}

	return TRUE;
}

void CDCFreqItemMgr::locatePosition(CDCFreqItemsets *freqKItemsets)
{
	if (!freqKItemsets)
	{
		ASSERT(FALSE);
		return;
	}

	POSITION pos1 = freqKItemsets->GetHeadPosition();
	CDCFreqItemsets* f1 = m_pF1;
	POSITION posF1 = f1->GetHeadPosition();

	for (int i = 0; i < m_numF1; i ++)
	{
		m_indexFreqItemset[i] = NULL;
	}

//	CDCFreqItemset* itemsetF1 = f1->GetAt(posF1);

	while (pos1 != NULL)
	{
		CDCFreqItemset* itemset = freqKItemsets->GetAt(pos1);

//		TFreqItemID id1 = itemsetF1->GetHead()->getFreqItemID();
		TFreqItemID id2 = itemset->GetHead()->getFreqItemID();
		if (m_indexFreqItemset[id2] == NULL)
		{
			m_indexFreqItemset[id2] = pos1;
		}
		freqKItemsets->GetNext(pos1);
	}

}

void CDCFreqItemMgr::findInterval(POSITION& start, POSITION& end, TFreqItemID id)
{
	start = NULL;
	end = NULL;

	if (m_indexFreqItemset[id] != NULL)
	{
		start = m_indexFreqItemset[id];
	}

	int tempID = id;
	while (++tempID < m_numF1)
	{
		if (m_indexFreqItemset[tempID] != NULL)
		{
			end = m_indexFreqItemset[tempID];
			break;
		}
	}

}

BOOL CDCFreqItemMgr::isInKMinus(POSITION start, POSITION end, CDCFreqItemset *itemset)
{
	POSITION posKMinus;
	POSITION pos1, pos2;
	CDCFreqItemset* freqKMinusSet = NULL;
	TFreqItemID itemID1, itemID2;

	if (!itemset)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (start == NULL)
	{
		posKMinus = m_pKMinusItemsets->GetHeadPosition();
	}
	else
	{
		posKMinus = start;
	}

	while (posKMinus != end)
	{
		pos1 = itemset->GetHeadPosition();
		freqKMinusSet = m_pKMinusItemsets->GetAt(posKMinus);
		ASSERT(freqKMinusSet);
	
		pos2 = freqKMinusSet->GetHeadPosition();

		while (pos2 != NULL)
		{
			itemID1 = itemset->GetAt(pos1)->getFreqItemID();
			itemID2 = freqKMinusSet->GetAt(pos2)->getFreqItemID();

			if (itemID1 != itemID2)
			{
				break;			//Two itemsets are not equal
			}

			freqKMinusSet->GetNext(pos2);
			itemset->GetNext(pos1);
		}
		if (pos2 == NULL)
		{
			break;				//Found the matching itemset
		}
		m_pKMinusItemsets->GetNext(posKMinus);
	}

	if (posKMinus == end)		//Didn't find the matching itemset
	{
		return FALSE;
	}

	return TRUE;
}
