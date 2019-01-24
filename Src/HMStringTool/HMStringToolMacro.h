#pragma once
#ifdef   __HMStringTool__
#define  HM_StringTool_EXT   __declspec(dllexport)
#pragma message("已经定义__HMStringTool__")
#else
#define  HM_StringTool_EXT   __declspec(dllimport)
#ifdef _DEBUG
#pragma message("没有定义__HMStringTool__")
#pragma comment(lib,"HMStringToolD.lib")
#pragma message("Automatically linking with HMStringToolD.dll")
#else
#pragma comment(lib,"HMStringTool.lib")
#pragma message("Automatically linking with HMStringTool.dll")
#endif
#endif
#define _HM_StringTool_BEGIN   namespace HM_StringTool{
#define _HM_StringTool_END    }
#ifdef _DEBUG
#define _HM_StringToolTEST_EXT HM_StringTool_EXT
#else
#define _HM_StringToolTEST_EXT
#endif
