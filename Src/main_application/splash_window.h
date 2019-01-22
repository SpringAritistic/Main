#pragma once

class splash_window : public CWnd
{
	DECLARE_DYNAMIC(splash_window)
	DECLARE_MESSAGE_MAP()

public:
	splash_window(void);
	virtual ~splash_window(void);

public:
	virtual void on_draw(Gdiplus::Graphics& gp);

public:
	BOOL Create(); // ¸²¸ÇCWndµÄCreate½Ó¿Ú

private:
	bool load_png_image_from_resource(const CString& res_name);
	bool load_png_image_from_file(const CString& path_name);
	void redraw_window();

private:
	Gdiplus::Image* m_image;
};
