
// MFC_UI.h : главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CMFC_UIApp:
// О реализации данного класса см. MFC_UI.cpp
//

class CMFC_UIApp : public CWinApp
{
public:
	CMFC_UIApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CMFC_UIApp theApp;