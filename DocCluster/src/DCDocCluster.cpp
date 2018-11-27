//---------------------------------------------------------------------------
// File:
//      DCDocCluster.h, DCDocCluster.cpp
// Description: 
//      Main application object. Starting point of the whole program.
// History:
//      Feb. 23, 2002   Created by Benjamin Fung
//---------------------------------------------------------------------------

#include "stdafx.h"

#if !defined(DCDOCCLUSTER_H)
    #include "DCDocCluster.h"
#endif

#if !defined(DCCONTROLLER_H)
    #include "DCController.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DC_NUM_ARGS_USAGE1  6
#define DC_NUM_ARGS_USAGE2  3

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

BOOL parseArgs(int nArgs, TCHAR* argv[], FLOAT& globalSupport, FLOAT& clusterSupport, BOOL& bKMeans, int& kClusters, CString& inputDir, CString& solFile, CString& classFile)
{
    if ((nArgs != DC_NUM_ARGS_USAGE1 && nArgs != DC_NUM_ARGS_USAGE2) || !argv) {
        cout << _T("Usage1: DocCluster <global_support> <cluster_support> <run_K-Means> <num_clusters> <input_directory>") << endl;
        cout << _T("global_support -- Global Support.  For frequent items algorithm only.") << endl;
        cout << _T("cluster_support -- Cluster Support.  For frequent items algorithm only.") << endl;
        cout << _T("run_K-Means -- A boolean flag (TRUE/FALSE).") << endl;
        cout << _T("num_clusters -- Desired number of clusters. For either frequent items algorithm or K-Means.") << endl;
        cout << _T("input_directory -- Input Directory.  Either relative path or absolute path is acceptable.") << endl;

        cout << _T("Usage2: DocCluster <solution_file> <class_file>") << endl;
        cout << _T("solution_file -- Solution file from Cluto.") << endl;
        cout << _T("class_file -- Class file specified the correct natural class of each document.") << endl;
        return FALSE;
    }

    if (nArgs == DC_NUM_ARGS_USAGE1) {
        globalSupport = StrToFloat(argv[1]);
        clusterSupport = StrToFloat(argv[2]);

	    if (_tcsicmp(argv[3], _T("TRUE")) == 0)
	        bKMeans = TRUE;
	    else if (_tcsicmp(argv[3], _T("FALSE")) == 0)
		    bKMeans = FALSE;
	    else
            return FALSE;

        kClusters = StrToInt(argv[4]);
        if (bKMeans && kClusters < 1) {
            cout << _T("num_clusters must be > 1 if K-Means is TRUE.") << endl;
            return FALSE;
        }
        inputDir = argv[5];
        return TRUE;
    }
    else if (nArgs == DC_NUM_ARGS_USAGE2) {
        solFile = argv[1];
        classFile = argv[2];
        return TRUE;
    }
    else {
        return FALSE;
    }
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		CString strHello;
		strHello.LoadString(IDS_HELLO);
		//cout << (LPCTSTR)strHello << endl;

        int kClusters = 1;
        CString inputDir, solFile, classFile;
        FLOAT globalSupport, clusterSupport;
        BOOL bKMeans = FALSE;

        if (!parseArgs(argc, argv, globalSupport, clusterSupport, bKMeans, kClusters, inputDir, solFile, classFile)) {
		    cerr << _T("Input Error: invalid arguments") << endl;
		    return 1;
        }

        CDCController controller;
        if (argc == DC_NUM_ARGS_USAGE1) {
            if (bKMeans) {
                if (!controller.initialize(DC_ALGO_MODE_KMEANS))
                    ASSERT(FALSE); 
                if (!controller.runKMeans(kClusters, inputDir))
                    cout << _T("Error occured.") << endl;
            }
            else {
                if (!controller.initialize(DC_ALGO_MODE_FREQ_ITEMS))
                    ASSERT(FALSE); 
                if (!controller.runFreqItems(globalSupport, clusterSupport, kClusters, inputDir))
                    cout << _T("Error occured.") << endl;
            }
        }
        else if (argc == DC_NUM_ARGS_USAGE2) {
            if (!controller.initialize(DC_ALGO_MODE_CLUTO))
                ASSERT(FALSE); 
            if (!controller.runCluto((LPCTSTR) solFile, (LPCTSTR) classFile))
                cout << _T("Error occured.") << endl;
        }
	}

    DEBUGPrint("Bye!\n");
#ifdef _DEBUG
    getch();
#endif
	return nRetCode;
}
