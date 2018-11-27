//---------------------------------------------------------------------------
// File:
//      BFFileHelper.cpp BFFileHelper.hpp
//
// Module:
//      CBFFileHelper
//
// History:
//		May. 7, 2002		Created by Benjamin Fung
//---------------------------------------------------------------------------

#include "BFPch.h"

#if !defined(BFFILEHELPER_H)
	#include "BFFileHelper.h"
#endif

//--------------------------------------------------------------------
//--------------------------------------------------------------------
CBFFileHelper::CBFFileHelper()
{
}

CBFFileHelper::~CBFFileHelper()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void CBFFileHelper::splitPath(LPCTSTR fullPath, CString& drive, CString& dir, CString& fname, CString& ext)
{
    TCHAR tDrive[_MAX_DRIVE];
    TCHAR tDir[_MAX_DIR];
    TCHAR tFname[_MAX_FNAME];
    TCHAR tExt[_MAX_EXT];
    _tsplitpath(fullPath, tDrive, tDir, tFname, tExt);

    drive = tDrive;
    dir = tDir;
    fname = tFname;
    ext = tExt;
}