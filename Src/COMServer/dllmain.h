// dllmain.h: 模块类的声明。

class CCOMServerModule : public ATL::CAtlDllModuleT< CCOMServerModule >
{
public :
	DECLARE_LIBID(LIBID_COMServerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_COMSERVER, "{34233144-1299-4C54-87C7-106DA076F151}")
};

extern class CCOMServerModule _AtlModule;
