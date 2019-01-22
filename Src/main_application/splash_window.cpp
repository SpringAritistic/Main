#include "stdafx.h"

#include "x_framework/macro.h"
#include "x_framework/tools/file_tools.h"

#include "splash_window.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;
using namespace Gdiplus;


IMPLEMENT_DYNAMIC(splash_window, CWnd)
BEGIN_MESSAGE_MAP(splash_window, CWnd)
END_MESSAGE_MAP()

splash_window::splash_window(void)
{
	m_image = nullptr;
}

splash_window::~splash_window(void)
{
	safe_delete(m_image);
}

BOOL splash_window::Create()
{
	// 主程序资源中的splash图片以及res中的图片文件仅用于开发测试，不需要删除，最终以gui_resource模块中定制的图片为准
	// load_png_image_from_resource(_T("IDB_SPLASH"));
	load_png_image_from_file(file_tools::get_app_path_with_xg() + _T("gui_xml\\splash.png"));

	CRect wnd_rect;
	if (!m_image)
	{
		wnd_rect.right = wnd_rect.left + 100;
		wnd_rect.bottom = wnd_rect.top + 100;
	}
	else
	{
		wnd_rect.right = wnd_rect.left + m_image->GetWidth();
		wnd_rect.bottom = wnd_rect.top + m_image->GetHeight();
	}

	CRect work_area;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, work_area, NULL);
	wnd_rect.MoveToXY((work_area.right - m_image->GetWidth()) / 2,
		(work_area.bottom - m_image->GetHeight()) / 2);

	DWORD ex_style = WS_EX_TOOLWINDOW |/* WS_EX_TOPMOST |*/ WS_EX_LAYERED;
	DWORD style = WS_POPUP | WS_VISIBLE | WS_SYSMENU;
	if (!CWnd::CreateEx(ex_style, _T("app_splash"), nullptr, style,
		wnd_rect.left, wnd_rect.top, wnd_rect.Width(), wnd_rect.Height(), nullptr, nullptr))
		return FALSE;

	redraw_window();
	
	return TRUE;
}

bool splash_window::load_png_image_from_resource(const CString& res_name)
{
	if (res_name.IsEmpty()) return false;

	HINSTANCE ins = ::GetModuleHandle(nullptr);
	HRSRC res = ::FindResource(ins, res_name, _T("PNG"));
	if (!res) return false;

	// load resource into memory 
	DWORD len = SizeofResource(ins, res);
	BYTE* res_buffer = (BYTE*) LoadResource(ins, res);
	if (!res_buffer) return false;

	// Allocate global memory on which to create stream 
	HGLOBAL stream_buffer = GlobalAlloc(GMEM_FIXED, len);
	BYTE* locked_stream_buffer = (BYTE*) GlobalLock(stream_buffer);
	memcpy(locked_stream_buffer, res_buffer, len);

	IStream* stream_interface = nullptr;
	CreateStreamOnHGlobal(stream_buffer, FALSE, &stream_interface);

	// load from stream 
	safe_delete(m_image);
	m_image = Gdiplus::Image::FromStream(stream_interface);

	GlobalUnlock(stream_buffer);
	stream_interface->Release();
	FreeResource(res_buffer);

	return true;
}

bool splash_window::load_png_image_from_file(const CString& path_name)
{
	if (path_name.IsEmpty()) return false;

	safe_delete(m_image);
	m_image = Gdiplus::Image::FromFile(string_tools::CString_to_wstring(path_name).c_str());

	return true;
}

void splash_window::on_draw(Gdiplus::Graphics& gp)
{
	if (!m_image) return;

	gp.DrawImage(m_image, 0, 0, m_image->GetWidth(), m_image->GetHeight());
}

void splash_window::redraw_window(void)
{
	if (!m_image) return;
	if (!GetSafeHwnd()) return;

	HDC dc = ::GetDC(GetSafeHwnd());
	HDC mem_dc = ::CreateCompatibleDC(dc);

	BITMAPINFO bmp_info;
	bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmp_info.bmiHeader.biBitCount = 32;
	bmp_info.bmiHeader.biWidth = m_image->GetWidth();
	bmp_info.bmiHeader.biHeight = m_image->GetHeight();
	bmp_info.bmiHeader.biPlanes = 1;
	bmp_info.bmiHeader.biCompression = BI_RGB;
	bmp_info.bmiHeader.biXPelsPerMeter = 0;
	bmp_info.bmiHeader.biYPelsPerMeter = 0;
	bmp_info.bmiHeader.biClrUsed = 0;
	bmp_info.bmiHeader.biClrImportant = 0;
	bmp_info.bmiHeader.biSizeImage = bmp_info.bmiHeader.biWidth *
		bmp_info.bmiHeader.biHeight * bmp_info.bmiHeader.biBitCount / 8;

	HBITMAP cur_bitmap = CreateDIBSection(mem_dc, &bmp_info, 0, nullptr, 0, 0);
	HBITMAP old_bitmap = (HBITMAP) SelectObject(mem_dc, cur_bitmap);
	Graphics gp(mem_dc);

	on_draw(gp);

	//设置透明窗口
	CPoint dest_pos(0, 0);
	CSize dest_size(m_image->GetWidth(), m_image->GetHeight());
	BLENDFUNCTION blend_fun_32_bpp;
	blend_fun_32_bpp.AlphaFormat = AC_SRC_ALPHA;
	blend_fun_32_bpp.BlendFlags = 0;
	blend_fun_32_bpp.BlendOp = AC_SRC_OVER;
	blend_fun_32_bpp.SourceConstantAlpha = 255;
	::UpdateLayeredWindow(GetSafeHwnd(), dc, nullptr, &dest_size, mem_dc, &dest_pos, 0, &blend_fun_32_bpp, ULW_ALPHA);

	//释放资源
	::SelectObject(mem_dc, old_bitmap);
	::DeleteObject(cur_bitmap);
	::DeleteDC(mem_dc);
	::ReleaseDC(GetSafeHwnd(), dc);
}
