#pragma once

#ifdef X_FRAMEWORK_DLL
#	define X_FRAMEWORK_API __declspec(dllexport)
#else
#	define X_FRAMEWORK_API __declspec(dllimport)
#	ifdef _DEBUG
#		pragma comment(lib,"x_framework_d.lib")
#		pragma message("auto linking to x_framework_d.lib")
#	else
#		pragma comment(lib,"x_framework.lib")
#		pragma message("auto linking to x_framework.lib")
#	endif
#endif
