// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__71D3CFDD_A495_4F79_8715_F44B28966213__INCLUDED_)
#define AFX_STDAFX_H__71D3CFDD_A495_4F79_8715_F44B28966213__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxtempl.h>

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>
#include <conio.h>
//#include <Shlwapi.h>

// TODO: reference additional headers your program requires here

#if !defined(DCDEF_HPP)
    #include "DCDef.hpp"
#endif

// BFLib
#if !defined (TYPES_HPP)
    #include "types.hpp"
#endif

#if !defined (STRUTIL_HPP)
    #include "strutil.hpp"
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__71D3CFDD_A495_4F79_8715_F44B28966213__INCLUDED_)
