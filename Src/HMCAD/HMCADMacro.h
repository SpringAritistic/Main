#pragma once
#ifdef   __HMCAD__
#define  HM_CAD_EXT   __declspec(dllexport)
#else
#define  HM_CAD_EXT   __declspec(dllimport)
#ifdef _DEBUG
#pragma comment(lib,"HMCADD.lib")
#pragma message("Automatically linking with HMCADD.dll")
#else
#pragma comment(lib,"HMCAD.lib")
#pragma message("Automatically linking with HMCAD.dll")
#endif
#endif
#define _HM_CAD_BEGIN   namespace HM_CAD{
#define _HM_CAD_END    }
#ifdef _DEBUG
#define _HM_CADTEST_EXT HM_CAD_EXT
#else
#define _HM_CADTEST_EXT
#endif
