// DCDebug.h: interface for the CDCDebug class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCDEBUG_H)
#define DCDEBUG_H

#if !defined(DCFREQITEM_H)
	#include "DCFREQITEM.h"
#endif

#if !defined(DCDOCVECTOR_H)
	#include "DCDocVector.h"
#endif

#if !defined(DCCLUSTER_H)
    #include "DCCluster.h"
#endif

//#include "iostream.h"

//#define DEBUG_OUTPUT cout

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDCDebug  
{
public:
	CDCDebug();
	virtual ~CDCDebug();

    static void printDocVector(const CDCDocVector& docVector);
	static void printFreqItemsets(CDCFreqItemsets& freqItemsets);		
    static void printFreqItemset(const CDCFreqItemset* itemset);
    static void printCluster(const CDCCluster* pCluster);
    static void printClusters(const CDCClusters* pClusters);
    static void printTime();

	CDCFreqItemsets* standardVector;

};

#endif





