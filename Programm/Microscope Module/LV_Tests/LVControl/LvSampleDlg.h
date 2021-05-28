
// LvSampleDlg.h : header file
//

#pragma once

#include "afxwin.h"
#include "MicroscopeControl.h"
#include "PropCombo.h"
#include "PropCheck.h"
#include "PropSlider.h"
#include "PropEdit.h"
#include "PropStatic.h"
#include "EventButton.h"
#include "HoldButton.h"

#define EPI_SHUTTER_TIMER 1
#define EPI_CTRL_ENABLE_TIME 500

// CLvSampleDlg dialog
class CLvSampleDlg : public CDialog
{
// Construction
public:
	CLvSampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_LVSAMPLE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

// Member
public:
	CMicroscopeControl m_Microscope;

private:
	// View expand
	BOOL m_bExpand;
	// Observation
	CPropCombo m_cboObservation;
	// EPI
	CPropCheck m_chkEpi;
	CPropSlider m_slEpi;
	CPropStatic m_stEpiMin;
	CPropStatic m_stEpiMax;
	// Fiber
	CPropCheck m_chkFiber;
	CButton m_btnFiberCmd;
	CEdit m_edFiber;
	CPropCombo m_cboFiber;
	// Nosepiece
	CPropCombo m_cboNosepiece;
	// Cube
	CPropCombo m_cboCube;
	// DIA
	CPropCheck m_chkDia;
	CPropSlider m_slDia;
	CPropStatic m_stDiaMin;
	CPropStatic m_stDiaMax;
	// Z Drive
	CComboBox m_cboZDriveStep;
	CComboBox m_cboZDriveSearch;
	CComboBox m_cboZDriveSpeed;
	CPropSlider m_slZDrive;
	CPropCheck m_chkZEscape;
	CButton m_chkAlf;
	CPropCheck m_chkAF;
	CEventButton m_btnSearch;
	CEventButton m_btnStopSearch;
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
	// Aperture
	CPropSlider m_slAperture;
	CPropStatic m_stApertureMin;
	CPropStatic m_stApertureMax;
	// DIC
	CPropCombo m_cboDIC;
	CPropSlider m_slDIC;
	CStatic m_stLED;
	CBitmap m_bmpLEDOn;
	CBitmap m_bmpLEDOff;
	CStatic m_stDicLbl1;
	CStatic m_stDicLbl2;
	CStatic m_stDicLbl3;
	CStatic m_stDicLbl4;
	CStatic m_stDicLbl5;
	// Interlock
	CComboBox m_cboInterlock;

protected:
	afx_msg void OnDestroy();
	HRESULT ConnectMipParam();
	HRESULT DisconnectMipParam();

	void ValueChanged(WPARAM wParam);
	void InfoChanged(WPARAM wParam);

public:
	afx_msg LRESULT OnParamEvent(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
// Control
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	// Observation
	afx_msg void OnCbnSelchangeComboObservation();
	// EPI
	afx_msg void OnBnClickedCheckEpi();
	afx_msg void OnBnClickedButtonEpiDec();
	afx_msg void OnBnClickedButtonEpiInc();
	afx_msg void OnBnClickedButtonEpiPreset();
	// Fiber
	afx_msg void OnBnClickedCheckFiber();
	afx_msg void OnBnClickedButtonFiberCmd();
	afx_msg void OnCbnSelchangeComboND();
	// Nosepiece
	afx_msg void OnCbnSelchangeComboNosepiece();
	afx_msg void OnBnClickedButtonNosepiecePrev();
	afx_msg void OnBnClickedButtonNosepieceNext();
	// Cube
	afx_msg void OnCbnSelchangeComboCube();
	afx_msg void OnBnClickedButtonCubePrev();
	afx_msg void OnBnClickedButtonCubeNext();
	// DIA
	afx_msg void OnBnClickedCheckDia();
	afx_msg void OnBnClickedButtonDiaDec();
	afx_msg void OnBnClickedButtonDiaInc();
	afx_msg void OnBnClickedButtonDiaPreset();
	// Z Drive
	afx_msg void OnBnClickedButtonZDriveU();
	afx_msg void OnBnClickedButtonZDriveUu();
	afx_msg void OnBnClickedButtonZDriveD();
	afx_msg void OnBnClickedButtonZDriveDd();
	afx_msg LRESULT OnHoldButtonDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHoldButtonUp(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonZDriveChange();
	afx_msg void OnBnClickedCheckZDriveALF();
	afx_msg void OnCbnSelchangeComboZDriveStep();
	afx_msg void OnCbnSelchangeComboZDriveSearch();
	afx_msg void OnCbnSelchangeComboZDriveSpeed();
	afx_msg void OnBnClickedButtonZEscape();
	afx_msg void OnBnClickedButtonAF();
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnBnClickedButtonStopSearch();
	// Aperture
	afx_msg void OnBnClickedButtonApertureDec();
	afx_msg void OnBnClickedButtonApertureInc();
	afx_msg void OnBnClickedButtonAperturePreset();
	// DIC
	afx_msg void OnBnClickedButtonDICDec();
	afx_msg void OnBnClickedButtonDICInc();
	afx_msg void OnBnClickedButtonDICCalibration();
	afx_msg void OnBnClickedButtonDICClear();
	afx_msg void OnCbnSelchangeComboDIC();
	// Interlock
	afx_msg void OnCbnSelchangeComboInterlock();

// Menu command
	afx_msg void OnAppAbout();
	// File
	afx_msg void OnFileSelectDevice();
	afx_msg void OnFileCommand();
	// View
	afx_msg void OnViewStandard();
	afx_msg void OnViewExpand();
	// Setting
	afx_msg void OnSettingInitialize();
	afx_msg void OnSettingInterlock();
	afx_msg void OnSettingOffset();
	afx_msg void OnSettingControl();
	afx_msg void OnSettingFocus();
	afx_msg void OnSettingSoftLimit();
	afx_msg void OnDicCalibration();
	afx_msg void OnDicClear();
	afx_msg void OnSettingAFOffset();
	// Status
	afx_msg void OnStatusNosepiese();
	afx_msg void OnStatusCube();
	afx_msg void OnStatusInterlock();
	afx_msg void OnStatusUserObservation();
	afx_msg void OnStatusEpiDia();
	afx_msg void OnStatusZDrive();
	afx_msg void OnStatusDic();
	afx_msg void OnStatusSystem();
	// Help
	afx_msg void OnHelpIndex();

private:
	// Observation
	void UpdateObservationInformation();
	void UpdateObservationMode();
	// EPI
	void UpdateEpiLamp();
	void UpdateEpiValue();
	// Fiber
	void UpdateFiberShutter();
	void UpdateNDFilter();
	// Nosepiece
	void UpdateNosepieceInformation();
	void UpdateNosepiece();
	void MoveNosepiece(int nType);
	BOOL RetryNosepieceCommand(int nType);
	// Cube
	void UpdateCubeInformation();
	void UpdateCube();
	void MoveCube(int nType);
	// DIA
	void UpdateDiaLamp();
	void UpdateDiaValue();
	// Z Drive
	void UpdateZDriveEnable();
	void UpdateZDriveRange();
	void UpdateZDrivePos();
	void UpdateZDriveInterlockMode();
	void UpdateZEscape();
	void UpdateZDriveAF();
	void SetZDriveSpeedByStep();
	void SetZDriveSpeedContinuous();
	// Aperture
	void UpdateApertureEnable();
	void UpdateApertureValue();
	// DIC
	void UpdateDICEnable();
	void UpdateDICValue();
	void UpdateDICPosition();
	void UpdateDICCalibration();
	// Interlock
	void UpdateInterlock();
	// View expand
	void SetViewExpand(BOOL bExpand, BOOL bInit = FALSE);

	// Mode
	BOOL m_bDarkField, m_bDarkFieldCube, m_bObservationDIC, m_bNosepieceDIC;
	// EPI
	CString m_strEpiUnit;
	long m_lEpiUnit;
	// DIA
	CString m_strDiaUnit;
	long m_lDiaUnit;
	// Z Drive
	CString m_strZDriveUnit;
	long m_lZDriveUnit;
	long m_lZDriveStep;
	BOOL m_bIsEscape;
	BOOL m_bSupportedAF;
	EnumAfSearch m_eAfSearch;
	EnumSpeed m_eZDriveSpeed;
	// Aperture
	CString m_strApertureUnit;
	long m_lApertureUnit;
	// DIC
	CString m_strDICUnit;
	long m_lDICUnit;
	// Interlock
	EnumAccessory m_eInterlockTarget;
};
