
// DialogTest.h : main header file for the DialogTest application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CDialogTestApp:
// See DialogTest.cpp for the implementation of this class
//

class CDialogTestApp : public CWinAppEx
{
public:
	CDialogTestApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CDialogTestApp theApp;
