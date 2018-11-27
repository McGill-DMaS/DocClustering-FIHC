// DCDef.hpp: define the constants/types shared among all classes
//
//////////////////////////////////////////////////////////////////////

#if !defined(DCDEF_HPP)
#define DCDEF_HPP

// Common types
//typedef CList<FLOAT, FLOAT> CDCListFloat;
enum TCompareResult {
    COMPARE_SMALLER = -1,
    COMPARE_EQUAL,
    COMPARE_LARGER
};

// pre-processor
#ifdef _DEBUG
    #define _DEBUG_PRT_INFO
#endif
#define _DEBUG_PRT_INFO

//#define DC_DOCVECTOR_CONVERTOR
//#define DC_SHOW_PROGRESS

#define DEBUGPrint printf       // print in console
//#define DEBUGPrint TRACE      // print in debug window

#define P_ASSERT(p) if (!p) { ASSERT(FALSE); return FALSE; }

// types
typedef CArray<FLOAT, FLOAT> CDCFloatArray;

enum TDCAlgoMode {
    DC_ALGO_MODE_FREQ_ITEMS,
    DC_ALGO_MODE_KMEANS,
    DC_ALGO_MODE_CLUTO
};

// Constants
#define NUM_RUNNING_CHARS                   4
#define DC_STOPWORDS_FILENAME               _T("stop_words.txt")
#define DC_DOCUMENTS_DIRECTORY              _T("") //_T("docSet")

#define DC_XML_FREQITEM_FILENAME            _T("fq")
#define DC_XML_KMEANS_FILENAME              _T("km")
#define DC_XML_CLUTO_FILENAME               _T("cluto")
#define DC_XML_EXT                          _T(".xml");

#define DC_XML_TAG_ROOT                     _T("root")
#define DC_XML_TAG_CLUSTER                  _T("cluster")
#define DC_XML_TAG_DOCUMENT                 _T("document")
#define DC_XML_TAG_DOCUMENTS                _T("documents")
#define DC_XML_TAG_FREQITEM                 _T("description")
#define DC_XML_TAG_FREQITEMS                _T("cluster_freq_items")
#define DC_XML_ATB_FMEASURE                 _T("fmeasure")
#define DC_XML_ATB_OVERALLSIM               _T("overall_sim")
#define DC_XML_ATB_ENTROPY_HIE              _T("entropy_h")
#define DC_XML_ATB_ENTROPY_FLT              _T("entropy_f")
#define DC_XML_ATB_LABEL                    _T("label")
#define DC_XML_ATB_GLOBAL_SUPPORT           _T("global_support")
#define DC_XML_ATB_CLUSTER_SUPPORT          _T("cluster_support")
#define DC_XML_ATB_NUM_CLUSTERS             _T("num_clusters")
#define DC_XML_ATB_NUM_CHILDREN             _T("num_children")
#define DC_XML_ATB_NUM_DOCS_WITH_CHILDREN   _T("num_docs")
#define DC_XML_ATB_NUM_DOCS                 _T("num_docs")
#define DC_XML_ATB_NUM_FREQITEMS            _T("num_items")

// Thresholds
#define DC_GLOBAL_SUPPORT_THRESHOLD         0.1f
#define DC_CLUSTER_SUPPORT_THRESHOLD        0.2f
#define DC_INTERSIM_THRESHOLD               0.0f
#define DC_CF                               0.25

#endif