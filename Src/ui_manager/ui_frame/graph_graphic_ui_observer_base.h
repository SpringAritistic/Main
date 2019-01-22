#pragma once


// 图纸图形化编辑窗口观察者基类
class UI_MANAGER_API graph_graphic_ui_observer_base : public CObject
{
	DECLARE_SERIAL(graph_graphic_ui_observer_base)

public:
	graph_graphic_ui_observer_base();
	virtual ~graph_graphic_ui_observer_base();

public:
	virtual void save_data_from_ui();
	virtual void load_data_to_ui();

public:
	// 图纸数据的tag，用于从数据中心索取图纸数据
	CString m_graph_data_tag;
};
