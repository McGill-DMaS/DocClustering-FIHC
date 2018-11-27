//---------------------------------------------------------------------------
// File:
//      BRConfig.h
//
// Objective:
//      Forget about this file
//
// History:
//      Mar. 12, 2002   Created by Benjamin Fung
//
//---------------------------------------------------------------------------

#if !defined (BRCONFIG_H)
#define BRCONFIG_H

// First, synthesize a couple of symbols:  CRW_ONLY and CRPE_ONLY.  These,
// plus CDW and CRW_IN_A_DLL should be used whenever it's necessary to
// test for a particular kind of build.

#if defined (VIEW_FOR_CI50)
    #define VIEW_FOR_CRW50
#endif //(VIEW_FOR_CI50)

#if defined (VIEW_FOR_CRW50) || defined (VIEW_FOR_CI50) || defined (ESSBASE)
    #define PEOPLESOFT          //always define this after 5.0
    #define ALLOW_ACCESS_MDB
#endif //defined (VIEW_FOR_CRW50) || defined (VIEW_FOR_CI50)

#if defined (VIEWER_INIT_REFRESH_ONLY)
    #define REFRESH_VIEWER
#endif //(VIEWER_INIT_REFRESH_ONLY)

#if defined (REFRESH_VIEWER)
    #define VIEWER
#endif //(REFRESH_VIEWER)

#if !defined (CRPE) && !defined (CDW) && !defined (CRW_IN_A_DLL)
    #define CRW_ONLY 1
#endif //!defined (CRPE) && !defined (CDW) && !defined (CRW_IN_A_DLL)

#if defined (CRPE) && !defined (CRW_IN_A_DLL)
    #define CRPE_ONLY 1
#endif //defined (CRPE) && !defined (CRW_IN_A_DLL)

#if defined (CRW_ONLY) || defined (CRW_IN_A_DLL)
    #define CRW 1
#endif //defined (CRW_ONLY) || defined (CRW_IN_A_DLL)

#if defined (CRPE_ONLY) || defined (CRW_IN_A_DLL)
    #define CRPE 1
#endif //defined (CRPE_ONLY) || defined (CRW_IN_A_DLL)

#if defined (INTEL)
    #if !defined (CRW30PRO)
        #define CRW30PRO
    #endif //!defined (CRW30PRO)

    #if !defined (CRPE_ONLY)
        #define INCL_DICT_SHOW_SQL
    #endif //!defined (CRPE_ONLY)
#endif //(INTEL)

#if defined (GREAT_PLAINS)
    #define POOL_PRINTERS
    #define PLATINUM
    #define INCL_FIELDLISTBOX
    #define EXTRA_STATUS_INFO
    #if defined (CRPE_ONLY)
        #define IGNORE_SPECIFIC_PRINTER_LOADFROM
    #endif //(CRPE_ONLY)
#endif //(GREAT_PLAINS)

#if defined (MICROSOFT)
    #define CRW20PRO

    #if !defined (CRPE_ONLY)
        #define CTL3D
        #define PRE_AUTHORIZE_DIALOG
    #endif //!defined (CRPE_ONLY)
#endif //(MICROSOFT)

#if defined (PEACHTREE)
    #define INCL_SETPRINTDATE
    #define INCL_GROUPANALYSIS
    #define CLOSE_BOXES
#endif //(PEACHTREE)

#if defined (AGTECH)
    #define PAINT_BAR_BACKGROUND
    #define RESOURCE_DLL
    #define DBCS
#endif //(AGTECH)

#if defined (BORLAND)
    #define DBASE_FOR_WINDOWS
    #define BUILD_AUTO_REPORT
    #define INCL_BLOB
//  #define SQL_TO_REPORT
//  #define QBE_TO_REPORT

    #if defined (CRPE_ONLY)
        #define MORE_PE_FUNCTIONS
        #define MORE_PE_FUNCTIONS_2
        #define CAN_CREATE_REPORT
        #define INCL_ZOOM_PREVIEW_WINDOW
    #else
        #define USE_NON_BOLD_STATUS_BAR_FONT
        #define MSG_TO_REPORT
        #define SPLASH_DLG
        #define PRE_AUTHORIZE_DIALOG
        #define NAG_ON_HELP
        #define DAILY_NAG_ON_HELP
        #define NEGATIVE_TWIPS_IN_DESIGNWIN
    #endif //(CRPE_ONLY)
#endif //(BORLAND)

#if defined (PEOPLESOFT)
    #define RESIZABLE_INSERT_FROM_VIEW
    #define RESIZABLE_DB_FLD_DLG
//  #define INCL_FIELD_DESCRIPTION
    #define OBEY_HIDDEN_SECTION_FORMAT
    #define INCL_ERRORHANDLEDBYDBDLL
    #define INCL_DLLPROMPTLOGON
    #define DB_DLL_PATH_IN_INI_FILE

    #if !defined (CRPE_ONLY)
        #define COMMAND_DLLS
        #define COMMAND_DLL_PATH_IN_INI_FILE
    #endif //!defined (CRPE_ONLY)
#endif //(PEOPLESOFT)

#if defined (SYMANTEC)
    #define RESOURCE_DLL
    #define EXPORTING
#endif //(SYMANTEC)

#if defined (WALLDATA)
    #define CTL3D
    #define SOFTBRIDGE_BASIC
//  #define UNDO
#endif //(WALLDATA)

#if defined (NT_SERVICE)
    #if defined (CRPE_ONLY)
        // dont get default printer  when running CRPE sys service apps
        #define POOL_PRINTERS
        // don't load any printer info saved with report
        #define IGNORE_SPECIFIC_PRINTER_LOADFROM
        // never use extended Print Engine msgs and alert dialogs
        #define DISABLE_DEBUG_MODE_REPORT_OPTION
    #endif //(CRPE_ONLY)
#endif //(NT_SERVICE)

#if defined (HTML)
    #if defined (CRPE_ONLY)
        // dont get default printer when running CRPE sys service apps
        #define POOL_PRINTERS
        // never use extended Print Engine msgs and alert dialogs
        #define DISABLE_DEBUG_MODE_REPORT_OPTION
    #endif //(CRPE_ONLY)

    // export field adornments/bitmaps/charts/blobs
    #define EXPORT_ADORNMENTS
    #define EXPORT_FRAME
#endif //(HTML)

#if defined (NETSCAPE)
    #if defined (CRW_ONLY)
        // show registration once only on CRW 4.5 startup
        #define NAG_TO_REGISTER_ONCE_ONLY
    #endif //(CRW_ONLY)
#endif //(NETSCAPE)

#if defined (CRW_IN_A_DLL)
    #define BUILD_AUTO_REPORT
#endif //(CRW_IN_A_DLL)

#if defined (CRW50PRO)
    #define CRW50
    #define CRW45PRO
#endif //(CRW50PRO)

#if defined (CRW50)
    #define CRW45
    #define MORE_CROSSTAB_GROUP_OPTIONS
#endif //(CRW50)

#if defined (CRW45PRO)
    #define CRW45
    #define CRW40PRO
#endif //(CRW45PRO)

#if defined (CRW45)
    #define CRW40
    #define PROMPTING_VARIABLES
    #define KEEP_PROMPT_VALUES
    #define INCL_QUERY
    #define EXPORT_ADORNMENTS
    #define EXPORT_FRAME
    #define REUSE_LOGON

    #if !defined (VIEWER) || defined (REFRESH_VIEWER)
        #if !defined (QUERY_FOR_CRW50) && !defined (VIEW_FOR_CRW50) || defined (VIEW_FOR_CI50)
            #define CRS_COLUMN_SECURITY
            #define CRS_ROW_SECURITY
        #endif //!defined (QUERY_FOR_CRW50) && !defined (VIEW_FOR_CRW50) || defined (VIEW_FOR_CI50)
    
        #define SAVE_BROWSED_DATA_IN_DICT
        #define USE_XBASE_DLL_IN_INFOVIEW
    #endif //!defined (VIEWER) || defined (REFRESH_VIEWER)

    #if defined (CRS_COLUMN_SECURITY) || defined (CRS_ROW_SECURITY)
        #define MORE_SECURE
    #endif //defined (CRS_COLUMN_SECURITY) || defined (CRS_ROW_SECURITY)

    #if defined (CRPE_ONLY)
        #define GOVERNORS
    #endif //(CRPE_ONLY)
#endif //(CRW45)

#if defined (CRW40PRO)
    #define CRW40
    #define CRW30PRO
    #define EXPORT_ADORNMENTS
    #define EXPORT_FRAME
#endif //(CRW40PRO)

#if defined (CRW40)
    #define CRW30
    #define SMART_STRING_COMPARES
    #define KEYWORD_LIKE
    #define KEYWORD_STARTWITH
//  #define CRW_SHELL
    #define STORED_PROCEDURES
    #define INDEX_SAVED_RECORDS
    #define KEEP_BATCHES_IN_MEM

    #if defined (KEEP_BATCHES_IN_MEM)
        #define REUSE_RECURRING_RECORD_BUFFER
    #endif //(KEEP_BATCHES_IN_MEM)
  
    #define METAPAGES
    #define USE_SMRTHEAP
    
    #if defined (KEEP_BATCHES_IN_MEM)
        #define USE_ALL_RECS_IN_MEM
        #if defined (USE_ALL_RECS_IN_MEM)
            #if defined (USE_SMRTHEAP)
                #define METAPAGES2
            #endif //(USE_SMRTHEAP)
        #endif //(USE_ALL_RECS_IN_MEM)
    #endif //(KEEP_BATCHES_IN_MEM)

//  #define ALLOW_SP_LINKS

    #if defined (INDEX_SAVED_RECORDS)
        #define PRE_SORTED
    #endif //(INDEX_SAVED_RECORDS)
  
    #define INCL_BLOB
    #define INCL_CHART
    #define DBASE_FOR_WINDOWS
    #define BUILD_AUTO_REPORT
    #define MORE_GROUP_OPTIONS
//  #define MORE_CROSSTAB_GROUP_OPTIONS
    #define SET_LINEHEIGHT
    #define FIX_USERINDEXLIST
    #define STARTOFF_MAGNIFICATION
    #define INCL_DICT_SHOW_SQL
    #define TITLE_SUM_SECTIONS
    #define VERIFY_FOR_PDBXBSE
//  #define NO_CLIP_NUMERIC
    #define INCL_NULL_OPTION
    #define CONVERT_PDBPDX_TO_PDBBDE
  
    #if !defined (OWL2) && !defined (MFC)
        #define FIX_3DOWL
    #endif //!defined (OWL2) && !defined (MFC)
  
    #define INCL_REPORT_OPTIONS
    #define PDBDLL_USE_MORE_RANGES
//  #define INCL_QUERY
    #define CHAR_WIDTH_OPTION
    #define REENTRANT_DLL
    #define SAVE_DICT

    #if defined (WIN32) && defined (CONVERT_PDBPDX_TO_PDBBDE)
        #define CONVERT_QE
    #endif //defined (WIN32) && defined (CONVERT_PDBPDX_TO_PDBBDE)

    #if defined (CRPE_ONLY)
        #define MORE_PE_FUNCTIONS
        #define CAN_CREATE_REPORT
        #define INCL_ZOOM_PREVIEW_WINDOW
    #else
        #define CTL3D
        #define MSG_TO_REPORT
        #define UNIFY_SNAP_GRID
        #define UPDATE_DISPLAY_OPTIONS
        #define NEW_OPTIONS_DIALOG
        #define DRILL_DOWN
        #define SEARCH_TOOL
        #define SELECT_TOOL
        #define SELECT_TOOL2
//      #define ASSISTANCE
        #define DRAW_GRID
        #define SUPPORT_HRULER
        #define SHOW_TRACKER_COORDS
        #define RESIZABLE_INSERT_FROM_VIEW
        #define RESIZABLE_DB_FLD_DLG
        #define RESIZABLE_FORMULA_EDIT_DLG
        #define RECENT_REPORTS
//      #define LARGE_TWIPS
        #define NEGATIVE_TWIPS_IN_DESIGNWIN
//      #define ITSYBITSY_DB_FLD_DLG
        #define BESTFIT
        #define DISTRIBUTE_REPORT
//      #define LINE_PALETTE
        #define WIZARD
        #define GRAPHICAL_LINKING
        #define REPORT_STYLES
//      #define SHOW_BTN_BALLOON
        #define UNDO
        #define MOVABLE_FONTBAR
        #define BORDER_AROUND_STATUSBAR
        #define EDIT_CHART
    #endif //(CRPE_ONLY)
#endif //(CRW40)

#if defined (CRW30PRO)
    #define CRW30
    #define CRW20PRO
#endif //(CRW30PRO)

#if defined (CRW30)
    #define CRW20
    #define SQL_EDITION
    #define EXPORTING
    #define INCL_SETPRINTDATE
    #define INCL_GROUPANALYSIS
    #define CLOSE_BOXES
    #define RESOURCE_DLL
  
    #if !defined (QUERY_FOR_CRW50) && !defined (QUERY_FOR_CINFO50)
        #define OLE2
    #endif //!defined (QUERY_FOR_CRW50) && !defined (QUERY_FOR_CINFO50)

    #define RELUCTANT_CPDFILE
    #define CROSSTABS
    #define CUSTOM_PRINT_WINDOW
    #define OBEY_HIDDEN_SECTION_FORMAT
    #define THREE_WAY_MAGNIFICATION
    #define EXTRA_STATUS_INFO
    #define DEPENDENCY
    #define DB_DLL_PATH_IN_INI_FILE
    #define INCL_DICTIONARY
    #define KEEP_RECORDS_AROUND
    #define ALLOW_EDIT_SQL_STMT
    #define OEM_TO_ANSI_STRING_OPTION
//  #define FOUR_WAY_MAGNIFICATION
    #define PRINT_CTL_AT_BOTTOM
    #define PRINT_CTL_CLOSE_BUTTON
    #define PRINT_CTL_EXPORT_BUTTON
    #define PRINT_CTL_MAIL_BUTTON
    #define INCL_DEFAULTMAILDEST
    #define EXPORT_REPORT_FILES
    #define INCL_SQL_TABLES_RESTRICT
    #define INCL_MAX_STRING_FIELD_DISP_LEN
    #define INCL_SERVER_OPTIONS

    #if defined (CRPE_ONLY)
        #if defined (GREAT_PLAINS)
            #define INCL_SHOW_NTH_PAGE
            #define INCL_ZOOM_PREVIEW_WINDOW
            #define INCL_EXPORT_TO_FILE_PE_CALL
            #define LOCAL_DB_ONLY
            #undef  OLE2
            #undef  CROSSTABS
            #undef  ALLOW_EDIT_SQL_STMT
            #undef  INCL_DICTIONARY
        #else
            #define CRPE_SQL_EDIT
            #define INCL_DEBUG_MODE
        #endif //(GREAT_PLAINS)

//      #define OUTPUT_TO_VIEWER
    #endif //(CRPE_ONLY)

    #if !defined (CRPE_ONLY)
        #define EDIT_IN_PRINT_WINDOW
        #define INS_DEL_LINES
        #define DND_FIELDLIST
        #define BLUE_FIELD_SELECTION
        #define CANCEL_AND_USE_RECORDS_SO_FAR
        #define INCL_MULTI_DND
        #define INCL_MOUSE_MULTI_SELECT
        #define SHOW_HIDE_SECTIONS_DLG
        #define INCL_POPUPLABELLAYOUT
        #define INCL_STREAMLINEDFORMATDIALOGS
        #define PRINT_CTL_REFRESH_BUTTON
        #define PAINT_BAR_BACKGROUND
        #define USE_WINDOWS_DECIMAL_SYMBOL
        #define INCL_FRONTENDDIALOG
    #endif //!defined (CRPE_ONLY)

    #if !defined (CRPE)
        #define INCL_FONTBAR
        #define THINNER_TOOLBAR
        #define INCL_TOOLBAREXTENSIONS
        #define TOOL_BAR_MAIL_BUTTON
        #define TOOL_BAR_REFRESH_BUTTON
        #define TOOL_BAR_OLE_BUTTON
        #define INCL_RULER
        #define CURRENT_POSITION_IN_RULER
        #define USE_SUPPORT_DLL
        #define PRIVATE_TUTOR
    #endif //!defined (CRPE)
#endif //(CRW30)

#if defined (CRW20PRO)
    #define CRW20
    #define SQL_EDITION
#endif //(CRW20PRO)

#if defined (CRW20)
    #define ADORNMENTS
    #define GRAPHIC_FIELDS
    #define LINE_DRAWING
    #define PLATINUM
    #define STDDEV_VARIANCE
    #define PRINT_MAGNIFICATION
    #define INCL_COLOUREDTEXTANDFIELDS
    #define INCL_LOGONSERVER
    #define INCL_MOREROUNDINGTYPES
    #define UDF
    #define DLLS_IN_SYS_DIR
    #define LIMIT_DB_FIELD_RANGES

    #if !defined (CRPE_ONLY)
        #define INCL_FULLFIELDTRACKER
        #define INCL_MOVEFLDOUTSEC
        #define INCL_INSERTHEADING
        #define INCL_MULTIFLDSEL
        #define INCL_SECTIONSEL
        #define INCL_DIRECTSELECTION
        #define INCL_FIELDLISTBOX
        #define INCL_BROWSEDATA
        #define INCL_AUTOSCROLL
        #define INCL_TRACKINSERTFIELDOUTLINE
        #define INCL_MULTIFIELDMOUSEMOVE
        #define INCL_SHORTSECTIONNAME
        #define INCL_DRAGSECTION
        #define INCL_SENDTOBACK
        #define INCL_SEPARATEBORDERCOMMAND
        #define INCL_USESMALLERCURSORS
        #define INCL_MULTIFIELDMOUSERESIZE
        #define INCL_NULLSELECTION
        #define INCL_ENGLISHSUMMARYDIALOGS
        #define CS_HELP
    #endif //!defined (CRPE_ONLY)

    #if !defined (CRPE)
        #define INCL_STATUSBAR
        #define INCL_COMPILEREPORT
        #define AUTHORIZE_DIALOG
    #endif //!defined (CRPE)
#endif //(CRW20)

#if defined (SOFTBRIDGE_BASIC)
    #undef LIMIT_DB_FIELD_RANGES
    #undef UDF
    #undef INCL_DIRECTSELECTION
#endif //(SOFTBRIDGE_BASIC)

#if defined (BORLAND)
    #if !defined (CRPE_ONLY)
        #undef INCL_FRONTENDDIALOG
        #undef AUTHORIZE_DIALOG
    #endif //!defined (CRPE_ONLY)
#endif //(BORLAND)

#if defined (CRW_IN_A_DLL)
    #undef  OLE2
    #define MEW_HACK
#endif //(CRW_IN_A_DLL)

#if defined (WALLDATA)
    #undef INCL_BROWSEDATA
    #undef OLE2
    #undef INCL_FRONTENDDIALOG
#endif //(WALLDATA)

#if defined (CDW)
    #define INCL_SUBCLASS_COMDLG

    //#if defined (INCL_QUERY)
    //      #define BACKGROUND_ADD
    //      #define CDW_SPEEDUPS
    //#endif
  
    #undef PROMPTING_VARIABLES
    #undef INCL_QUERY

    #if defined (CDW45)
        #define INFO_LIBRARY
        #define BACKGROUND_ADD
        #define CDW_SPEEDUPS
        #define SAVE_LOGON
        #undef INCL_SUBCLASS_COMDLG
        #undef REUSE_LOGON
    #endif //(CDW45)

    #undef BESTFIT
    #undef RESIZABLE_FORMULA_EDIT_DLG
    #undef UNIFY_SNAP_GRID
    #undef STARTOFF_MAGNIFICATION
    #undef UPDATE_DISPLAY_OPTIONS
    #undef NEW_OPTIONS_DIALOG
    #undef SUPPORT_HRULER
    #undef SHOW_TRACKER_COORDS
    #undef EXPORTING
    #undef INCL_SETPRINTDATE
    #undef INCL_GROUPANALYSIS
    #undef KEEP_RECORDS_AROUND
    #undef CLOSE_BOXES
    #undef INCL_MOREROUNDINGTYPES
    #undef USE_WINDOWS_DECIMAL_SYMBOL
    #undef INCL_FULLFIELDTRACKER
    #undef INCL_TRACKINSERTFIELDOUTLINE
    #undef MOVE_ON_VERTICAL_GRID
    #undef GRAPHIC_FIELDS
    #undef LINE_DRAWING
    #undef INCL_AUTOSCROLL
    #undef OLE2
    #undef CROSSTABS
    #undef CUSTOM_PRINT_WINDOW
    #undef OBEY_HIDDEN_SECTION_FORMAT
    #undef THREE_WAY_MAGNIFICATION
    #undef EDIT_IN_PRINT_WINDOW
    #undef INS_DEL_LINES
    #undef INCL_MOUSE_MULTI_SELECT
    #undef INCL_TOOLBAREXTENSIONS
    #undef PRINT_CTL_AT_BOTTOM
    #undef EXPORT_PRINT_CTL
    #undef SHORT_PRINT_CTL_DIALOG
    #undef PRINT_CTL_EXPORT_BUTTON
    #undef PRINT_CTL_MAIL_BUTTON
    #undef PRINT_CTL_REFRESH_BUTTON
    #undef PRINT_CTL_CLOSE_BUTTON
    #undef SHOW_HIDE_SECTIONS_DLG
    #undef TOOL_BAR_MAIL_BUTTON
    #undef TOOL_BAR_REFRESH_BUTTON
    #undef TOOL_BAR_OLE_BUTTON
    #undef INCL_RULER
    #undef CURRENT_POSITION_IN_RULER
    #undef INCL_FRONTENDDIALOG
    
    #if !defined (CRW40)
        #undef RESOURCE_DLL
    #endif //!defined (CRW40)
  
    #undef DRILL_DOWN
    #undef LINE_PALETTE
    #undef INCL_CHART
    #undef INCL_DICT_SHOW_SQL
  
    #if !defined (INCL_QUERY) && !defined (INFO_LIBRARY)
        #undef GRAPHICAL_LINKING
    #endif //!defined (INCL_QUERY) && !defined (INFO_LIBRARY)
  
    #undef REPORT_STYLES
    #undef SHOW_BTN_BALLOON
    #undef WIZARD
    #undef UNDO
    #undef DBASE_FOR_WINDOWS
    #undef EDIT_CHART
    #undef INCL_NULL_OPTION
    #undef INCL_REPORT_OPTIONS
    #undef PDBDLL_USE_MORE_RANGES
    #undef RECENT_REPORTS
    #undef REENTRANT_DLL
    #undef SAVE_DICT
#endif //(CDW)

#if defined (CRW_ONLY) || defined (CRW_IN_A_DLL)
    #define CAN_CREATE_REPORT
#endif //defined (CRW_ONLY) || defined (CRW_IN_A_DLL)

#if defined (CRW_ONLY) || defined (EXPORTING) || defined (CAN_CREATE_REPORT)
    #define CAN_SAVE_REPORT
#endif //defined (CRW_ONLY) || defined (EXPORTING) || defined (CAN_CREATE_REPORT)

#if defined (CRW_ONLY) || defined (CRPE) && !defined (GREAT_PLAINS)
    #define CAN_VERIFY_DATABASE
#endif //defined (CRW_ONLY) || defined (CRPE) && !defined (GREAT_PLAINS)

#define SEPARATE_MODULE

#if defined (CRPE) || defined (CIVAPI)
    #define BRAHMA_DLL
#endif //defined (CRPE) || defined (CIVAPI)

#if !defined (CRPE_ONLY)
    #define CAN_EDIT_REPORT
#endif //!defined (CRPE_ONLY)

#if defined (GREAT_PLAINS) && defined (CRPE)
//  #define ALLOW_TRACING
#endif //defined (GREAT_PLAINS) && defined (CRPE)

#if defined (CORE)
    #undef USE_SUPPORT_DLL
#endif //(CORE)

#if defined (WIZARD)
    #define BACKGROUND_ADD
    #define MORE_REPORT_KINDS_IN_MENU
    #define MASKED_FIELD_LIST
#endif //(WIZARD)


#if defined (KEEP_RECORDS_AROUND)
    #define ALLOW_RESTORE_OF_SAVED_DATA
#endif //(KEEP_RECORDS_AROUND)

#if defined (CRW40) && defined (WIN32)
    #define DBCS
    #define CONVERT_JET_TO_DAO
#endif //defined (CRW40) && defined (WIN32)

// #define GDI_TRACE

#ifndef GROUP_BY
   #define GROUP_BY
#endif

#endif //!defined (BRCONFIG_H)

