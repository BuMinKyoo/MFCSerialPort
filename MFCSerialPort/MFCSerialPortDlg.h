
// MFCSerialPortDlg.h: 헤더 파일
//

#pragma once
#include "Rs232.h"


// CMFCSerialPortDlg 대화 상자
class CMFCSerialPortDlg : public CDialogEx
{
// 생성입니다.
public:
	CMFCSerialPortDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCSERIALPORT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	Rs232* m_Rs232_comm;
	LRESULT OnThreadClosed(WPARAM length, LPARAM lpara);
	LRESULT OnReceive(WPARAM length, LPARAM lpara);

	BOOL comport_state;
	CString m_str_comport;
	CString m_str_baudrate;
	CEdit m_edit_rcv_view;
	CEdit m_edit_send_data;
	afx_msg void OnBnClickedConnectBtn();
	afx_msg void OnBnClickedClearBtn();
	afx_msg void OnBnClickedSendBtn();
	afx_msg void OnCbnSelchangeComboComport();
	afx_msg void OnCbnSelchangeComboBaudrate();
	CComboBox m_combo_comport_list;
	CComboBox m_combo_baudrate_list;
};
