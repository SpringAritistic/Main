#include "stdafx.h"

#include "x_framework/macro.h"
#include "x_framework/interface/window_interface.h"
#include "x_framework/interface/cmd_container_interface.h"
#include "x_framework/interface/app_ui_interface.h"
#include "x_framework/tools/file_tools.h"
#include "x_framework/tools/string_tools.h"

#include "app_configs.h"

#include "start_page.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(start_page, CWnd, 1)
BEGIN_MESSAGE_MAP(start_page, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

start_page::start_page() : CWnd(), CXTPMarkupContext(), window_interface()
{
	m_markup_page = nullptr;
	m_is_page_in_rendering = false;
	
	// 不用事件响应的方式，这要求xaml中要定义Click事件名，如果xaml中定义了事件名，但此处没有关联响应函数，xtp库
	// 会认为xaml格式有误，显示空白，给开发工作带来不便，因此改用AddHandler的方式
	// SetDelegate(L"new_project_clicked", CreateMarkupClassDelegate(this, &start_page::on_hyperlink_clicked));
	AddHandler(CXTPMarkupHyperlink::m_pClickEvent, CreateMarkupClassDelegate(this, &start_page::on_hyperlink_clicked));
}

start_page::~start_page(void)
{
	if (m_markup_page)
	{
		// Release会delete(this)但没有置为nullptr
		m_markup_page->Release();
		m_markup_page = nullptr;
	}
}

CString start_page::get_title()
{
	return _T("起始页");
}

BOOL start_page::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	return CWnd::PreCreateWindow(cs);
}

CString start_page::get_markup_xml_file_path_name(app_theme_type& style_type)
{
	app_ui_interface* app = get_app_ui_interface();
	if (!app) return _T("");

	theme_style_settings settings = app->get_style_and_settings();

	// 禁用主题的时候就返回白色的起始页吧
	if (!app_configs::is_skin_enable())
		return (file_tools::get_app_path_with_xg() + _T("gui_xml\\start_page_white.xaml")); 

	if (app_theme_type::theme_office_2007_blue == style_type
		|| app_theme_type::theme_office_2010_blue == style_type)
	{
		return file_tools::get_app_path_with_xg() + _T("gui_xml\\start_page_blue.xaml");
	}
	else if (app_theme_type::theme_office_2007_black == style_type
		|| app_theme_type::theme_office_2010_black == style_type)
	{
		return (file_tools::get_app_path_with_xg() + _T("gui_xml\\start_page_black.xaml"));
	}
	else if (app_theme_type::theme_office_2007_silver == style_type
		|| app_theme_type::theme_office_2010_silver == style_type
		|| app_theme_type::theme_office_2013_white == style_type)
	{
		return (file_tools::get_app_path_with_xg() + _T("gui_xml\\start_page_white.xaml"));
	}
	else if (app_theme_type::theme_office_2013_dark == style_type)
	{
		return (file_tools::get_app_path_with_xg() + _T("gui_xml\\start_page_dark.xaml"));
	}
	else
	{
		return _T("");
	}
}

void start_page::create_markup_page(const CString& file_path_name)
{
	if (file_path_name.IsEmpty()) return;

	// 此文件在读取时按utf - 8编码处理，否则中文无法解析，2016 - 7 - 28号把xtp从15.3.1升级到17.2.0时发现xtp的脚本引擎不再支持
	// 多字节，粗略看了一下源码，应该只支持utf - 8, utf - 16（utf - 32不支持，ucs - 16, ucs - 32也不再支持），特作此注释并更改文件编码
	// 为简化文件读取的逻辑，代码中按无BOM标志读取（即小端无BOM方式），编辑时请注意不得存为带BOM的编码
	// 另外，17.2.0版本的脚本引擎要求xmal文档最后至少要一个空行，否则解析错误
	// （感觉这个最新版本的质量有点下降啊，皮肤和主题也是遇到了几个bug）
	// 15.3.0按ANSI的GB2312编码解析的代码先留着（该版本下能正常工作）
	//CStringW xml_text;
	//try
	//{
	//	CStdioFile xml_file;
	//	if (TRUE == xml_file.Open(file_path_name, CFile::modeRead))
	//	{
	//		CString row;
	//		while (true)
	//		{
	//			if (FALSE == xml_file.ReadString(row)) break;

	//			// 避免写成xml_file += row + _T("\n");避免生成临时对象，文件中经常有很长的行，因此分成两行
	//			xml_text += row;
	//			xml_text += _T("\r\n");
	//		}
	//	}
	//	xml_file.Close();
	//}
	//catch (...)
	//{
	//	// nothing;
	//}

	// 此代码为读取utf-8/utf-16/utf-32编码的文本文件，来源于xtp 17.2.0 Markup.exe源代码，略有修改
	CStringW xml_text;
	try
	{
		CFile xml_file;
		if (TRUE == xml_file.Open(file_path_name, CFile::modeRead))
		{
			wchar_t c;
			char t;
			while (xml_file.Read(&t, 1) == 1)
			{
				if (0 == (t & '\x80'))
				{
					c = t;
				}
				else if ('\xF0' == (t & '\xF0')) // 1111 - error, more than 16-bit char
				{
					c = L'\0';
				}
				else if ('\xE0' == (t & '\xF0')) // 1110xxxx 10xxxxxx 10xxxxxx
				{
					char t2,t3;
					xml_file.Read(&t2, 1);
					xml_file.Read(&t3, 1);

					c = (WCHAR) ((WCHAR(t & '\x0F') << 12) | (WCHAR(t2 & '\x3F') << 6) | WCHAR(t3 & '\x3F'));
				}
				else if ('\xC0' == (t & '\xE0')) // 110xxxxx 10xxxxxx 
				{
					char t2;
					xml_file.Read(&t2, 1);

					c = (WCHAR) ((WCHAR(t & '\x1F') << 6) | (t2 & '\x3F'));
				}
				else
				{
					c = L'\0';
				}

				if (c != L'\0') xml_text += c;
			}
		}
		xml_file.Close();
	}
	catch (...)
	{
		// nothing;
	}

	// 17.2.0版本的脚本引擎要求xmal文档最后至少要一个空行，否则特殊情况下格式复杂会解析错误
	xml_text += L"\r\n";
	
	const wchar_t* xml_text_buffer = xml_text.GetString();
	
	// 指向结束符，注意不得乘以sizeof(wchar_t)，原因是xml_text_buffer为wchar_t类型的，向后偏移一个单元就已经是一个sizeof(wchar_t)的长度了
	const wchar_t* xml_text_buffer_end = xml_text_buffer + (xml_text.GetLength()/* * sizeof(wchar_t)*/);

	CXTPMarkupParser sc;
	sc.SetBuffer(xml_text_buffer, xml_text_buffer_end);

	CXTPMarkupBuilder builder(this);
	CXTPMarkupUIElement* page = builder.Parse(&sc);
	CString last_error = builder.GetLastError();
	if (!last_error.IsEmpty())
	{
		AfxMessageBox(_T("start page error : ") + last_error, MB_OK | MB_ICONERROR);

		if (page)
		{
			page->Release();
			page = nullptr;
		}
	}

	if (m_markup_page)
	{
		// Release会delete(this)但没有置为nullptr
		m_markup_page->Release();
		m_markup_page = nullptr;
	}

	// 空的也照样赋过去，以便显示空白页面
	m_markup_page = page;

	return;
}

void start_page::load_dynamic_markup()
{
	if (!m_markup_page) return;

	CXTPMarkupStackPanel* stack_panel = dynamic_cast<CXTPMarkupStackPanel*>(m_markup_page->FindName(L"recent_list_items"));
	if (!stack_panel) return;

	// 内部每一个元素为一个历史列表，xml样式为：
	// <TextBlock FontSize = "11px" Height = "20">
	// 	<Hyperlink Foreground = "#FF15428B" TextDecorations = "" Tag = "command|sys.open">最近打开01</Hyperlink>
	// </TextBlock>

	app_ui_interface* app = get_app_ui_interface();
	if (!app) return;

	vector<CString> recent_list;
	app->get_recent_file_list(recent_list);

	for (CString& path_name : recent_list)
	{
		if (!PathFileExists(path_name)) continue;

		CString file_name = file_tools::get_name_from_file_path_name(path_name);
		if (file_name.IsEmpty()) continue;

		CXTPMarkupTextBlock* text_block = MARKUP_CREATE(CXTPMarkupTextBlock, this);
		if (!text_block) continue;
		text_block->SetTextAlignment(xtpMarkupTextAlignmentLeft);
		text_block->SetFontSize(11);
		text_block->SetHeight(20);

		CXTPMarkupHyperlink* hyperlink = MARKUP_CREATE(CXTPMarkupHyperlink, this);
		if (!hyperlink) continue;
		hyperlink->SetForeground((COLORREF) 0Xff8B4215); // 0xabgr
		hyperlink->SetTextDecorations(0);
		hyperlink->SetTag(new CXTPMarkupString(_T("command|sys.open|") + path_name));
		hyperlink->SetToolTip(path_name);
		
		CXTPMarkupRun* run_text = MARKUP_CREATE(CXTPMarkupRun, this);
		if (!run_text) continue;		
		run_text->SetText(new CXTPMarkupString(file_name));
		//run_text->SetForeground((COLORREF) 0Xff8B4215); // 0xabgr
		
		hyperlink->GetInlines()->Add(run_text);
		text_block->GetInlines()->Add(hyperlink);

		stack_panel->GetChildren()->Add(text_block);
	}
}

void start_page::DrawMarkup(CXTPMarkupDrawingContext* pDC, CRect rc)
{
	if (!m_markup_page) return;
	if (m_is_page_in_rendering) return;

	m_is_page_in_rendering = true;

	m_markup_page->Measure(pDC, rc.Size());
	m_markup_page->Arrange(rc);
	m_markup_page->Render(pDC);

	m_is_page_in_rendering = false;
}

void start_page::on_hyperlink_clicked_command(const CString& command, const CString& param)
{
	CString domain_text;
	CString cmd_text;
	cmd_container_interface::parse_command(command, domain_text, cmd_text);
	if (cmd_text.IsEmpty()) return;
	if (domain_text.IsEmpty()) domain_text = _T("app");

	cmd_trigger_manager::fire(domain_text, cmd_text, param);
}

void start_page::on_hyperlink_clicked_url(const CString& url)
{
	ShellExecute(GetSafeHwnd(), _T("open"), url, nullptr, nullptr, SW_SHOWNORMAL);
}

void start_page::on_hyperlink_clicked_file(const CString& file_path)
{
	ShellExecute(GetSafeHwnd(), _T("open"), file_path, nullptr, nullptr, SW_SHOWNORMAL);
}

int start_page::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	if (CWnd::OnCreate(lpCreateStruct) == -1) return -1;

	app_ui_interface* app = get_app_ui_interface();
	if (app)
	{
		theme_style_settings settings = app->get_style_and_settings();
		create_markup_page(get_markup_xml_file_path_name(settings.m_style_type));
		load_dynamic_markup();
	}

	// 基类成员必须初始化，否则整个页面无法响应任何消息
	m_hContextWnd = m_hWnd;

	return 0;
}

void start_page::OnSize(UINT type, int cx, int cy)
{
	CWnd::OnSize(type, cx, cy);
}

BOOL start_page::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void start_page::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CXTPClientRect rc(this);

	CXTPBufferDC dcMem(dc);
	dcMem.FillSolidRect(rc, GetSysColor(COLOR_WINDOW));

	CXTPMarkupDrawingContext dcMarkup(this, dcMem);
	DrawMarkup(&dcMarkup, rc);
}

BOOL start_page::OnWndMsg(UINT message, WPARAM wparam, LPARAM lparam, LRESULT* result)
{
	if (m_markup_page && CXTPMarkupContext::OnWndMsg(m_markup_page, message, wparam, lparam, result))
		return TRUE;

	return CWnd::OnWndMsg(message, wparam, lparam, result);
}

void start_page::app_theme_changed(theme_style_settings& settings)
{
	create_markup_page(get_markup_xml_file_path_name(settings.m_style_type));
	load_dynamic_markup();
}

void start_page::OnCommandUpdate(const CString& strCommand, bool& enable, bool& checked)
{
	enable = false;
	checked = false;
}

void start_page::on_hyperlink_clicked(CXTPMarkupObject* sender, CXTPMarkupRoutedEventArgs* args)
{
	CXTPMarkupHyperlink* link_sender = dynamic_cast<CXTPMarkupHyperlink*>(sender);
	if (!link_sender) return;

	CXTPMarkupString* tag = static_cast<CXTPMarkupString*>(link_sender->GetTag());
	if (!tag) return;
		
	CString tag_text = string_tools::wstring_to_CString((LPCWSTR) (*tag));
	
	vector<CString> tag_items;
	string_tools::split_string(tag_items, tag_text, _T("|"));
	if (tag_items.empty())return;
	if (tag_items.front().IsEmpty()) return;

	if (0 == tag_items.front().CompareNoCase(_T("command")))
	{
		CString command = (tag_items.size() >= 2 ? tag_items[1] : _T(""));
		CString param = (tag_items.size() >= 3 ? tag_items[2] : _T(""));
		if (command.IsEmpty()) return;

		on_hyperlink_clicked_command(command, param);
	}
	else if (0 == tag_items.front().CompareNoCase(_T("url")))
	{
		CString url = (tag_items.size() >= 2 ? tag_items[1] : _T(""));
		if (url.IsEmpty()) return;

		on_hyperlink_clicked_url(url);
	}
	else if (0 == tag_items.front().CompareNoCase(_T("file")))
	{
		CString file_path = (tag_items.size() >= 2 ? tag_items[1] : _T(""));
		if (file_path.IsEmpty()) return;

		if (file_tools::is_relative_path(file_path))
		{
			CString first_char_on_left = file_path.Left(1);
			if (_T("/") == first_char_on_left || _T("\\") == first_char_on_left)
				first_char_on_left.Delete(0);

			file_path.Insert(0, file_tools::get_app_path_with_xg());
		}

		on_hyperlink_clicked_file(file_path);
	}
	else
	{
		// nothing;
	}


	args->SetHandled();
}
