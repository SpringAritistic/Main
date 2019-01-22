#pragma once
#include "../interface/serialize_interface.h"

// 这个类用于存储解析之后的弯钩指定信息，仅内部使用
class X_FRAMEWORK_API frame_hook_setting
{
public:
	frame_hook_setting();
	~frame_hook_setting();

public:
	void serialize_members(member_rw_interface& mt);

public:
	CString		m_hook_name;		// 弯钩名称
	CString		m_ref_line_name;	// 参考线名
	CString		m_is_top_hook;		// 弯钩是否用于顶部点，否表示用于底部点

	CString		m_node_index;		// 控制点序号(控制点限制条件完全相同且X坐标相同时，按层号从小到大排，层
									// 号小的在前，层号大的在后)。控制点仅指参照m_ref_line_name的控制点，控
									// 制点从最左边开始算起，一直到参考线右边最后一个为止，第一个序号为1（不
									// 是0），注意确保控制点数据结构识别距离一样远时对应的序号

	CString		m_node_begin_layer; // 弯钩信息是否应于序号为m_node_index的控制点的弯起层，如果为否则表示用于弯止层
	CString		m_segment_name;		// 弯钩m_hook_name所贴的外轮廓线段名
};

// 把指定弯钩信息字符串转换指定弯钩信息类的对象，方便后面操作，算法会预先清除数组
void X_FRAMEWORK_API string_to_hook_settings(const CString& setting_text, std::vector<frame_hook_setting>& setting_data);
void X_FRAMEWORK_API hook_settings_to_string(const std::vector<frame_hook_setting>& setting_data, CString& setting_text);
