#pragma once
#ifdef   __HMDrawCell__
#define  HM_DrawCell_EXT   __declspec(dllexport)
#pragma message("已经定义__HMDrawCell__")
#else
#define  HM_DrawCell_EXT   __declspec(dllimport)
#ifdef _DEBUG
#pragma message("没有定义__HMDrawCell__")
#pragma comment(lib,"HMDrawCellD.lib")
#pragma message("Automatically linking with HMDrawCellD.dll")
#else
#pragma comment(lib,"HMDrawCell.lib")
#pragma message("Automatically linking with HMDrawCell.dll")
#endif
#endif
#define _HM_DrawCell_BEGIN   namespace HM_DrawCell{
#define _HM_DrawCell_END    }
#ifdef _DEBUG
#define _HM_DrawCellTEST_EXT HM_DrawCell_EXT
#else
#define _HM_DrawCellTEST_EXT
#endif
