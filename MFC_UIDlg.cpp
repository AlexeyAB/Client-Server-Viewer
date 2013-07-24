
// MFC_UIDlg.cpp : файл реализации
//

#include "stdafx.h"
#include "MFC_UI.h"
#include "MFC_UIDlg.h"
#include "afxdialogex.h"

#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

template<typename T>
static inline CString cast_to_cstring(T const& src) {
	return CString(boost::lexical_cast<std::string>(src).c_str());
}


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Данные диалогового окна
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

// Реализация
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// диалоговое окно CMFC_UIDlg



CMFC_UIDlg::CMFC_UIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFC_UIDlg::IDD, pParent), file_view(17)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFC_UIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFC_UIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFC_UIDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFC_UIDlg::OnBnClickedButton2)
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// обработчики сообщений CMFC_UIDlg

BOOL CMFC_UIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Задает значок для этого диалогового окна. Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	CEdit &Edit1 = *((CEdit *)(GetDlgItem(IDC_EDIT1)));
	CEdit &Edit2 = *((CEdit *)(GetDlgItem(IDC_NETADDRESS1)));
	CEdit &Edit3 = *((CEdit *)(GetDlgItem(IDC_EDIT3)));
	CEdit &Edit6 = *((CEdit *)(GetDlgItem(IDC_EDIT6)));
	CEdit &Edit7 = *((CEdit *)(GetDlgItem(IDC_EDIT7)));
	//Edit1.SetLimitText(T_load_file::c_block_size);
	Edit2.SetWindowTextW(CString("127.0.0.1"));
	Edit3.SetWindowTextW(CString("10001"));
	Edit6.SetWindowTextW(CString("1"));
	Edit7.SetWindowTextW(CString("0"));
	connecting_in_progress = false;
	global_pos = 0;
	symbols_per_scroll = 50;

	CScrollBar &ScrollBar = *((CScrollBar *)(GetDlgItem(IDC_SCROLLBAR2)));
	ScrollBar.SetScrollRange(0, 0, TRUE);
	ScrollBar.SetScrollPos(0);

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void CMFC_UIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок. Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CMFC_UIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CMFC_UIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// Try to connect to the server
void CMFC_UIDlg::Create_new_connection(std::string server_address, unsigned int server_port, unsigned int timeout) {
	CEdit &Edit8 = *((CEdit *)(GetDlgItem(IDC_EDIT8)));
	try {
		std::string connecting = "Соединяется с: " + server_address + ":" + boost::lexical_cast<std::string>(server_port);
		Edit8.SetWindowTextW(CString(connecting.c_str()));

		boost::scoped_ptr<ba::io_service> io_service_temp_ptr(new ba::io_service);
		// Use shared_pointer polymorphism
		// (If it need - for reverse conversion need to use boost::dynamic_pointer_cast<T_client>() )
		boost::shared_ptr<T_get_data> client_temp_ptr = 
			boost::make_shared<T_client>(*io_service_temp_ptr.get(), server_port, server_address, timeout );

		{
			// lock mutex for init variables of the new connection
			boost::lock_guard<boost::recursive_mutex> lock_reset_connection(mtx_reset_connection);
			file_view.client_ptr.swap(client_temp_ptr);
			io_service_ptr.swap(io_service_temp_ptr);

			// get file size
			file_size = file_view.get_file_size();

			// calculate scroll bar resolution
			if(file_size < 32700) symbols_per_scroll = 1;
			else symbols_per_scroll = file_size/32700;

			// reset position of file_view object
			file_view.reset_position(symbols_per_scroll);

			// set actual range for the ScrollBar
			CScrollBar &ScrollBar = *((CScrollBar *)(GetDlgItem(IDC_SCROLLBAR2)));
			ScrollBar.SetScrollRange(0, file_size/symbols_per_scroll, TRUE);
			ScrollBar.SetScrollPos(0);

			// set global position
			global_pos = -1;
		}

		// show address:port and size of file
		CEdit &Edit4 = *((CEdit *)(GetDlgItem(IDC_EDIT4)));
		CEdit &Edit5 = *((CEdit *)(GetDlgItem(IDC_EDIT5)));
		Edit4.SetWindowTextW(CString(cast_to_cstring(file_view.get_source())));
		Edit5.SetWindowTextW(CString(cast_to_cstring(file_size)));

		// show data in Edit(Memo)
		show_data(0);

		Edit8.SetWindowTextW(CString("Успешно"));
	} catch(std::exception const& ex) {
		AfxMessageBox(CString(ex.what()));
	} catch(...) {
		AfxMessageBox(CString("Unknown exception!"));
	}
	Edit8.SetWindowTextW(CString(""));
	connecting_in_progress = false;
}

/// Connect to the server
void CMFC_UIDlg::OnBnClickedButton1()
{

	try {
		CEdit &Edit2 = *((CEdit *)(GetDlgItem(IDC_NETADDRESS1)));
		CEdit &Edit3 = *((CEdit *)(GetDlgItem(IDC_EDIT3)));
		CEdit &Edit6 = *((CEdit *)(GetDlgItem(IDC_EDIT6)));
		CString ServerAddress;
		CString ServerPort;
		CString Timeout;
		Edit2.GetWindowTextW(ServerAddress);
		Edit3.GetWindowTextW(ServerPort);
		Edit6.GetWindowTextW(Timeout);
		CT2CA ServerAddressAnsiString(ServerAddress);
		CT2CA ServerPortAnsiString(ServerPort);
		CT2CA TimeoutAnsiString(Timeout);

		std::string server_address = ServerAddressAnsiString;
		unsigned int server_port = boost::lexical_cast<unsigned int>(ServerPortAnsiString);
		unsigned int timeout = boost::lexical_cast<unsigned int>(TimeoutAnsiString);
				
		if(!connecting_in_progress) {
			connecting_in_progress = true;
			boost::thread(boost::bind(&CMFC_UIDlg::Create_new_connection, this, server_address, server_port, timeout)).detach();
			// By default in (BOOST_THREAD_VERSION == 2) in destructor of boost::thread is called: thread_ptr->detach();
			// In (BOOST_THREAD_VERSION >= 3) in destructor is called: if (joinable()) { std::terminate(); }
			// it can provide deadlock. To avoid this we do it(detach) manualy. 
			// http://www.boost.org/doc/libs/1_53_0/boost/thread/detail/thread.hpp
		} else {
			AfxMessageBox(CString("Connecting already in progress!"));
		}

	} catch(std::exception const& ex) {
		AfxMessageBox(CString(ex.what()));
		connecting_in_progress = false;
	} catch(...) {
		AfxMessageBox(CString("Unknown exception!"));
		connecting_in_progress = false;
	}	

	// TODO: добавьте свой код обработчика уведомлений
}


/// Show the text of another page
void CMFC_UIDlg::show_data(const size_t pos) {
	try {
		if(!file_view.client_ptr) {
			// This function can emit event and immediately call to the same handler in this thread and lock the same mutex again.
			// If we use mutex we can get deadlock. To resolve this problem need to use recursive_mutex.
			AfxMessageBox(CString("Try first to connect!"));	
			return;
		}
		CEdit &Edit1 = *((CEdit *)(GetDlgItem(IDC_EDIT1)));
		CEdit &Edit2 = *((CEdit *)(GetDlgItem(IDC_EDIT2)));
		CScrollBar &ScrollBar = *((CScrollBar *)(GetDlgItem(IDC_SCROLLBAR2)));

		std::string res_str = file_view.get_strings_for_pos(pos);
		Edit1.SetWindowTextW(CString(res_str.c_str()));
		Edit2.SetWindowTextW(cast_to_cstring(pos));

	} catch(std::exception const& ex) {
		AfxMessageBox(CString(ex.what()));
	} catch(...) {
		AfxMessageBox(CString("Unknown exception!"));
	}	
}

// Event by scroll bar of Edit1(Memo)
void CMFC_UIDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	try {
		// wait for init variables of the new connection
		boost::lock_guard<boost::recursive_mutex> lock_reset_connection(mtx_reset_connection);
		if(!pScrollBar) return;
		CScrollBar &ScrollBar = *((CScrollBar *)(GetDlgItem(IDC_SCROLLBAR2)));

		if(pScrollBar == &ScrollBar) {

			switch (nSBCode) 
			{
				case SB_THUMBTRACK: 
					ScrollBar.SetScrollPos(nPos); 
					global_pos = nPos*symbols_per_scroll; 
					break;
				/*case SB_THUMBPOSITION: 
					ScrollBar.SetScrollPos(nPos); 
					global_pos = nPos*symbols_per_scroll; 
					break;*/
				case SB_PAGEDOWN: 
					global_pos += 10000;
					if(global_pos > file_size)
						global_pos = file_size;
					ScrollBar.SetScrollPos(global_pos/symbols_per_scroll); 
					break;
				case SB_PAGEUP: 
					if(global_pos >= 10000) global_pos -= 10000;
					else global_pos = 0;
					ScrollBar.SetScrollPos(global_pos/symbols_per_scroll); 
				case SB_LINEDOWN: 
					global_pos += file_view.get_next_row_size();
					if(global_pos > file_size)
						global_pos = file_size;
					ScrollBar.SetScrollPos(global_pos/symbols_per_scroll); 
					break;
				case SB_LINEUP: 
					if(global_pos >= file_view.get_prev_row_size()) global_pos -= file_view.get_prev_row_size();
					else global_pos = 0;
					ScrollBar.SetScrollPos(global_pos/symbols_per_scroll); 
					break;
				default: return;
			}

			show_data(global_pos);

		}

	} catch(std::exception const& ex) {
		AfxMessageBox(CString(ex.what()));
	} catch(...) {
		AfxMessageBox(CString("Unknown exception!"));
	}	
}

/// Go to the entered offset number
void CMFC_UIDlg::OnBnClickedButton2()
{
	// TODO: добавьте свой код обработчика уведомлений
	try {
		// wait for init variables of the new connection
		boost::lock_guard<boost::recursive_mutex> lock_reset_connection(mtx_reset_connection);

		CEdit &Edit7 = *((CEdit *)(GetDlgItem(IDC_EDIT7)));
		CString PageIndex;
		Edit7.GetWindowTextW(PageIndex);
		CT2CA PageIndexAnsiString(PageIndex);
		global_pos = boost::lexical_cast<unsigned int>(PageIndexAnsiString);

		CScrollBar &ScrollBar = *((CScrollBar *)(GetDlgItem(IDC_SCROLLBAR2)));
		ScrollBar.SetScrollPos(global_pos/symbols_per_scroll); 

		show_data(global_pos);

	} catch(std::exception const& ex) {
		AfxMessageBox(CString(ex.what()));
	} catch(...) {
		AfxMessageBox(CString("Unknown exception!"));
	}	
}