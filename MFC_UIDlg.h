
// MFC_UIDlg.h : файл заголовка
//

#pragma once
#include "load_file.hpp"
#include "server.hpp"
#include "client.hpp"

#include <boost/scoped_ptr.hpp>

// диалоговое окно CMFC_UIDlg
class CMFC_UIDlg : public CDialogEx
{
// Создание
public:
	CMFC_UIDlg(CWnd* pParent = NULL);	// стандартный конструктор
	~CMFC_UIDlg() { }

// Данные диалогового окна
	enum { IDD = IDD_MFC_UI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	boost::scoped_ptr<boost::thread> thread_ptr;
	boost::scoped_ptr<ba::io_service> io_service_ptr;
	boost::scoped_ptr<T_client> client_ptr;
	boost::mutex mtx_new_connection, mtx_reset_connection;
	void Create_new_connection(std::string server_address, unsigned int server_port, unsigned int timeout);

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnVscrollEdit1();
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);

	void show_data(const int delta_index);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};