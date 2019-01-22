// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#ifndef NO_WARN_MBCS_MFC_DEPRECATION
#define NO_WARN_MBCS_MFC_DEPRECATION
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

#include <XTToolkitPro.h>



//////////////////////////////////////////////////////////////////////////

#ifndef safe_delete
#	define safe_delete(p) (delete(p), (p) = nullptr)
#endif

#ifndef safe_delete_array
#	define safe_delete_array(p) (delete[](p), (p) = nullptr)
#endif

//////////////////////////////////////////////////////////////////////////

// 启用数学库中的常量
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

//////////////////////////////////////////////////////////////////////////

#include <tchar.h>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <utility>
#include <fstream>
#include <string>

#include "macro.h"

//////////////////////////////////////////////////////////////////////////

