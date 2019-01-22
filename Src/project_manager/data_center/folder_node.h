#pragma once

#include "x_framework/macro.h"
#include "x_framework/interface/data_center_interface.h"
#include "x_framework/interface/serialize_interface.h"


class PROJECT_MANAGER_API folder_node : public data_node_base
{
	DECLARE_SERIAL(folder_node)

public:
	folder_node(const CString& name = _T(""));
	virtual ~folder_node();

public:
	virtual void serialize_members(member_rw_interface& mt);
};
