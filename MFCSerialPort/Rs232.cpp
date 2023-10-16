#include "pch.h"
#include "Rs232.h"

Rs232::~Rs232()
{
    if (m_bIsOpenned)
        Close();
    delete m_pEvent;
}

Rs232::Rs232(CString port, CString baudrate, CString parity, CString databit, CString stopbit)
{
    m_sComPort = port;
    m_sBaudRate = baudrate;
    m_sParity = parity;
    m_sDataBit = databit;
    m_sStopBit = stopbit;
    m_bFlowChk = 1;
    m_bIsOpenned = FALSE;
    m_nLength = 0;
    memset(m_sInBuf, 0, sizeof(m_sInBuf));
    m_pEvent = new CEvent(FALSE, TRUE);
}

void Rs232::ResetSerial()

{
    DCB     dcb;
    DWORD   DErr;
    COMMTIMEOUTS CommTimeOuts;

    if (!m_bIsOpenned)
        return;

    // �����Ʈ�� ��� ������ ����
    ClearCommError(m_hComDev, &DErr, NULL);
    // �����Ʈ��Input/Output Buffer ����� ����
    SetupComm(m_hComDev, InBufSize, OutBufSize);
    // ��� Rx/Tx ������ �����ϰ� ���� Buffer�� ������ ����
    PurgeComm(m_hComDev, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

    // set up for overlapped I/O (MSDN ����)
    // ��� ���λ󿡼� �ѹ���Ʈ�� ���۵ǰ� ���� ����Ʈ�� ���۵Ǳ������ �ð�
    CommTimeOuts.ReadIntervalTimeout = MAXDWORD;

    // Read doperation ���� TimeOut�� ������� ����
    CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
    CommTimeOuts.ReadTotalTimeoutConstant = 0;

    // CBR_9600 is approximately 1byte/ms. For our purposes, allow
    // double the expected time per character for a fudge factor.
    CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
    CommTimeOuts.WriteTotalTimeoutConstant = 1000;

    // �����Ʈ��TimeOut������
    SetCommTimeouts(m_hComDev, &CommTimeOuts);


    memset(&dcb, 0, sizeof(DCB));
    dcb.DCBlength = sizeof(DCB);
    // �����Ʈ�� DCB�� ����
    GetCommState(m_hComDev, &dcb);
    // DCB�� ����(DCB: �ø�������� ���� �Ķ����, MSDN ����)
    dcb.fBinary = TRUE;
    dcb.fParity = TRUE;

    if (m_sBaudRate == "300")
        dcb.BaudRate = CBR_300;
    else if (m_sBaudRate == "600")
        dcb.BaudRate = CBR_600;
    else if (m_sBaudRate == "1200")
        dcb.BaudRate = CBR_1200;
    else if (m_sBaudRate == "2400")
        dcb.BaudRate = CBR_2400;
    else if (m_sBaudRate == "4800")
        dcb.BaudRate = CBR_4800;
    else if (m_sBaudRate == "9600")
        dcb.BaudRate = CBR_9600;
    else if (m_sBaudRate == "14400")
        dcb.BaudRate = CBR_14400;
    else if (m_sBaudRate == "19200")
        dcb.BaudRate = CBR_19200;
    else if (m_sBaudRate == "28800")
        dcb.BaudRate = CBR_38400;
    else if (m_sBaudRate == "33600")
        dcb.BaudRate = CBR_38400;
    else if (m_sBaudRate == "38400")
        dcb.BaudRate = CBR_38400;
    else if (m_sBaudRate == "56000")
        dcb.BaudRate = CBR_56000;
    else if (m_sBaudRate == "57600")
        dcb.BaudRate = CBR_57600;
    else if (m_sBaudRate == "115200")
        dcb.BaudRate = CBR_115200;
    else if (m_sBaudRate == "128000")
        dcb.BaudRate = CBR_128000;
    else if (m_sBaudRate == "256000")
        dcb.BaudRate = CBR_256000;
    else if (m_sBaudRate == "PCI_9600")
        dcb.BaudRate = 1075;
    else if (m_sBaudRate == "PCI_19200")
        dcb.BaudRate = 2212;
    else if (m_sBaudRate == "PCI_38400")
        dcb.BaudRate = 4300;
    else if (m_sBaudRate == "PCI_57600")
        dcb.BaudRate = 6450;
    else if (m_sBaudRate == "PCI_500K")
        dcb.BaudRate = 56000;


    if (m_sParity == "None")
        dcb.Parity = NOPARITY;
    else if (m_sParity == "Even")
        dcb.Parity = EVENPARITY;
    else if (m_sParity == "Odd")
        dcb.Parity = ODDPARITY;

    if (m_sDataBit == "7 Bit")
        dcb.ByteSize = 7;
    else if (m_sDataBit == "8 Bit")
        dcb.ByteSize = 8;

    if (m_sStopBit == "1 Bit")
        dcb.StopBits = ONESTOPBIT;
    else if (m_sStopBit == "1.5 Bit")
        dcb.StopBits = ONE5STOPBITS;
    else if (m_sStopBit == "2 Bit")
        dcb.StopBits = TWOSTOPBITS;

    dcb.fRtsControl = RTS_CONTROL_ENABLE;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fOutxDsrFlow = FALSE;

    if (m_bFlowChk) {
        dcb.fOutX = FALSE;
        dcb.fInX = FALSE;
        dcb.XonLim = 2048;
        dcb.XoffLim = 1024;
    }
    else {
        dcb.fOutxCtsFlow = TRUE;
        dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
    }

    // ������ DCB�� �����Ʈ�� ���� �Ķ���͸� ����
    SetCommState(m_hComDev, &dcb);
    // Input Buffer�� �����Ͱ� ������ �� �̺�Ʈ�� �߻��ϵ��� ����
    SetCommMask(m_hComDev, EV_RXCHAR);
}

void Rs232::Close()
{
    if (!m_bIsOpenned)
        return;

    m_bIsOpenned = FALSE;
    SetCommMask(m_hComDev, 0);
    EscapeCommFunction(m_hComDev, CLRDTR);
    PurgeComm(m_hComDev, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
    Sleep(500);

}

UINT CommThread(LPVOID lpData)
{
    extern short          g_nRemoteStatus;
    DWORD                ErrorFlags;
    COMSTAT            ComStat;
    DWORD                EvtMask;
    char                    buf[MAXBUF];
    DWORD                Length;
    int                       size;
    int                       insize = 0;

    // ���Ŭ������ ��ü�����͸� ����
    Rs232* Rs232_Com = (Rs232*)lpData;

    // �����Ʈ�� ���� �ִٸ�
    while (Rs232_Com->m_bIsOpenned) {
        EvtMask = 0;
        Length = 0;
        insize = 0;
        memset(buf, '\0', MAXBUF);
        // �̺�Ʈ�� ��ٸ�
        WaitCommEvent(Rs232_Com->m_hComDev, &EvtMask, NULL);
        ClearCommError(Rs232_Com->m_hComDev, &ErrorFlags, &ComStat);
        // EV_RXCHAR���� �̺�Ʈ�� �߻��ϸ�
        if ((EvtMask & EV_RXCHAR) && ComStat.cbInQue) {
            if (ComStat.cbInQue > MAXBUF)
                size = MAXBUF;
            else
                size = ComStat.cbInQue;
            do {
                ClearCommError(Rs232_Com->m_hComDev, &ErrorFlags, &ComStat);
                // overlapped I/O�� ���� �����͸� ����
                if (!ReadFile(Rs232_Com->m_hComDev, buf + insize, size, &Length, &(Rs232_Com->m_OLR))) {
                    // ����
                    TRACE("Error in ReadFile\n");
                    if (GetLastError() == ERROR_IO_PENDING) {
                        if (WaitForSingleObject(Rs232_Com->m_OLR.hEvent, 1000) != WAIT_OBJECT_0)
                            Length = 0;
                        else
                            GetOverlappedResult(Rs232_Com->m_hComDev, &(Rs232_Com->m_OLR), &Length, FALSE);
                    }
                    else
                        Length = 0;
                }
                insize += Length;
            } while ((Length != 0) && (insize < size));
            ClearCommError(Rs232_Com->m_hComDev, &ErrorFlags, &ComStat);

            if (Rs232_Com->m_nLength + insize > MAXBUF * 2)
                insize = (Rs232_Com->m_nLength + insize) - MAXBUF * 2;

            // �̺�Ʈ �߻��� ��� �ߴ��ϰ� input buffer�� �����͸� ����
            Rs232_Com->m_pEvent->ResetEvent();
            memcpy(Rs232_Com->m_sInBuf + Rs232_Com->m_nLength, buf, insize);
            Rs232_Com->m_nLength += insize;
            // ���簡 ������ �ٽ� �̺�Ʈ�� Ȱ��ȭ ��Ű��
            Rs232_Com->m_pEvent->SetEvent();
            LPARAM temp = (LPARAM)Rs232_Com;
            // �����Ͱ� ���Դٴ� �޽����� �߻�
            SendMessage(Rs232_Com->m_hWnd, WM_MYRECEIVE, Rs232_Com->m_nLength, temp);
        }
    }
    PurgeComm(Rs232_Com->m_hComDev, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
    LPARAM temp = (LPARAM)Rs232_Com;
    // �����尡 ����� �� ���� �޽����� ����
    SendMessage(Rs232_Com->m_hWnd, WM_MYCLOSE, 0, temp);
    return 0;
}

BOOL Rs232::Create(HWND hWnd)

{
    m_hWnd = hWnd;       // �޽����� ������ ���
    m_hComDev = CreateFile(m_sComPort, GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        NULL);                    // �ø��� ��Ʈ ����

    if (m_hComDev != INVALID_HANDLE_VALUE)    // ��Ʈ�� ���������� ������ 
        m_bIsOpenned = TRUE;                 // ����
    else                                  // �ƴϸ�
        return FALSE;                 // �����ϰ� ��������

    ResetSerial();            // �ø��� ��Ʈ�� ��������� �ʱ�ȭ
    m_OLW.Offset = 0;               // Write OVERLAPPED structure �ʱ�ȭ
    m_OLW.OffsetHigh = 0;
    m_OLR.Offset = 0;                // Read OVERLAPPED structure �ʱ�ȭ
    m_OLR.OffsetHigh = 0;

    // Overlapped ����ü�� �̺�Ʈ�� ����
    m_OLR.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (m_OLR.hEvent == NULL) {
        CloseHandle(m_OLR.hEvent);
        return FALSE;
    }
    m_OLW.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (m_OLW.hEvent == NULL) {
        CloseHandle(m_OLW.hEvent);
        return FALSE;
    }
    // �ø��� �����͸� �ޱ����� ������ ����
    AfxBeginThread(CommThread, (LPVOID)this);
    // DTR (Data Terminal Ready) signal �� ����
    EscapeCommFunction(m_hComDev, SETDTR);   // MSDN ����
    return TRUE;
}


BOOL Rs232::Send(LPCTSTR outbuf, int len)
{
    BOOL                  bRet = TRUE;
    DWORD                ErrorFlags;
    COMSTAT            ComStat;

    DWORD                BytesWritten;
    DWORD                BytesSent = 0;

#ifdef _UNICODE
    // �����ڵ�� �����ϵ� ������Ʈ
    USES_CONVERSION;
    const char* outbuf_char = T2A(outbuf);
#else
    // ��Ƽ����Ʈ ���� �������� �����ϵ� ������Ʈ
    // �ٸ� ���� ����
#endif



#ifdef _UNICODE
// �����ڵ�� �����ϵ� ������Ʈ
     // ��� ��Ʈ�� ��� ������ ����
    ClearCommError(m_hComDev, &ErrorFlags, &ComStat);

    // overlapped I/O�� ���Ͽ� outbuf�� ������ len���� ��ŭ ����
    if (!WriteFile(m_hComDev, outbuf_char, len, &BytesWritten, &m_OLW)) {
        if (GetLastError() == ERROR_IO_PENDING) {
            if (WaitForSingleObject(m_OLW.hEvent, 1000) != WAIT_OBJECT_0)
                bRet = FALSE;
            else
                GetOverlappedResult(m_hComDev, &m_OLW, &BytesWritten, FALSE);
        }
        else /* I/O error */
            bRet = FALSE; /* ignore error */
    }
    // �ٽ� �ѹ� �����Ʈ�� ��� ������ ����
    ClearCommError(m_hComDev, &ErrorFlags, &ComStat);
#else
    // ��Ƽ����Ʈ ���� �������� �����ϵ� ������Ʈ
     // ��� ��Ʈ�� ��� ������ ����
    ClearCommError(m_hComDev, &ErrorFlags, &ComStat);

    // overlapped I/O�� ���Ͽ� outbuf�� ������ len���� ��ŭ ����
    if (!WriteFile(m_hComDev, outbuf, len, &BytesWritten, &m_OLW)) {
        if (GetLastError() == ERROR_IO_PENDING) {
            if (WaitForSingleObject(m_OLW.hEvent, 1000) != WAIT_OBJECT_0)
                bRet = FALSE;
            else
                GetOverlappedResult(m_hComDev, &m_OLW, &BytesWritten, FALSE);
        }
        else /* I/O error */
            bRet = FALSE; /* ignore error */
    }
    // �ٽ� �ѹ� �����Ʈ�� ��� ������ ����
    ClearCommError(m_hComDev, &ErrorFlags, &ComStat);
#endif

    return bRet;

}

int Rs232::Receive(LPSTR inbuf, int len)
{
    CSingleLock lockObj((CSyncObject*)m_pEvent, FALSE);
    // argument value is not valid
    if (len == 0)
        return -1;
    else if (len > MAXBUF)
        return -1;

    if (m_nLength == 0) {
        inbuf[0] = '\0';
        return 0;
    }
    // �������̶�� ����ƾ���� ���� ���� ���� �������� ���� ��ŭ Input Buffer���� �����͸� ����
    else if (m_nLength <= len) {
        lockObj.Lock();
        memcpy(inbuf, m_sInBuf, m_nLength);
        memset(m_sInBuf, 0, MAXBUF * 2);
        int tmp = m_nLength;
        m_nLength = 0;
        lockObj.Unlock();
        return tmp;
    }
    else {
        lockObj.Lock();
        memcpy(inbuf, m_sInBuf, len);
        memmove(m_sInBuf, m_sInBuf + len, MAXBUF * 2 - len);
        m_nLength -= len;
        lockObj.Unlock();
        return len;
    }
}

void Rs232::Clear()
{
    PurgeComm(m_hComDev, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
    memset(m_sInBuf, 0, MAXBUF * 2);
    m_nLength = 0;
}

void Rs232::HandleClose()
{
    CloseHandle(m_hComDev);
    CloseHandle(m_OLR.hEvent);
    CloseHandle(m_OLW.hEvent);
}

///////////////////////////////////////////////////////////////////////////////
// �� �� �� : OnTest
// ��ɼ��� : �׽�Ʈ��ư
// �Ű����� : -
// �� �� �� : SDW-2006-0726-NEW
///////////////////////////////////////////////////////////////////////////////
//void CMycomm::OnTest()
//{
//    char szTemp[50 + 1],
//        szSendData[50 + 1],
//        szRecvData[50 + 1];
//    CRs232C clsSerialPrinter;
//    int		inPrintPort;
//
//    memset(szTemp, 0x00, sizeof(szTemp));
//    memset(szRecvData, 0x00, sizeof(szRecvData));
//    memset(szSendData, 0x00, sizeof(szSendData));
//
//    if (m_inSelectRowNo == -1)
//    {
//        AfxMessageBox("���� �׽�Ʈ�� ���ϴ� �����͸� ����Ʈ���� �����ϼ���");
//        return;
//    }
//
//    // KHS-2010-1101-ADD : ��Ʈ��ũ ������ �׽�Ʈ�� �Ѵ�.
//    if (m_stKitchenSetUp[m_inSelectRowNo].szPrtKind[0] == 'N')
//    {
//        NetworkPrtTest();
//        return;
//    }
//
//    inPrintPort = atoi(m_stKitchenSetUp[m_inSelectRowNo].szPortNo);
//
//    m_btnTest.EnableWindow(FALSE);
//    clsSerialPrinter.ComPortOpen(inPrintPort, 0);
//
//    if (m_stKitchenSetUp[m_inSelectRowNo].szPrtKind[0] == 'I') //	2005-1114-KHG(ADD)
//        sprintf(szSendData, "\x1B\x64\x08");
//    else
//    {
//        for (int i = 0; i < 4; i++)
//            sprintf(szSendData, "%s\r\n", szSendData);
//    }
//
//    clsSerialPrinter.WriteCommBlock(szSendData, strlen(szSendData));
//
//
//    if (m_stKitchenSetUp[m_inSelectRowNo].szPrtKind[0] == 'I')
//        sprintf(szSendData, "*************PRINTER TEST*************%c%c", 0x1B, 0x76);
//    else
//        sprintf(szSendData, "*************PRINTER TEST*************%c%c\x4", DLE, EOT);
//
//    for (int i = 0; i < 4; i++)
//        sprintf(szSendData, "%s\r\n", szSendData);
//
//    ZeroMemory(szRecvData, sizeof(szRecvData));
//    Sleep(200);
//
//    clsSerialPrinter.SendRecvDataBlock(szSendData, szRecvData, 500);
//
//
//    switch (szRecvData[0])
//    {
//    case 0x12:						// �����Ͱ� ������ ��� 
//    case 0x14:						// �����Ͱ� ������ ��� 
//        ::MessageBox(NULL, _T("������ ���� ��� ����."), _T("����"), MB_OK);
//        break;
//
//    case 0x00:
//        if (m_stKitchenSetUp[m_inSelectRowNo].szPrtKind[0] == 'T')
//        {
//            //������ ������ ���// Į�� ������ ���� ������ ��� (EPSON)
//            ::MessageBox(NULL, _T("������ �������� Ȯ�� �ٶ�."), _T("����"), MB_OK);
//        }
//        else {
//            // ������ �Ѳ��� ���� ���(E, W, I)  //������ ������ ���
//            ::MessageBox(NULL, _T("������ �������¸� Ȯ�� �ٶ�."), _T("����"), MB_OK);
//        }
//
//        break;
//
//    case 0x72:
//    case 0x7e:
//        ::MessageBox(NULL, _T("���� ���� Ȯ�� �ٶ�"), _T("����"), MB_OK);
//        break;
//
//    case 0x1C:
//        if (m_stKitchenSetUp[m_inSelectRowNo].szPrtKind[0] == 'T')
//        {
//            ::MessageBox(NULL, _T("Į�� ���� ���� �߻�"), _T("����"), MB_OK);
//            break;
//        }
//
//    case 0x48:
//
//        if (m_stKitchenSetUp[m_inSelectRowNo].szPrtKind[0] == 'I')
//        {
//            ::MessageBox(NULL, _T("������ Ŀ�� Ȥ�� ���� Ȯ��"), _T("����"), MB_OK);
//            break;
//        }
//
//    default:
//        ::MessageBox(NULL, _T("������ ��Ÿ ���� �߻�"), _T("����"), MB_OK);
//        break;
//    }
//
//    clsSerialPrinter.ClosePort();
//
//    Sleep(100);
//    m_btnTest.EnableWindow(TRUE);
//
//}
//
//BOOL CMycomm::SendRecvDataBlock(char* pszSendData, char* pszRecvData, int pnSleepTime)
//{
//    char szRecvData[MAXBUFFSIZE + 1];
//    int  nRecvLength = 0;
//
//    if (!WriteCommBlock(pszSendData, strlen(pszSendData))) return FALSE;
//
//    memset(szRecvData, NULL, sizeof(szRecvData));
//    memset(pszRecvData, NULL, sizeof(pszRecvData));
//
//    nRecvLength = GetReadCommBlock(szRecvData, pszSendData, pnSleepTime);
//    if (nRecvLength < 1) return FALSE;
//
//    memcpy(pszRecvData, szRecvData, nRecvLength);
//
//    return TRUE;
//}

//==============================================================================
// �� �� �� : GenOneLineDataByCstring			
// ��ɼ��� : CString �Լ��� CBuff.m_PrtBuff�� Ȱ���ؼ� ��¿� ���� Array(g_szPrintSendData)�� ���κ��� �и������Ѵ�.
// �Ű����� : pcsPrtData : ����� ��ü ���ڿ�
//			  pinLineNo  : ���� ���̸� ����
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
//    // kangdh78 �б� ������, MAX_PRTBUFF_SIZE
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
//                inEpoint = inEpoint + 2;  // ã�Ƴ� "\r\n"�� ���� ��ġ�� ���� Point�� �Ѵ�.
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
//                inEpoint = inEpoint + 2;  // ã�Ƴ� "\r\n"�� ���� ��ġ�� ���� Point�� �Ѵ�.
//            }
//        }
//
//        // ��¹��ۿ� ���� ���� üũ
//        if (inLineNo >= MAX_PRTBUFF_SIZE)
//        {
//            sprintf_s(g_szPrintSendData[inLineNo++], "** ���ѵ� ��¹����� �ʰ��Ͽ����ϴ�. **");
//            break;
//        }
//
//        sprintf_s(g_szPrintSendData[inLineNo++], "%s%c", (LPCTSTR)pcsPrtData.Mid(inSpoint, inStrLen), LIN);
//        inSpoint = inEpoint;
//        inEpoint = pcsPrtData.Find("\r\n", inSpoint);
//
//        // "\r\n"�� ã�� ��ġ�� ������ ���ڿ��� �ʰ��� ��� ������ ��ġ�� ������ ���ڿ� ���̷� �Ѵ�.
//        if (inEpoint > inTotLen)
//            inEpoint = inTotLen;
//    }
//
//    *pinLineNo = inLineNo;
//}


//==============================================================================
// �� �� �� : RealSendDataPrinter_STP			
// ��ɼ��� : ���� ����Ʈ�ϱ����� �������� ���� ó���Ͽ� ����Ʈ �Ѵ�. CRC ��������.
// �Ű����� : ����.
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
//        if (csSendData.Find("<?", 0) > -1)		// �̹��� ���
//        {
//            // KKM-2014-1219-MOD : GLOBAL_TAX_FREE _ ���ù�
//            if (g_inEnvFlag[23] == 8)
//                PrintSignSerialAndParallelToshiba(csSendData);
//            else
//                PrintSignSerialAndParallel(csSendData);
//
//            continue;
//        }
//        else if (csSendData.Find("<@", 0) > -1)	// ���ڵ� ���
//        {
//            // KSH-2017-0831-ADD : �ΰ� ����� <! �� ����
//            CString csBarcode = csSendData.Mid(csSendData.Find("<@", 0) + 2, csSendData.Find("@>", 0) - csSendData.Find("<@", 0) - 2);
//
//            if (g_inEnvFlag[87] == 5) // NSR-2017-0912-ADD : �۷ι���� ���ڵ� ��� 
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
//            // LSK-2017-0512-MOD : CubeRefund ������ �̹��� ����� ���� ����
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
//            /* //���� �ڵ�
//            CString csBarcode = csSendData.Mid(csSendData.Find("<@", 0) + 2, csSendData.Find("@>", 0) -csSendData.Find("<@", 0) - 2);
//            PrintBarCode_STP(csBarcode.GetBuffer(0), 1);
//            continue;
//            */
//        }
//        else if (csSendData.Find("<!", 0) > -1) // �ΰ����
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
//        else if (csSendData.Find("<$", 0) > -1) // �ؽ�Ʈ ��� ��� (����)
//        {
//            csSendData.Replace("<$", "");
//            csSendData.Replace("$>", "");
//            g_clsRs232C_Scanner.WriteCommBlock(csSendData, csSendData.GetLength());
//            continue;
//        }
//        else if (csSendData.Find("<PaperCut>", 0) > -1) // Ŀ��
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
//        //  2011-1117-NYO-ADD :  QR�ڵ� ��� // g_cSaleFg - ���� ���� //g_snRebillFlag - ����� 
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
//    // KKM-2014-1215-MOD : GLOBAL_TAX_FREE _ �߰�
//    // KJY-2015-0122-ADD : �ֹ� �ֹ������� ���ڵ� ��� ����.
//    // if(g_szPrintBarcode[0] != NULL && g_inEnvFlag[87] == 1){
//    // KSH-2018-0907-ADD : ������ ���
//    // KMS-2020-0701-ADD : ���ڵ� ��� ���� ENV
//    if ((g_inEnvFlag[94] == 5) || (g_inEnvFlag[105] == 1 && g_snOrderPrintFlag == FALSE)
//        || (g_szPrintBarcode[0] != NULL && g_inEnvFlag[87] > 0 && g_snOrderPrintFlag == FALSE)
//        )
//    {
//        BarcodeDataPrinter();
//    }
//    // KSG-2014-1217-ADD : �ý����ݵ忡���� ��� �ý��������� ��¾���
//    if (g_szTaxSerialNo[0] != NULL && g_inEnvFlag[87] == 1) {
//        TaxRefundBarcodeDataPrinter();
//    }
//
//    // KKM-2014-1218-MOD : GLOBAL_TAX_FREE _ Thosiba 8�߰�
//    if (g_inEnvFlag[23] == 8)
//        PaperCut_STP(6);
//    else
//        PaperCut_STP(4);
//}


//short CSubFunc::PrintQrCode(CString pcsQrCode) // JMC-2021-0806-ADD : QRCODE ��� �߰�
//{
//
//    char	szSendBuff[1024];
//    CString			LF = _T("\x0A");
//    CString			GS = _T("\x1d");
//
//
//    //��� ���
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
//    g_clsRs232C_Scanner.WriteCommBlock(szSendBuff, 8 + pcsQrCode.GetLength()); // QR�� ������ ���
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
//    g_clsRs232C_Scanner.WriteCommBlock(szSendBuff, 8); // QR�ڵ� �μ�
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
//    // �۾���� ���� �ʱ�ȭ(��������)
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
// �� �� �� : WriteCommBlock
// ��ɼ��� : ����Ʈ�� ���� Write�Ѵ�.
// �Ű����� : pszByte         : pointer to data to write to file
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
//        // ����Ʈ�� �����͸� ����� ����� ������ ����̴�.
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