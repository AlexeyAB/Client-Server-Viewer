
// MFC_UI.h : ������� ���� ��������� ��� ���������� PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#include "resource.h"		// �������� �������


// CMFC_UIApp:
// � ���������� ������� ������ ��. MFC_UI.cpp
//

class CMFC_UIApp : public CWinApp
{
public:
	CMFC_UIApp();

// ���������������
public:
	virtual BOOL InitInstance();

// ����������

	DECLARE_MESSAGE_MAP()
};

extern CMFC_UIApp theApp;