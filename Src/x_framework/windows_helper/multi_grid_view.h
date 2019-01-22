#pragma once

class CTHExcel;
class CTHDataBase;


#ifndef ID_MULTI_GRID_START
#	define ID_MULTI_GRID_START 2000
#endif

#ifndef ID_MULTI_GRID_END
#	define ID_MULTI_GRID_END   5000
#endif

#ifndef WM_TAB_ITEM_BE_CLICKED
#define WM_TAB_ITEM_BE_CLICKED    WM_USER + 129
#endif


struct X_FRAMEWORK_API multi_grid_item_info
{
	UINT			m_wnd_id = -1;				// excel创建后的窗口id
	bool			m_is_visible = true;		// 是否需要显示
	CString			m_db_name;					// 表格实例的索引名
	CTHExcel*		m_excel = nullptr;			// 装载表格的窗口
	CTHDataBase*	m_db = nullptr;				// 表格实例
};


class X_FRAMEWORK_API multi_grid_view : public CFormView
{
	DECLARE_SERIAL(multi_grid_view);
	DECLARE_MESSAGE_MAP()

public:
	multi_grid_view(UINT nIDTemplate = 0, bool need_delete_self_post_nc_destroy = true);
	virtual ~multi_grid_view(void);
	
public:
	BOOL create_from_static(CWnd* parent, UINT id, BOOL has_border = TRUE);
	BOOL create_direct(CWnd* parent, UINT id, const RECT& rect, BOOL has_border );
	void assign_all_excels(const std::vector<CString>& template_names); // 根据内部表模板名重新创建所有空表
	void assign_all_excels(const std::vector<std::pair<CString, CTHDataBase*>>& template_names); // 根据表格索引名和表格实例创建所有表格，表格必须是堆上的对象，内存由框架接管
	void add_excel(const CString& template_name);
	void add_excel(const CString& index_name, CTHDataBase* instance); // 根据表格索引名和表格实例添加表格，表格必须是堆上的对象，内存由框架接管
	void clear_all_excels_and_data(); // 清空所有表格
	void set_back_color(COLORREF color);
	CTHExcel* find_excel(const CString& data_base_name);
	CTHExcel* find_excel(UINT id);
	CTHDataBase* find_data_base(const CString& data_base_name);
	CTHDataBase* find_data_base(UINT id);
	void set_excel_visible(const CString& data_base_name, bool show = false);
	void set_excel_visible(UINT id, bool show = false);
	bool is_excel_visible(const CString& data_base_name) const;
	bool is_excel_visible(UINT id) const;
	void get_all_data_base(std::map<CString, CTHDataBase*>& dbs);
	void enable_auto_resize_excel(bool auto_resize_excel);
	void resize_excels();

public:
	virtual void OnInitialUpdate();
	virtual void PostNcDestroy();
	
protected:
	std::vector<multi_grid_item_info> m_all_excels; // 表格id，是否要显示，表格窗口实例，表格DB实例
	COLORREF						m_back_color;
	bool							m_auto_resize_excel;
	int								m_total_width;
	int								m_total_height;
	bool							m_is_standard_editor;
	bool							m_is_initialize;
	bool							m_need_delete_self_post_nc_destroy;

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);

	afx_msg BOOL on_remove_data_in_grid(UINT id, NMHDR* notify_struct, LRESULT* result);
	afx_msg BOOL on_add_data_in_grid(UINT id, NMHDR* notify_struct, LRESULT* result);
	afx_msg BOOL on_row_col_size_changed(UINT id, NMHDR* notify_struct, LRESULT* result);
	afx_msg BOOL on_cell_value_changed(UINT id, NMHDR* notify_struct, LRESULT* result);
	afx_msg BOOL on_cell_button_clicked(UINT id, NMHDR* notify_struct, LRESULT* result);
	afx_msg BOOL on_cell_focus_changed(UINT id, NMHDR* notify_struct, LRESULT* result);
};



// 一个能接收表格消息的标签类，且支持响应选项卡单击和返回控制尺寸(并不限定只能放多表控件)
// 无论内部放什么窗口，只要有表格，则父窗口必须是本标签控件，或者标签页内
// 的窗口是multi_grid_view也可以，否则要将消息往上层传递，直到本控件能接收到，否则本控件无法响应，
// 另外，只有表格的ID在ID_MULTI_GRID_START和ID_MULTI_GRID_END之间
class X_FRAMEWORK_API multi_grid_tab_control : public CXTPTabControl
{
	DECLARE_DYNCREATE(multi_grid_tab_control)
	DECLARE_MESSAGE_MAP()

public:
	multi_grid_tab_control();
	virtual ~multi_grid_tab_control();

public:
	int get_tab_header_height() const; // 含选项卡外侧的控件边框宽度
	int get_tab_border_width() const;

public:
	// 改写此虚函数，在保证原功能不变的情况下向父窗口发WM_TAB_ITEM_BE_CLICKED消息，便于父窗口不派生本窗口即可使用
	virtual void OnItemClick(CXTPTabManagerItem* pItem);

private:
	afx_msg BOOL on_remove_data_in_grid(UINT id, NMHDR* notify_struct, LRESULT* result);
	afx_msg BOOL on_add_data_in_grid(UINT id, NMHDR* notify_struct, LRESULT* result);
	afx_msg BOOL on_row_col_size_changed(UINT id, NMHDR* notify_struct, LRESULT* result);
	afx_msg BOOL on_cell_value_changed(UINT id, NMHDR * notify_struct, LRESULT * result);
	afx_msg BOOL on_cell_button_clicked(UINT id, NMHDR * notify_struct, LRESULT * result);
	afx_msg BOOL on_cell_focus_changed(UINT id, NMHDR * notify_struct, LRESULT * result);
};
