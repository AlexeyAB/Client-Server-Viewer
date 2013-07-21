
// MFC_UIDlg.cpp : файл реализации
//

#include "stdafx.h"
#include "MFC_UI.h"
#include "MFC_UIDlg.h"
#include "afxdialogex.h"

#include <boost/lexical_cast.hpp>

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
	: CDialogEx(CMFC_UIDlg::IDD, pParent)
	//, client(io_service, 10001, "127.0.0.1")
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
	ON_EN_VSCROLL(IDC_EDIT1, &CMFC_UIDlg::OnEnVscrollEdit1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CMFC_UIDlg::OnDeltaposSpin1)
	ON_BN_CLICKED(IDC_BUTTON1, &CMFC_UIDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFC_UIDlg::OnBnClickedButton2)
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
	Edit1.SetLimitText(T_load_file::c_block_size);
	Edit2.SetWindowTextW(CString("127.0.0.1"));
	Edit3.SetWindowTextW(CString("10001"));
	Edit6.SetWindowTextW(CString("1"));
	Edit7.SetWindowTextW(CString("0"));
//	show_data(0);

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



void CMFC_UIDlg::OnEnVscrollEdit1()
{
	//CEdit &Edit1 = *((CEdit *)(GetDlgItem(IDC_EDIT1)));
	//Edit1.GetScrollPos();

	//AfxMessageBox(CString("Hi"));
	// TODO: добавьте свой код обработчика уведомлений
}

// Try to connect to the server
void CMFC_UIDlg::Create_new_connection(std::string server_address, unsigned int server_port, unsigned int timeout) {
	CEdit &Edit8 = *((CEdit *)(GetDlgItem(IDC_EDIT8)));
	try {
		std::string connecting = "Соединяется с: " + server_address + ":" + boost::lexical_cast<std::string>(server_port);
		Edit8.SetWindowTextW(CString(connecting.c_str()));

		boost::lock_guard<boost::mutex> lock_new_connection(mtx_new_connection);
		ba::io_service* io_service_raw_ptr = new ba::io_service;
		T_client* client_raw_ptr = new T_client(*io_service_raw_ptr, server_port, server_address, timeout );

		boost::lock_guard<boost::mutex> lock_reset_connection(mtx_reset_connection);
		client_ptr.reset(client_raw_ptr);
		io_service_ptr.reset(io_service_raw_ptr);
		CEdit &Edit4 = *((CEdit *)(GetDlgItem(IDC_EDIT4)));
		CEdit &Edit5 = *((CEdit *)(GetDlgItem(IDC_EDIT5)));		
		Edit4.SetWindowTextW(CString(client_ptr->get_address_n_port().c_str()));
		Edit5.SetWindowTextW(CString(cast_to_cstring(client_ptr->get_max_block_position())));
		show_data(0);
		Edit8.SetWindowTextW(CString("Успешно"));
	} catch(std::exception const& ex) {
		AfxMessageBox(CString(ex.what()));
	} catch(...) {
		AfxMessageBox(CString("Unknown exception!"));
	}
	Edit8.SetWindowTextW(CString(""));
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
				
		if(!mtx_new_connection.try_lock()) {
			AfxMessageBox(CString("Connection already in progress!"));
			return;
		}else
			mtx_new_connection.unlock();

		thread_ptr.reset(
			new boost::thread(boost::bind(&CMFC_UIDlg::Create_new_connection, this, server_address, server_port, timeout)) );

	} catch(std::exception const& ex) {
		AfxMessageBox(CString(ex.what()));
	} catch(...) {
		AfxMessageBox(CString("Unknown exception!"));
	}	

	// TODO: добавьте свой код обработчика уведомлений

}


/// Button to switch the page
void CMFC_UIDlg::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{	
	boost::lock_guard<boost::mutex> lock_reset_connection(mtx_reset_connection);
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	show_data(pNMUpDown->iDelta);	// See the text of another page

	*pResult = 0;
}

/// See the text of another page
void CMFC_UIDlg::show_data(const int delta_index) {
	try {		
		if(!client_ptr) {
			AfxMessageBox(CString("Try first to connect!"));
			return;
		}
		CEdit &Edit1 = *((CEdit *)(GetDlgItem(IDC_EDIT1)));
		CEdit &Edit2 = *((CEdit *)(GetDlgItem(IDC_EDIT2)));

		size_t block_position = client_ptr->get_block_position();
		Edit2.SetWindowTextW(cast_to_cstring(block_position));

		const size_t read_size = client_ptr->reposition_and_load(delta_index);

		//block_position = load_file.get_block_position();
		if(block_position != client_ptr->get_block_position() || delta_index == 0) {
			block_position = client_ptr->get_block_position();
			Edit2.SetWindowTextW(cast_to_cstring(block_position));
			Edit1.SetWindowTextW(CString(client_ptr->get_buffer()));
		}
	} catch(std::exception const& ex) {
		AfxMessageBox(CString(ex.what()));
	} catch(...) {
		AfxMessageBox(CString("Unknown exception!"));
	}	
}

/// Go to the entered page number
void CMFC_UIDlg::OnBnClickedButton2()
{
	// TODO: добавьте свой код обработчика уведомлений
	try {
		boost::lock_guard<boost::mutex> lock_reset_connection(mtx_reset_connection);
		if(!client_ptr) {
			AfxMessageBox(CString("Try first to connect!"));
			return;
		}

		CEdit &Edit1 = *((CEdit *)(GetDlgItem(IDC_EDIT1)));
		CEdit &Edit2 = *((CEdit *)(GetDlgItem(IDC_EDIT2)));
		CEdit &Edit7 = *((CEdit *)(GetDlgItem(IDC_EDIT7)));
		CString PageIndex;
		Edit7.GetWindowTextW(PageIndex);
		CT2CA PageIndexAnsiString(PageIndex);
		client_ptr->load(boost::lexical_cast<unsigned int>(PageIndexAnsiString));
		Edit2.SetWindowTextW(cast_to_cstring(client_ptr->get_block_position()));
		Edit1.SetWindowTextW(CString(client_ptr->get_buffer()));

	} catch(std::exception const& ex) {
		AfxMessageBox(CString(ex.what()));
	} catch(...) {
		AfxMessageBox(CString("Unknown exception!"));
	}	
}
