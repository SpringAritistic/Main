#pragma once

// 工作空间标签控件
class client_tab_view : public CXTPTabControl
{
	DECLARE_MESSAGE_MAP()

public:
	client_tab_view();
	virtual ~client_tab_view();

public:
	// 重设标签状态，并根据当前激活标签处理ribbon上下文标签状态，创建完成或标签数目发生变化时调用
	void reset_tab_status();

	// 返回标签总数
	int get_tab_count() const;

	// 关闭标签并清理相应的缓存信息，有标签被删除，则返回true，否则返回false
	// force_close为true表示框架上不判断窗口数据是否要保存，不判断窗口能不能被关闭，直接强制关闭，主要用于主程序直接关闭
	bool close_tab_and_clear_info(int index, bool force_close = false);
	bool close_all_tab_and_clear_info(bool force_close = false);
	
	// 保存标签对应的窗口数据
	void save_tab_window_data(int index);
	void save_all_tab_window_data();

	// 查找指定管理类名，且对象路径相同的窗口是否已经存在
	bool is_special_window_exist(const CString& window_class, const CString& object_path) const;
	
	// 返回指定管理类名，且对象路径相同的窗口所在标签索引，如果不存在则返回-1
	int get_special_window_index(const CString& window_class, const CString& object_path) const;

	// 删除指定管理类名，且对象路径相同的窗口临时记录信息
	void remove_special_window_info(const CString& window_class, const CString& object_path);

	// 删除指定管数据对象路径的所有窗口，不论管理类是否相同均删除
	bool remove_windows_by_path(const CString& object_path);

	// 添加一个窗口到控件中（会记录原窗口的指针），返回添加后的tab所在的标签序号
	// 如果对应的对象已经添加过，则直接返回所在tab索引，操作无法正常执行返回-1
	// 注意本方法仅添加标签，不允许对标签的激活状态作变动（除非原激活标签不存在等特殊情况）
	int add_tab_window_and_info(const CString& window_class, const CString& node_tag, int index = -1, int image_index = -1);

	// 添加一个外部实例化好的窗口到控件中，并返回添加后的tab所在的标签序号，内存由工作区拉管，注意：这个窗口必须是在堆上分配的
	// 外部只要创建好窗口即可，数据的加载等流程性接口和属性设置由框架自动完成
	int add_tab_window_and_info(window_interface* wnd, const CString node_tag, int index = -1, int image_index = -1);

	// 取得指定标签的窗口指针(送入时的指针）
	CWnd* get_tab_window(int index) const;
	window_interface* get_tab_window_interface(int index) const;
	
	// 获取指定标签的窗口附加信息 0：窗口类名， 1：管理对象tag，2：窗口指针
	bool get_tab_window_and_info(std::tuple<CString, CString, CWnd*>& info, int index) const;

	// 获取上一次右键点击的标签序号，如果无效则返回-1（这个序号由外部维护，一旦菜单响应完成请设置为-1，内部当标签数目变动会也会自动清为-1
	int get_last_right_clicked_tab_index() const;
	void clear_last_right_clicked_tab_index();

protected:
	int implement_add_tab_window_and_info(window_interface* wnd, const CString node_tag, int index, int image_index);

protected:
	// 上次右键点击的选项卡（只负责暂记，不负责追踪选项卡是否有效，仅用于tab标签的右键菜单响应）
	CXTPTabManagerItem* m_last_right_clicked_tab;

	// 只记录对象，不管理指针(0：窗口类名， 1：管理对象tag，2：窗口指针)
	std::vector<std::tuple<CString, CString, CWnd*>> m_all_windows;

protected:
	// 标签被点击或拖动导致活动标签状态变化时调用
	virtual void OnItemClick(CXTPTabManagerItem* pItem);

protected:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
};

