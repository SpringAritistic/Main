#pragma once
#ifdef   __HMGridControl__
#define  HM_GridControl_EXT   __declspec(dllexport)
#else
#define  HM_GridControl_EXT   __declspec(dllimport)
#ifdef _DEBUG
#pragma comment(lib,"HMGridControlD.lib")
#pragma message("Automatically linking with HMGridControlD.dll")
#else
#pragma comment(lib,"HMGridControl.lib")
#pragma message("Automatically linking with HMGridControl.dll")
#endif
#endif
#define _HM_GridControl_BEGIN   namespace HM_GridControl{
#define _HM_GridControl_END    }
#ifdef _DEBUG
#define _HM_GridControlTEST_EXT HM_GridControl_EXT
#else
#define _HM_GridControlTEST_EXT
#endif
