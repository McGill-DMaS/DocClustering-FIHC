// Configuration file for Database DLLs
#if !defined (DLLCFG_H)
#define DLLCFG_H

//#define STRICT

#if defined (CRW70PRODLL)
    #define CRW50PRODLL
#endif //(CRW70PRODLL)

#if defined (CRW50PRODLL)
    #define CRW50DLL
    #define CRW40PRODLL
#endif //(CRW50PRODLL)
  
#if defined (CRW40PRODLL)
    #define CRW40DLL
    #define CRW30PRODLL
#endif //(CRW40PRODLL)

#if defined (CRW40DLL)
    #define CRW30DLL
    #define PRE_SORTED
    #define RESOURCE_OEMDBMSNAME  //-Allan OEM Resource for driver filter
    #define SUPPORT_GRAPHIC_BLOB
    #define KEYWORD_LIKE          //-John
    #define KEYWORD_STARTWITH     //-John   
  
    #if !defined (WIN32)
        #if defined (SYBASE) //Could add them into mak file! - John
            #define RESOURCE_DLL
        #endif //(SYBASE)
    #endif //!defined (WIN32)

    #if defined (PDSODBC) || defined (SQLSVR)
  	    #define	NULL_VALUE_FOR_SP_PARAMS
        #define PASS_SILENTMODE_AROUND
    #endif //(PDSODBC)

    #if defined (PDSODBC) || defined (SYBASE) || defined (SQLSVR) || defined (ORACLE) || defined (DB2WIN) || defined (DB2SUN) || defined (DB2AIX) || defined (DB2HP) || defined (INFORMIX)
        #define STORED_PROCEDURES
    #endif //defined (PDSODBC) || defined(SYBASE) || defined(SQLSVR) || defined(ORACLE) || defined(DB2WIN) || defined(DB2SUN) || defined (DB2AIX) || defined (DB2HP) || defined (INFORMIX)

    #if defined (STORED_PROCEDURES)
        #define ALLOW_SP_LINKS
        #define UNIFY_STOREDPROC_PARAMETER
    #endif //(STORED_PROCEDURES)

    #define USE_QELIB
    
    #if defined (USE_QELIB)
        #define RENAMED_QEDLL
    #endif //(USE_QELIB)
#endif //(CRW40DLL)

#if defined (CRW30PRODLL)
    #define CRW30DLL
    #define CRW20PRODLL
#endif //(CRW30PRODLL)

#if defined (CRW30DLL)
    #define CRW20DLL
    #define INCL_SQL_TABLES_RESTRICT
    #define INCL_SERVER_OPTIONS
    #define SQL92BRACKETS
#endif //(CRW30DLL)

#ifndef GROUP_BY
  #define GROUP_BY
#endif

// if USE_BIND_DATA is defined for all 32 bit dlls, some types in
// phystype.hpp will not be recognized, as they are defined in 
// \dbdlls\ODBC\32\sqltypes.h and they are not really needed by all
// 32 bit dlls
#if !defined (USE_BIND_DATA) && defined (WIN32) && defined (PDSODBC)
  #define USE_BIND_DATA
#endif // if !defined (USE_BIND_DATA)

#endif //!defined (DLLCFG_H)

