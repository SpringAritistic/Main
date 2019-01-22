#pragma once
#ifndef __HMGLOBALTOOL_TOOLS_COMMON_TOOLS_H__
#define __HMGLOBALTOOL_TOOLS_COMMON_TOOLS_H__

// 判断当前程序的运行模式是否是debug模式
HM_GlobalTool_EXT bool app_is_debug_mode();

// 在界面主进程中的非界面耗时代码中合理间隔内调用此函数可以避免界面被阻塞
// 此函数只能在主界面进程中调用，不能在其它界面进程或其它线程中使用，否则会有跨线程问题
HM_GlobalTool_EXT void x_do_ui_events();
#endif //__HMGLOBALTOOL_TOOLS_COMMON_TOOLS_H__
