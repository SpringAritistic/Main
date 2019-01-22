#pragma once

#ifdef UI_MANAGER_DLL
#	define UI_MANAGER_API __declspec(dllexport)
#else
#	define UI_MANAGER_API __declspec(dllimport)
#	ifdef _DEBUG
#		pragma comment(lib,"ui_manager_d.lib")
#		pragma message("auto linking to ui_manager_d.lib")
#	else
#		pragma comment(lib,"ui_manager.lib")
#		pragma message("auto linking to ui_manager.lib")
#	endif
#endif
