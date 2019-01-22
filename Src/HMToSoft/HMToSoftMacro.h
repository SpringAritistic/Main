#pragma once
#ifdef   __HMToSoft__
#define  HM_ToSoft_EXT   __declspec(dllexport)
#pragma message("已经定义__HMToSoft__")
#else
#define  HM_ToSoft_EXT   __declspec(dllimport)
#ifdef _DEBUG
#pragma message("没有定义__HMToSoft__")
#pragma comment(lib,"HMToSoftD.lib")
#pragma message("Automatically linking with HMToSoftD.dll")
#else
#pragma comment(lib,"HMToSoft.lib")
#pragma message("Automatically linking with HMToSoft.dll")
#endif
#endif
#define _HM_ToSoft_BEGIN   namespace HM_ToSoft{
#define _HM_ToSoft_END    }
#ifdef _DEBUG
#define _HM_ToSoftTEST_EXT HM_ToSoft_EXT
#else
#define _HM_ToSoftTEST_EXT
#endif
