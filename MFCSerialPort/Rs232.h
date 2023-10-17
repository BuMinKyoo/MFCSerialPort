#pragma once
#include <afxwin.h>

#define MAXBUF 4096
#define InBufSize 4096
#define OutBufSize 4096
#define ASCII_XON 0x11
#define ASCII_XOFF 0x13
#define WM_MYRECEIVE (WM_USER+1) // ������ ���� �޽���
#define WM_MYCLOSE (WM_USER+2) // ���� �޽���

class Rs232 :
    public CCmdTarget
{
public:
    Rs232(CString port, CString baudrate, CString parity, CString databit, CString stopbit);
    ~Rs232();

    HANDLE m_hComDev;
    HWND m_hWnd;

    BOOL m_bIsOpenned;
    CString m_sComPort;
    CString m_sBaudRate;
    CString m_sParity;
    CString m_sDataBit;
    CString m_sStopBit;
    BOOL m_bFlowChk;

    OVERLAPPED m_OLW;
    OVERLAPPED m_OLR;

    char m_sInBuf[MAXBUF * 2];
    int m_nLength;
    CEvent* m_pEvent;

    // Rs232���
public:
    void Clear();
    int Receive(LPSTR inbuf, int len); // COM���� ���� ������ �ޱ�
    BOOL Send(LPCTSTR outbuf, int len); // COM���� ������ ����
    BOOL Create(HWND hWnd);
    void HandleClose();
    void Close();
    void ResetSerial();

    // ������ ������ ���� �Լ�
public:
    short PrintQrCode(CString pcsQrCode); // ����Ʈ qr�ڵ� ��� ����
};

