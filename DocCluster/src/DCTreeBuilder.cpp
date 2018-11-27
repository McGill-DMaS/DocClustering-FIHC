// DCTreeBuilder.cpp: implementation of the CDCTreeBuilder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include <FLOAT.H>

#if !defined(DCTREEBUILDER_H)
    #include "DCTreeBuilder.h"
#endif

//#define DC_RECOMPUTE_F1

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern char runningChars[];

CDCTreeBuilder::CDCTreeBuilder()
{

}

CDCTreeBuilder::~CDCTreeBuilder()
{

}

BOOL CDCTreeBuilder::initialize(CDCClusterMgr* pClusterMgr)
{
    if (!pClusterMgr)
        return FALSE;

    m_pClusterMgr = pClusterMgr;
    return TRUE;
}

void CDCTreeBuilder::cleanup()
{
}

//---------------------------------------------------------------------------
// Construct the cluster tree
//---------------------------------------------------------------------------
BOOL CDCTreeBuilder::buildTree()
{    
	CDCClusterWH* pClusterWH = NULL;
	m_pClusterMgr->getClusterWH(pClusterWH);

	if(!pClusterWH) {
		ASSERT(FALSE);
		return FALSE;
	}

	CDCClusters allClusters;
	if(!pClusterWH->getAllClustersReverseOrder(allClusters)) {
		ASSERT(FALSE);
		return FALSE;	
	}	

	CDCCluster * pCluster = NULL;
	POSITION pos = allClusters.GetHeadPosition();
	int nClusters = allClusters.GetCount();
	DEBUGPrint(_T("*** Building the tree\n"));
	int counter = 0;
	while (pos != NULL) {
		pCluster = allClusters.GetNext(pos);	
#ifdef DC_SHOW_PROGRESS
		DEBUGPrint("%c%d\r", runningChars[counter % NUM_RUNNING_CHARS], counter++);
#endif
		CDCDocVector* pFreqVec = NULL;
		pCluster ->getFrequencies(pFreqVec);

		CDCDocVector resultOccurences;
		resultOccurences.SetSize(pFreqVec->GetSize(), -1);
		
		if (!pCluster->computeTreeChildrenOccurences(resultOccurences)) {
			ASSERT(FALSE);
			return FALSE;
		}
	
		CDCClusterFreqItemset* pCoreItems = NULL;
		pCluster->getClusterCoreItems(pCoreItems);
		CDCClusters potentialParents;

        // Experiment (FMeasure) shows that don't consider great...grandparents produces best result
		if (!pClusterWH->findPotentialParents(FALSE, pCoreItems, potentialParents)) {
			ASSERT(FALSE);
			return FALSE;
		}

		// set root as its parent, and add it into root's children list
		if (pCoreItems->GetCount() == 1 || potentialParents.GetCount() == 0) {
			CDCCluster* pRoot = NULL;
			pClusterWH->getTreeRoot(pRoot);
			if (!pRoot->addTreeChild(pCluster)) {
				ASSERT(FALSE);
				return FALSE;	
			}
			pCluster->setTreeParent(pRoot);	
		}		
		else {
            // find as its parent who has the highest score from the potential parents set. 
			CDCCluster* pTargetCluster = NULL;
			if (!m_pClusterMgr->getHighestScoreCluster(pClusterWH,
                                                       &resultOccurences,
                                                       &potentialParents, 
                                                       pTargetCluster)) {
				ASSERT(FALSE);
				return FALSE;
			}

			if (!pTargetCluster->addTreeChild(pCluster)) {
				ASSERT(FALSE);
				return FALSE;	
			}
			pCluster->setTreeParent(pTargetCluster);
		}
	}

    // recompute the frequent 1-itemset based on tree children
    if (!calFreqOneItemsetsUsingTreeChildren()) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Calculate the frequent 1-itemsets for each cluster according to their
// tree children.
//---------------------------------------------------------------------------
BOOL CDCTreeBuilder::calFreqOneItemsetsUsingTreeChildren()
{
    DEBUGPrint("*** Recomputing the frequent one itemsets using tree children\n");
    P_ASSERT(m_pClusterMgr);
	CDCClusterWH* pClusterWH = NULL;
	m_pClusterMgr -> getClusterWH(pClusterWH);
	P_ASSERT(pClusterWH);

    CDCClusters* pAllClusters = NULL;
    if (!pClusterWH->getAllClusters(pAllClusters)) {
        ASSERT(FALSE);
        return FALSE;
    }

    CDCDocVector* pFrequencies = NULL;
    int nTotalDocs = 0, counter = 0;
    CDCCluster* pCluster = NULL;
    POSITION pos = pAllClusters->GetHeadPosition();
    while (pos != NULL) {
        pCluster = pAllClusters->GetNext(pos);
        P_ASSERT(pCluster);

        pCluster->getFrequencies(pFrequencies);
        P_ASSERT(pFrequencies);

        CDCDocVector clusterFrequencies;
        clusterFrequencies.SetSize(pFrequencies->GetSize());
        if (!pCluster->computeTreeChildrenFrequencies(clusterFrequencies)) {
            ASSERT(FALSE);
            return FALSE;
        }

        nTotalDocs = pCluster->getNumDocumentsIncludeTreeChildren();
        if (!pCluster->calFreqOneItemsets(&clusterFrequencies, nTotalDocs, m_pClusterMgr->getClusterSupport())) {
            ASSERT(FALSE);
            return FALSE;
        }
#ifdef DC_SHOW_PROGRESS
        printf(_T("%c:%d\r"), runningChars[counter % NUM_RUNNING_CHARS], counter++);
#endif
    }
    return TRUE;
}

//---------------------------------------------------------------------------
// Remove empty clusters
//---------------------------------------------------------------------------
BOOL CDCTreeBuilder::removeEmptyClusters(BOOL bRemoveInternal)
{
	DEBUGPrint("*** Removing empty clusters\n");
	CDCClusterWH* pClusterWH = NULL;
	m_pClusterMgr->getClusterWH(pClusterWH);
	P_ASSERT(pClusterWH);

    CDCClusters* pAllClusters = NULL;
    pClusterWH->getAllClusters(pAllClusters);

	CDCClusters allRevClusters;
	pClusterWH->getAllClustersReverseOrder(allRevClusters);

    int nEmptyPruned = 0, nEmptyInternal = 0;
	CDCCluster* pCluster = NULL;    
	POSITION posCluster = allRevClusters.GetHeadPosition();
	while (posCluster != NULL) {
        pCluster = allRevClusters.GetAt(posCluster);
		P_ASSERT(pCluster);

        if (pCluster->getNumDocuments() == 0) {
            CDCClusters* pChildren = NULL;
		    pCluster->getTreeChildren(pChildren);
		    P_ASSERT(pChildren);

            if (pChildren->GetCount() == 0) {
                // remove this empty leaf node
                // get parent
                CDCCluster* pParent = NULL;
			    pCluster->getTreeParent(pParent);
			    P_ASSERT(pParent);

                // get siblings
                CDCClusters* pSiblings = NULL;
			    pParent->getTreeChildren(pSiblings);
			    P_ASSERT(pSiblings);

                // remove from parents' children list
			    pSiblings->RemoveAt(pSiblings->Find(pCluster));
                pCluster->clearTreeParent();

                // remove from all clusters
                pAllClusters->RemoveAt(pAllClusters->Find(pCluster));
                pCluster = NULL;

                // remove from reverse all clusters
			    POSITION tmpPos = posCluster;
			    allRevClusters.GetNext(posCluster);
			    allRevClusters.RemoveAt(tmpPos);           
 			    ++nEmptyPruned;
            }
            else if (FALSE) {
                // this is an empty internal node
                // merge its children with itself
                CDCCluster* pChild = NULL;
                POSITION posChild = pChildren->GetHeadPosition();
                while (posChild != NULL) {
                    pChild = pChildren->GetNext(posChild);
                    if (!pCluster->mergeCluster(pChild, pAllClusters)) {
                        ASSERT(FALSE);
                        return FALSE;
                    }
                    ++nEmptyInternal;
                }
            }
            else if (bRemoveInternal) {
                // this is an empty internal node
                // get parent
                CDCCluster* pParent = NULL;
			    pCluster->getTreeParent(pParent);
			    P_ASSERT(pParent);

                // move children up
                CDCCluster* pChild = NULL;
                POSITION posChild = pChildren->GetHeadPosition();
                while (posChild != NULL) {
                    pChild = pChildren->GetNext(posChild);
                    ASSERT(pChild);
                    
                    if (!pParent->addTreeChild(pChild)) {
                        ASSERT(FALSE);
                        return FALSE;
                    }
                    pChild->setTreeParent(pParent);
                }
                pChildren->RemoveAll();

                // get siblings
                CDCClusters* pSiblings = NULL;
			    pParent->getTreeChildren(pSiblings);
			    P_ASSERT(pSiblings);

                // remove from parents' children list
			    pSiblings->RemoveAt(pSiblings->Find(pCluster));
                pCluster->clearTreeParent();

                // remove from all clusters
                pAllClusters->RemoveAt(pAllClusters->Find(pCluster));
                pCluster = NULL;

                // remove from reverse all clusters
			    POSITION tmpPos = posCluster;
			    allRevClusters.GetNext(posCluster);
			    allRevClusters.RemoveAt(tmpPos);
                ++nEmptyInternal;
            }
            else {
                allRevClusters.GetNext(posCluster);
            }
		}
        else {
            allRevClusters.GetNext(posCluster);
        }
    }
    DEBUGPrint(_T("%d internal empty clusters are pruned!\n"), nEmptyInternal);
    DEBUGPrint(_T("%d empty clusters are pruned!\n"), nEmptyPruned);
    return TRUE;
}

//---------------------------------------------------------------------------
// Prune the tree based on pessimistic error rate
//---------------------------------------------------------------------------
BOOL CDCTreeBuilder::pruneTree()
{
    DEBUGPrint("*** Pruning the tree\n");
	CDCClusterWH* pClusterWH = NULL;
	m_pClusterMgr->getClusterWH(pClusterWH);
	P_ASSERT(pClusterWH);

	CDCCluster* pRoot = NULL;
	pClusterWH->getTreeRoot(pRoot);
	P_ASSERT(pRoot);

	CDCClusters allClusters;
	pClusterWH->getAllClustersReverseOrder(allClusters);
	if (allClusters.GetCount() == 0)
		return TRUE;

	POSITION posCluster = allClusters.GetHeadPosition();
	CDCCluster* pCluster = NULL;

	CDCCluster* pParent = NULL;
	CDCClusters* pChildren = NULL;
	double numDocuments = 0;
	double pessChildrenError = 0;
	double pessParentError = 0;
	double pessErrorAfter = 0;

	POSITION posChildCluster = NULL;
	FLOAT score = -1;

	CDCDocVector* pOccurences = NULL;
	CDCDocVector* pFrequencies = NULL;
	CDCDocVector clusterFrequencies;
	CDCDocVector afterOccurences;

	int nTreePruned = 0, counter = 0;
	while (posCluster != NULL)
	{
		pCluster = allClusters.GetAt(posCluster);
		P_ASSERT(pCluster);

		pCluster->getTreeChildren(pChildren);
		P_ASSERT(pChildren);

		if (pChildren->GetCount() != 0)
		{
			pCluster->setStatus(TOUCHED);
			allClusters.GetNext(posCluster);
			continue;
		}
#ifdef DC_SHOW_PROGRESS
		printf(_T("%c:%d\r"), runningChars[counter % NUM_RUNNING_CHARS], counter++);
#endif
		switch (pCluster->getStatus())
		{
		case UNTOUCHED:
			pCluster->getTreeParent(pParent);

			P_ASSERT(pParent);


			if (pParent != pRoot)
			{
				FLOAT errTemp = 0;
				int numDocChildren = 0;
				int numTemp = 0;
				pessChildrenError = 0;
				pessParentError = 0;
				pessErrorAfter = 0;

				pParent->getTreeChildren(pChildren);
				
				P_ASSERT(pChildren);
				
				posChildCluster = pChildren->GetHeadPosition();

				P_ASSERT(posChildCluster);

				CDCCluster* pChildCluster = pChildren->GetAt(posChildCluster);
				pChildCluster->getOccurences(pOccurences);

				clusterFrequencies.RemoveAll();
				afterOccurences.RemoveAll();
				clusterFrequencies.SetSize(pOccurences->GetSize());
				afterOccurences.SetSize(pOccurences->GetSize());

				BOOL noChildren = TRUE;
				while (posChildCluster != NULL)
				{
					pChildCluster = pChildren->GetNext(posChildCluster);

					if ((pChildCluster->getNumTreeChildren() != 0))
					{
						noChildren = FALSE;
						break;
					}
					pChildCluster->getOccurences(pOccurences);
//					DEBUGPrint(pOccurences->toString());

					pChildCluster->getFrequencies(pFrequencies);
//					DEBUGPrint(pFrequencies->toString());

                    ASSERT(pChildCluster->getNumTreeChildren() == 0);
					
					clusterFrequencies.addUp(pFrequencies);
					
					m_pClusterMgr->calClusterScoreV1(pClusterWH, pOccurences, pChildCluster, score);
					
					numTemp = pChildCluster->getNumDocuments();
					numDocChildren += numTemp;

                    UINT sumOccurences = pOccurences->getSum();
					if (score / sumOccurences > 1)
					{
						ASSERT(FALSE);
						return FALSE;
					}

					errTemp = numTemp * (1 - (score / sumOccurences));
					
					pessChildrenError += (errTemp + calculatePessimisticError(errTemp, numTemp )); 
				}

				if (noChildren)
				{
					pParent->getOccurences(pOccurences);
					pParent->getFrequencies(pFrequencies);
					clusterFrequencies.addUp(pFrequencies);

					// backup the old frequent items of parent (perform a deep copy)
					CDCClusterFreqItemset oldFreqItems;
					if (!pParent->copyClusterFreqItems(oldFreqItems)) {
						ASSERT(FALSE);
						oldFreqItems.cleanup();
						return FALSE;
					}

					m_pClusterMgr->calClusterScoreV1(pClusterWH, pOccurences, pParent, score);
					numTemp = pParent->getNumDocuments();

					UINT sumOccurences = pOccurences->getSum();
					if (score / sumOccurences > 1)
					{
						ASSERT(FALSE);
						oldFreqItems.cleanup();
						return FALSE;
					}
					errTemp = numTemp * (1 - (score / sumOccurences));
					pessParentError = errTemp + calculatePessimisticError(errTemp, numTemp);

					pParent->computeTreeChildrenOccurences(afterOccurences);
					numTemp += numDocChildren;
					ASSERT(numTemp == pParent->getNumDocumentsIncludeTreeChildren());
					if (!pParent->calFreqOneItemsets(&clusterFrequencies, numTemp, m_pClusterMgr->getClusterSupport())) {
						ASSERT(FALSE);
						oldFreqItems.cleanup();
						return FALSE;
					}
					if (!m_pClusterMgr->calClusterScoreV1(pClusterWH, &afterOccurences, pParent, score)) {
						ASSERT(FALSE);
						oldFreqItems.cleanup();
						return FALSE;
					}

					sumOccurences = afterOccurences.getSum();
					if (score / sumOccurences > 1) {
						ASSERT(FALSE);
						oldFreqItems.cleanup();
						return FALSE;
					}

					errTemp = numTemp * (1- (score / sumOccurences));
					pessErrorAfter = errTemp + calculatePessimisticError(errTemp, numTemp);

					if ((pessChildrenError + pessParentError ) >= pessErrorAfter)
					{
						// delete the backup items, since it is useless now.
						oldFreqItems.cleanup();
                        int nChildrenPruned = pParent->pruneAllChildren();
                        if (nChildrenPruned == -1) {
							ASSERT(FALSE);                        
							return FALSE;
						}

                        nTreePruned += nChildrenPruned;
					}
					else 
					{
						// restore the cluster's frequent itemset.
						// the cluster is responsible to delete the itemset.
						pParent->setClusterFreqItems(&oldFreqItems);
						if (!pParent->touchAllChildren())
						{
							ASSERT(FALSE);
							return FALSE;
						}	
					};
				}
				else
				{
					if (!pParent->touchAllChildren())
					{
						ASSERT(FALSE);
						return FALSE;
					}	
				}
			}
			break;
		case TOUCHED:
		case PRUNED:
			break;
		default:
			DEBUGPrint("Wrong Cluster Status!\n");
			ASSERT(FALSE);
			return FALSE;
		}
		allClusters.GetNext(posCluster);

	}				
    DEBUGPrint(_T("%d children clusters are pruned!\n"), nTreePruned);

//#ifdef DC_RECOMPUTE_F1
#if 0
    /* Experiment (FMeasure) shows that not recomputing produces better result */    
    // recompute the frequent 1-itemset based on tree children
    if (!calFreqOneItemsetsUsingTreeChildren()) {
        ASSERT(FALSE);
        return FALSE;
    }
#endif
	return TRUE;
}

//---------------------------------------------------------------------------
// Calculate the pessimistic error
// err: observed errors
// num: # of docs
//---------------------------------------------------------------------------
double CDCTreeBuilder::calculatePessimisticError(double err, double num)
{
	static double Val[] = {0, 0.001, 0.005, 0.01, 0.05, 0.10, 0.20, 0.40, 1.00};
	static double Dev[] = {100, 3.09, 2.58, 2.33, 1.65, 1.28, 0.84, 0.25, 0.00};

	static double Coeff = 0;

	double Val0, Pr;

	if (! Coeff)
	{
		//Compute and retain the coefficient value, interpolating from the values in Val and Dev

		int i = 0;
		
		while ( DC_CF > Val[i] )
		{
			i++;
		}

		Coeff = Dev[i - 1] + (Dev[i] - Dev[i-1]) * (DC_CF - Val[i-1]) / (Val[i] - Val[i-1]);
		Coeff = Coeff * Coeff;
	}

	if (err < 1E-6)
	{
		return num * (1 - exp(log(DC_CF) / num));
	}
	else
	{
		if ( err < 0.9999 )
		{
			Val0 = num * (1 - exp(log(DC_CF) / num));
			return Val0 + err * (calculatePessimisticError(1.0, num) - Val0);
		}
		else
		{
			if ( err + 0.5 >= num)
			{
				return 0.67 * (num - err);
			}
			else
			{
				Pr = (err + 0.5 + Coeff / 2 + sqrt(Coeff * ((err + 0.5) * (1- (err + 0.5) / num) + Coeff / 4 ))) / (num + Coeff);
				return (num * Pr - err);
			}
		}
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCTreeBuilder::pruneChildren()
{
    DEBUGPrint(_T("*** Pruning children clusters based on inter-cluster similarity with parent\n"));
	CDCClusterWH* pClusterWH = NULL;
	m_pClusterMgr->getClusterWH(pClusterWH);
	P_ASSERT(pClusterWH);

    // get all the clusters from bottom-up
	CDCClusters allRevClusters;
	pClusterWH->getAllClustersReverseOrder(allRevClusters);
	if (allRevClusters.GetCount() == 0)
		return TRUE;

    int nPruned = 0, nTotalPruned = 0;
	CDCCluster* pCluster = NULL;    
	POSITION pos = allRevClusters.GetHeadPosition();
	while (pos != NULL) {
#ifdef DC_SHOW_PROGRESS
        printf(_T("%c:%d\r"), runningChars[nTotalPruned % NUM_RUNNING_CHARS], nTotalPruned);
#endif
        pCluster = allRevClusters.GetNext(pos);
		P_ASSERT(pCluster);

        // skip leaf node
        if (pCluster->getNumTreeChildren() == 0)
            continue;

        // merge children of this cluster
        if (!pruneChildren(pCluster, 0.2f, nPruned)) {
        //if (!pruneChildren(pCluster, nPruned)) {
            ASSERT(FALSE);
            return FALSE;
        }
        nTotalPruned += nPruned;
    }
    DEBUGPrint(_T("%d children clusters are pruned!\n"), nTotalPruned);
    return TRUE;
}

//---------------------------------------------------------------------------
// Prune children of the given cluster with a threshold inter-cluster similarity
//---------------------------------------------------------------------------
BOOL CDCTreeBuilder::pruneChildren(CDCCluster* pParentCluster, FLOAT minInterSimThreshold, int& nPruned)
{
    nPruned = 0;
    CDCClusters* pChildren = NULL;
	pParentCluster->getTreeChildren(pChildren);
	P_ASSERT(pChildren);

    int nChildren = pChildren->GetCount();
    FLOAT* arySim = new FLOAT[nChildren];
    CDCCluster** aryCluster = new CDCCluster*[nChildren];

    // compute inter-sim with each child
    int idx = 0;
    FLOAT interSim = 0.0f;
    CDCCluster* pChild = NULL;
	POSITION pos = pChildren->GetHeadPosition();	
	while (pos != NULL) {        
		pChild = pChildren->GetNext(pos);
        if (!m_pClusterMgr->calculateInterSimilarity(pParentCluster, pChild, interSim)) {
            ASSERT(FALSE);
            delete [] arySim;
            delete [] aryCluster;
            return FALSE;
        }

        arySim[idx] = interSim;
        aryCluster[idx] = pChild;
        ++idx;
    }
    
    // get all clusters list
	CDCClusterWH* pClusterWH = NULL;
	m_pClusterMgr->getClusterWH(pClusterWH);
	ASSERT(pClusterWH);
    CDCClusters* pAllClusters = NULL;
    pClusterWH->getAllClusters(pAllClusters);
    ASSERT(pAllClusters);

    // merge
    for (int i = 0; i < idx; ++i) {
        if (arySim[i] > minInterSimThreshold) {
            if (!pParentCluster->mergeCluster(aryCluster[i], pAllClusters)) {
                ASSERT(FALSE);
                delete [] arySim;
                delete [] aryCluster;
                return FALSE;
            }            
            ++nPruned;
        }
    }
    delete [] arySim;
    delete [] aryCluster;
    return TRUE;
}

//---------------------------------------------------------------------------
// Prune children of the given cluster based on individual pessimistic error
// rate
//---------------------------------------------------------------------------
BOOL CDCTreeBuilder::pruneChildren(CDCCluster* pParentCluster, int& nPruned)
{
    nPruned = 0;
    ASSERT(m_pClusterMgr);
	CDCClusterWH* pClusterWH = NULL;
	m_pClusterMgr->getClusterWH(pClusterWH);
	ASSERT(pClusterWH);

    CDCClusters* pChildren = NULL;
	pParentCluster->getTreeChildren(pChildren);
	ASSERT(pChildren);

    int nDocs;
    FLOAT errTemp = 0.0f, normScore = 0.0f;

    // calculate parent error rate (before prune)    
    CDCDocVector* pParentOccurences = NULL;
    pParentCluster->getOccurences(pParentOccurences);
    if (!m_pClusterMgr->calNormalizedScore(pClusterWH, pParentOccurences, pParentCluster, normScore)) {
        ASSERT(FALSE);
        return FALSE;
    }
    nDocs = pParentCluster->getNumDocuments();
	errTemp = nDocs * (1 - normScore);
	DOUBLE pessErrParent = errTemp + calculatePessimisticError(errTemp, nDocs);
    
    CDCDocVector* pOccurences = NULL;
    int nChildren = pChildren->GetCount();
    FLOAT* beforeErrs = new FLOAT[nChildren];
    FLOAT* afterErrs = new FLOAT[nChildren];
    CDCCluster** clusterAry = new CDCCluster*[nChildren];

    // calculate children error rates (before prune)
    int idx = 0, nDocsChild = 0;
    CDCCluster* pChild = NULL;
	POSITION pos = pChildren->GetHeadPosition();
	while (pos != NULL) {        
        pOccurences = NULL;
		pChild = pChildren->GetNext(pos);          
        pChild->getOccurences(pOccurences);
        nDocs = pChild->getNumDocuments();
        if (!m_pClusterMgr->calNormalizedScore(pClusterWH, pOccurences, pChild, normScore)) {
            ASSERT(FALSE);
            delete [] beforeErrs;
            delete [] afterErrs;
            delete [] clusterAry;
            return FALSE;
        }        
        errTemp = nDocs * (1 - normScore);        

        // error rate before prune
        beforeErrs[idx] = errTemp + calculatePessimisticError(errTemp, nDocs) + pessErrParent;
        clusterAry[idx] = pChild;
        ++idx;
    }

    // calculate children error rates (after prune)
    idx = 0;    
	pos = pChildren->GetHeadPosition();
	while (pos != NULL) {        
        pOccurences = NULL;
		pChild = pChildren->GetNext(pos);                
        pChild->getOccurences(pOccurences);
        nDocs = pChild->getNumDocuments() + pParentCluster->getNumDocuments();;

        CDCDocVector parentChildOccurences;      
        if (!parentChildOccurences.addUp(pParentOccurences)) {
            ASSERT(FALSE);
            delete [] beforeErrs;
            delete [] afterErrs;
            delete [] clusterAry;
            return FALSE;
        }
        if (!parentChildOccurences.addUp(pOccurences)) {
            ASSERT(FALSE);
            delete [] beforeErrs;
            delete [] afterErrs;
            delete [] clusterAry;
            return FALSE;
        }

        if (!m_pClusterMgr->calNormalizedScore(pClusterWH, &parentChildOccurences, pParentCluster, normScore)) {
            ASSERT(FALSE);
            delete [] beforeErrs;
            delete [] afterErrs;
            delete [] clusterAry;
            return FALSE;
        }        
        errTemp = nDocs * (1 - normScore);        

        // error rate before prune
        afterErrs[idx] = errTemp + calculatePessimisticError(errTemp, nDocs);
        ++idx;
    }

    // get all clusters list
    CDCClusters* pAllClusters = NULL;
    pClusterWH->getAllClusters(pAllClusters);
    ASSERT(pAllClusters);

    // merge
    for (int i = 0; i < idx; ++i) {
        if (beforeErrs[i] > afterErrs[i]) {
            if (!pParentCluster->mergeClusterPruneChildren(clusterAry[i], pAllClusters)) {
                ASSERT(FALSE);
                delete [] beforeErrs;
                delete [] afterErrs;
                delete [] clusterAry;
                return FALSE;
            }
            ++nPruned;
        }
    }
    delete [] beforeErrs;
    delete [] afterErrs;
    delete [] clusterAry;
    return TRUE;
}

//---------------------------------------------------------------------------
// Prune the tree based on inter-cluster similarity
// kClusters = minimum # of clusters
//---------------------------------------------------------------------------
BOOL CDCTreeBuilder::interSimPrune(int kClusters)
{
    DEBUGPrint(_T("*** Pruning clusters based on inter-cluster similarity\n"));
	CDCClusterWH* pClusterWH = NULL;
	m_pClusterMgr->getClusterWH(pClusterWH);
	P_ASSERT(pClusterWH);

    // get all the clusters from bottom-up
	CDCClusters allRevClusters;
	pClusterWH->getAllClustersReverseOrder(allRevClusters);
	if (allRevClusters.GetCount() == 0)
		return TRUE;

    int nSiblings = 0, nParents = 0, nTotalSiblings = 0, nTotalParents = 0, counter = 0;
/*
	CDCCluster* pCluster = NULL;    
	POSITION pos = allRevClusters.GetHeadPosition();
	while (pos != NULL) {
#ifdef DC_SHOW_PROGRESS
        printf(_T("%c:%d\r"), runningChars[counter % NUM_RUNNING_CHARS], counter++);
#endif
        pCluster = allRevClusters.GetNext(pos);
		P_ASSERT(pCluster);

        // skip leaf node
        if (pCluster->getNumTreeChildren() == 0)
            continue;

        // merge children of this cluster
        if (!mergeChildren(pCluster, FALSE, 0, DC_INTERSIM_THRESHOLD, nSiblings, nParents)) {
            ASSERT(FALSE);
            return FALSE;
        }
        nTotalSiblings += nSiblings;
        nTotalParents += nParents;
    }
    DEBUGPrint(_T("%d children clusters are merged with sibling!\n"), nTotalSiblings);
    DEBUGPrint(_T("%d children clusters are merged with parent!\n"), nTotalParents);
*/
    // merge level 1 clsuters
    CDCCluster* pRoot = NULL;
    if (!pClusterWH->getTreeRoot(pRoot)) {
        ASSERT(FALSE);
        return FALSE;
    }
    if (kClusters == 0) {
        if (!mergeChildren(pRoot, FALSE, kClusters, DC_INTERSIM_THRESHOLD, nSiblings, nParents)) {
            ASSERT(FALSE);
            return FALSE;
        }
    }
    else {
        if (!mergeChildren(pRoot, FALSE, kClusters, 0.0f, nSiblings, nParents)) {
            ASSERT(FALSE);
            return FALSE;
        }        
    }
    ASSERT(nParents == 0);
    DEBUGPrint(_T("%d clusters at level 1 are merged with sibling!\n"), nSiblings);
    return TRUE;
}

//---------------------------------------------------------------------------
// Merge the children of the given parent cluster based on inter-cluster
// similarity. If kClusters == 0, then use minInterSimThreshold as a stopping 
// criteria.
//---------------------------------------------------------------------------
BOOL CDCTreeBuilder::mergeChildren(CDCCluster* pParentCluster, BOOL bMerParent, int kClusters, FLOAT minInterSimThreshold, int& mergedSiblingCount, int& mergedParentCount)
{
    mergedSiblingCount = mergedParentCount = 0;
	CDCClusters* pChildren = NULL;
	CDCCluster* pCluster1 = NULL;
	CDCCluster* pCluster2 = NULL;
	POSITION posChildren1;
	POSITION posChildren2;

	pParentCluster->getTreeChildren(pChildren);
	P_ASSERT(pChildren);

    int numChildren = pChildren->GetCount();
    if (numChildren <= kClusters)
        return TRUE;

    FLOAT* aryInterSimilarities = NULL;
    CDCCluster** aryPositions1 = NULL;
    CDCCluster** aryPositions2 = NULL;
    if (!bMerParent) {
	    aryInterSimilarities = new FLOAT[numChildren * (numChildren - 1) / 2];
	    aryPositions1 = new CDCCluster*[numChildren * (numChildren - 1) / 2];
	    aryPositions2 = new CDCCluster*[numChildren * (numChildren - 1) / 2];
    }
    else {
	    aryInterSimilarities = new FLOAT[numChildren * (numChildren - 1) / 2 + numChildren];
	    aryPositions1 = new CDCCluster*[numChildren * (numChildren - 1) / 2 + numChildren];
	    aryPositions2 = new CDCCluster*[numChildren * (numChildren - 1) / 2 + numChildren];
    }

    int count = 0;
	posChildren1 = pChildren->GetHeadPosition();	
	while (posChildren1 != NULL) {        
		pCluster1 = pChildren->GetAt(posChildren1);
		if (!pCluster1) {
			delete [] aryInterSimilarities;
			delete [] aryPositions1;
			delete [] aryPositions2;
			ASSERT(FALSE);
			return FALSE;
		}

        // compute inter-cluster similarity with each sibling
        FLOAT interSim = 0.0f;
		posChildren2 = posChildren1;
		pChildren->GetNext(posChildren2);
		while (posChildren2 != NULL) {
			pCluster2 = pChildren->GetAt(posChildren2);
			if (!pCluster2) {
				delete [] aryInterSimilarities;
				delete [] aryPositions1;
				delete [] aryPositions2;
				ASSERT(FALSE);
				return FALSE;
			}			
			if (!m_pClusterMgr->calculateInterSimilarity(pCluster1, pCluster2, interSim)) {
				delete [] aryInterSimilarities;
				delete [] aryPositions1;
				delete [] aryPositions2;
				ASSERT(FALSE);
				return FALSE;
			}
			aryInterSimilarities[count] = interSim;
			aryPositions1[count] = pCluster1;
			aryPositions2[count] = pCluster2;
			count++;
			pChildren->GetNext(posChildren2);
		}

        if (bMerParent) {
            // compute inter-cluster similarity with parent
		    if (!m_pClusterMgr->calculateInterSimilarity(pCluster1, pParentCluster, interSim)) {
			    delete [] aryInterSimilarities;
			    delete [] aryPositions1;
			    delete [] aryPositions2;
			    ASSERT(FALSE);
			    return FALSE;
		    }
            aryInterSimilarities[count] = interSim;
            aryPositions1[count] = pCluster1;
            aryPositions2[count] = pParentCluster;
            count++;        
        }

		pChildren->GetNext(posChildren1);
	}

    // find the clusters to be merged
    int highIndex = -1;
    while ((highIndex = findHighestInterSimilarity(aryInterSimilarities, count, minInterSimThreshold)) != -1) {
        if (numChildren <= kClusters)
            break;

		pCluster1 = aryPositions1[highIndex];
		pCluster2 = aryPositions2[highIndex];
		if (!pCluster1 || !pCluster2) {
			delete [] aryInterSimilarities;
			delete [] aryPositions1;
			delete [] aryPositions2;
			ASSERT(FALSE);
			return FALSE;
		}

        // swap if necessary
        if (pCluster2 == pParentCluster) {
            CDCCluster* pTempCluster = pCluster2;
            pCluster2 = pCluster1;
            pCluster1 = pTempCluster;
        }
        else if (pCluster1->getNumDocumentsIncludeTreeChildren() < pCluster2->getNumDocumentsIncludeTreeChildren()) {
            CDCCluster* pTempCluster = pCluster2;
            pCluster2 = pCluster1;
            pCluster1 = pTempCluster;
        }

        // merge cluster2 to cluster1
        if (!pCluster1->mergeCluster(pCluster2, NULL)) {
			delete [] aryInterSimilarities;
			delete [] aryPositions1;
			delete [] aryPositions2;
            ASSERT(FALSE);
            return FALSE;
        }
#ifdef DC_SHOW_PROGRESS
        printf(_T("%c:%d\r"), runningChars[(mergedParentCount + mergedSiblingCount) % NUM_RUNNING_CHARS], mergedParentCount + mergedSiblingCount);
#endif
        //printf("merged sim = %f\n", aryInterSimilarities[highIndex]);        
        --numChildren;

        if (pCluster1 == pParentCluster) {            
            ++mergedParentCount;
        }
        else {
            ++mergedSiblingCount;
        }

        // cleanup inter-similarity of cluster2
        if (!cleanSimilarity(pCluster2, aryPositions1, aryPositions2, aryInterSimilarities, count)) {
			delete [] aryInterSimilarities;
			delete [] aryPositions1;
			delete [] aryPositions2;
            ASSERT(FALSE);
            return FALSE;
        }

        // update the inter-similarity among clusters
		posChildren2 = pChildren->GetHeadPosition();
		while (posChildren2 != NULL) {
			pCluster2 = pChildren->GetAt(posChildren2);
			
			if (pCluster2 == pCluster1) {
				pChildren->GetNext(posChildren2);
				continue;
			}

			if (!pCluster2) {
				delete [] aryInterSimilarities;
				delete [] aryPositions1;
				delete [] aryPositions2;
				ASSERT(FALSE);
				return FALSE;
			}

			int index = findClusterIndex(pCluster1, aryPositions1, pCluster2, aryPositions2, count);
			if (index == -1) {
				delete [] aryInterSimilarities;
				delete [] aryPositions1;
				delete [] aryPositions2;
				ASSERT(FALSE);
				return FALSE;
			}

			FLOAT interSim = 0;
			if (!m_pClusterMgr->calculateInterSimilarity(pCluster1, pCluster2, interSim)) {
				delete [] aryInterSimilarities;
				delete [] aryPositions1;
				delete [] aryPositions2;
				ASSERT(FALSE);
				return FALSE;
			}
			aryInterSimilarities[index] = interSim;
			pChildren->GetNext(posChildren2);
		}
	}
	delete [] aryInterSimilarities;
	delete [] aryPositions1;
	delete [] aryPositions2;
	return TRUE;
}

//---------------------------------------------------------------------------
// Similarity based pruning (using inter-cluster similarity)
//---------------------------------------------------------------------------
BOOL CDCTreeBuilder::simBasedPrune(int kClusters)
{
    if (kClusters <= 0)
        return TRUE;

    DEBUGPrint(_T("*** Merging clusters based on similarity\n"));
	CDCClusterWH* pWH;
	CDCCluster* pRoot;
	CDCClusters* pF1Children;

	CDCCluster* pCluster1;
	CDCCluster* pCluster2;
	POSITION posChildren1;
	POSITION posChildren2;

	m_pClusterMgr->getClusterWH(pWH);
	P_ASSERT(pWH);

	pWH->getTreeRoot(pRoot);
	P_ASSERT(pRoot);

	pRoot->getTreeChildren(pF1Children);
	P_ASSERT(pF1Children);

	int numF1 = pF1Children->GetCount();
    if (numF1 <= kClusters)
        return TRUE;

    int runs = numF1 - kClusters;

	FLOAT* aryInterSimilarities = new FLOAT[numF1*(numF1-1)/2];
	CDCCluster** aryPositions1 = new CDCCluster*[numF1*(numF1-1)/2];
	CDCCluster** aryPositions2 = new CDCCluster*[numF1*(numF1-1)/2];

	posChildren1 = pF1Children->GetHeadPosition();
	int count = 0;

	int counter = 0;

	while (posChildren1 != NULL)
	{
#ifdef DC_SHOW_PROGRESS
        printf(_T("%c:%d\r"), runningChars[counter%NUM_RUNNING_CHARS], counter++);
#endif
		pCluster1 = pF1Children->GetAt(posChildren1);
		if (!pCluster1)
		{
			delete [] aryInterSimilarities;
			delete [] aryPositions1;
			delete [] aryPositions2;
			ASSERT(FALSE);
			return FALSE;
		}

		posChildren2 = posChildren1;
		pF1Children->GetNext(posChildren2);

		while (posChildren2 != NULL)
		{
			pCluster2 = pF1Children->GetAt(posChildren2);
			if (!pCluster2)
			{
				delete [] aryInterSimilarities;
				delete [] aryPositions1;
				delete [] aryPositions2;
				ASSERT(FALSE);
				return FALSE;
			}
			FLOAT interSim = 0;
			if (!m_pClusterMgr->calculateInterSimilarity(pCluster1, pCluster2, interSim))
			{
				delete [] aryInterSimilarities;
				delete [] aryPositions1;
				delete [] aryPositions2;
				ASSERT(FALSE);
				return FALSE;
			}
			aryInterSimilarities[count] = interSim;
			aryPositions1[count] = pCluster1;
			aryPositions2[count] = pCluster2;
			count++;
			pF1Children->GetNext(posChildren2);
		}
		pF1Children->GetNext(posChildren1);
	}

    int mergedCount = 0;
    int highIndex = -1;
	for (int i = 0; i < runs; i ++)
	{
#ifdef DC_SHOW_PROGRESS
        printf(_T("%c:%d\r"), runningChars[counter%NUM_RUNNING_CHARS], i);
#endif
		highIndex = findHighestInterSimilarity(aryInterSimilarities, count, 0.0f);
		if (highIndex == -1)
		{
			break;
		}

		pCluster1 = aryPositions1[highIndex];
		pCluster2 = aryPositions2[highIndex];
		if (!pCluster1 || !pCluster2) {
			delete [] aryInterSimilarities;
			delete [] aryPositions1;
			delete [] aryPositions2;
			ASSERT(FALSE);
			return FALSE;
		}

        // swap if necessary
        if (pCluster1->getNumDocumentsIncludeTreeChildren() < pCluster2->getNumDocumentsIncludeTreeChildren()) {
            CDCCluster* pTempCluster = pCluster2;
            pCluster2 = pCluster1;
            pCluster1 = pTempCluster;
        }

        if (!pCluster1->mergeCluster(pCluster2, NULL)) {
			delete [] aryInterSimilarities;
			delete [] aryPositions1;
			delete [] aryPositions2;
            ASSERT(FALSE);
            return FALSE;
        }
        ++mergedCount;
		
        if (!cleanSimilarity(pCluster2, aryPositions1, aryPositions2, aryInterSimilarities, count)) {
			delete [] aryInterSimilarities;
			delete [] aryPositions1;
			delete [] aryPositions2;
            ASSERT(FALSE);
            return FALSE;
        }

		posChildren2 = pF1Children->GetHeadPosition();
		while (posChildren2 != NULL)
		{
			pCluster2 = pF1Children->GetAt(posChildren2);
			
			if (pCluster2 == pCluster1)
			{
				pF1Children->GetNext(posChildren2);
				continue;
			}

			if (!pCluster2)
			{
				delete [] aryInterSimilarities;
				delete [] aryPositions1;
				delete [] aryPositions2;
				ASSERT(FALSE);
				return FALSE;
			}

			int index = findClusterIndex(pCluster1, aryPositions1, pCluster2, aryPositions2, count);

			if (index == -1)
			{
				delete [] aryInterSimilarities;
				delete [] aryPositions1;
				delete [] aryPositions2;
				ASSERT(FALSE);
				return FALSE;
			}

			FLOAT interSim = 0;
			if (!m_pClusterMgr->calculateInterSimilarity(pCluster1, pCluster2, interSim))
			{
				delete [] aryInterSimilarities;
				delete [] aryPositions1;
				delete [] aryPositions2;
				ASSERT(FALSE);
				return FALSE;
			}
			aryInterSimilarities[index] = interSim;
			pF1Children->GetNext(posChildren2);
		}
	}
	delete [] aryInterSimilarities;
	delete [] aryPositions1;
	delete [] aryPositions2;
    DEBUGPrint("%d clusters at level 1 are merged!\n", mergedCount);

//#ifdef DC_RECOMPUTE_F1
#if 0
    /* Experiment (FMeasure) shows that not recomputing produces better result */    
    // recompute the frequent 1-itemset based on tree children
    if (!calFreqOneItemsetsUsingTreeChildren()) {
        ASSERT(FALSE);
        return FALSE;
    }
#endif
	return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int CDCTreeBuilder::findHighestInterSimilarity(const FLOAT* aryInterSimilarities, int length, FLOAT minInterSimThreshold)
{
	P_ASSERT(aryInterSimilarities);

	FLOAT highSim = minInterSimThreshold;
	int highIndex = -1;
	for (int i = 0; i < length; ++i) {
		if (aryInterSimilarities[i] > highSim) {
			highSim = aryInterSimilarities[i];
			highIndex = i;
		}
	}
	return highIndex;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int CDCTreeBuilder::findClusterIndex(CDCCluster* pos1, CDCCluster** aryPositions1, CDCCluster* pos2, CDCCluster** aryPositions2, int length)
{
	for (int i = 0; i < length; i ++) {
		if ((pos1 == aryPositions1[i]) && (pos2 == aryPositions2[i])) {
			return i;
		}
		if ((pos1 == aryPositions2[i]) && (pos2 == aryPositions1[i])) {
			return i;
		}
	}
	return -1;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CDCTreeBuilder::cleanSimilarity(CDCCluster* pos, CDCCluster** aryPositions1, CDCCluster** aryPositions2, FLOAT* arySims, int length)
{
	for (int i = 0; i < length; i++) {
		if (aryPositions1[i] == pos) {
			arySims[i] = -FLT_MAX;
		}

		if (aryPositions2[i] == pos) {
			arySims[i] = -FLT_MAX;
		}
	}
	return TRUE;
}

//---------------------------------------------------------------------------
// Over prune clusters using inter-cluster similarity
//---------------------------------------------------------------------------
BOOL CDCTreeBuilder::interSimOverPrune(int kClusters)
{
    if (kClusters <= 0)
        return TRUE;

    DEBUGPrint(_T("*** Over prune clusters to satisfy specified # of clusters\n"));    
	CDCCluster* pRoot = NULL;
	CDCClusterWH* pClusterWH = NULL;

    ASSERT(m_pClusterMgr);
	m_pClusterMgr->getClusterWH(pClusterWH);
	ASSERT(pClusterWH);

    // get all clusters list
    CDCClusters* pAllGlobalClusters = NULL;
    pClusterWH->getAllClusters(pAllGlobalClusters);
    ASSERT(pAllGlobalClusters);

	pClusterWH->getTreeRoot(pRoot);
	ASSERT(pRoot);

    // get all one item clusters
	CDCClusters* pAllClusters = NULL;
	pRoot->getTreeChildren(pAllClusters);
    P_ASSERT(pAllClusters);
    if (pAllClusters->GetCount() == 0) {
		ASSERT(FALSE);
		return FALSE;
	}
   
    int mergedCount = 0, nMinDocs = INT_MAX;
    FLOAT score = 0.0f, bestScore = -FLT_MAX;
    POSITION pos = NULL, posX = NULL;
    CDCCluster* pCandidate = NULL;
    CDCCluster* pBest = NULL;
    CDCCluster* pClusterX = NULL;
    CDCCluster* pVictim = NULL;
    int nClusters = pAllClusters->GetCount();
    while (nClusters > kClusters && !pAllClusters->IsEmpty()) {
        nMinDocs = INT_MAX;
        posX = pAllClusters->GetHeadPosition();
        while (posX != NULL) {
            pClusterX = pAllClusters->GetNext(posX);
            if (pClusterX->getNumDocumentsIncludeTreeChildren() < nMinDocs) {
                pVictim = pClusterX;
                nMinDocs = pClusterX->getNumDocuments();
            }
        }
        P_ASSERT(pVictim);
        pAllClusters->RemoveAt(pAllClusters->Find(pVictim));

        // pick the most similar cluster from the rest of the list
        pBest = NULL;        
        bestScore = -FLT_MAX;
        pos = pAllClusters->GetHeadPosition();
        while (pos != NULL) {
            pCandidate = pAllClusters->GetNext(pos);

            if (!m_pClusterMgr->calculateInterScore(pVictim, pCandidate, score)) {
            //if (!m_pClusterMgr->calculateInterSimilarity(pVictim, pCandidate, score)) {                
                ASSERT(FALSE);
                return FALSE;
            }

            if (score > bestScore) {
                pBest = pCandidate;
                bestScore = score;
            }
        }
        P_ASSERT(pBest);

        // merge them
        if (!pBest->mergeClusterPruneChildren(pVictim, pAllGlobalClusters)) {
            ASSERT(FALSE);
            return FALSE;
        }
        ++mergedCount;
        --nClusters;
    }
    DEBUGPrint(_T("%d clusters at level 1 are merged!\n"), mergedCount);
    return TRUE;
}
