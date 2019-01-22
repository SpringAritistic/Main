#pragma once
#include "client_tab_view.h"

// 工作空间的id由mfc内定，如果基类是CView，则必须关联文档，否则关闭时崩溃
class workspace_view : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	workspace_view();
	virtual ~workspace_view();

public:
	// 获得当前的标签个数
	int get_tab_count() const;

	// 获取当前选中的标签索引（从0开始）没有则返回-1
	int get_current_tab_index() const;

	// 添加一个项目到控件中（会记录原窗口的指针），返回添加后的tab所在的标签序号
	// 如果路径为空，说明没有关联的管理对象，这类窗口只会添加一个（如起始页）
	// 注意：
	//   如果窗口为CView的派生类，因CView会在窗口销毁之后OnPostDestroy调用delete this，且无法置空，导致框架中
	//   指针悬空，销毁窗口时崩溃，因此对CView及其派生类要特殊处理，重载PostNcDestroy虚函数并禁止调用CView的版本
	//   即可，也不用再调用delete，工作区间控件会正确析构，这并不会造成内存泄漏
	int add_tab_window_and_info(const CString window_class, const CString node_tag, int index = -1, int image_index = -1);

	// 添加一个外部实例化好的窗口到工作区，并返回添加后的tab所在的标签序号，内存由工作区拉管，注意：这个窗口必须是在堆上分配的
	// 外部只要创建好窗口即可，数据的加载等流程性接口和属性设置由框架自动完成（除非特殊需求，否则不推荐使用，尽量使用窗口类的重载版本）
	// 注意：
	//   如果窗口为CView的派生类，因CView会在窗口销毁之后OnPostDestroy调用delete this，且无法置空，导致框架中
	//   指针悬空，销毁窗口时崩溃，因此对CView及其派生类要特殊处理，重载PostNcDestroy虚函数并禁止调用CView的版本
	//   即可，也不用再调用delete，工作区间控件会正确析构，这并不会造成内存泄漏
	int add_tab_working_space_view(window_interface* wnd, const CString node_tag, int index = -1, int image_index = -1);

	// 取得指定标签的窗口指针(送入时的指针）
	CWnd* get_tab_window(int index);

	// 关闭所有窗口（会自动调用保存界面数据的接口）
	bool close_all_window(bool force_close = false);

	// 关闭并保存指定索引的窗口（索引从0开始）成功返回true，失败返回false（此时保证不移除）
	bool close_tab_by_index(int index, bool force_close = false);

	// 保存所有窗口
	void save_all_window_data();

	// 删除指定管数据对象路径的所有窗口，不论管理类是否相同均删除
	bool remove_windows_by_path(const CString& object_path);

	// 获取指定标签的窗口附加信息 0：窗口类名， 1：管理对象tag，2：窗口指针
	bool get_tab_window_and_info(std::tuple<CString, CString, CWnd*>& info, int index) const;

	// 确保选项卡最多只有指定的个数，少于这个数目直接返回，多余则从前面开始移除
	// 操作成功返回true,否则返回false，参数removed_count返回移除的窗口数目（操作失败也可能移除了一部分）
	bool make_tab_max_count(int max_count, int& removed_count);

	// 设置指定索引的选项卡的标题文字
	void set_tab_caption(int index, const CString& caption);

	// 更新一下指定索引标签的选项卡文字,-1表示全部更新
	void update_tab_caption(int index);

protected:
	client_tab_view m_client_tab;
	int m_max_tab_count = 30;

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();

	afx_msg void on_workspace_popup_menu_save();
	afx_msg void on_workspace_popup_menu_close();
	afx_msg void on_workspace_popup_menu_save_all();
	afx_msg void on_workspace_popup_menu_close_all();

};
