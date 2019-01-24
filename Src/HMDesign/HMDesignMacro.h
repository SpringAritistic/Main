#pragma once
#ifdef   __HMDesign__
#define  HM_Design_EXT   __declspec(dllexport)
#pragma message("已经定义__HMDesign__")
#else
#define  HM_Design_EXT   __declspec(dllimport)
#ifdef _DEBUG
#pragma message("没有定义__HMDesign__")
#pragma comment(lib,"HMDesignD.lib")
#pragma message("Automatically linking with HMDesignD.dll")
#else
#pragma comment(lib,"HMDesign.lib")
#pragma message("Automatically linking with HMDesign.dll")
#endif
#endif
#define _HM_Design_BEGIN   namespace HM_Design{
#define _HM_Design_END    }
#ifdef _DEBUG
#define _HM_DesignTEST_EXT HM_Design_EXT
#else
#define _HM_DesignTEST_EXT
#endif
