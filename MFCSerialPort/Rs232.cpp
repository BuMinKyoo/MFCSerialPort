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

// ����Ʈ qr�ڵ� ��� : TM-T83ll ����
short Rs232::PrintQrCode(CString pcsQrCode)
{
    char	szSendBuff[1024];
    CString			LF = _T("\x0A");
    CString			GS = _T("\x1d");

    //��� ���
    memset(szSendBuff, 0x00, sizeof(szSendBuff));
    szSendBuff[0] = 0x1B;
    szSendBuff[1] = 0x61;
    szSendBuff[2] = 0x01;
    Send(szSendBuff, 3);

    //QR Code: Select the model
    memset(szSendBuff, 0x00, sizeof(szSendBuff));
    szSendBuff[0] = 0x1D;
    szSendBuff[1] = 0x28;
    szSendBuff[2] = 0x6B;
    szSendBuff[3] = 0x04;
    szSendBuff[4] = 0x00;
    szSendBuff[5] = 0x31;
    szSendBuff[6] = 0x41;
    szSendBuff[7] = 0x32;			// 49 model1, 50 model2, 51 micro QR Code+
    szSendBuff[8] = 0x00;
    Send(szSendBuff, 9);

    //QR Code: Set the size of module
    memset(szSendBuff, 0x00, sizeof(szSendBuff));
    szSendBuff[0] = 0x1D;
    szSendBuff[1] = 0x28;
    szSendBuff[2] = 0x6B;
    szSendBuff[3] = 0x03;
    szSendBuff[4] = 0x00;
    szSendBuff[5] = 0x31;
    szSendBuff[6] = 0x43;
    szSendBuff[7] = 0x06;
    Send(szSendBuff, 8);

    //QR Code: Select the error correction level
    memset(szSendBuff, 0x00, sizeof(szSendBuff));
    szSendBuff[0] = 0x1D;
    szSendBuff[1] = 0x28;
    szSendBuff[2] = 0x6B;
    szSendBuff[3] = 0x03;
    szSendBuff[4] = 0x00;
    szSendBuff[5] = 0x31;
    szSendBuff[6] = 0x45;
    szSendBuff[7] = 0x30;		// 48 Selects Error correction level L , Recover 7% - default
    // 49 Selects Error correction level M , Recover 15%
    // 50 Selects Error correction level Q , Recover 25%
    // 51 Selects Error correction level H , Recover 30%
    Send(szSendBuff, 8);

    //QR Code: Store the data in the symbol storage area
    memset(szSendBuff, 0x00, sizeof(szSendBuff));
    int inLen = pcsQrCode.GetLength() + 3;
    szSendBuff[0] = 0x1D;
    szSendBuff[1] = 0x28;
    szSendBuff[2] = 0x6B;
    szSendBuff[3] = inLen % 256;
    szSendBuff[4] = inLen / 256;
    szSendBuff[5] = 0x31;
    szSendBuff[6] = 0x50;
    szSendBuff[7] = 0x30;

    strcat_s(szSendBuff + 8, sizeof(szSendBuff) - 8, pcsQrCode);
    Send(szSendBuff, 8 + pcsQrCode.GetLength()); // QR�� ������ ���

    //QR Code: Print the symbol data in the symbol storage area
    memset(szSendBuff, 0x00, sizeof(szSendBuff));
    szSendBuff[0] = 0x1D;
    szSendBuff[1] = 0x28;
    szSendBuff[2] = 0x6B;
    szSendBuff[3] = 0x03;
    szSendBuff[4] = 0x00;
    szSendBuff[5] = 0x31;
    szSendBuff[6] = 0x51;
    szSendBuff[7] = 0x30;
    Send(szSendBuff, 8); // QR�ڵ� �μ�

    // �۾���� ���� �ʱ�ȭ(��������)
    memset(szSendBuff, 0x00, sizeof(szSendBuff));
    szSendBuff[0] = 0x1B;
    szSendBuff[1] = 0x61;
    szSendBuff[2] = 0x00;
    Send(szSendBuff, 3);

    return 1;
}