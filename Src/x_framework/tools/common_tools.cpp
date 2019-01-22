#include "StdAfx.h"

#include "common_tools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


bool app_is_debug_mode()
{
#ifdef _DEBUG
	return true;
#else
	return false;
#endif
}

void x_do_ui_events()
{
	DWORD st = GetTickCount();
	MSG msg;
	do
	{
		DWORD en = GetTickCount();
		if (en - st > 3000) return; // 最多只能占用3秒，否则用户处理个消息时不科学导致产生大量消息时，会让队列都陷在循环里面了

		if (FALSE == ::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE)) break; // 消息队列中没有消息时返回FALSE
		if (FALSE == ::GetMessage(&msg, nullptr, 0, 0)) // // 收到WM_QUIT消息时返回FALSE
		{
			AfxPostQuitMessage(0);
			return;
		}

		// 不能使用AfxPumpMessage投递消息，要自己手动使用API转发，因为控制台程序使用AfxPumpMessage把界面阻塞死掉
		// AfxPumpMessage() // 收到WM_QUIT消息时返回FALSE

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	} while (true);
}
