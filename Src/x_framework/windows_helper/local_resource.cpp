#include "stdafx.h"
#include "local_resource.h"


local_resource::local_resource(void)
{
	extern HINSTANCE g_resource_handle;

	m_old_resource = AfxGetResourceHandle();
	AfxSetResourceHandle(g_resource_handle);
}

local_resource::~local_resource(void)
{
	AfxSetResourceHandle(m_old_resource);
}