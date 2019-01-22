// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#ifndef NO_WARN_MBCS_MFC_DEPRECATION
#define NO_WARN_MBCS_MFC_DEPRECATION
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展
#include <afxdisp.h>        // MFC 自动化类

#ifndef _AFX_NO_OLE_SUPPORT
#	include <afxole.h>         // MFC OLE classes
#	include <afxodlgs.h>       // MFC OLE dialog classes
#	include <afxdisp.h>		   // MFC 自动化类
#	include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif

#ifndef _AFX_NO_AFXCMN_SUPPORT
#	include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持
#include <gdiplus.h>



//////////////////////////////////////////////////////////////////////////

//#ifndef safe_delete
//#	define safe_delete(p) do{delete(p); (p) = nullptr;}while(false)
//#endif
//
//#ifndef safe_delete_array
//#	define safe_delete_array(p) do{delete[] (p); (p) = nullptr;}while(false)
//#endif

#ifndef safe_delete
#    define safe_delete(p) (delete(p), (p) = nullptr)
#endif
#ifndef safe_delete_array
#    define safe_delete_array(p) (delete[](p), (p) = nullptr)
#endif
//////////////////////////////////////////////////////////////////////////

// 启用数学库中的常量
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MSDN上规定的用户可自定义的消息范围（注释以备参考）
//   WM_USER through 0x7FFF		Integer messages for use by private window classes. 
//   0x7FFF 转为10进制为 32767
// 其余为系统或应用程序保留
//   0 through WM_USER–1		Messages reserved for use by the system. 
//   WM_APP through 0xBFFF		Messages available for use by applications. 
//   0xC000 through 0xFFFF		String messages for use by applications. 
//   Greater than 0xFFFF		Reserved by the system. 
//////////////////////////////////////////////////////////////////////////

// 不得和系统的其它命令按钮、菜单、工具条按钮等会触发WM_COMMAND消息的元素的id有重叠
// 注：x_framework\tools\app_popup_menu_helper.h中上下文菜单的区域从40000-49999

// 注意这些id要与主窗口的菜单、ribbon的id范围区分开，否则消息响应会映射错
#define ID_ACCELERATOR_CMD_MIN							10000	// 加速键中所有命令的最小可用ID
#define ID_ACCELERATOR_CMD_MAX							19499	// 加速键中所有命令的最大可用ID

// ribbon相关
#define WM_RIBBON_UI_COMMAND							(WM_USER + 20003) // 消息为20003开始吧


//ribbon中快速访问栏中图标的ID范围
#define IDS_RIBBON_QUICK_ACCESS_MIN						19500
#define IDS_RIBBON_QUICK_ACCESS_MAX						19699

//ribbon中文件中图标的ID范围
#define IDS_RIBBON_FILE_MENU_MIN						19700
#define IDS_RIBBON_FILE_MENU_MAX						19899



// 后面定义的所有ID均要在这两个值范围内
#define IDS_RIBBON_UI_MIN								20000	// Ribbon中所有命令的最小可用ID
#define IDS_RIBBON_UI_AUTO_MAX							28999	// 可配置的最大ID（之后的可用ID为特定的，不允许xml中配置的自动管理的控件使用）
#define IDS_RIBBON_UI_MAX								29998	// Ribbon中所有命令的最大可用ID
#define IDS_RIBBON_UI_INVALID							29999	// Ribbon中自动配置的ID中凡是为此值表示无效命令，界面不显示，主要用于处理命令的无效图标

//ribbon的Option及杂项
#define IDS_RIBBON_OPTIONS_MIN							30000	 // 功能区命令的最小可用ID
#define IDS_RIBBON_OPTIONS_MINI_MODE					30001	 // 功能区最小化
#define IDS_RIBBON_OPTIONS_THEMECHANGE					30002	 // 主题切换
#define IDS_RIBBON_OPTIONS_ABOUT						30003	 // 关于对话框
#define IDS_RIBBON_OPTIONS_MAX							30099	 // 功能区命令的最大可用ID

//////////////////////////////////////////////////////////////////////////

#include <tchar.h>
#include <limits>
#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <algorithm>
#include <utility>
#include <array>
#include <memory>
#include <utility>


#include "XTToolkitPro.h"
#include "x_framework/macro.h"
#include "x_framework/tools/tinyxml2.h"
#include "x_framework/tools/xml_tools.h"
#include "x_framework/tools/string_tools.h"

#include "app_size_config_helper.h"

//////////////////////////////////////////////////////////////////////////

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif


