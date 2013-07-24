
// MFC_UIDlg.h : файл заголовка
//

#pragma once
#include "load_file.hpp"
#include "server.hpp"
#include "client.hpp"
#include "file_view.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>

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

	size_t symbols_per_scroll;	/// symbols per single step of scroll bar (excluding scroll buttons-rows)	
	size_t global_pos;			/// global position in remote file
	size_t file_size;			/// size of remote file

	boost::scoped_ptr<ba::io_service> io_service_ptr;	/// unique pointer, of I/O service for exchange data with the server
	T_file_view file_view;								/// object, that allows to view the remote file
	boost::recursive_mutex mtx_reset_connection;		/// recursive_mutex for thread-safe using of the unique pointer to the client object
	//boost::mutex mtx_reset_connection;				/// mutex for thread-safe using of the unique pointer to the client object
	volatile bool connecting_in_progress;				/// volatile flag - for thread-safe detecting, that connecting in progress

	// Try to connect to the server
	void Create_new_connection(std::string server_address, unsigned int server_port, unsigned int timeout);

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP()
public:

	/// Show the text of another page
	void show_data(const size_t pos);
};