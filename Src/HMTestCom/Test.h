
#pragma once


// CTest command target

class CTest : public CCmdTarget
{
	DECLARE_DYNCREATE(CTest)

public:
	CTest();
	virtual ~CTest();

	virtual void OnFinalRelease();

protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CTest)
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	void HelloWorld();

	enum
	{
		dispidHelloWorld = 1L
	};
};


