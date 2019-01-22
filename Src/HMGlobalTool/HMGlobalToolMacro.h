#pragma once
#ifdef   __HMGlobalTool__
#define  HM_GlobalTool_EXT   __declspec(dllexport)
#else
#define  HM_GlobalTool_EXT   __declspec(dllimport)
#ifdef _DEBUG
#pragma comment(lib,"HMGlobalToolD.lib")
#pragma message("Automatically linking with HMGlobalToolD.dll")
#else
#pragma comment(lib,"HMGlobalTool.lib")
#pragma message("Automatically linking with HMGlobalTool.dll")
#endif
#endif
#define _HM_GlobalTool_BEGIN   namespace HM_GlobalTool{
#define _HM_GlobalTool_END    }
#ifdef _DEBUG
#define _HM_GlobalToolTEST_EXT HM_GlobalTool_EXT
#else
#define _HM_GlobalToolTEST_EXT
#endif
