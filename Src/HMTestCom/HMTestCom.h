// HMTestCom.h : main header file for the HMTestCom DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CHMTestComApp
// See HMTestCom.cpp for the implementation of this class
//

class CHMTestComApp : public CWinApp
{
public:
	CHMTestComApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
