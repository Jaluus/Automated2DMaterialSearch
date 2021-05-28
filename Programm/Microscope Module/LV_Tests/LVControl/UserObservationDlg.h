#pragma once

#include "MicroscopeControl.h"
#include "afxwin.h"

// CUserObservationDlg ダイアログ

class CUserObservationDlg : public CDialog
{
	DECLARE_DYNAMIC(CUserObservationDlg)

public:
	CUserObservationDlg(CMicroscopeControl* pMicroscope, CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CUserObservationDlg();

// ダイアログ データ
	enum { IDD = IDD_DIALOG_USEROBSERVATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CMicroscopeControl* m_pMicroscope;

private:
	void UpdateObservationMode();
	void UpdateOptionalInformation();
	void UpdateOptionalMode();
	void SetOptionalMode();
	void MemorizePresets(BOOL bInit);
	void ControlUnknown();
	void MakeOptionalMode(CString strHead, EnumObservationMode eOptional);
	BOOL IsObservationDic();

	BOOL m_bEpiDia, m_bEpi, m_bDia;
	CComboBox m_cboObservation;
	CComboBox m_cboOptional;
	CComboBox m_cboNosepiece;
	CButton m_rdInterlockOn;
	CButton m_rdInterlockOff;

protected:
	afx_msg void OnDestroy();
	HRESULT ConnectMipParam();
	HRESULT DisconnectMipParam();

public:
	afx_msg void OnCbnSelchangeComboObservation();
	afx_msg void OnCbnSelchangeComboOptional();
	afx_msg void OnCbnSelchangeComboNosepiece();
	afx_msg void OnBnClickedRadioInterlockOn();
	afx_msg void OnBnClickedRadioInterlockOff();
	afx_msg void OnBnClickedButtonUserMemory();
	afx_msg void OnBnClickedButtonUserMemoryInit();
};
