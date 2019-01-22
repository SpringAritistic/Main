#pragma once

#ifdef PROJECT_MANAGER_DLL
#	define PROJECT_MANAGER_API __declspec(dllexport)
#else
#	define PROJECT_MANAGER_API __declspec(dllimport)
#	ifdef _DEBUG
#		pragma comment(lib,"project_manager_d.lib")
#		pragma message("auto linking to project_manager_d.lib")
#	else
#		pragma comment(lib,"project_manager.lib")
#		pragma message("auto linking to project_manager.lib")
#	endif
#endif
