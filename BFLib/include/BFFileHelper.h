// BFFileHelper.h: interface for the File Helper classes
//
//////////////////////////////////////////////////////////////////////

#if !defined(BFFILEHELPER_H)
#define BFFILEHELPER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//--------------------------------------------------------------------
// XML node
//--------------------------------------------------------------------
class CBFFileHelper
{
public:
    CBFFileHelper();
    virtual ~CBFFileHelper();

// operations    
    static void splitPath(LPCTSTR fullPath, CString& drive, CString& dir, CString& fname, CString& ext);
};

#endif // !defined(BFFILEHELPER_H)