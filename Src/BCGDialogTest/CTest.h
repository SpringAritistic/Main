// Machine generated IDispatch wrapper class(es) created with Add Class from Typelib Wizard

#import "F:\\清风细雨\\C++练习\\工程—练习\\Main\\Release\\Bin\\HMTestCom.dll" no_namespace
// CTest wrapper class

class CTest : public COleDispatchDriver
{
public:
	CTest(){} // Calls COleDispatchDriver default constructor
	CTest(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CTest(const CTest& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// Attributes
public:

	// Operations
public:


	// ITest methods
public:
	void HelloWorld()
	{
		InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

	// ITest properties
public:

};
