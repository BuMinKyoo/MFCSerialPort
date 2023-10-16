
// MFCSerialPortDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCSerialPort.h"
#include "MFCSerialPortDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCSerialPortDlg 대화 상자



CMFCSerialPortDlg::CMFCSerialPortDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCSERIALPORT_DIALOG, pParent)
	, m_str_comport(_T(""))
	, m_str_baudrate(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCSerialPortDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_COMBO_COMPORT, m_str_comport);
	DDX_CBString(pDX, IDC_COMBO_BAUDRATE, m_str_baudrate);
	DDX_Control(pDX, IDC_EDIT_RCV_VIEW, m_edit_rcv_view);
	DDX_Control(pDX, IDC_EDIT_SEND_DATA, m_edit_send_data);
	DDX_Control(pDX, IDC_COMBO_COMPORT, m_combo_comport_list);
	DDX_Control(pDX, IDC_COMBO_BAUDRATE, m_combo_baudrate_list);
}

BEGIN_MESSAGE_MAP(CMFCSerialPortDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_MESSAGE(WM_MYCLOSE, &CMFCSerialPortDlg::OnThreadClosed)
	ON_MESSAGE(WM_MYRECEIVE, &CMFCSerialPortDlg::OnReceive)

	ON_BN_CLICKED(IDC_CONNECT_BTN, &CMFCSerialPortDlg::OnBnClickedConnectBtn)
	ON_BN_CLICKED(IDC_CLEAR_BTN, &CMFCSerialPortDlg::OnBnClickedClearBtn)
	ON_BN_CLICKED(IDC_SEND_BTN, &CMFCSerialPortDlg::OnBnClickedSendBtn)
	ON_BN_CLICKED(IDC_TEST_BTN, &CMFCSerialPortDlg::OnBnClickedTestBtn)
	ON_CBN_SELCHANGE(IDC_COMBO_COMPORT, &CMFCSerialPortDlg::OnCbnSelchangeComboComport)
	ON_CBN_SELCHANGE(IDC_COMBO_BAUDRATE, &CMFCSerialPortDlg::OnCbnSelchangeComboBaudrate)
END_MESSAGE_MAP()


// CMFCSerialPortDlg 메시지 처리기

BOOL CMFCSerialPortDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_combo_comport_list.AddString(_T("COM1"));
	m_combo_comport_list.AddString(_T("COM2"));
	m_combo_comport_list.AddString(_T("COM3"));
	m_combo_comport_list.AddString(_T("COM4"));
	m_combo_comport_list.AddString(_T("COM5"));
	m_combo_comport_list.AddString(_T("COM6"));
	m_combo_comport_list.AddString(_T("COM7"));
	m_combo_comport_list.AddString(_T("COM8"));

	m_combo_baudrate_list.AddString(_T("9600"));
	m_combo_baudrate_list.AddString(_T("19200"));
	m_combo_baudrate_list.AddString(_T("115200"));

	comport_state = FALSE;
	GetDlgItem(IDC_CONNECT_BTN)->SetWindowText(_T("연결"));
	UpdateData(FALSE);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCSerialPortDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCSerialPortDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CMFCSerialPortDlg::OnThreadClosed(WPARAM length, LPARAM lpara)
{
	//overlapped i/o 핸들을 닫는다.
	((Rs232*)lpara)->HandleClose();
	delete ((Rs232*)lpara);

	return 0;
}

LRESULT CMFCSerialPortDlg::OnReceive(WPARAM length, LPARAM lpara)
{
	CString str;
	char data[20000];
	if (m_Rs232_comm)
	{
		m_Rs232_comm->Receive(data, length); // length 길이만큼 데이터를 받는다
		data[length] = _T('\0');
		str += _T("\r\n");
		for (int i = 0; i < length; i++)
		{
			str += data[i];
		}
		m_edit_rcv_view.ReplaceSel(str); // 에디트박스에표시하기위함
		str = "";

		m_edit_rcv_view.LineScroll(m_edit_rcv_view.GetLineCount()); // 스크롤바를 맨아래로 내린다
	}

	return 0;
}


void CMFCSerialPortDlg::OnBnClickedConnectBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (comport_state)
	{
		if (m_Rs232_comm) // 컴포트가 존재하면
		{
			m_Rs232_comm->Close();
			m_Rs232_comm = NULL;
			AfxMessageBox(_T("COM포트가 닫혔습니다."));
			comport_state = false;
			GetDlgItem(IDC_CONNECT_BTN)->SetWindowText(_T("COM포트 열기"));
			GetDlgItem(IDC_SEND_BTN)->EnableWindow(FALSE);
		}
	}
	else
	{
		// "\\\\.\\"는 CreateFile 함수 때문에 넣어 주는 것입니다. COM1 ~ COM9 까지는 앞의 내용이 없어도 정상적으로 동작하지만 시리얼포트 번호가 COM10 이상이 되면 앞에 "\\\\.\\" 문자열을 넣지 않으면 INVALID_HANDLE_VALUE 즉 NULL 이 리턴되어 시리얼포트가 열리지 않습니다.
		m_Rs232_comm = new Rs232(_T("\\\\.\\") + m_str_comport, m_str_baudrate, _T("None"), _T("8 Bit"), _T("1 Bit")); // initial Comm port
		if (m_Rs232_comm->Create(GetSafeHwnd()) != 0) // 통신포트를 열고 윈도우의 핸들을 넘긴다
		{
			AfxMessageBox(_T("COM포트가 열렸습니다."));
			comport_state = true;
			GetDlgItem(IDC_CONNECT_BTN)->SetWindowText(_T("COM포트 닫기"));
			GetDlgItem(IDC_SEND_BTN)->EnableWindow(TRUE);
		}
		else
		{
			AfxMessageBox(_T("COM포트가 열리지 않았습니다."));
		}
	}
}


void CMFCSerialPortDlg::OnBnClickedClearBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_EDIT_RCV_VIEW)->SetWindowText(_T(""));
}


void CMFCSerialPortDlg::OnBnClickedSendBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString str;
	GetDlgItem(IDC_EDIT_SEND_DATA)->GetWindowText(str);
	str += "\r\n";
	m_Rs232_comm->Send(str, str.GetLength());
}


void CMFCSerialPortDlg::OnBnClickedTestBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMFCSerialPortDlg::OnCbnSelchangeComboComport()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
}


void CMFCSerialPortDlg::OnCbnSelchangeComboBaudrate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
}
