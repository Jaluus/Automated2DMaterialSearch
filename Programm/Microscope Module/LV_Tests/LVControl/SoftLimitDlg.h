#pragma once

#include "MicroscopeControl.h"
#include "PropSlider.h"
#include "PropCheck.h"
#include "EventButton.h"
#include "HoldButton.h"
#include "afxwin.h"

enum EnumLimit { E_Upper = 1, E_Lower };

// CSoftLimitDlg ダイアログ

class CSoftLimitDlg : public CDialog
{
	DECLARE_DYNAMIC(CSoftLimitDlg)

public:
	CSoftLimitDlg(CMicroscopeControl* pMicroscope, CWnd* pParent = NULL);   // 標準コンストラクタタ
	virtual ~CSoftLimitDlg();

// ダイアログ データ
	enum { IDD = IDD_DIALOG_SOFTLIMIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CMicroscopeControl* m_pMicroscope;

private:
	// Z Drive
	void UpdateZDriveEnable();
	void UpdateZDriveLimit();
	void UpdateZDriveRange();
	void UpdateZDrivePos();
	void UpdateZDriveAF();
	void SetZDriveSpeedByStep();
	void SetZDriveSpeedContinuous();

	// Z Drive
	CComboBox m_cboZDriveTarget;
	CComboBox m_cboZDriveStep;
	CComboBox m_cboZDriveSearch;
	CComboBox m_cboZDriveSpeed;
	CPropCheck m_chkAF;
	CEventButton m_btnSearch;
	CEventButton m_btnStopSearch;
	CPropSlider m_slZDrive;
	CButton m_btnZDriveU;
	CButton m_btnZDriveUu;
	CButton m_btnZDriveD;
	CButton m_btnZDriveDd;
	CHoldButton m_btnZMax;
	CHoldButton m_btnZMin;
	CBitmap m_bmpZDriveU;
	CBitmap m_bmpZDriveUu;
	CBitmap m_bmpZDriveD;
	CBitmap m_bmpZDriveDd;

protected:
	afx_msg void OnDestroy();
	HRESULT ConnectMipParam();
	HRESULT DisconnectMipParam();

	void ValueChanged(WPARAM wParam);
	void InfoChanged(WPARAM wParam);

public:
	afx_msg LRESULT OnParamEvent(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	// Z Drive
	afx_msg void OnBnClickedButtonZDriveU();
	afx_msg void OnBnClickedButtonZDriveUu();
	afx_msg void OnBnClickedButtonZDriveD();
	afx_msg void OnBnClickedButtonZDriveDd();
	afx_msg LRESULT OnHoldButtonDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHoldButtonUp(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonZDriveChange();
	afx_msg void OnBnClickedButtonSetZDriveLimit();
	afx_msg void OnBnClickedButtonZDriveClear();
	afx_msg void OnCbnSelchangeComboZDriveTarget();
	afx_msg void OnCbnSelchangeComboZDriveStep();
	afx_msg void OnCbnSelchangeComboZDriveSearch();
	afx_msg void OnCbnSelchangeComboZDriveSpeed();
	afx_msg void OnBnClickedButtonAF();
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnBnClickedButtonStopSearch();

private:
	// Z Drive
	EnumLimit m_eLimitTarget;
	CString m_strZDriveUnit;
	long m_lZDriveUnit;
	long m_lZDriveStep;
	BOOL m_bSupportedAF;
	EnumAfSearch m_eAfSearch;
	EnumSpeed m_eZDriveSpeed;
};
