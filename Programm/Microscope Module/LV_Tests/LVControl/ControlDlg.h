#pragma once

#include "MicroscopeControl.h"
#include "afxwin.h"

// CControlDlg ダイアログ

class CControlDlg : public CDialog
{
	DECLARE_DYNAMIC(CControlDlg)

public:
	CControlDlg(CMicroscopeControl* pMicroscope, CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CControlDlg();

// ダイアログ データ
	enum { IDD = IDD_DIALOG_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CMicroscopeControl* m_pMicroscope;

private:
	// Nosepiece
	void SetNosepieceControl(BOOL bEnable);
	void UpdateNosepiece();
	// Switch
	void SetSwitchControl(BOOL bEnable);
	void UpdateSwitch();
	// Buzzer
	void SetBuzzerControl(BOOL bEnable);
	void UpdateBuzzer();

	// Nosepiece
	CButton m_rdNosepieceOn;
	CButton m_rdNosepieceOff;
	CButton m_chkNosepiece;
	// Switch
	CButton m_rdSwitchOn;
	CButton m_rdSwitchOff;
	// Buzzer
	CButton m_rdBuzzerOn;
	CButton m_rdBuzzerOff;

public:
	// Nosepiece
	afx_msg void OnBnClickedRadioNosepieceOn();
	afx_msg void OnBnClickedRadioNosepieceOff();
	afx_msg void OnBnClickedCheckNosepiece();
	// Switch
	afx_msg void OnBnClickedRadioSwitchOn();
	afx_msg void OnBnClickedRadioSwitchOff();
	// Buzzer
	afx_msg void OnBnClickedRadioBuzzerOn();
	afx_msg void OnBnClickedRadioBuzzerOff();
};
