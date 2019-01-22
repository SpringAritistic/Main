#pragma once

class local_resource
{
public:
	local_resource();
	~local_resource();

private:
	HINSTANCE   m_old_resource;
};