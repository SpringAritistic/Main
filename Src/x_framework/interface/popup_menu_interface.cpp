#include "StdAfx.h"

#include "popup_menu_interface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;



popup_menu_item::popup_menu_item(const CString& text,
	std::function<void()> on_command /*= nullptr*/,
	std::function<void(bool&, bool&, bool&)> update_status /*= nullptr*/)
{
	m_text = text;
	m_do_command = on_command;
	m_update_status = update_status;
	m_id = make_menu_item_id();
}

popup_menu_item* popup_menu_item::add_sub_item( const CString& text,
	std::function<void()> on_command /*= nullptr*/,
	std::function<void(bool&, bool&, bool&)> update_status /*= nullptr*/)
{
	popup_menu_item* item = new popup_menu_item(text, on_command, update_status);
	m_sub_menus.push_back(item);
	return item;
}

popup_menu_item* popup_menu_item::add_sub_item(popup_menu_item* sub_mu)
{
	if (!sub_mu) return nullptr;
	m_sub_menus.push_back(sub_mu);
	return sub_mu;
}

popup_menu_item::~popup_menu_item()
{
	clear_items();
}

void popup_menu_item::clear_items()
{
	for (auto& x : m_sub_menus) safe_delete(x);
	m_sub_menus.clear();

	m_update_status = nullptr;
	m_do_command = nullptr;

	for (CMenu* x : m_created_menus)
	{
		x->DestroyMenu();
		safe_delete(x);
	}

	m_created_menus.clear();
}

unsigned popup_menu_item::make_menu_item_id()
{
	static unsigned int current_menu_id = APP_POPUP_MENU_ID_START;

	++current_menu_id;
	if (current_menu_id > APP_POPUP_MENU_ID_END)
		current_menu_id = APP_POPUP_MENU_ID_START;

	return current_menu_id;
}

void popup_menu_item::make_menu_implement(CMenu& parent_menu)
{
	if (m_text.IsEmpty()) return;
	if (!parent_menu.GetSafeHmenu()) return;

	bool menu_item_enabled = false;
	bool menu_item_hidden = false;
	bool menu_item_checked = false;

	// 如果菜单项没有状态处理函数，则只要有事件处理函数就默认把菜单的enabled置为true，否则按状态处理函数处理
	if (!m_update_status && m_do_command) menu_item_enabled = true;
	
	if (m_update_status) m_update_status(menu_item_enabled, menu_item_hidden, menu_item_checked);
	if (menu_item_hidden) return;

	if (_T("-") == m_text)
	{
		parent_menu.AppendMenu(MF_SEPARATOR);
	}
	else if (!m_sub_menus.empty())
	{
		CMenu* sub_menu = new CMenu();
		sub_menu->CreatePopupMenu();
		m_created_menus.push_back(sub_menu);

		for (popup_menu_item* x : m_sub_menus)
			if (x) x->make_menu_implement(*sub_menu);

		parent_menu.AppendMenu(MF_STRING | MF_POPUP | (menu_item_enabled ? MF_ENABLED : MF_DISABLED),
			(UINT_PTR) sub_menu->GetSafeHmenu(), m_text);
	}
	else
	{
		m_id = popup_menu_item::make_menu_item_id();
		parent_menu.AppendMenu(MF_STRING | (menu_item_checked ? MF_CHECKED : MF_UNCHECKED)
			| (menu_item_enabled ? MF_ENABLED : MF_DISABLED), (UINT_PTR) m_id, m_text);
	}
}

popup_menu_interface::popup_menu_interface()
{
	// 不能在此调用，此时没有多态性，将其改为纯虚函数更好
	// initialize_menus();
}

popup_menu_interface::~popup_menu_interface()
{
	clear_all_menu_items();
}

popup_menu_item* popup_menu_interface::add_popup_menu( const CString& tag)
{
	popup_menu_item* item = nullptr;
	auto it = m_all_menu_roots.find(tag);
	if (m_all_menu_roots.end() == it)
	{
		item = new popup_menu_item(_T(""));
		m_all_menu_roots.insert(make_pair(tag, item));
	}
	else
	{
		item = it->second;
	}

	return item;
}

void popup_menu_interface::clear_all_menu_items()
{
	for (auto& x : m_all_menu_roots) safe_delete(x.second);
	m_all_menu_roots.clear();
}

void popup_menu_interface::popup_menu(const CString& tag)
{
	CWnd* wnd = dynamic_cast<CWnd*>(this);
	if (!wnd) return;

	CPoint cursor_pos;
	GetCursorPos(&cursor_pos);

	auto it = m_all_menu_roots.find(tag);
	if (m_all_menu_roots.end() == it) return;
	if (!it->second || it->second->m_sub_menus.empty()) return; // 子菜单都没有

	CMenu menu_root;
	menu_root.CreatePopupMenu();

	vector<CMenu*> created_manu_item;
	for (popup_menu_item* x : it->second->m_sub_menus)
		if (x) x->make_menu_implement(menu_root);

	// 如果创建的菜单项第一级全是隐藏的，或者没有菜单项，则不弹出
	if (menu_root.GetMenuItemCount() > 0)
		menu_root.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, cursor_pos.x, cursor_pos.y, wnd);

	for (CMenu* x : created_manu_item)
	{
		if (!x) continue;
		x->DestroyMenu();
		safe_delete(x);
	}

	created_manu_item.clear();
	menu_root.DestroyMenu();
}

bool popup_menu_interface::on_transform_popup_menu_command_implement(popup_menu_item *item, unsigned id)
{
	if (!item) return false;

	if (id == item->m_id)
	{
		if (item->m_do_command) 
			item->m_do_command();
		
		// 响应函数为空也表示正常响应了
		return true;
	}

	for (auto& x : item->m_sub_menus)
	{
		if (!x) continue;
		if (on_transform_popup_menu_command_implement(x, id)) return true;
	}

	return false;
}

void popup_menu_interface::on_transform_popup_menu_command(unsigned id)
{
	for (auto& x : m_all_menu_roots)
	{
		if (!x.second) continue;
		if (on_transform_popup_menu_command_implement(x.second, id)) return;
	}
}
