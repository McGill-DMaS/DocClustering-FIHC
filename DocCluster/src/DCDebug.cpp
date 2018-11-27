// DCDebug.cpp: implementation of the CDCDebug class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#if !defined(DCDEBUG_H)
    #include "DCDebug.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCDebug::CDCDebug()
{
	standardVector = new CDCFreqItemsets();
}

CDCDebug::~CDCDebug()
{
	ASSERT(standardVector);
//	DEBUGPrint("CDCDebug::~CDCDebug\n");
	if (standardVector != NULL)
	{
		standardVector->cleanup();
		delete standardVector;
	}
//	DEBUGPrint("CDCDebug::~CDCDebug Done\n");

}

void CDCDebug::printFreqItemsets(CDCFreqItemsets& freqItemsets)
{
	if (freqItemsets.IsEmpty())
	{
		DEBUGPrint("Frequent Itemsets is empty\n");
		return;
	}

	POSITION pos = freqItemsets.GetHeadPosition();

	while (pos != NULL)
	{
		DEBUGPrint(freqItemsets.GetNext(pos)->toString());
	}

	return;
}

void CDCDebug::printDocVector(const CDCDocVector &docVector)
{
    DEBUGPrint(docVector.toString());
}

void CDCDebug::printFreqItemset(const CDCFreqItemset *itemset)
{
	DEBUGPrint(itemset->toString());
}

void CDCDebug::printCluster(const CDCCluster* pCluster)
{
    DEBUGPrint(pCluster->toString());
}

void CDCDebug::printClusters(const CDCClusters* pClusters)
{
	if (pClusters->IsEmpty()) {
		DEBUGPrint("Clusters is empty\n");
		return;
	}

	POSITION pos = pClusters->GetHeadPosition();
	while (pos != NULL) {
		DEBUGPrint(pClusters->GetNext(pos)->toString());
	}
}

void CDCDebug::printTime()
{
    //CTime t = CTime::GetCurrentTime();
    //CString s = t.Format(_T("%H:%M:%S"));
    //DEBUGPrint(_T("Current System Time = %s\n"), s);

    time_t ltime;
    time(&ltime);
    DEBUGPrint(_T("Current System Time = %ld\n"), ltime);
}