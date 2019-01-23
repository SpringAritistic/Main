// Test.cpp : implementation file
//

#include "stdafx.h"
#include "HMTestCom.h"
#include "Test.h"


// CTest

IMPLEMENT_DYNCREATE(CTest, CCmdTarget)


CTest::CTest()
{
	EnableAutomation();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
}

CTest::~CTest()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();
}


void CTest::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CTest, CCmdTarget)
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CTest, CCmdTarget)
	DISP_FUNCTION_ID(CTest, "HelloWorld", dispidHelloWorld, HelloWorld, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

// Note: we add support for IID_ITest to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .IDL file.

// {F6652258-E66F-40DA-9AE4-1FA1A000243A}
static const IID IID_ITest =
{ 0xF6652258, 0xE66F, 0x40DA, { 0x9A, 0xE4, 0x1F, 0xA1, 0xA0, 0x0, 0x24, 0x3A } };

BEGIN_INTERFACE_MAP(CTest, CCmdTarget)
	INTERFACE_PART(CTest, IID_ITest, Dispatch)
END_INTERFACE_MAP()

// {8C232D31-CC22-487C-901E-C9FB883D3D74}
IMPLEMENT_OLECREATE_FLAGS(CTest, "HMTestCom.Test", afxRegApartmentThreading, 0x8c232d31, 0xcc22, 0x487c, 0x90, 0x1e, 0xc9, 0xfb, 0x88, 0x3d, 0x3d, 0x74)


// CTest message handlers


void CTest::HelloWorld()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	AfxMessageBox(_T("Hello World!"));

}
		