
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
	GetDlgItem(IDC_CONNECT_BTN)->SetWindowText(_T("포트 열기"));
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
	char data[2000];
	if (m_Rs232_comm)
	{
		m_Rs232_comm->Receive(data, length); // length 길이만큼 데이터를 받는다
		data[length] = _T('\0');
		str += _T("\r\n");
		for (int i = 0; i < length; i++)
		{
			str += data[i];
		}

		///////////////////////////////
		///////////////////////////////

		// 이곳에 기타 로직넣기

		///////////////////////////////
		///////////////////////////////

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
			GetDlgItem(IDC_CONNECT_BTN)->SetWindowText(_T("포트 열기"));
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
			GetDlgItem(IDC_CONNECT_BTN)->SetWindowText(_T("포트 닫기"));
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
	//str.Format(_T("PRINTER TEST%c%c\x4"), 0x10, 0x04); // 0x04 통신 EOT문자
	str += "\r\n";
	m_Rs232_comm->Send(str, str.GetLength());
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



//==============================================================================
// 함 수 명 : GenOneLineDataByCstring			
// 기능설명 : CString 함수인 CBuff.m_PrtBuff를 활용해서 출력용 전역 Array(g_szPrintSendData)에 라인별로 분리설정한다.
// 매개변수 : pcsPrtData : 출력할 전체 문자열
//			  pinLineNo  : 행의 길이를 저장
//==============================================================================
//void CMycomm::GenOneLineDataByCstring(CString pcsPrtData, int* pinLineNo)
//{
//    int inSpoint = 0, inEpoint = 0, inTotLen = 0, inStrLen = 0;
//    int inLineNo = 0;
//
//    inLineNo = *pinLineNo;
//
//    inEpoint = pcsPrtData.Find("\r\n", inSpoint);
//    inTotLen = pcsPrtData.GetLength();
//
//    // kangdh78 읽기 오버런, MAX_PRTBUFF_SIZE
//    while (inEpoint != -1 && inLineNo < MAX_PRTBUFF_SIZE)
//    {
//        inStrLen = inEpoint - inSpoint;
//
//        if (g_inEnvFlag[23] == 7)
//        {
//            if (inStrLen > LEN_PRINT_ROW_48 + 2)
//            {
//                inStrLen = LEN_PRINT_ROW_48 + 2;
//                inEpoint = inSpoint + LEN_PRINT_ROW_48;
//            }
//            else
//            {
//                inEpoint = inEpoint + 2;  // 찾아낸 "\r\n"의 다음 위치를 시작 Point로 한다.
//            }
//        }
//        else
//        {
//            if (inStrLen > LEN_PRINT_ROW_40 + 2)
//            {
//                inStrLen = LEN_PRINT_ROW_40 + 2;
//                inEpoint = inSpoint + LEN_PRINT_ROW_40;
//            }
//            else
//            {
//                inEpoint = inEpoint + 2;  // 찾아낸 "\r\n"의 다음 위치를 시작 Point로 한다.
//            }
//        }
//
//        // 출력버퍼에 대한 범위 체크
//        if (inLineNo >= MAX_PRTBUFF_SIZE)
//        {
//            sprintf_s(g_szPrintSendData[inLineNo++], "** 제한된 출력범위를 초과하였습니다. **");
//            break;
//        }
//
//        sprintf_s(g_szPrintSendData[inLineNo++], "%s%c", (LPCTSTR)pcsPrtData.Mid(inSpoint, inStrLen), LIN);
//        inSpoint = inEpoint;
//        inEpoint = pcsPrtData.Find("\r\n", inSpoint);
//
//        // "\r\n"을 찾은 위치가 영수증 문자열을 초과한 경우 마지막 위치를 영수증 문자열 길이로 한다.
//        if (inEpoint > inTotLen)
//            inEpoint = inTotLen;
//    }
//
//    *pinLineNo = inLineNo;
//}


//==============================================================================
// 함 수 명 : RealSendDataPrinter_STP			
// 기능설명 : 실제 프린트하기위한 포맷으로 변경 처리하여 프린트 한다. CRC 산정없음.
// 매개변수 : 없음.
//==============================================================================
//void CSubFunc::RealSendDataPrinter_STP(int QR_Print)
//{
//    char szSendData[1024] = { 0, };
//    int  inSendCnt, inSendLeng = 0;
//    CString csSendData;
//    CString csQRData = "";
//    char szBarcodeData[1024] = { 0, };
//
//    TRACE("result: %d\n", csSendData.Find("<Q", 0));
//
//    for (inSendCnt = 0; inSendCnt < m_inMaxSendCnt; inSendCnt++)
//    {
//        memset(szSendData, NULL, sizeof(szSendData));
//        strcat_s(szSendData, g_szPrintSendData[inSendCnt]);
//        csSendData.Format("%s", g_szPrintSendData[inSendCnt]);
//
//        if (csSendData.Find("<?", 0) > -1)		// 이미지 출력
//        {
//            // KKM-2014-1219-MOD : GLOBAL_TAX_FREE _ 도시바
//            if (g_inEnvFlag[23] == 8)
//                PrintSignSerialAndParallelToshiba(csSendData);
//            else
//                PrintSignSerialAndParallel(csSendData);
//
//            continue;
//        }
//        else if (csSendData.Find("<@", 0) > -1)	// 바코드 출력
//        {
//            // KSH-2017-0831-ADD : 로고 출력은 <! 로 변경
//            CString csBarcode = csSendData.Mid(csSendData.Find("<@", 0) + 2, csSendData.Find("@>", 0) - csSendData.Find("<@", 0) - 2);
//
//            if (g_inEnvFlag[87] == 5) // NSR-2017-0912-ADD : 글로벌블루 바코드 출력 
//            {
//                PrintBarcodeSerialAndParallel(csBarcode.GetBuffer(0));
//            }
//            else if (g_inEnvFlag[87] == 6)
//            {
//                PrintBarCode_STP(csBarcode.GetBuffer(0), 0);
//            }
//            else
//            {
//                PrintBarCode_STP(csBarcode.GetBuffer(0), 1);
//            }
//
//            // LSK-2017-0512-MOD : CubeRefund 영수증 이미지 출력을 위해 수정
//// 			if(g_inEnvFlag[87] == 1){
//// 				CString csBarcode = csSendData.Mid(csSendData.Find("<@", 0) + 2, csSendData.Find("@>", 0) -csSendData.Find("<@", 0) - 2);
//// 				if(strcmp(csBarcode,"CubeLogo") == 0)
//// 				{
//// 					TaxFreeLogoprint(CUBE_PRINTLOG_FILE_NM);
//// 					g_clsRs232C_Scanner.WriteCommBlock( "\r\n\r\n", 8 );
//// 					//ReplaceCharToNull_STP( szSendData, STP_131_REP_CHAR );
//// 				}
//// 				else if(strcmp(csBarcode,"PreCube") == 0)
//// 				{
//// 					TaxFreeLogoprint(CUBE_PRE_PRINTLOG_FILE_NM);
//// 					g_clsRs232C_Scanner.WriteCommBlock( "\r\n\r\n", 8 );
//// 					//ReplaceCharToNull_STP( szSendData, STP_131_REP_CHAR );
//// 				}
//// 				else
//// 				{					
//// 					PrintBarCode_STP(csBarcode.GetBuffer(0), 1);
//// 				}
//// 			}
//// 			else
//// 			{
////				CString csBarcode = csSendData.Mid(csSendData.Find("<@", 0) + 2, csSendData.Find("@>", 0) -csSendData.Find("<@", 0) - 2);
////				PrintBarCode_STP(csBarcode.GetBuffer(0), 1);	
////			}
//            continue;
//            /* //기존 코드
//            CString csBarcode = csSendData.Mid(csSendData.Find("<@", 0) + 2, csSendData.Find("@>", 0) -csSendData.Find("<@", 0) - 2);
//            PrintBarCode_STP(csBarcode.GetBuffer(0), 1);
//            continue;
//            */
//        }
//        else if (csSendData.Find("<!", 0) > -1) // 로고출력
//        {
//            CString csLogoName = csSendData.Mid(csSendData.Find("<!", 0) + 2, csSendData.Find("!>", 0) - csSendData.Find("<!", 0) - 2);
//            if (g_inEnvFlag[87] == 1) {
//                if (strcmp(csLogoName, "CubeLogo") == 0)
//                {
//                    TaxFreeLogoprint(CUBE_PRINTLOG_FILE_NM);
//                    g_clsRs232C_Scanner.WriteCommBlock("\r\n\r\n", 8);
//                    //ReplaceCharToNull_STP( szSendData, STP_131_REP_CHAR );
//                }
//                else if (strcmp(csLogoName, "PreCube") == 0)
//                {
//                    TaxFreeLogoprint(CUBE_PRE_PRINTLOG_FILE_NM);
//                    g_clsRs232C_Scanner.WriteCommBlock("\r\n\r\n", 8);
//                    //ReplaceCharToNull_STP( szSendData, STP_131_REP_CHAR );
//                }
//            }
//            else if (g_inEnvFlag[87] == 6)
//            {
//                CString csLogoPath = "";
//                csLogoPath.Format("%s%s", FILE_DATABASE_EXE_DIR, csLogoName);
//                TaxFreeLogoprint(csLogoPath.GetBuffer(), 0);
//                g_clsRs232C_Scanner.WriteCommBlock("\r\n\r\n", 4);
//            }
//            else
//            {
//                CString csLogoPath = "";
//                csLogoPath.Format("%s%s", FILE_DATABASE_EXE_DIR, csLogoName);
//                TaxFreeLogoprint(csLogoPath.GetBuffer(), 1);
//                g_clsRs232C_Scanner.WriteCommBlock("\r\n\r\n", 4);
//            }
//            continue;
//        }
//        else if (csSendData.Find("<Q", 0) > -1)
//        {
//            if (csSendData.Find("Q>", 0) > -1)
//            {
//                CString csQrName = csSendData.Mid(csSendData.Find("<Q", 0) + 2, csSendData.Find("Q>", 0) - csSendData.Find("<Q", 0) - 2);
//                PrintQrCode(csQrName);
//                continue;
//            }
//            else
//            {
//                csQRData = csSendData;
//                csQRData.Replace("\n", "");
//                // csQRData.Delete(csQRData.GetLength() - 2, 2);
//                continue;
//            }
//        }
//        else if (csSendData.Find("Q>", 0) > -1)
//        {
//            if (csQRData.Compare("") != 0)
//            {
//                csQRData += csSendData;
//                CString csQrName = csQRData.Mid(csQRData.Find("<Q", 0) + 2, csQRData.Find("Q>", 0) - csQRData.Find("<Q", 0) - 2);
//                PrintQrCode(csQrName);
//                continue;
//            }
//            else
//            {
//                csQRData = "";
//                continue;
//            }
//        }
//        else if (csSendData.Find("<$", 0) > -1) // 텍스트 가운데 출력 (무시)
//        {
//            csSendData.Replace("<$", "");
//            csSendData.Replace("$>", "");
//            g_clsRs232C_Scanner.WriteCommBlock(csSendData, csSendData.GetLength());
//            continue;
//        }
//        else if (csSendData.Find("<PaperCut>", 0) > -1) // 커팅
//        {
//            PaperCut_STP(4);
//            continue;
//        }
//        // 		else
//        // 			continue;
//        inSendLeng = strlen(szSendData);
//
//        ReplaceCharToNull_STP(szSendData, STP_131_REP_CHAR);
//        g_clsRs232C_Scanner.WriteCommBlock(szSendData, inSendLeng);
//
//
//
//        //  2011-1117-NYO-ADD :  QR코드 출력 // g_cSaleFg - 정상 매출 //g_snRebillFlag - 재발행 
//
//    }
//
//    if (g_snWePassFlag && g_cSaleFg == S_NORM && g_snRebillFlag == FALSE && QR_Print == 1)
//    {
//        g_snWePassFlag = FALSE;
//
//        PrinterWePassQRCode_STP();
//    }
//
//    if (g_snEggmonQRFlag == TRUE && g_cSaleFg == S_NORM && g_snRebillFlag == FALSE && QR_Print == 1)
//    {
//        g_snEggmonQRFlag = FALSE;
//
//        LineFeed_STP(1);
//        PrintBitmapSerialAndParallel(QRCODE_FILE_NM);
//        LineFeed_STP(1);
//
//    }
//    // KKM-2014-1215-MOD : GLOBAL_TAX_FREE _ 추가
//    // KJY-2015-0122-ADD : 주방 주문서에는 바코드 출력 안함.
//    // if(g_szPrintBarcode[0] != NULL && g_inEnvFlag[87] == 1){
//    // KSH-2018-0907-ADD : 릴샵도 출력
//    // KMS-2020-0701-ADD : 바코드 출력 여부 ENV
//    if ((g_inEnvFlag[94] == 5) || (g_inEnvFlag[105] == 1 && g_snOrderPrintFlag == FALSE)
//        || (g_szPrintBarcode[0] != NULL && g_inEnvFlag[87] > 0 && g_snOrderPrintFlag == FALSE)
//        )
//    {
//        BarcodeDataPrinter();
//    }
//    // KSG-2014-1217-ADD : 택스리펀드에서만 출력 택스프리에선 출력안함
//    if (g_szTaxSerialNo[0] != NULL && g_inEnvFlag[87] == 1) {
//        TaxRefundBarcodeDataPrinter();
//    }
//
//    // KKM-2014-1218-MOD : GLOBAL_TAX_FREE _ Thosiba 8추가
//    if (g_inEnvFlag[23] == 8)
//        PaperCut_STP(6);
//    else
//        PaperCut_STP(4);
//}


//short CSubFunc::PrintQrCode(CString pcsQrCode) // JMC-2021-0806-ADD : QRCODE 출력 추가
//{
//
//    char	szSendBuff[1024];
//    CString			LF = _T("\x0A");
//    CString			GS = _T("\x1d");
//
//
//    //가운데 출력
//    memset(szSendBuff, 0x00, sizeof(szSendBuff));
//    szSendBuff[0] = 0x1B;
//    szSendBuff[1] = 0x61;
//    szSendBuff[2] = 0x01;
//    g_clsRs232C_Scanner.WriteCommBlock(szSendBuff, 3);
//
//
//    //QR Code: Select the model
//    memset(szSendBuff, 0x00, sizeof(szSendBuff));
//    szSendBuff[0] = 0x1D;
//    szSendBuff[1] = 0x28;
//    szSendBuff[2] = 0x6B;
//    szSendBuff[3] = 0x04;
//    szSendBuff[4] = 0x00;
//    szSendBuff[5] = 0x31;
//    szSendBuff[6] = 0x41;
//    szSendBuff[7] = 0x32;			// 49 model1, 50 model2, 51 micro QR Code+
//    szSendBuff[8] = 0x00;
//
//    g_clsRs232C_Scanner.WriteCommBlock(szSendBuff, 9);
//
//    //QR Code: Set the size of module
//
//    int inUrlLen = strlen(g_stEventInfo.szEventMsg6);
//    //inUrlLen =strlen(g_stEventInfo.szEventMsg6);
//    memset(szSendBuff, 0x00, sizeof(szSendBuff));
//    szSendBuff[0] = 0x1D;
//    szSendBuff[1] = 0x28;
//    szSendBuff[2] = 0x6B;
//    szSendBuff[3] = 0x03;
//    szSendBuff[4] = 0x00;
//    szSendBuff[5] = 0x31;
//    szSendBuff[6] = 0x43;
//
//    if (inUrlLen > 0)
//    {
//        szSendBuff[7] = 0x06;		// Sets the size of the module for QR Code to n dots.
//    }
//    else
//    {
//        szSendBuff[7] = 0x07;		// Sets the size of the module for QR Code to n dots.
//    }
//
//    g_clsRs232C_Scanner.WriteCommBlock(szSendBuff, 8);
//
//    //QR Code: Select the error correction level
//    memset(szSendBuff, 0x00, sizeof(szSendBuff));
//    szSendBuff[0] = 0x1D;
//    szSendBuff[1] = 0x28;
//    szSendBuff[2] = 0x6B;
//    szSendBuff[3] = 0x03;
//    szSendBuff[4] = 0x00;
//    szSendBuff[5] = 0x31;
//    szSendBuff[6] = 0x45;
//    szSendBuff[7] = 0x30;		// 48 Selects Error correction level L , Recover 7% - default
//    // 49 Selects Error correction level M , Recover 15%
//    // 50 Selects Error correction level Q , Recover 25%
//    // 51 Selects Error correction level H , Recover 30%
//    g_clsRs232C_Scanner.WriteCommBlock(szSendBuff, 8);
//
//    //QR Code: Store the data in the symbol storage area
//    memset(szSendBuff, 0x00, sizeof(szSendBuff));
//    int inLen = pcsQrCode.GetLength() + 3;
//    szSendBuff[0] = 0x1D;
//    szSendBuff[1] = 0x28;
//    szSendBuff[2] = 0x6B;
//    szSendBuff[3] = inLen % 256;
//    szSendBuff[4] = inLen / 256;
//    szSendBuff[5] = 0x31;
//    szSendBuff[6] = 0x50;
//    szSendBuff[7] = 0x30;
//
//    strcat_s(szSendBuff + 8, sizeof(szSendBuff) - 8, pcsQrCode);
//
//    g_clsRs232C_Scanner.WriteCommBlock(szSendBuff, 8 + pcsQrCode.GetLength()); // QR에 데이터 담기
//
//
//    //QR Code: Print the symbol data in the symbol storage area
//    memset(szSendBuff, 0x00, sizeof(szSendBuff));
//    szSendBuff[0] = 0x1D;
//    szSendBuff[1] = 0x28;
//    szSendBuff[2] = 0x6B;
//    szSendBuff[3] = 0x03;
//    szSendBuff[4] = 0x00;
//    szSendBuff[5] = 0x31;
//    szSendBuff[6] = 0x51;
//    szSendBuff[7] = 0x30;
//
//    g_clsRs232C_Scanner.WriteCommBlock(szSendBuff, 8); // QR코드 인쇄
//
//    // 	
//    // 	//QR Code: Transmit the size information of the symbol data in the symbol storage area
//    // 	memset(szSendBuff,0x00,sizeof(szSendBuff));
//    // 	szSendBuff[0] = 0x1D;
//    // 	szSendBuff[1] = 0x28;
//    // 	szSendBuff[2] = 0x6B;
//    // 	szSendBuff[3] = 0x03;
//    // 	szSendBuff[4] = 0x00;
//    // 	szSendBuff[5] = 0x31;
//    // 	szSendBuff[6] = 0x52;
//    // 	szSendBuff[7] = 0x30;
//    // 	g_clsRs232C_Scanner.WriteCommBlock(szSendBuff, 8);
//    // 	
//
//    // 글씨출력 설정 초기화(왼쪽정렬)
//    memset(szSendBuff, 0x00, sizeof(szSendBuff));
//    szSendBuff[0] = 0x1B;
//    szSendBuff[1] = 0x61;
//    szSendBuff[2] = 0x00;
//    g_clsRs232C_Scanner.WriteCommBlock(szSendBuff, 3);
//
//
//    return 1;
//}

//===============================================================================
// 함 수 명 : WriteCommBlock
// 기능설명 : 컴포트에 값을 Write한다.
// 매개변수 : pszByte         : pointer to data to write to file
//            pdwBytesToWrite : number of bytes to write
//===============================================================================
//BOOL CRs232C::WriteCommBlock(LPCSTR pszByte, DWORD pdwBytesToWrite)
//{
//    COMSTAT    stComStat;
//    BOOL       blfWriteStat;
//    DWORD      dwBytesWritten;
//    DWORD      dwError;
//    DWORD      dwErrorFlags = 0;
//    char       szError[10] = { 0, };
//
//    blfWriteStat = WriteFile(m_hComDev, pszByte, pdwBytesToWrite, &dwBytesWritten, &m_stOsWrite);
//
//    if (!blfWriteStat)
//    {
//        // 컴포트에 데이터를 제대로 써넣지 못했을 경우이다.
//        dwError = GetLastError();
//
//        if (GetLastError() == ERROR_IO_PENDING)
//        {
//            while (!GetOverlappedResult(m_hComDev, &m_stOsWrite, &dwBytesWritten, TRUE))
//            {
//                dwError = GetLastError();
//                if (dwError == ERROR_IO_INCOMPLETE)
//                    // normal result if not finished
//                    continue;
//                else
//                {
//                    // an error occurred, try to recover
//                    wsprintf(szError, "<CE-%u>", dwError);
//                    ClearCommError(m_hComDev, &dwErrorFlags, &stComStat);
//                    if (dwErrorFlags > 0)
//                    {
//                        wsprintf(szError, "<CE-%u>", dwErrorFlags);
//                    }
//                    break;
//                }
//            }
//        }
//        else
//        {
//            // some other error occurred
//            ClearCommError(m_hComDev, &dwErrorFlags, &stComStat);
//
//            if (dwErrorFlags > 0)
//            {
//                wsprintf(szError, "<CE-%u>", dwErrorFlags);
//            }
//            return (FALSE);
//        }
//
//    }
//    return (TRUE);
//}