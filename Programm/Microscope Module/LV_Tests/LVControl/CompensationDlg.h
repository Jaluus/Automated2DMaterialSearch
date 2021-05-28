#pragma once

#include "MicroscopeControl.h"
#include "PropSlider.h"
#include "PropCheck.h"
#include "EventButton.h"
#include "HoldButton.h"
#include "afxwin.h"

// CCompensationDlg ダイアログ

class CCompensationDlg : public CDialog
{
	DECLARE_DYNAMIC(CCompensationDlg)

public:
	CCompensationDlg(CMicroscopeControl* pMicroscope, CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CCompensationDlg();

// ダイアログ データ
	enum { IDD = IDD_DIALOG_COMPENSATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CMicroscopeControl* m_pMicroscope;

private:
	// Z Drive
	void UpdateZDriveEnable();
	void UpdateZDriveRange();
	void UpdateZDrivePos();
	void UpdateZDriveAF();
	void SetZDriveSpeedByStep();
	void SetZDriveSpeedContinuous();
	// Nosepiece
	void SetNosepiece(int nIndex);
	void UpdateNosepiece();
	void UpdateCompensation(int nIndex);
	void RetryNosepieceCommand(int nIndex);
	// Momorize/Initialize
	void UpdateMemorizedPresetsEx(BOOL bIsInit);

	// Z Drive
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
	// Nosepiece
	CButton m_chkNosepiece1;
	CButton m_chkNosepiece2;
	CButton m_chkNosepiece3;
	CButton m_chkNosepiece4;
	CButton m_chkNosepiece5;
	CButton* m_chkNosepiece[5];

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
	afx_msg void OnBnClickedButtonZDriveMax();
	afx_msg void OnBnClickedButtonZDriveMin();
	afx_msg void OnBnClickedButtonZDriveChange();
	afx_msg void OnCbnSelchangeComboZDriveStep();
	afx_msg void OnCbnSelchangeComboZDriveSearch();
	afx_msg void OnCbnSelchangeComboZDriveSpeed();
	afx_msg void OnBnClickedButtonAF();
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnBnClickedButtonStopSearch();
	// Nosepiece
	afx_msg void OnBnClickedButtonNosepiece1();
	afx_msg void OnBnClickedButtonNosepiece2();
	afx_msg void OnBnClickedButtonNosepiece3();
	afx_msg void OnBnClickedButtonNosepiece4();
	afx_msg void OnBnClickedButtonNosepiece5();
	// Momorize
	afx_msg void OnBnClickedButtonNosepieceMemorize();
	// Initialize
	afx_msg void OnBnClickedButtonNosepieceClear();

private:
	// Z Drive
	CString m_strZDriveUnit;
	long m_lZDriveUnit;
	long m_lZDriveStep;
	BOOL m_bSupportedAF;
	EnumAfSearch m_eAfSearch;
	EnumSpeed m_eZDriveSpeed;
};
