
// LvSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LvSample.h"
#include "LvSampleDlg.h"
#include "SetupDlg.h"
#include "ZDriveRangeDlg.h"
#include "CommandDlg.h"
#include "DeviceDlg.h"
#include "UserObservationDlg.h"
#include "ControlDlg.h"
#include "CompensationDlg.h"
#include "SoftLimitDlg.h"
#include "AfOffsetDlg.h"
#include "InfoDlg.h"
#include "LvAboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLvSampleDlg dialog

CLvSampleDlg* s_pThisDlg = NULL;

CLvSampleDlg::CLvSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLvSampleDlg::IDD, pParent)
	, m_bExpand(FALSE)
	, m_bDarkField(FALSE)
	, m_bDarkFieldCube(FALSE)
	, m_bObservationDIC(FALSE)
	, m_bNosepieceDIC(FALSE)
	, m_lEpiUnit(10)
	, m_lDiaUnit(10)
	, m_lZDriveUnit(200)
	, m_lZDriveStep(20)
	, m_bIsEscape(FALSE)
	, m_bSupportedAF(FALSE)
	, m_eAfSearch(AfSearchMode1)
	, m_eZDriveSpeed(Speed1)
	, m_lApertureUnit(10)
	, m_lDICUnit(1000)
	, m_eInterlockTarget(AccessoryNosepiece)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_LVCONTROL);
}

void CLvSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	// Observation
	DDX_Control(pDX, IDC_COMBO_OBSERVATION_MODE, m_cboObservation);
	// EPI
	DDX_Control(pDX, IDC_BUTTON_EPI_LAMP, m_chkEpi);
	DDX_Control(pDX, IDC_SLIDER_EPI_LAMP, m_slEpi);
	DDX_Control(pDX, IDC_STATIC_EPI_MIN, m_stEpiMin);
	DDX_Control(pDX, IDC_STATIC_EPI_MAX, m_stEpiMax);
	// Fiber
	DDX_Control(pDX, IDC_BUTTON_FIBER, m_chkFiber);
	DDX_Control(pDX, IDC_BUTTON_FIBER_CMD, m_btnFiberCmd);
	DDX_Control(pDX, IDC_COMBO_FIBER, m_cboFiber);
	DDX_Control(pDX, IDC_EDIT_FIBER, m_edFiber);
	// Nosepiece
	DDX_Control(pDX, IDC_COMBO_NOSEPIECE, m_cboNosepiece);
	// Cube
	DDX_Control(pDX, IDC_COMBO_CUBE, m_cboCube);
	// DIA
	DDX_Control(pDX, IDC_BUTTON_DIA_LAMP, m_chkDia);
	DDX_Control(pDX, IDC_SLIDER_DIA_LAMP, m_slDia);
	DDX_Control(pDX, IDC_STATIC_DIA_MIN, m_stDiaMin);
	DDX_Control(pDX, IDC_STATIC_DIA_MAX, m_stDiaMax);
	// Z Drive
	DDX_Control(pDX, IDC_COMBO_ZDRIVE_STEP, m_cboZDriveStep);
	DDX_Control(pDX, IDC_COMBO_ZDRIVE_SEARCH, m_cboZDriveSearch);
	DDX_Control(pDX, IDC_COMBO_ZDRIVE_SPEED, m_cboZDriveSpeed);
	DDX_Control(pDX, IDC_CHECK_ZDRIVE_ALF, m_chkAlf);
	DDX_Control(pDX, IDC_SLIDER_ZDRIVE, m_slZDrive);
	DDX_Control(pDX, IDC_BUTTON_ZDRIVE_U, m_btnZDriveU);
	DDX_Control(pDX, IDC_BUTTON_ZDRIVE_UU, m_btnZDriveUu);
	DDX_Control(pDX, IDC_BUTTON_ZDRIVE_D, m_btnZDriveD);
	DDX_Control(pDX, IDC_BUTTON_ZDRIVE_DD, m_btnZDriveDd);
	DDX_Control(pDX, IDC_BUTTON_ZDRIVE_MIN, m_btnZMin);
	DDX_Control(pDX, IDC_BUTTON_ZDRIVE_MAX, m_btnZMax);
	DDX_Control(pDX, IDC_BUTTON_ZDRIVE_ESCAPE, m_chkZEscape);
	DDX_Control(pDX, IDC_BUTTON_AF, m_chkAF);
	DDX_Control(pDX, IDC_BUTTON_SEARCH, m_btnSearch);
	DDX_Control(pDX, IDC_BUTTON_STOPSEARCH, m_btnStopSearch);
	// Aperture
	DDX_Control(pDX, IDC_SLIDER_APERTURE, m_slAperture);
	DDX_Control(pDX, IDC_STATIC_APERTURE_MIN, m_stApertureMin);
	DDX_Control(pDX, IDC_STATIC_APERTURE_MAX, m_stApertureMax);
	// DIC
	DDX_Control(pDX, IDC_COMBO_DIC, m_cboDIC);
	DDX_Control(pDX, IDC_SLIDER_DIC, m_slDIC);
	DDX_Control(pDX, IDC_STATIC_LED, m_stLED);
	DDX_Control(pDX, IDC_STATIC_DIC1, m_stDicLbl1);
	DDX_Control(pDX, IDC_STATIC_DIC2, m_stDicLbl2);
	DDX_Control(pDX, IDC_STATIC_DIC3, m_stDicLbl3);
	DDX_Control(pDX, IDC_STATIC_DIC4, m_stDicLbl4);
	DDX_Control(pDX, IDC_STATIC_DIC5, m_stDicLbl5);
	// InterLock
	DDX_Control(pDX, IDC_COMBO_INTERLOCK, m_cboInterlock);
}

BEGIN_MESSAGE_MAP(CLvSampleDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_TIMER()
	ON_MESSAGE(WM_PAREVENT, &CLvSampleDlg::OnParamEvent)
// Control
	// Observation
	ON_CBN_SELCHANGE(IDC_COMBO_OBSERVATION_MODE, &CLvSampleDlg::OnCbnSelchangeComboObservation)
	// EPI
	ON_BN_CLICKED(IDC_BUTTON_EPI_LAMP, &CLvSampleDlg::OnBnClickedCheckEpi)
	ON_BN_CLICKED(IDC_BUTTON_EPI_L, &CLvSampleDlg::OnBnClickedButtonEpiDec)
	ON_BN_CLICKED(IDC_BUTTON_EPI_R, &CLvSampleDlg::OnBnClickedButtonEpiInc)
	ON_BN_CLICKED(IDC_BUTTON_EPI_PRESET, &CLvSampleDlg::OnBnClickedButtonEpiPreset)
	// Fiber
	ON_BN_CLICKED(IDC_BUTTON_FIBER, &CLvSampleDlg::OnBnClickedCheckFiber)
	ON_BN_CLICKED(IDC_BUTTON_FIBER_CMD, &CLvSampleDlg::OnBnClickedButtonFiberCmd)
	ON_CBN_SELCHANGE(IDC_COMBO_FIBER, &CLvSampleDlg::OnCbnSelchangeComboND)
	// Nosepiece
	ON_CBN_SELCHANGE(IDC_COMBO_NOSEPIECE, &CLvSampleDlg::OnCbnSelchangeComboNosepiece)
	ON_BN_CLICKED(IDC_BUTTON_NOSEPIECE_L, &CLvSampleDlg::OnBnClickedButtonNosepiecePrev)
	ON_BN_CLICKED(IDC_BUTTON_NOSEPIECE_R, &CLvSampleDlg::OnBnClickedButtonNosepieceNext)
	// Cube
	ON_CBN_SELCHANGE(IDC_COMBO_CUBE, &CLvSampleDlg::OnCbnSelchangeComboCube)
	ON_BN_CLICKED(IDC_BUTTON_CUBE_L, &CLvSampleDlg::OnBnClickedButtonCubePrev)
	ON_BN_CLICKED(IDC_BUTTON_CUBE_R, &CLvSampleDlg::OnBnClickedButtonCubeNext)
	// DIA
	ON_BN_CLICKED(IDC_BUTTON_DIA_LAMP, &CLvSampleDlg::OnBnClickedCheckDia)
	ON_BN_CLICKED(IDC_BUTTON_DIA_L, &CLvSampleDlg::OnBnClickedButtonDiaDec)
	ON_BN_CLICKED(IDC_BUTTON_DIA_R, &CLvSampleDlg::OnBnClickedButtonDiaInc)
	ON_BN_CLICKED(IDC_BUTTON_DIA_PRESET, &CLvSampleDlg::OnBnClickedButtonDiaPreset)
	// Z Drive
	ON_BN_CLICKED(IDC_BUTTON_ZDRIVE_U, &CLvSampleDlg::OnBnClickedButtonZDriveU)
	ON_BN_CLICKED(IDC_BUTTON_ZDRIVE_UU, &CLvSampleDlg::OnBnClickedButtonZDriveUu)
	ON_BN_CLICKED(IDC_BUTTON_ZDRIVE_D, &CLvSampleDlg::OnBnClickedButtonZDriveD)
	ON_BN_CLICKED(IDC_BUTTON_ZDRIVE_DD, &CLvSampleDlg::OnBnClickedButtonZDriveDd)
	ON_MESSAGE(WM_HOLD_BUTTON_DOWN, &CLvSampleDlg::OnHoldButtonDown)
	ON_MESSAGE(WM_HOLD_BUTTON_UP, &CLvSampleDlg::OnHoldButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_ZDRIVE_CHANGE, &CLvSampleDlg::OnBnClickedButtonZDriveChange)
	ON_BN_CLICKED(IDC_CHECK_ZDRIVE_ALF, &CLvSampleDlg::OnBnClickedCheckZDriveALF)
	ON_CBN_SELCHANGE(IDC_COMBO_ZDRIVE_STEP, &CLvSampleDlg::OnCbnSelchangeComboZDriveStep)
	ON_CBN_SELCHANGE(IDC_COMBO_ZDRIVE_SEARCH, &CLvSampleDlg::OnCbnSelchangeComboZDriveSearch)
	ON_CBN_SELCHANGE(IDC_COMBO_ZDRIVE_SPEED, &CLvSampleDlg::OnCbnSelchangeComboZDriveSpeed)
	ON_BN_CLICKED(IDC_BUTTON_ZDRIVE_ESCAPE, &CLvSampleDlg::OnBnClickedButtonZEscape)
	ON_BN_CLICKED(IDC_BUTTON_AF, &CLvSampleDlg::OnBnClickedButtonAF)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CLvSampleDlg::OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_STOPSEARCH, &CLvSampleDlg::OnBnClickedButtonStopSearch)
	// Aperture
	ON_BN_CLICKED(IDC_BUTTON_APERTURE_L, &CLvSampleDlg::OnBnClickedButtonApertureDec)
	ON_BN_CLICKED(IDC_BUTTON_APERTURE_R, &CLvSampleDlg::OnBnClickedButtonApertureInc)
	ON_BN_CLICKED(IDC_BUTTON_APERTURE_PRESET, &CLvSampleDlg::OnBnClickedButtonAperturePreset)
	// DIC
	ON_BN_CLICKED(IDC_BUTTON_DIC_L, &CLvSampleDlg::OnBnClickedButtonDICDec)
	ON_BN_CLICKED(IDC_BUTTON_DIC_R, &CLvSampleDlg::OnBnClickedButtonDICInc)
	ON_BN_CLICKED(IDC_BUTTON_DIC_CALIBRATION, &CLvSampleDlg::OnBnClickedButtonDICCalibration)
	ON_BN_CLICKED(IDC_BUTTON_DIC_CLEAR, &CLvSampleDlg::OnBnClickedButtonDICClear)
	ON_CBN_SELCHANGE(IDC_COMBO_DIC, &CLvSampleDlg::OnCbnSelchangeComboDIC)
	// Interlock
	ON_CBN_SELCHANGE(IDC_COMBO_INTERLOCK, &CLvSampleDlg::OnCbnSelchangeComboInterlock)
// Menu command
	ON_COMMAND(ID_APP_ABOUT, &CLvSampleDlg::OnAppAbout)
	// File
	ON_COMMAND(ID_FILE_SELECT_DEVICE, &CLvSampleDlg::OnFileSelectDevice)
	ON_COMMAND(ID_FILE_COMMAND, &CLvSampleDlg::OnFileCommand)
	// View
	ON_COMMAND(ID_VIEW_STANDARD, &CLvSampleDlg::OnViewStandard)
	ON_COMMAND(ID_VIEW_EXPAND, &CLvSampleDlg::OnViewExpand)
	// Setting
	ON_COMMAND(ID_SETTING_INITIALIZE, &CLvSampleDlg::OnSettingInitialize)
	ON_COMMAND(ID_SETTING_INTERLOCK, &CLvSampleDlg::OnSettingInterlock)
	ON_COMMAND(ID_SETTING_OFFSET, &CLvSampleDlg::OnSettingOffset)
	ON_COMMAND(ID_SETTING_CONTROL, &CLvSampleDlg::OnSettingControl)
	ON_COMMAND(ID_SETTING_FOCUS, &CLvSampleDlg::OnSettingFocus)
	ON_COMMAND(ID_SETTING_SOFTLIMIT, &CLvSampleDlg::OnSettingSoftLimit)
	ON_COMMAND(ID_DIC_CALIBRATION, &CLvSampleDlg::OnDicCalibration)
	ON_COMMAND(ID_DIC_CLEAR, &CLvSampleDlg::OnDicClear)
	ON_COMMAND(ID_SETTING_AF_OFFSET, &CLvSampleDlg::OnSettingAFOffset)
	// Status
	ON_COMMAND(ID_STATUS_NOSEPIESE, &CLvSampleDlg::OnStatusNosepiese)
	ON_COMMAND(ID_STATUS_CUBE, &CLvSampleDlg::OnStatusCube)
	ON_COMMAND(ID_STATUS_INTERLOCK, &CLvSampleDlg::OnStatusInterlock)
	ON_COMMAND(ID_STATUS_USER_OBSERVATION, &CLvSampleDlg::OnStatusUserObservation)
	ON_COMMAND(ID_STATUS_EPIDIA, &CLvSampleDlg::OnStatusEpiDia)
	ON_COMMAND(ID_STATUS_ZDRIVE, &CLvSampleDlg::OnStatusZDrive)
	ON_COMMAND(ID_STATUS_DIC, &CLvSampleDlg::OnStatusDic)
	ON_COMMAND(ID_STATUS_SYSTEM, &CLvSampleDlg::OnStatusSystem)
	ON_COMMAND(ID_HELP_INDEX, &CLvSampleDlg::OnHelpIndex)
END_MESSAGE_MAP()


// CLvSampleDlg message handlers

BOOL CLvSampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	// EPI Shutter OpenWithTimer time
	m_edFiber.SetWindowText(_T("0.2"));

	// ND filter ComboBox
	m_cboFiber.SetUseUnknownSel(FALSE);

	// Nosepiece ComboBox
	m_cboNosepiece.SetUseUnknownSel(FALSE);

	// Cube ComboBox
	m_cboCube.SetUseUnknownSel(FALSE);

	// Z Drive step ComboBox
	m_cboZDriveStep.ResetContent();
	m_cboZDriveStep.SetItemData(m_cboZDriveStep.AddString(_T("1um")), 20);
	m_cboZDriveStep.SetItemData(m_cboZDriveStep.AddString(_T("10um")), 200);
	m_cboZDriveStep.SetItemData(m_cboZDriveStep.AddString(_T("100um")), 2000);
	m_cboZDriveStep.SetItemData(m_cboZDriveStep.AddString(_T("1000um")), 20000);
	m_cboZDriveStep.SetCurSel(0);

	// Z Drive AF search ComboBox
	m_cboZDriveSearch.ResetContent();
	m_cboZDriveSearch.SetItemData(m_cboZDriveSearch.AddString(_T("Up")), AfSearchMode1);
	m_cboZDriveSearch.SetItemData(m_cboZDriveSearch.AddString(_T("UpDown")), AfSearchMode2);
	m_cboZDriveSearch.SetCurSel(0);

	// Z Drive speed ComboBox
	m_cboZDriveSpeed.ResetContent();
	CString strSpeedHigh, strSpeedMiddle, strSpeedLow;
	strSpeedHigh.LoadString(IDS_SPEED_HIGH);
	strSpeedMiddle.LoadString(IDS_SPEED_MIDDLE);
	strSpeedLow.LoadString(IDS_SPEED_LOW);
	m_cboZDriveSpeed.SetItemData(m_cboZDriveSpeed.AddString(strSpeedHigh), Speed1);
	m_cboZDriveSpeed.SetItemData(m_cboZDriveSpeed.AddString(strSpeedMiddle), Speed3);
	m_cboZDriveSpeed.SetItemData(m_cboZDriveSpeed.AddString(strSpeedLow), Speed6);
	m_cboZDriveSpeed.SetCurSel(0);

	// Z Drive move button
	m_bmpZDriveU.LoadBitmap(IDB_BITMAP_U);
	m_bmpZDriveUu.LoadBitmap(IDB_BITMAP_UU);
	m_bmpZDriveD.LoadBitmap(IDB_BITMAP_D);
	m_bmpZDriveDd.LoadBitmap(IDB_BITMAP_DD);
	m_btnZDriveU.SetBitmap(m_bmpZDriveU);
	m_btnZDriveUu.SetBitmap(m_bmpZDriveUu);
	m_btnZDriveD.SetBitmap(m_bmpZDriveD);
	m_btnZDriveDd.SetBitmap(m_bmpZDriveDd);
	m_btnZMax.SetBitmap(m_bmpZDriveUu);
	m_btnZMin.SetBitmap(m_bmpZDriveDd);

	// DIC Calibration LED
	m_bmpLEDOn.LoadBitmap(IDB_BITMAP_LED_ON);
	m_bmpLEDOff.LoadBitmap(IDB_BITMAP_LED_OFF);
	m_stLED.SetBitmap(m_bmpLEDOff);

	// Interlock mode ComboBox
	m_cboInterlock.ResetContent();
	CString strDefault, strOptional;
	strDefault.LoadString(IDS_INTERLOCK_DEFAULT);
	strOptional.LoadString(IDS_INTERLOCK_OPTIONAL);
	m_cboInterlock.AddString(strDefault);
	m_cboInterlock.AddString(strOptional);
	
	// Connect to Microscope and MipParam
	s_pThisDlg = this;
	if (FAILED(m_Microscope.Initialize())) {
		EndDialog(0);
	}
	else {
		m_Microscope.Connect();
		ConnectMipParam();
	}

	SetViewExpand(FALSE, TRUE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLvSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
/*	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
*/
		CDialog::OnSysCommand(nID, lParam);
//	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLvSampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLvSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLvSampleDlg::OnDestroy()
{
	KillTimer(EPI_SHUTTER_TIMER);

	DisconnectMipParam();
	m_Microscope.Disconnect();
	m_Microscope.Uninitialize();

	m_bmpZDriveU.DeleteObject();
	m_bmpZDriveUu.DeleteObject();
	m_bmpZDriveD.DeleteObject();
	m_bmpZDriveDd.DeleteObject();

	m_bmpLEDOn.DeleteObject();
	m_bmpLEDOff.DeleteObject();

	CDialog::OnDestroy();
}

// App command to run the dialog
void CLvSampleDlg::OnAppAbout()
{
	CLvAboutDlg dlg(&m_Microscope);
	dlg.DoModal();
//	CAboutDlg aboutDlg;
//	aboutDlg.DoModal();
}

FPropComboCallback fPropComboCallback(EnumAccessory eAccessory, long lVal, CString& strName)
{
	if (s_pThisDlg)
		s_pThisDlg->m_Microscope.GetName(eAccessory, lVal, strName);

	return 0;
}

HRESULT CLvSampleDlg::ConnectMipParam()
{
	HRESULT hr = S_OK;

	try {
		INikonLv* pNikonLv = m_Microscope.GetInterface();

		VARIANT val;
		UINT nShow;
		// Observation
		CComPtr<IPresets> pPresets = NULL;
		// EPI
		CComPtr<IEpiLamp> pEpiLamp = NULL;
		// Fiber
		CComPtr<IEpiShutter> pEpiShutter = NULL;
		CComPtr<INDFilter> pNDFilter = NULL;
		// Nosepiece
		CComPtr<INosepiece> pNosepiece = NULL;
		// Cube
		CComPtr<IFilterBlockCassette> pFilterBlockCassette = NULL;
		// DIA
		CComPtr<IDiaLamp> pDiaLamp = NULL;
		// Z Drive
		CComPtr<IZDrive> pZDrive = NULL;
		// Aperture
		CComPtr<IEpiApertureStop> pEpiApertureStop = NULL;
		// DIC
		CComPtr<IDICPrism> pDicPrism = NULL;

		// Dialog caption
		CString str;
		CString strSystemName;
		strSystemName.Format(_T("%s"), pNikonLv->SystemName.GetBSTR());
		if (strSystemName.IsEmpty()) {
			_bstr_t bstrDeviceName = pNikonLv->Device->Address;
			CString strDeviceName(bstrDeviceName.GetBSTR());
			if (strDeviceName.IsEmpty())
				str = _T("LV Control (null)");
			else
				str.Format(_T("LV Control (%s)"), strDeviceName);
		}
		else
			str.Format(_T("LV Control (%s)"), strSystemName);
		SetWindowText(str);

		// Observation
		if (m_Microscope.IsLVSupportObservationMode()) {
			if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
				_com_raise_error(hr);
			if (FAILED(hr = pPresets->get_ObservationMode(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_cboObservation.Connect(val.pdispVal)))
				_com_raise_error(hr);

			UpdateObservationInformation();
			UpdateObservationMode();
		}
		// AF Observation
		else if (m_Microscope.IsAFController()) {
			if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
				_com_raise_error(hr);
			if (FAILED(hr = pPresets->get_AfObservationMode(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_cboObservation.Connect(val.pdispVal)))
				_com_raise_error(hr);
		}
		else if (m_Microscope.IsLV150NAController() || m_Microscope.IsLVINADController()) {
			GetDlgItem(IDC_STATIC_OBSERVATION_MODE)->EnableWindow(FALSE);
			GetDlgItem(IDC_COMBO_OBSERVATION_MODE)->EnableWindow(FALSE);
		}

		// EPI
		if (FAILED(hr = pNikonLv->get_EpiLamp(&pEpiLamp)))
			_com_raise_error(hr);
		m_Microscope.m_bEpiLampMounted = (EnumStatus)((long)pEpiLamp->IsMounted) == StatusTrue ? TRUE : FALSE;

		if (m_Microscope.m_bEpiLampMounted) {
			m_lEpiUnit = (long)pEpiLamp->ValuePerUnit;
			BSTR bstrUnit = NULL;
			if (FAILED(hr = pEpiLamp->get_Unit(&bstrUnit)))
				_com_raise_error(hr);
			m_strEpiUnit = CString(bstrUnit);
			if (bstrUnit != NULL) {
				::SysFreeString(bstrUnit);
				bstrUnit = NULL;
			}
			EnumLampType eLampType = pEpiLamp->Get_LampType();
			if (eLampType == LampTypeLED)
				m_slEpi.SetTicFreq(m_lEpiUnit * 10);
			else
				m_slEpi.SetTicFreq(m_lEpiUnit);

			m_slEpi.SetUnit(m_lEpiUnit);
			m_slEpi.SetDecimal(1);
			m_slEpi.SetDispType(VT_R8);
			m_slEpi.SetUnitString(m_strEpiUnit);
			m_slEpi.SetDispUnit(TRUE);

			if (FAILED(hr = pEpiLamp->get_Value(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_slEpi.Connect(val.pdispVal)))
				_com_raise_error(hr);

			if (m_eInterlockTarget == AccessoryNosepiece)
				m_eInterlockTarget = AccessoryEpiLampVoltage;

			m_stEpiMin.SetUnit(m_lEpiUnit);
			m_stEpiMin.SetDecimal(1);
			m_stEpiMin.SetDispType(VT_R8);
			if (FAILED(hr = pEpiLamp->get_LowerLimit(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_stEpiMin.Connect(val.pdispVal)))
				_com_raise_error(hr);

			m_stEpiMax.SetUnit(m_lEpiUnit);
			m_stEpiMax.SetDecimal(1);
			m_stEpiMax.SetDispType(VT_R8);
			if (FAILED(hr = pEpiLamp->get_UpperLimit(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_stEpiMax.Connect(val.pdispVal)))
				_com_raise_error(hr);

			if (FAILED(hr = pEpiLamp->get_IsOn(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_chkEpi.Connect(val.pdispVal)))
				_com_raise_error(hr);

			UpdateEpiLamp();
			UpdateEpiValue();
		}
		nShow = m_Microscope.m_bEpiLampMounted ? SW_SHOW : SW_HIDE;
		GetDlgItem(IDC_STATIC_FRM_EPI)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_EPI_LAMP)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_EPI_LAMP)->ShowWindow(nShow);
		GetDlgItem(IDC_SLIDER_EPI_LAMP)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_EPI_L)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_EPI_R)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_EPI_PRESET)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_EPI_MIN)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_EPI_VALUE)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_EPI_MAX)->ShowWindow(nShow);

		// Fiber(Epi Sutter)
		if (FAILED(hr = pNikonLv->get_EpiShutter(&pEpiShutter)))
			_com_raise_error(hr);
		m_Microscope.m_bFiberMounted = (EnumStatus)((long)pEpiShutter->IsMounted) == StatusTrue ? TRUE : FALSE;

		if (m_Microscope.m_bFiberMounted) {
			if (FAILED(hr = pEpiShutter->get_IsOpened(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_chkFiber.Connect(val.pdispVal)))
				_com_raise_error(hr);

			long lSupportedTimer = 0;
			if (FAILED(hr = pEpiShutter->get_IsSupportedOpenWithTimer(&lSupportedTimer)))
				_com_raise_error(hr);
			if (!lSupportedTimer) {
				GetDlgItem(IDC_BUTTON_FIBER_CMD)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_FIBER)->ShowWindow(SW_HIDE);
			}
		}
		nShow = m_Microscope.m_bFiberMounted ? SW_SHOW : SW_HIDE;
		GetDlgItem(IDC_BUTTON_FIBER)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_FIBER_OPEN)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_FIBER_CMD)->ShowWindow(nShow);
		GetDlgItem(IDC_EDIT_FIBER)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_FIBER_UNIT)->ShowWindow(nShow);

		// Fiber(ND Filter)
		if (FAILED(hr = pNikonLv->get_NDFilter(&pNDFilter)))
			_com_raise_error(hr);
		m_Microscope.m_bNDFilterMounted = (EnumStatus)((long)pNDFilter->IsMounted) == StatusTrue ? TRUE : FALSE;

		if (m_Microscope.m_bNDFilterMounted) {
			if (FAILED(hr = pNDFilter->get_Position(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_cboFiber.Connect(val.pdispVal)))
				_com_raise_error(hr);
			UpdateNDFilter();
		}
		nShow = m_Microscope.m_bNDFilterMounted ? SW_SHOW : SW_HIDE;
		GetDlgItem(IDC_STATIC_FIBER_ND)->ShowWindow(nShow);
		GetDlgItem(IDC_COMBO_FIBER)->ShowWindow(nShow);
		nShow = m_Microscope.m_bFiberMounted || m_Microscope.m_bNDFilterMounted ? SW_SHOW : SW_HIDE;
		GetDlgItem(IDC_STATIC_FRM_FIBER)->ShowWindow(nShow);
		UpdateFiberShutter();

		// Nosepiece
		if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
			_com_raise_error(hr);
		m_Microscope.m_bNosepieceMounted = (EnumStatus)((long)pNosepiece->IsMounted) == StatusTrue ? TRUE : FALSE;

		if (m_Microscope.m_bNosepieceMounted) {
			m_cboNosepiece.SetAccessory(AccessoryNosepiece);
			m_cboNosepiece.CallbackSet((FPropComboCallback)fPropComboCallback);

			if (FAILED(hr = pNosepiece->get_Value(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_cboNosepiece.Connect(val.pdispVal)))
				_com_raise_error(hr);

			BOOL bNosepieceEnable = GetDlgItem(IDC_COMBO_NOSEPIECE)->IsWindowEnabled() && !m_Microscope.IsLVINADController();
			GetDlgItem(IDC_COMBO_NOSEPIECE)->EnableWindow(bNosepieceEnable);
			GetDlgItem(IDC_BUTTON_NOSEPIECE_L)->EnableWindow(bNosepieceEnable);
			GetDlgItem(IDC_BUTTON_NOSEPIECE_R)->EnableWindow(bNosepieceEnable);
			UpdateNosepieceInformation();
			UpdateNosepiece();
		}
		nShow = m_Microscope.m_bNosepieceMounted ? SW_SHOW : SW_HIDE;
		GetDlgItem(IDC_STATIC_FRM_NOSEPIECE)->ShowWindow(nShow);
		GetDlgItem(IDC_COMBO_NOSEPIECE)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_NOSEPIECE_L)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_NOSEPIECE_R)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_NOSEPIECE)->ShowWindow(nShow);

		// Cube
		if (FAILED(hr = pNikonLv->get_FilterBlockCassette(&pFilterBlockCassette)))
			_com_raise_error(hr);
		m_Microscope.m_bCubeMounted = (EnumStatus)((long)pFilterBlockCassette->IsMounted) == StatusTrue ? TRUE : FALSE;

		if (m_Microscope.m_bCubeMounted) {
			m_cboCube.SetAccessory(AccessoryFilterBlockCassette);
			m_cboCube.CallbackSet((FPropComboCallback)fPropComboCallback);

			if (FAILED(hr = pFilterBlockCassette->get_Value(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_cboCube.Connect(val.pdispVal)))
				_com_raise_error(hr);

			GetDlgItem(IDC_BUTTON_CUBE_L)->EnableWindow(GetDlgItem(IDC_COMBO_CUBE)->IsWindowEnabled());
			GetDlgItem(IDC_BUTTON_CUBE_R)->EnableWindow(GetDlgItem(IDC_COMBO_CUBE)->IsWindowEnabled());
			UpdateCubeInformation();
			UpdateCube();
		}
		nShow = m_Microscope.m_bCubeMounted ? SW_SHOW : SW_HIDE;
		GetDlgItem(IDC_STATIC_FRM_CUBE)->ShowWindow(nShow);
		GetDlgItem(IDC_COMBO_CUBE)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_CUBE_L)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_CUBE_R)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_CUBE)->ShowWindow(nShow);

		// DIA
		if (FAILED(hr = pNikonLv->get_DiaLamp(&pDiaLamp)))
			_com_raise_error(hr);
		m_Microscope.m_bDiaLampMounted = (EnumStatus)((long)pDiaLamp->IsMounted) == StatusTrue ? TRUE : FALSE;

		if (m_Microscope.m_bDiaLampMounted) {
			m_lDiaUnit = (long)pDiaLamp->_ValuePerUnit;
			BSTR bstrUnit = NULL;
			if (FAILED(hr = pDiaLamp->get_Unit(&bstrUnit)))
				_com_raise_error(hr);
			m_strDiaUnit = CString(bstrUnit);
			if (bstrUnit != NULL) {
				::SysFreeString(bstrUnit);
				bstrUnit = NULL;
			}
			m_slDia.SetTicFreq(m_lDiaUnit);
			m_slDia.SetUnit(m_lDiaUnit);
			m_slDia.SetDecimal(1);
			m_slDia.SetDispType(VT_R8);
			m_slDia.SetUnitString(m_strDiaUnit);
			m_slDia.SetDispUnit(TRUE);

			if (FAILED(hr = pDiaLamp->get_Value(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_slDia.Connect(val.pdispVal)))
				_com_raise_error(hr);

			if (m_eInterlockTarget == AccessoryNosepiece)
				m_eInterlockTarget = AccessoryDiaLampVoltage;

			m_stDiaMin.SetUnit(m_lDiaUnit);
			m_stDiaMin.SetDecimal(1);
			m_stDiaMin.SetDispType(VT_R8);
			if (FAILED(hr = pDiaLamp->get_LowerLimit(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_stDiaMin.Connect(val.pdispVal)))
				_com_raise_error(hr);

			m_stDiaMax.SetUnit(m_lDiaUnit);
			m_stDiaMax.SetDecimal(1);
			m_stDiaMax.SetDispType(VT_R8);
			if (FAILED(hr = pDiaLamp->get_UpperLimit(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_stDiaMax.Connect(val.pdispVal)))
				_com_raise_error(hr);

			if (FAILED(hr = pDiaLamp->get_IsOn(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_chkDia.Connect(val.pdispVal)))
				_com_raise_error(hr);

			UpdateDiaLamp();
			UpdateDiaValue();
		}
		nShow = m_Microscope.m_bDiaLampMounted ? SW_SHOW : SW_HIDE;
		GetDlgItem(IDC_STATIC_FRM_DIA)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_DIA_LAMP)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_DIA_LAMP)->ShowWindow(nShow);
		GetDlgItem(IDC_SLIDER_DIA_LAMP)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_DIA_L)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_DIA_R)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_DIA_PRESET)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_DIA_MIN)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_DIA_VALUE)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_DIA_MAX)->ShowWindow(nShow);

		// Z Drive
		if (FAILED(hr = pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);
		m_Microscope.m_bZDriveMounted = (EnumStatus)((long)pZDrive->IsMounted) == StatusTrue ? TRUE : FALSE;

		if (m_Microscope.m_bZDriveMounted) {
			m_lZDriveUnit = (long)pZDrive->ValuePerUnit;
			BSTR bstrUnit = NULL;
			if (FAILED(hr = pZDrive->get_Unit(&bstrUnit)))
				_com_raise_error(hr);
			m_strZDriveUnit = CString(bstrUnit);
			if (bstrUnit != NULL) {
				::SysFreeString(bstrUnit);
				bstrUnit = NULL;
			}
			m_slZDrive.SetReverse(TRUE);
			m_slZDrive.SetUnit(m_lZDriveUnit);
			m_slZDrive.SetDecimal(2);
			m_slZDrive.SetDispType(VT_R8);
			m_slZDrive.SetUnitString(m_strZDriveUnit);
			m_slZDrive.SetDispUnit(TRUE);

			if (FAILED(hr = pZDrive->get_Value(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_slZDrive.Connect(val.pdispVal)))
				_com_raise_error(hr);
			m_Microscope.InitZDriveRange();

			UpdateZDriveRange();
			UpdateZDriveInterlockMode();

			m_bSupportedAF = pZDrive->IsSupportedAF ? TRUE : FALSE;
			m_cboZDriveSearch.EnableWindow(m_bSupportedAF ? TRUE : FALSE);

			if (m_bSupportedAF) {
				if (FAILED(hr = pZDrive->get_AF(&val)))
					_com_raise_error(hr);
				if (FAILED(hr = m_chkAF.Connect(val.pdispVal)))
					_com_raise_error(hr);

				if (FAILED(hr = pZDrive->get_IsAfSearch(&val)))
					_com_raise_error(hr);
				if (FAILED(hr = m_btnSearch.Connect(val.pdispVal)))
					_com_raise_error(hr);
			}
			UpdateZDriveAF();

			if (FAILED(hr = pZDrive->get_IsZEscape(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_chkZEscape.Connect(val.pdispVal)))
				_com_raise_error(hr);
			UpdateZEscape();
			UpdateZDrivePos();
			UpdateZDriveEnable();
		}
		else {
			GetMenu()->EnableMenuItem(ID_SETTING_AF_OFFSET, MFS_DISABLED);
			GetMenu()->EnableMenuItem(ID_SETTING_FOCUS, MFS_DISABLED);
			GetMenu()->EnableMenuItem(ID_SETTING_SOFTLIMIT, MFS_DISABLED);
		}
		nShow = m_Microscope.m_bZDriveMounted ? SW_SHOW : SW_HIDE;
		GetDlgItem(IDC_STATIC_FRM_ZDRIVE)->ShowWindow(nShow);
		GetDlgItem(IDC_SLIDER_ZDRIVE)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_ZDRIVE_MIN)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_ZDRIVE_MAX)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_ZDRIVE_VALUE)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_ZDRIVE_U)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_ZDRIVE_UU)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_ZDRIVE_D)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_ZDRIVE_DD)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_ZDRIVE_STEP)->ShowWindow(nShow);
		GetDlgItem(IDC_COMBO_ZDRIVE_STEP)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_ZDRIVE_RANGE)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_ZDRIVE_CHANGE)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_ZDRIVE_ALF)->ShowWindow(nShow);
		GetDlgItem(IDC_CHECK_ZDRIVE_ALF)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_AF)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_SEARCH)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_STOPSEARCH)->ShowWindow(nShow);
		GetDlgItem(IDC_COMBO_ZDRIVE_SEARCH)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_ZDRIVE_FOCUS1)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_ZDRIVE_FOCUS2)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_ZDRIVE_CONTINUOUS)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_ZDRIVE_MIN)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_ZDRIVE_MAX)->ShowWindow(nShow);
		GetDlgItem(IDC_COMBO_ZDRIVE_SPEED)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_ZDRIVE_ESCAPE)->ShowWindow(nShow);

		// Aperture
		if (FAILED(hr = pNikonLv->get_EpiApertureStop(&pEpiApertureStop)))
			_com_raise_error(hr);
		m_Microscope.m_bApertureMounted = (EnumStatus)((long)pEpiApertureStop->IsMounted) == StatusTrue ? TRUE : FALSE;

		if (m_Microscope.m_bApertureMounted) {
			m_lApertureUnit = (long)pEpiApertureStop->ValuePerUnit;
			BSTR bstrUnit = NULL;
			if (FAILED(hr = pEpiApertureStop->get_Unit(&bstrUnit)))
				_com_raise_error(hr);
			m_strApertureUnit = CString(bstrUnit);
			if (bstrUnit != NULL) {
				::SysFreeString(bstrUnit);
				bstrUnit = NULL;
			}
			m_slAperture.SetTicFreq(m_lApertureUnit);
			m_slAperture.SetUnit(m_lApertureUnit);
			m_slAperture.SetDecimal(1);
			m_slAperture.SetDispType(VT_R8);
			m_slAperture.SetUnitString(m_strApertureUnit);
			m_slAperture.SetDispUnit(TRUE);

			if (FAILED(hr = pEpiApertureStop->get_Value(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_slAperture.Connect(val.pdispVal)))
				_com_raise_error(hr);

			if (m_eInterlockTarget == AccessoryNosepiece)
				m_eInterlockTarget = AccessoryEpiApertureStop;

			m_stApertureMin.SetUnit(m_lApertureUnit);
			m_stApertureMin.SetDecimal(1);
			m_stApertureMin.SetDispType(VT_R8);
			if (FAILED(hr = pEpiApertureStop->get_LowerLimit(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_stApertureMin.Connect(val.pdispVal)))
				_com_raise_error(hr);

			m_stApertureMax.SetUnit(m_lApertureUnit);
			m_stApertureMax.SetDecimal(1);
			m_stApertureMax.SetDispType(VT_R8);
			if (FAILED(hr = pEpiApertureStop->get_UpperLimit(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_stApertureMax.Connect(val.pdispVal)))
				_com_raise_error(hr);

			UpdateApertureEnable();
			UpdateApertureValue();
		}
		nShow = m_Microscope.m_bApertureMounted ? SW_SHOW : SW_HIDE;
		GetDlgItem(IDC_STATIC_FRM_APERTURE)->ShowWindow(nShow);
		GetDlgItem(IDC_SLIDER_APERTURE)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_APERTURE_L)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_APERTURE_R)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_APERTURE_PRESET)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_APERTURE_MIN)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_APERTURE_VALUE)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_APERTURE_MAX)->ShowWindow(nShow);

		// DIC
		if (FAILED(hr = pNikonLv->get_DICPrism(&pDicPrism)))
			_com_raise_error(hr);
		m_Microscope.m_bDicMounted = (EnumStatus)((long)pDicPrism->IsMounted) == StatusTrue ? TRUE : FALSE;

		if (m_Microscope.m_bDicMounted) {
			m_lDICUnit = (long)pDicPrism->ValuePerUnit;
			BSTR bstrUnit = NULL;
			if (FAILED(hr = pDicPrism->get_Unit(&bstrUnit)))
				_com_raise_error(hr);
			m_strDICUnit = CString(bstrUnit);
			if (bstrUnit != NULL) {
				::SysFreeString(bstrUnit);
				bstrUnit = NULL;
			}
			m_slDIC.SetTicFreq(m_lDICUnit / 4);
			m_slDIC.SetUnit(1);
			m_slDIC.SetDispType(VT_I4);

			if (FAILED(hr = pDicPrism->get_DICPosition(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_cboDIC.Connect(val.pdispVal)))
				_com_raise_error(hr);

			if (FAILED(hr = pDicPrism->get_Value(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = m_slDIC.Connect(val.pdispVal)))
				_com_raise_error(hr);

			if (m_eInterlockTarget == AccessoryNosepiece)
				m_eInterlockTarget = AccessoryDICPrismShift;

			UpdateDICEnable();
			UpdateDICValue();
			UpdateDICCalibration();
		}
		else {
			GetMenu()->EnableMenuItem(ID_DIC_CALIBRATION, MFS_DISABLED);
			GetMenu()->EnableMenuItem(ID_DIC_CLEAR, MFS_DISABLED);
		}
		nShow = m_Microscope.m_bDicMounted ? SW_SHOW : SW_HIDE;
		GetDlgItem(IDC_STATIC_FRM_DIC)->ShowWindow(nShow);
		GetDlgItem(IDC_COMBO_DIC)->ShowWindow(nShow);
		GetDlgItem(IDC_SLIDER_DIC)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_DIC_L)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_DIC_R)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_DIC_VALUE)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_DIC1)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_DIC2)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_DIC3)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_DIC4)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_DIC5)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_DIC_CALIBRATION)->ShowWindow(nShow);
		GetDlgItem(IDC_BUTTON_DIC_CLEAR)->ShowWindow(nShow);
		GetDlgItem(IDC_STATIC_LED)->ShowWindow(nShow);

		// Interlock
		BOOL bEnable = !m_Microscope.IsAFController() && !m_Microscope.IsLVNController();
		GetDlgItem(IDC_STATIC_FRM_INTERLOCK)->EnableWindow(bEnable);
		GetDlgItem(IDC_STATIC_INTERLOCK_MODE)->EnableWindow(bEnable);
		GetDlgItem(IDC_COMBO_INTERLOCK)->EnableWindow(bEnable);
		UpdateInterlock();

		// Menu
		GetMenu()->EnableMenuItem(ID_SETTING_AF_OFFSET, m_bSupportedAF ? MFS_ENABLED : MFS_DISABLED);
		GetMenu()->EnableMenuItem(ID_STATUS_NOSEPIESE, m_Microscope.m_bNosepieceMounted ? MFS_ENABLED : MFS_DISABLED);
		GetMenu()->EnableMenuItem(ID_STATUS_CUBE, m_Microscope.m_bCubeMounted ? MFS_ENABLED : MFS_DISABLED);
		GetMenu()->EnableMenuItem(ID_STATUS_ZDRIVE, m_Microscope.m_bZDriveMounted ? MFS_ENABLED : MFS_DISABLED);
		GetMenu()->EnableMenuItem(ID_STATUS_DIC, m_Microscope.m_bDicMounted ? MFS_ENABLED : MFS_DISABLED);
		GetMenu()->EnableMenuItem(ID_SETTING_INTERLOCK, m_Microscope.m_bNosepieceMounted && !m_Microscope.IsAFController() && !m_Microscope.IsLVNController() ? MFS_ENABLED : MFS_DISABLED);
		GetMenu()->EnableMenuItem(ID_SETTING_OFFSET, m_Microscope.m_bNosepieceMounted && (m_Microscope.m_bEpiLampMounted || m_Microscope.m_bDiaLampMounted || m_Microscope.m_bFiberMounted || m_Microscope.m_bApertureMounted) && !m_Microscope.IsLVNController() ? MFS_ENABLED : MFS_DISABLED);
		GetMenu()->EnableMenuItem(ID_SETTING_CONTROL, m_Microscope.IsLV150NAController() || m_Microscope.IsLVINADController() ? MFS_DISABLED : MFS_ENABLED);
		GetMenu()->EnableMenuItem(ID_STATUS_INTERLOCK, !m_Microscope.IsAFController() && !m_Microscope.IsLV150NAController() && !m_Microscope.IsLVINADController() ? MFS_ENABLED : MFS_DISABLED);
		GetMenu()->EnableMenuItem(ID_STATUS_USER_OBSERVATION, m_Microscope.m_bNosepieceMounted && !m_Microscope.IsAFController() && !m_Microscope.IsLVNController() ? MFS_ENABLED : MFS_DISABLED);
		GetMenu()->EnableMenuItem(ID_STATUS_EPIDIA, (m_Microscope.m_bEpiLampMounted || m_Microscope.m_bDiaLampMounted || m_Microscope.m_bFiberMounted || m_Microscope.m_bApertureMounted) && !m_Microscope.IsLV150NAController() && !m_Microscope.IsLVINADController() ? MFS_ENABLED : MFS_DISABLED);
		GetMenu()->EnableMenuItem(ID_STATUS_SYSTEM, m_Microscope.IsLV150NAController() || m_Microscope.IsLVINADController() ? MFS_DISABLED : MFS_ENABLED);

		// Release
		// Observation
		if (pPresets)
			pPresets.Release();
		// EPI
		if (pEpiLamp)
			pEpiLamp.Release();
		// Fiber
		if (pEpiShutter)
			pEpiShutter.Release();
		if (pNDFilter)
			pNDFilter.Release();
		// Nosepiece
		if (pNosepiece)
			pNosepiece.Release();
		// Cube
		if (pFilterBlockCassette)
			pFilterBlockCassette.Release();
		// DIA
		if (pDiaLamp)
			pDiaLamp.Release();
		// Z Drive
		if (pZDrive)
			pZDrive.Release();
		// Aperture
		if (pEpiApertureStop)
			pEpiApertureStop.Release();
		// DIC
		if (pDicPrism)
			pDicPrism.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}

	return hr;
}

HRESULT CLvSampleDlg::DisconnectMipParam()
{
	// Observation
	m_cboObservation.Disconnect();
	// EPI
	m_chkEpi.Disconnect();
	m_slEpi.Disconnect();
	m_stEpiMin.Disconnect();
	m_stEpiMax.Disconnect();
	// Fiber
	m_chkFiber.Disconnect();
	m_cboFiber.Disconnect();
	// Nosepiece
	m_cboNosepiece.Disconnect();
	// Cube
	m_cboCube.Disconnect();
	// DIA
	m_chkDia.Disconnect();
	m_slDia.Disconnect();
	m_stDiaMin.Disconnect();
	m_stDiaMax.Disconnect();
	// Z Drive
	m_slZDrive.Disconnect();
	m_chkZEscape.Disconnect();
	m_chkAF.Disconnect();
	m_btnSearch.Disconnect();
	// Aperture
	m_slAperture.Disconnect();
	m_stApertureMin.Disconnect();
	m_stApertureMax.Disconnect();
	// DIC
	m_cboDIC.Disconnect();
	m_slDIC.Disconnect();

	return S_OK;
}

void CLvSampleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	BOOL bSned = FALSE;
	BOOL bUpdate = FALSE;

	switch (nSBCode) {
	case SB_LEFT:
	case SB_RIGHT:
	case SB_LINELEFT:
	case SB_LINERIGHT:
	case SB_PAGELEFT:
	case SB_PAGERIGHT:
	case SB_THUMBPOSITION:
		bSned = TRUE;
		// not break
	case SB_THUMBTRACK:
		bUpdate = TRUE;
		break;
	case SB_ENDSCROLL:
		break;
	default:
		break;
	}
	if (bSned) {
		CWaitCursor w;
		if (pScrollBar->m_hWnd == m_slEpi.m_hWnd)
			m_slEpi.PutValue();
		else if (pScrollBar->m_hWnd == m_slDia.m_hWnd)
			m_slDia.PutValue();
		else if (pScrollBar->m_hWnd == m_slAperture.m_hWnd)
			m_slAperture.PutValue();
		else if (pScrollBar->m_hWnd == m_slDIC.m_hWnd)
			m_slDIC.PutValue();
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
void CLvSampleDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	BOOL bSned = FALSE;
	BOOL bUpdate = FALSE;

	switch (nSBCode) {
	case SB_TOP:
	case SB_BOTTOM:
	case SB_LINEUP:
	case SB_LINEDOWN:
	case SB_PAGEUP:
	case SB_PAGEDOWN:
	case SB_THUMBPOSITION:
		bSned = TRUE;
		// not break
	case SB_THUMBTRACK:
		bUpdate = TRUE;
		break;
	case SB_ENDSCROLL:
		break;
	default:
		break;
	}
	if (bSned) {
		if (pScrollBar->m_hWnd == m_slZDrive.m_hWnd) {
			CWaitCursor w;
			m_Microscope.ExecZDriveSetSpeed(Speed1);
			m_slZDrive.PutValue();
		}
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CLvSampleDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent) {
	case EPI_SHUTTER_TIMER:
		KillTimer(nIDEvent);
		GetDlgItem(IDC_BUTTON_FIBER)->EnableWindow(TRUE);
		UpdateFiberShutter();
		break;
	default:
		break;
	}
}

LRESULT CLvSampleDlg::OnParamEvent(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)	{
	case EMIPPAR_VALUECHANGED:
		ValueChanged(wParam);
		break;
	case EMIPPAR_INFOCHANGED:
		InfoChanged(wParam);
		ValueChanged(wParam);
		break;
	}
	return 0L;
}
void CLvSampleDlg::ValueChanged(WPARAM wParam)
{
	switch (wParam)	{
	// Observation
	case IDC_COMBO_OBSERVATION_MODE:
		if (m_Microscope.IsAFController()) {
			UpdateObservationMode();
			UpdateDICValue();
		}
		else {
			UpdateObservationMode();
			UpdateApertureEnable();
			UpdateDICEnable();
			UpdateDICValue();
		}
		break;
	// EPI
	case IDC_BUTTON_EPI_LAMP:
		UpdateEpiLamp();
		break;
	case IDC_SLIDER_EPI_LAMP:
		UpdateEpiValue();
		break;
	// Fiber
	case IDC_BUTTON_FIBER:
		UpdateFiberShutter();
		break;
	case IDC_COMBO_FIBER:
		UpdateNDFilter();
		break;
	// Nosepiece
	case IDC_COMBO_NOSEPIECE:
		UpdateNosepiece();
		break;
	// Cube
	case IDC_COMBO_CUBE:
		UpdateCube();
		UpdateApertureEnable();
		break;
	// DIA
	case IDC_BUTTON_DIA_LAMP:
		UpdateDiaLamp();
		break;
	case IDC_SLIDER_DIA_LAMP:
		UpdateDiaValue();
		break;
	// Z Drive
	case IDC_SLIDER_ZDRIVE:
		UpdateZDrivePos();
		break;
	case IDC_BUTTON_ZDRIVE_ESCAPE:
		UpdateZEscape();
		break;
	case IDC_BUTTON_AF:
	case IDC_BUTTON_SEARCH:
		UpdateZDriveAF();
		break;
	// Aperture
	case IDC_SLIDER_APERTURE:
		UpdateApertureValue();
		break;
	// DIC
	case IDC_SLIDER_DIC:
		UpdateDICEnable();
		UpdateDICValue();
		break;
	case IDC_COMBO_DIC:
		UpdateDICPosition();
		break;
	default:
		break;
	}
}
void CLvSampleDlg::InfoChanged(WPARAM wParam)
{
	switch (wParam)	{
	// Observation
	case IDC_COMBO_OBSERVATION_MODE:
		UpdateObservationInformation();
		break;
	case IDC_COMBO_NOSEPIECE:
		UpdateNosepieceInformation();
		break;
	case IDC_COMBO_CUBE:
		UpdateCubeInformation();
		break;
	default:
		break;
	}
}

// Observation
void CLvSampleDlg::UpdateObservationInformation()
{
	if (m_Microscope.IsLV150NAController() || m_Microscope.IsLVINADController()) {
		GetDlgItem(IDC_STATIC_OBSERVATION_MODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_OBSERVATION_MODE)->EnableWindow(FALSE);
		return;
	}
	else {
		GetDlgItem(IDC_STATIC_OBSERVATION_MODE)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_OBSERVATION_MODE)->EnableWindow(TRUE);
	}

	m_cboObservation.ResetContent();
	HRESULT hr = S_OK;

	try {
		INikonLv* pNikonLv = m_Microscope.GetInterface();
		int i;
		CString str;
		VARIANT val;
		IMipParameterPtr spiParam;
		CComPtr<IPresets> pPresets = NULL;

		if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
			_com_raise_error(hr);

		if (m_Microscope.IsAFController()) {
			if (FAILED(hr = pPresets->get_AfObservationMode(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);

			for (i = 1; i <= (long)spiParam->EnumSet->Count; i++) {
				EnumAfObservationMode eMode = (EnumAfObservationMode)spiParam->EnumSet->GetItem(i).RawValue.lVal;
				if (eMode != AfObservationModeUnknown) {
					str.Format(_T("%s"), spiParam->EnumSet->GetItem(i).DisplayString);
					m_cboObservation.SetItemData(m_cboObservation.AddString(str), eMode);
				}
			}
		}
		else {
			if (FAILED(hr = pPresets->get_ObservationMode(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);

			for (i = 1; i <= (long)spiParam->EnumSet->Count; i++) {
				EnumObservationMode eMode = (EnumObservationMode)spiParam->EnumSet->GetItem(i).RawValue.lVal;
				if (eMode != ObservationModeNone && eMode != ObservationModeUnknown) {
					BOOL bAdd = TRUE;
					if (eMode >= ObservationMode1 && eMode <= ObservationMode6) {
						EnumOptionalObservationMode eOptional;
						EnumStatus eInterlock;
						pPresets->ReadPresetModes(eMode, &eOptional, &eInterlock);
						if (eOptional == OptionalObservationModeUnknown || eOptional == OptionalObservationModeNone)
							bAdd = FALSE;
					}
					if (bAdd) {
						str.Format(_T("%s"), spiParam->EnumSet->GetItem(i).DisplayString);
						m_cboObservation.SetItemData(m_cboObservation.AddString(str), eMode);
					}
				}
			}
		}

		if (spiParam)
			spiParam.Release();
		if (pPresets)
			pPresets.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}
void CLvSampleDlg::UpdateObservationMode()
{
	if (m_Microscope.IsLV150NAController() || m_Microscope.IsLVINADController())
		return;

	m_bDarkField = FALSE;
	m_bObservationDIC = FALSE;

	HRESULT hr = S_OK;
	try {
		INikonLv* pNikonLv = m_Microscope.GetInterface();

		int i;
		CComPtr<IPresets> pPresets = NULL;

		if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
			_com_raise_error(hr);

		if (m_Microscope.IsAFController()) {
			EnumAfObservationMode eMode;
			if (FAILED(hr = pPresets->get__AfObservationMode(&eMode)))
				_com_raise_error(hr);

			// Check Observation mode
			for (i = 0; i < m_cboObservation.GetCount(); i++) {
				if (m_cboObservation.GetItemData(i) == eMode) {
					if (m_cboObservation.GetCurSel() != i)
						m_cboObservation.SetCurSel(i);
					break;
				}
			}
			// Check Cube
			for (i = 0; i < m_cboCube.GetCount(); i++) {
				if (m_cboCube.GetItemData(i) == eMode) {
					if (m_cboCube.GetCurSel() != i)
						m_cboCube.SetCurSel(i);
					break;
				}
			}
		}
		else {
			EnumObservationMode eMode;
			if (FAILED(hr = pPresets->get__ObservationMode(&eMode)))
				_com_raise_error(hr);

			// Check Observation mode
			for (i = 0; i < m_cboObservation.GetCount(); i++) {
				if (m_cboObservation.GetItemData(i) == eMode) {
					if (eMode == DarkField)
						m_bDarkField = TRUE;
					else if (eMode == DIC)
						m_bObservationDIC = TRUE;
					else if (eMode >= ObservationMode1 && eMode <= ObservationMode6) {
						EnumOptionalObservationMode eOptional;
						EnumStatus eInterlock;
						long lNosepiece;
						if (FAILED(hr = pPresets->ReadPresetModesEx(eMode, &eOptional, &eInterlock, &lNosepiece)))
							_com_raise_error(hr);
						if (eOptional == DICEpiDia || eOptional == DICEpi)
							m_bObservationDIC = TRUE;
					}
					if (m_cboObservation.GetCurSel() != i)
						m_cboObservation.SetCurSel(i);
					break;
				}
			}
		}

		if (pPresets)
			pPresets.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}
// EPI
void CLvSampleDlg::UpdateEpiLamp()
{
	if (m_Microscope.m_bEpiLampMounted) {
		VARIANT val;
		if (m_chkEpi.GetValue(&val)) {
			if (val.vt == VT_I4) {
				BOOL bIsOn = (EnumStatus)val.lVal == StatusTrue ? TRUE : FALSE;
				m_chkEpi.SetCheck(bIsOn ? BST_CHECKED : BST_UNCHECKED);
				GetDlgItem(IDC_STATIC_EPI_LAMP)->SetWindowText(bIsOn ? _T("On") : _T("Off"));
				GetDlgItem(IDC_SLIDER_EPI_LAMP)->EnableWindow(bIsOn);
				GetDlgItem(IDC_BUTTON_EPI_PRESET)->EnableWindow(bIsOn);
				GetDlgItem(IDC_BUTTON_EPI_L)->EnableWindow(bIsOn);
				GetDlgItem(IDC_BUTTON_EPI_R)->EnableWindow(bIsOn);
			}
		}
	}
}
void CLvSampleDlg::UpdateEpiValue()
{
	if (m_Microscope.m_bEpiLampMounted) {
		VARIANT val;
		if (m_slEpi.GetValue(&val)) {
			if (val.vt == VT_I4) {
				CString str;
				str.Format(_T("%.1lf %s"), (double)val.lVal / (double)m_lEpiUnit, m_strEpiUnit);
				GetDlgItem(IDC_STATIC_EPI_VALUE)->SetWindowText(str);
			}
		}
	}
}
// Fiber
void CLvSampleDlg::UpdateFiberShutter()
{
	if (m_Microscope.m_bFiberMounted) {
		VARIANT val;
		if (m_chkFiber.GetValue(&val)) {
			if (val.vt == VT_I4) {
				BOOL bIsOpen = (EnumStatus)val.lVal == StatusTrue ? TRUE : FALSE;
				m_chkFiber.SetCheck(bIsOpen ? BST_CHECKED : BST_UNCHECKED);
				GetDlgItem(IDC_STATIC_FIBER_OPEN)->SetWindowText(bIsOpen ? _T("Open") : _T("Close"));
				GetDlgItem(IDC_BUTTON_FIBER_CMD)->EnableWindow(!bIsOpen);
				GetDlgItem(IDC_EDIT_FIBER)->EnableWindow(!bIsOpen);
				GetDlgItem(IDC_STATIC_FIBER_UNIT)->EnableWindow(!bIsOpen);
				GetDlgItem(IDC_STATIC_FIBER_ND)->EnableWindow(bIsOpen);
				GetDlgItem(IDC_COMBO_FIBER)->EnableWindow(bIsOpen);
			}
		}
	}
}
void CLvSampleDlg::UpdateNDFilter()
{
	if (m_Microscope.m_bFiberMounted && m_Microscope.m_bNDFilterMounted) {
		VARIANT val;
		if (m_cboFiber.GetValue(&val)) {
			if (val.vt == VT_I4) {
				for (int i = 0; i < m_cboFiber.GetCount(); i++) {
					if (m_cboFiber.GetItemData(i) == val.lVal) {
						if (m_cboFiber.GetCurSel() != i)
							m_cboFiber.SetCurSel(i);
						break;
					}
				}
			}
		}
	}
}
// Nosepiece
void CLvSampleDlg::UpdateNosepieceInformation()
{
	m_cboNosepiece.ResetContent();
	if (m_Microscope.m_bNosepieceMounted) {
		HRESULT hr = S_OK;
		try {
			INikonLv* pNikonLv = m_Microscope.GetInterface();
			CComPtr<INosepiece> pNosepiece = NULL;
			if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
				_com_raise_error(hr);

			for (int i = (long)pNosepiece->LowerLimit; i <= (long)pNosepiece->UpperLimit; i++) {
				CString str;
				if (pNosepiece->Objectives->GetItem(i)->Code > 0) {
					double dMagnification = pNosepiece->Objectives->GetItem(i)->Magnification;
					if (dMagnification > 0.00001) {
						int nMagnification = (int)dMagnification;
						if (dMagnification - nMagnification < 0.1)
							str.Format(_T("%-20s %dx"), pNosepiece->Objectives->GetItem(i)->Name.GetBSTR(), nMagnification);
						else
							str.Format(_T("%-20s %.1fx"), pNosepiece->Objectives->GetItem(i)->Name.GetBSTR(), dMagnification);
					}
					else
						str.Format(_T("%-20s -----"), pNosepiece->Objectives->GetItem(i)->Name.GetBSTR());
					m_cboNosepiece.SetItemData(m_cboNosepiece.AddString(str), i);
				}
				else {
					str.Format(_T("%-20s"), pNosepiece->Objectives->GetItem(i)->Name.GetBSTR());
					m_cboNosepiece.SetItemData(m_cboNosepiece.AddString(str), -1);
				}
			}

			if (pNosepiece)
				pNosepiece.Release();
		}
		catch (_com_error/* &e*/) {
			AfxMessageBox(_T("Microscope error!"));
		}
	}
}
void CLvSampleDlg::UpdateNosepiece()
{
	if (m_Microscope.m_bNosepieceMounted) {
		VARIANT val;
		long lVal;
		if (m_cboNosepiece.GetValue(&val)) {
			if (val.vt == VT_I4) {
				for (int i = 0; i < m_cboNosepiece.GetCount(); i++) {
					lVal = (long)m_cboNosepiece.GetItemData(i);
					if (lVal == val.lVal) {
						if (m_cboNosepiece.GetCurSel() != i)
							m_cboNosepiece.SetCurSel(i);
						break;
					}
					else if (lVal < 1 && val.lVal == i + 1) {
						if (m_cboNosepiece.GetCurSel() != i)
							m_cboNosepiece.SetCurSel(i);
						break;
					}
				}
			}
		}

		m_bNosepieceDIC = FALSE;
		int nSel = m_cboNosepiece.GetCurSel();
		if (nSel > -1) {
			CString str;
			if (lVal > 0)
				str.Format(_T("%d"), lVal);
			else
				str.Format(_T("%d"), nSel + 1);
			GetDlgItem(IDC_STATIC_NOSEPIECE)->SetWindowText(str);

			if (m_Microscope.m_bDicMounted) {
				HRESULT hr = S_OK;
				try {
					INikonLv* pNikonLv = m_Microscope.GetInterface();
					CComPtr<INosepiece> pNosepiece = NULL;
					if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
						_com_raise_error(hr);

					if (lVal >= (long)pNosepiece->LowerLimit && lVal <= (long)pNosepiece->UpperLimit) {
						if (pNosepiece->Objectives->GetItem(lVal)->Code > 0) {
							if (m_Microscope.m_bIsSimulation || pNosepiece->Objectives->GetItem(lVal)->DICPrismPosition > 0)
								m_bNosepieceDIC = TRUE;
						}
					}

					if (pNosepiece)
						pNosepiece.Release();
				}
				catch (_com_error/* &e*/) {
					AfxMessageBox(_T("Microscope error!"));
				}
			}
		}
		else
			GetDlgItem(IDC_STATIC_NOSEPIECE)->SetWindowText(_T(""));
	}
}
void CLvSampleDlg::MoveNosepiece(int nType)
{
	if (m_Microscope.m_bNosepieceMounted) {
		HRESULT hr = S_OK;
		try {
			INikonLv* pNikonLv = m_Microscope.GetInterface();
			CComPtr<INosepiece> pNosepiece = NULL;
			if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
				_com_raise_error(hr);

			CWaitCursor w;
			if (FAILED(hr = pNosepiece->EnableInterlock()))
				_com_raise_error(hr);
			if (FAILED(hr = m_Microscope.ExecCommand(AccessoryNosepiece, nType)))
				_com_raise_error(hr);

			if (pNosepiece)
				pNosepiece.Release();
		}
		catch (_com_error/* &e*/) {
			if (!RetryNosepieceCommand(nType))
				UpdateNosepiece();
//			AfxMessageBox(_T("Microscope error!"));
		}
	}
}
BOOL CLvSampleDlg::RetryNosepieceCommand(int nType)
{
	BOOL bRetry = FALSE;
	HRESULT hr = S_OK;

	try {
		INikonLv* pNikonLv = m_Microscope.GetInterface();
		CComPtr<INosepiece> pNosepiece = NULL;
		if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
			_com_raise_error(hr);

		if (pNosepiece->IsNosepieceControlError) {
			if (m_Microscope.m_bConfirm) {
				if (AfxMessageBox(IDS_MESSAGE_COMPULSORILY_ROTATED_SAFETY_LOCK, MB_YESNO) == IDYES) {
					CWaitCursor w;
					pNosepiece->Put_NosepieceControl(NosepieceControlNone);
					if (FAILED(hr = pNosepiece->EnableInterlock()))
						_com_raise_error(hr);

					switch (nType) {
					case 0:
					case 1:
						if (FAILED(hr = m_Microscope.ExecCommand(AccessoryNosepiece, nType)))
							_com_raise_error(hr);
						bRetry = TRUE;
						break;
					case 2:
						{
							int nSel = m_cboNosepiece.GetCurSel();
							if (nSel >= 0) {
								long lValue = (long)m_cboNosepiece.GetItemData(nSel);
								if (lValue >= (long)pNosepiece->LowerLimit && lValue <= (long)pNosepiece->UpperLimit) {
									if (FAILED(hr = m_Microscope.ExecSelectCommand(AccessoryNosepiece, lValue)))
										_com_raise_error(hr);
									bRetry = TRUE;
								}
							}
						}
						break;
					default:
						break;
					}

					pNosepiece->Put_NosepieceControl(ZLimit);
				}
			}
			else {
				AfxMessageBox(IDS_MESSAGE_SAFETY_LOCK);
			}
		}

		if (pNosepiece)
			pNosepiece.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}

	return bRetry;
}
// Cube
void CLvSampleDlg::UpdateCubeInformation()
{
	if (!m_Microscope.IsAFController() && m_Microscope.m_bCubeMounted) {
		m_cboCube.ResetContent();
		HRESULT hr = S_OK;
		try {
			INikonLv* pNikonLv = m_Microscope.GetInterface();
			CComPtr<IFilterBlockCassette> pFilterBlockCassette = NULL;
			if (FAILED(hr = pNikonLv->get_FilterBlockCassette(&pFilterBlockCassette)))
				_com_raise_error(hr);

			for (int i = (long)pFilterBlockCassette->LowerLimit; i <= (long)pFilterBlockCassette->UpperLimit; i++) {
				CString str;
				str.Format(_T("%s"), pFilterBlockCassette->FilterBlocks->GetItem(i)->Name.GetBSTR());
				if (pFilterBlockCassette->FilterBlocks->GetItem(i)->Code > 0)
					m_cboCube.SetItemData(m_cboCube.AddString(str), i);
				else
					m_cboCube.SetItemData(m_cboCube.AddString(str), -1);
			}

			if (pFilterBlockCassette)
				pFilterBlockCassette.Release();
		}
		catch (_com_error/* &e*/) {
			AfxMessageBox(_T("Microscope error!"));
		}
	}
}
void CLvSampleDlg::UpdateCube()
{
	if (m_Microscope.m_bCubeMounted) {
		VARIANT val;
		long lVal;
		if (m_cboCube.GetValue(&val)) {
			if (val.vt == VT_I4) {
				for (int i = 0; i < m_cboCube.GetCount(); i++) {
					lVal = (long)m_cboCube.GetItemData(i);
					if (lVal == val.lVal) {
						if (m_cboCube.GetCurSel() != i)
							m_cboCube.SetCurSel(i);
						break;
					}
					else if (lVal < 1 && val.lVal == i + 1) {
						if (m_cboCube.GetCurSel() != i)
							m_cboCube.SetCurSel(i);
						break;
					}
				}
			}
		}

		m_bDarkFieldCube = FALSE;
		int nSel = m_cboCube.GetCurSel();
		if (nSel > -1) {
			CString str;
			if (lVal > 0)
				str.Format(_T("%d"), lVal);
			else
				str.Format(_T("%d"), nSel + 1);
			str.Format(_T("%d"), val.lVal);
			GetDlgItem(IDC_STATIC_CUBE)->SetWindowText(str);

			if (val.lVal == 2)
				m_bDarkFieldCube = TRUE;
		}
		else
			GetDlgItem(IDC_STATIC_CUBE)->SetWindowText(_T(""));
	}
}
void CLvSampleDlg::MoveCube(int nType)
{
	if (m_Microscope.m_bCubeMounted) {
		HRESULT hr = S_OK;
		try {
			INikonLv* pNikonLv = m_Microscope.GetInterface();
			CComPtr<IFilterBlockCassette> pFilterBlockCassette = NULL;
			if (FAILED(hr = pNikonLv->get_FilterBlockCassette(&pFilterBlockCassette)))
				_com_raise_error(hr);

			CWaitCursor w;
			if (FAILED(hr = pFilterBlockCassette->EnableInterlock()))
				_com_raise_error(hr);
			if (FAILED(hr = m_Microscope.ExecCommand(AccessoryFilterBlockCassette, nType)))
				_com_raise_error(hr);

			if (pFilterBlockCassette)
				pFilterBlockCassette.Release();
		}
		catch (_com_error/* &e*/) {
			AfxMessageBox(_T("Microscope error!"));
		}
	}
}
// DIA
void CLvSampleDlg::UpdateDiaLamp()
{
	if (m_Microscope.m_bDiaLampMounted) {
		VARIANT val;
		if (m_chkDia.GetValue(&val)) {
			if (val.vt == VT_I4) {
				BOOL bIsOn = (EnumStatus)val.lVal == StatusTrue ? TRUE : FALSE;
				m_chkDia.SetCheck(bIsOn ? BST_CHECKED : BST_UNCHECKED);
				GetDlgItem(IDC_STATIC_DIA_LAMP)->SetWindowText(bIsOn ? _T("On") : _T("Off"));
				GetDlgItem(IDC_SLIDER_DIA_LAMP)->EnableWindow(bIsOn);
				GetDlgItem(IDC_BUTTON_DIA_PRESET)->EnableWindow(bIsOn);
				GetDlgItem(IDC_BUTTON_DIA_L)->EnableWindow(bIsOn);
				GetDlgItem(IDC_BUTTON_DIA_R)->EnableWindow(bIsOn);
			}
		}
	}
}
void CLvSampleDlg::UpdateDiaValue()
{
	if (m_Microscope.m_bDiaLampMounted) {
		VARIANT val;
		if (m_slDia.GetValue(&val)) {
			if (val.vt == VT_I4) {
				CString str;
				str.Format(_T("%.1lf %s"), (double)val.lVal / (double)m_lDiaUnit, m_strDiaUnit);
				GetDlgItem(IDC_STATIC_DIA_VALUE)->SetWindowText(str);
			}
		}
	}
}
// Z Drive
void CLvSampleDlg::UpdateZDriveEnable()
{
	BOOL bEnable = m_Microscope.m_bZDriveMounted && !m_Microscope.IsAFController();
	GetDlgItem(IDC_BUTTON_ZDRIVE_CHANGE)->EnableWindow(!m_bIsEscape && bEnable);
	GetDlgItem(IDC_STATIC_ZDRIVE_RANGE)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_ZDRIVE_ALF)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_ZDRIVE_ALF)->EnableWindow(bEnable);
}
void CLvSampleDlg::UpdateZDriveRange()
{
	if (m_Microscope.m_bZDriveMounted) {
		m_slZDrive.SetUserRange(m_Microscope.GetZDriveMin(), m_Microscope.GetZDriveMax(), TRUE);
		m_slZDrive.SetTicFreq((m_Microscope.GetZDriveMax() - m_Microscope.GetZDriveMin()) / 10);
		CString strMin, strMax;
		strMin.Format(_T("%d"), m_slZDrive.GetUserRangeMin() / m_lZDriveUnit);
		GetDlgItem(IDC_STATIC_ZDRIVE_MIN)->SetWindowText(strMin);
		strMax.Format(_T("%d"), m_slZDrive.GetUserRangeMax() / m_lZDriveUnit);
		GetDlgItem(IDC_STATIC_ZDRIVE_MAX)->SetWindowText(strMax);
	}
}
void CLvSampleDlg::UpdateZDrivePos()
{
	if (m_Microscope.m_bZDriveMounted) {
		VARIANT val;
		if (m_slZDrive.GetValue(&val)) {
			if (val.vt == VT_I4) {
				CString str;
				str.Format(_T("%.2lf %s"), (double)val.lVal / (double)m_lZDriveUnit, m_strZDriveUnit);
				GetDlgItem(IDC_STATIC_ZDRIVE_VALUE)->SetWindowText(str);

				if (!m_bIsEscape) {
					BOOL bIsUpperLimit = (val.lVal >= m_slZDrive.GetUserRangeMax()) ? TRUE : FALSE;
					GetDlgItem(IDC_BUTTON_ZDRIVE_U)->EnableWindow(!bIsUpperLimit);
					GetDlgItem(IDC_BUTTON_ZDRIVE_UU)->EnableWindow(!bIsUpperLimit);
					GetDlgItem(IDC_BUTTON_ZDRIVE_MAX)->EnableWindow(!bIsUpperLimit);

					BOOL bIsLowerLimit = (val.lVal <= m_slZDrive.GetUserRangeMin()) ? TRUE : FALSE;
					GetDlgItem(IDC_BUTTON_ZDRIVE_D)->EnableWindow(!bIsLowerLimit);
					GetDlgItem(IDC_BUTTON_ZDRIVE_DD)->EnableWindow(!bIsLowerLimit);
					GetDlgItem(IDC_BUTTON_ZDRIVE_MIN)->EnableWindow(!bIsLowerLimit);
				}
			}
		}
	}
}
void CLvSampleDlg::UpdateZDriveInterlockMode()
{
	if (m_Microscope.m_bZDriveMounted && !m_Microscope.IsAFController()) {
		BOOL bIsInterlock;
		if (!FAILED(m_Microscope.ReadInterlock(AccessoryZDrive, &bIsInterlock)))
			m_chkAlf.SetCheck(bIsInterlock ? BST_CHECKED : BST_UNCHECKED);
		else
			m_chkAlf.SetCheck(BST_UNCHECKED);
	}
}
void CLvSampleDlg::UpdateZEscape()
{
	if (m_Microscope.m_bZDriveMounted) {
		VARIANT val;
		if (m_chkZEscape.GetValue(&val)) {
			if (val.vt == VT_I4) {
				m_bIsEscape = (EnumStatus)val.lVal == StatusTrue ? TRUE : FALSE;
				m_chkZEscape.SetCheck(m_bIsEscape ? BST_CHECKED : BST_UNCHECKED);
				CString strEscape;
				strEscape.LoadString(m_bIsEscape ? IDS_ZDRIVE_REFOCUS : IDS_ZDRIVE_ESCAPE);
				m_chkZEscape.SetWindowText(strEscape);

				GetDlgItem(IDC_BUTTON_ZDRIVE_CHANGE)->EnableWindow(!m_bIsEscape && !m_Microscope.IsAFController());
				GetDlgItem(IDC_SLIDER_ZDRIVE)->EnableWindow(!m_bIsEscape);
				GetDlgItem(IDC_BUTTON_ZDRIVE_D)->EnableWindow(!m_bIsEscape);
				GetDlgItem(IDC_BUTTON_ZDRIVE_DD)->EnableWindow(!m_bIsEscape);
				GetDlgItem(IDC_BUTTON_ZDRIVE_U)->EnableWindow(!m_bIsEscape);
				GetDlgItem(IDC_BUTTON_ZDRIVE_UU)->EnableWindow(!m_bIsEscape);
				GetDlgItem(IDC_BUTTON_ZDRIVE_MIN)->EnableWindow(!m_bIsEscape);
				GetDlgItem(IDC_BUTTON_ZDRIVE_MAX)->EnableWindow(!m_bIsEscape);
				GetMenu()->EnableMenuItem(ID_SETTING_FOCUS, m_Microscope.m_bNosepieceMounted && !m_Microscope.IsAFController() && !m_bIsEscape ? MFS_ENABLED : MFS_DISABLED);
				GetMenu()->EnableMenuItem(ID_SETTING_SOFTLIMIT, !m_bIsEscape ? MFS_ENABLED : MFS_DISABLED);

				// Nosepiece enable/disable
				GetDlgItem(IDC_COMBO_NOSEPIECE)->EnableWindow(!m_bIsEscape);
				GetDlgItem(IDC_BUTTON_NOSEPIECE_L)->EnableWindow(!m_bIsEscape);
				GetDlgItem(IDC_BUTTON_NOSEPIECE_R)->EnableWindow(!m_bIsEscape);
			}
		}
	}
}
void CLvSampleDlg::UpdateZDriveAF()
{
	if (m_Microscope.m_bZDriveMounted) {
		if (m_bSupportedAF) {
			HRESULT hr = S_OK;
			try {
				INikonLv* pNikonLv = m_Microscope.GetInterface();

				VARIANT val;
				IMipParameterPtr spiParam;

				CComPtr<IZDrive> pZDrive = NULL;
				if (FAILED(hr = pNikonLv->get_ZDrive(&pZDrive)))
					_com_raise_error(hr);

				if (FAILED(hr = pZDrive->get_AfStatus(&val)))
					_com_raise_error(hr);
				if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
					_com_raise_error(hr);
				GetDlgItem(IDC_STATIC_ZDRIVE_FOCUS1)->SetWindowText(spiParam->DisplayString.GetBSTR());

				if (FAILED(hr = pZDrive->get_AfSearchMode(&val)))
					_com_raise_error(hr);
				if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
					_com_raise_error(hr);
				GetDlgItem(IDC_STATIC_ZDRIVE_FOCUS2)->SetWindowText(spiParam->DisplayString.GetBSTR());

				EnumAf eAF = (EnumAf)((long)pZDrive->AF);
				EnumAfStatus eAfStatus = (EnumAfStatus)((long)pZDrive->AfStatus);
				EnumStatus eIsAfSearch = (EnumStatus)((long)pZDrive->IsAfSearch);

				if (eAF == AfRun) {	// AF Runのときはサーチ不可
					if (m_chkAF.GetCheck() != BST_CHECKED)
						m_chkAF.SetCheck(BST_CHECKED);
					m_chkAF.EnableWindow(TRUE);

					if (eAfStatus != AfStatusOutOfRange && eAfStatus != AfStatusUnknown) {
						GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(FALSE);
						GetDlgItem(IDC_BUTTON_STOPSEARCH)->EnableWindow(FALSE);
					}
					else if (eAfStatus == AfStatusOutOfRange) {	// サーチ中のときはサーチ停止のみ
						if (eIsAfSearch == StatusTrue) {
							GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(FALSE);
							GetDlgItem(IDC_BUTTON_STOPSEARCH)->EnableWindow(TRUE);
						}
						else {
							GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(TRUE);
							GetDlgItem(IDC_BUTTON_STOPSEARCH)->EnableWindow(FALSE);
						}
					}
					else {
						GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(FALSE);
						GetDlgItem(IDC_BUTTON_STOPSEARCH)->EnableWindow(FALSE);
					}
				}
				else {	// AF Waitのときはサーチステータスを確認
					if (m_chkAF.GetCheck() == BST_CHECKED)
						m_chkAF.SetCheck(BST_UNCHECKED);

					if (eIsAfSearch == StatusTrue) {	// サーチ中のときはサーチ停止のみ
						m_chkAF.EnableWindow(FALSE);
						GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(FALSE);
						GetDlgItem(IDC_BUTTON_STOPSEARCH)->EnableWindow(TRUE);
					}
					else {
						if (eAfStatus == AfStatusUnknown) {	// Unknownのときは何もできない
							m_chkAF.EnableWindow(FALSE);
							GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(FALSE);
							GetDlgItem(IDC_BUTTON_STOPSEARCH)->EnableWindow(FALSE);
						}
						else if (eAfStatus == AfStatusJustFocus || eAfStatus == AfStatusUnderFocus || eAfStatus == AfStatusOverFocus) {	// OutOfRange以外のときはAF可能、サーチ不可
							m_chkAF.EnableWindow(TRUE);
							GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(FALSE);
							GetDlgItem(IDC_BUTTON_STOPSEARCH)->EnableWindow(FALSE);
						}
						else {	// OutOfRangeのときはAF不可、サーチ可能
							m_chkAF.EnableWindow(FALSE);
							GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(TRUE);
							GetDlgItem(IDC_BUTTON_STOPSEARCH)->EnableWindow(FALSE);
						}
					}
				}

				if (spiParam)
					spiParam.Release();
				if (pZDrive)
					pZDrive.Release();
			}
			catch (_com_error/* &e*/) {
				AfxMessageBox(_T("Microscope error!"));
			}
		}
		else {
			GetDlgItem(IDC_STATIC_ZDRIVE_FOCUS1)->SetWindowText(_T(""));
			m_chkAF.EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_STOPSEARCH)->EnableWindow(FALSE);
		}
	}
}
void CLvSampleDlg::SetZDriveSpeedByStep()
{
	static const EnumSpeed aSpeed[4] = { Speed6, Speed4, Speed2, Speed1 };
	int nSel = m_cboZDriveStep.GetCurSel();
	if (nSel > -1 && nSel < 4)
		m_Microscope.ExecZDriveSetSpeed(aSpeed[nSel]);
}
void CLvSampleDlg::SetZDriveSpeedContinuous()
{
	EnumSpeed eSpeed = m_eZDriveSpeed;
	if (eSpeed == Speed6) {
		if (m_Microscope.m_bNosepieceMounted && !m_Microscope.IsAFController()) {
			HRESULT hr = S_OK;
			try {
				INikonLv* pNikonLv = m_Microscope.GetInterface();

				CComPtr<INosepiece> pNosepiece = NULL;
				if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
					_com_raise_error(hr);

				long lVal = (long)pNosepiece->Value;
				if (lVal >= (long)pNosepiece->LowerLimit && lVal <= (long)pNosepiece->UpperLimit) {
					if (pNosepiece->Objectives->GetItem(lVal)->Code > 0) {
						if (pNosepiece->Objectives->GetItem(lVal)->NumericalAperture > 0.5)
							eSpeed = Speed7;
					}
				}

				if (pNosepiece)
					pNosepiece.Release();
			}
			catch (_com_error/* &e*/) {
				AfxMessageBox(_T("Microscope error!"));
			}
		}
	}
	m_Microscope.ExecZDriveSetSpeed(eSpeed);
}
// Aperture
void CLvSampleDlg::UpdateApertureEnable()
{
	if (m_Microscope.m_bApertureMounted) {
		BOOL bControl = !m_bDarkField && !m_bDarkFieldCube;
		m_slAperture.EnableWindow(bControl);
		GetDlgItem(IDC_BUTTON_APERTURE_L)->EnableWindow(bControl);
		GetDlgItem(IDC_BUTTON_APERTURE_R)->EnableWindow(bControl);
		GetDlgItem(IDC_BUTTON_APERTURE_PRESET)->EnableWindow(bControl);
	}
}
void CLvSampleDlg::UpdateApertureValue()
{
	if (m_Microscope.m_bApertureMounted) {
		VARIANT val;
		if (m_slAperture.GetValue(&val)) {
			if (val.vt == VT_I4) {
				CString str;
				str.Format(_T("%.1lf %s"), (double)val.lVal / (double)m_lApertureUnit, m_strApertureUnit);
				GetDlgItem(IDC_STATIC_APERTURE_VALUE)->SetWindowText(str);
			}
		}
	}
}
// DIC
void CLvSampleDlg::UpdateDICEnable()
{
	if (m_Microscope.m_bDicMounted) {
		BOOL bControl = m_bObservationDIC && m_bNosepieceDIC;
		BOOL bCalibration = bControl;

		BOOL bIsOut;
		try {
			if (!bControl) {
				if (!FAILED(m_Microscope.GetDICOutOfPosition(&bIsOut))) {
					if (!bIsOut)
						m_Microscope.ExecCommand(AccessoryDICPrism, 4);
				}
			}
			else if (!FAILED(m_Microscope.GetDICOutOfPosition(&bIsOut))) {
				if (bIsOut)
					bCalibration = FALSE;
			}
			else {
				bControl = FALSE;
				bCalibration = FALSE;
			}
			m_slDIC.EnableWindow(bControl);
			m_cboDIC.EnableWindow(bControl);
			GetDlgItem(IDC_BUTTON_DIC_L)->EnableWindow(bControl);
			GetDlgItem(IDC_BUTTON_DIC_R)->EnableWindow(bControl);
			GetDlgItem(IDC_BUTTON_DIC_CALIBRATION)->EnableWindow(bCalibration);
			GetDlgItem(IDC_BUTTON_DIC_CLEAR)->EnableWindow(bCalibration);
			GetMenu()->EnableMenuItem(ID_DIC_CALIBRATION, bCalibration ? MFS_ENABLED : MFS_DISABLED);
			GetMenu()->EnableMenuItem(ID_DIC_CLEAR, bCalibration ? MFS_ENABLED : MFS_DISABLED);
		}
		catch (_com_error/* &e*/) {
			AfxMessageBox(_T("Microscope error!"));
		}
	}
}
void CLvSampleDlg::UpdateDICValue()
{
	if (m_Microscope.m_bDicMounted) {
		VARIANT val;
		if (m_slDIC.GetValue(&val)) {
			if (val.vt == VT_I4) {
				CString str;
				BOOL bIsOut;
				try {
					if (!FAILED(m_Microscope.GetDICOutOfPosition(&bIsOut))) {
						CString str;
						if (bIsOut)
							str = _T("Out of position");
						else {
							double dPhaseDifference;
							if (!FAILED(m_Microscope.GetDICPhaseDifference(&dPhaseDifference)))
								str.Format(_T("%.3lf %s"), (double)dPhaseDifference, m_strDICUnit);
						}
						GetDlgItem(IDC_STATIC_DIC_VALUE)->SetWindowText(str);
					}
				}
				catch (_com_error/* &e*/) {
					AfxMessageBox(_T("Microscope error!"));
				}
			}
		}
	}
}
void CLvSampleDlg::UpdateDICPosition()
{
	if (m_Microscope.m_bDicMounted) {
		VARIANT val;
		if (m_cboDIC.GetValue(&val)) {
			if (val.vt == VT_I4) {
				for (int i = 0; i < m_cboDIC.GetCount(); i++) {
					if (m_cboDIC.GetItemData(i) == val.lVal) {
						if (m_cboDIC.GetCurSel() != i)
							m_cboDIC.SetCurSel(i);
						break;
					}
				}
			}
		}
	}
}
void CLvSampleDlg::UpdateDICCalibration()
{
	if (m_Microscope.m_bDicMounted) {
		try {
			BOOL bIsCalibration;
			if (!FAILED(m_Microscope.GetDICCalibration(&bIsCalibration)))
				m_stLED.SetBitmap(bIsCalibration ? m_bmpLEDOn : m_bmpLEDOff);

			double dPhaseDifference;
			if (!FAILED(m_Microscope.GetDICPhaseDifference(&dPhaseDifference))) {
				static const CString strPhase[11] = {
					_T("-5/2"),
					_T("-2"),
					_T("-3/2"),
					_T("-1"),
					_T("-1/2"),
					_T("0"),
					_T("1/2"),
					_T("1"),
					_T("3/2"),
					_T("2"),
					_T("5/2"),
				};

				int nMin = m_slDIC.GetUserRangeMin();
				int nMax = m_slDIC.GetUserRangeMax();

				int nZeroPos = m_slDIC.GetPos() - (int)(dPhaseDifference * m_lDICUnit);
				int nPhaseMin = nMin - nZeroPos;
				int nPhaseMax = nMax - nZeroPos;

				CRect rcLbl;
				m_stDicLbl1.GetWindowRect(&rcLbl);
				ScreenToClient(&rcLbl);

				CRect rcCh;
				m_slDIC.GetChannelRect(&rcCh);
				double dPxPerPos = (double)rcCh.Width() / (double)(nMax - nMin);

				CRect rcSl;
				m_slDIC.GetWindowRect(&rcSl);
				ScreenToClient(&rcSl);
				int nZeroPosX = rcSl.left + rcCh.left + (int)(dPxPerPos * (double)(nZeroPos - nMin)) - rcLbl.Width() / 2;

				int nLbl = 0;
				CStatic* pstDicLbl[5] = { &m_stDicLbl1, &m_stDicLbl2, &m_stDicLbl3, &m_stDicLbl4, &m_stDicLbl5 };
				for (int i = 0; i < 11; i++) {
					int nScale = (i - 5) * 500;
					if (nPhaseMin <= nScale && nPhaseMax >= nScale) {
						pstDicLbl[nLbl]->ShowWindow(SW_SHOW);
						pstDicLbl[nLbl]->SetWindowText(strPhase[i]);
						int nLblPosX = nZeroPosX + (int)(dPxPerPos * nScale);
						pstDicLbl[nLbl]->SetWindowPos(NULL, nLblPosX, rcLbl.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
						nLbl++;
					}
					else if (nPhaseMax < nScale) {
						pstDicLbl[nLbl]->ShowWindow(SW_HIDE);
						nLbl++;
					}
					if (nLbl >= 5)
						break;
				}
			}
		}
		catch (_com_error/* &e*/) {
			AfxMessageBox(_T("Microscope error!"));
		}
	}
}
// Interlock
void CLvSampleDlg::UpdateInterlock()
{
	if (!m_Microscope.IsAFController() && !m_Microscope.IsLVNController()) {
		if (m_eInterlockTarget != AccessoryNosepiece) {
			BOOL bIsInterlock;
			if (!FAILED(m_Microscope.ReadInterlock(m_eInterlockTarget, &bIsInterlock)))
				m_cboInterlock.SetCurSel(bIsInterlock ? 1 : 0);
		}
	}
}

// View expand
void CLvSampleDlg::SetViewExpand(BOOL bExpand, BOOL bInit)
{
	if (bInit || (bExpand != m_bExpand)) {
		m_bExpand = bExpand;
		CRect rcDlg, rcChild;
		::GetWindowRect(m_hWnd, rcDlg);
		::GetWindowRect(GetDlgItem(IDC_STATIC_FRM_APERTURE)->m_hWnd, rcChild);
		if (m_bExpand)
			rcDlg.right = rcChild.right + 10;
		else
			rcDlg.right = rcChild.left - 1;
		::MoveWindow(m_hWnd, rcDlg.left, rcDlg.top, rcDlg.Width(), rcDlg.Height(), TRUE);
		GetMenu()->CheckMenuItem(ID_VIEW_STANDARD, m_bExpand ? MFS_UNCHECKED : MFS_CHECKED);
		GetMenu()->CheckMenuItem(ID_VIEW_EXPAND, m_bExpand ? MFS_CHECKED : MFS_UNCHECKED);
	}
}
// Observation
void CLvSampleDlg::OnCbnSelchangeComboObservation()
{
	CWaitCursor w;
	HRESULT hr = S_OK;

	int nSel = m_cboObservation.GetCurSel();
	if (nSel > -1) {
		try {
			INikonLv* pNikonLv = m_Microscope.GetInterface();

			CComPtr<IPresets> pPresets = NULL;
			if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
				_com_raise_error(hr);

			if (m_Microscope.IsAFController()) {
				EnumAfObservationMode eMode = (EnumAfObservationMode)m_cboObservation.GetItemData(nSel);
				pPresets->Put_AfObservationMode(eMode);
			}
			else {
				EnumObservationMode eMode = (EnumObservationMode)m_cboObservation.GetItemData(nSel);
				if (pPresets->Get_ObservationMode() != eMode) {
					BOOL bDicDisable = FALSE;
					if (eMode >= ObservationMode1 && eMode <= ObservationMode6) {
						EnumOptionalObservationMode eOptional;
						EnumStatus eInterlock;
						long lPos;
						if (FAILED(hr = pPresets->ReadPresetModesEx(eMode, &eOptional, &eInterlock, &lPos)))
							_com_raise_error(hr);
						if ((eOptional == DICEpiDia || eOptional == DICEpi) && !m_bNosepieceDIC)
							bDicDisable = TRUE;
					}
					else if (eMode == DIC && !m_bNosepieceDIC)
						bDicDisable = TRUE;

					if (bDicDisable) {
						AfxMessageBox(IDS_MESSAGE_CANNOT_CHANGE_DIC);
						UpdateObservationMode();
					}
					else
						pPresets->Put_ObservationMode(eMode);
				}
			}

			if (pPresets)
				pPresets.Release();
		}
		catch (_com_error/* &e*/) {
			AfxMessageBox(_T("Microscope error!"));
		}
	}
}
// EPI
void CLvSampleDlg::OnBnClickedCheckEpi()
{
	CWaitCursor w;
	BOOL bIsOn = m_chkEpi.GetCheck() == BST_CHECKED ? TRUE : FALSE;
	m_Microscope.ExecCommand(AccessoryEpiLamp, bIsOn ? 0 : 1);
}
void CLvSampleDlg::OnBnClickedButtonEpiDec()
{
	CWaitCursor w;
	m_Microscope.ExecCommand(AccessoryEpiLampVoltage, 0);
}
void CLvSampleDlg::OnBnClickedButtonEpiInc()
{
	CWaitCursor w;
	m_Microscope.ExecCommand(AccessoryEpiLampVoltage, 1);
}
void CLvSampleDlg::OnBnClickedButtonEpiPreset()
{
	CWaitCursor w;
	m_Microscope.ExecCommand(AccessoryEpiLampVoltage, 2);
}
// Fiber
void CLvSampleDlg::OnBnClickedCheckFiber()
{
	CWaitCursor w;
	BOOL bIsOpen = m_chkFiber.GetCheck() == BST_CHECKED ? TRUE : FALSE;
	m_Microscope.ExecCommand(AccessoryEpiShutter, bIsOpen ? 0 : 1);
}
void CLvSampleDlg::OnBnClickedButtonFiberCmd()
{
	CString strTime;
	m_edFiber.GetWindowText(strTime);
	if (strTime.GetLength() > 0) {
		TCHAR* e;
		double dTime = _tcstod(strTime, &e);
		if (*e == '\0') {
			long lTime = (long)(dTime * 1000);
			if (lTime >= 100) {
				CWaitCursor w;
				if(!FAILED(m_Microscope.ExecEpiShutterOpenWithTimer(lTime))) {
					GetDlgItem(IDC_STATIC_FIBER_OPEN)->SetWindowText(_T("Open"));
					GetDlgItem(IDC_BUTTON_FIBER)->EnableWindow(FALSE);
					GetDlgItem(IDC_BUTTON_FIBER_CMD)->EnableWindow(FALSE);
					GetDlgItem(IDC_EDIT_FIBER)->EnableWindow(FALSE);
					GetDlgItem(IDC_STATIC_FIBER_UNIT)->EnableWindow(FALSE);
					SetTimer(EPI_SHUTTER_TIMER, lTime + EPI_CTRL_ENABLE_TIME, NULL);
				}
				else {
					AfxMessageBox(IDS_MESSAGE_SHUTTER_FAILED);
				}
			}
			else {
				AfxMessageBox(IDS_MESSAGE_SHUTTER_SHORT);
			}
		}
		else {
			AfxMessageBox(IDS_MESSAGE_INVALID_FORMAT);
		}
	}
	else {
		AfxMessageBox(IDS_MESSAGE_SHUTTER_INPUT);
	}
}
void CLvSampleDlg::OnCbnSelchangeComboND()
{
	CWaitCursor w;
	m_cboFiber.PutValue();
}
// Nosepiece
void CLvSampleDlg::OnCbnSelchangeComboNosepiece()
{
	if (m_Microscope.m_bNosepieceMounted) {
		int nSel = m_cboNosepiece.GetCurSel();
		if (nSel >= 0) {
			long lValue = (long)m_cboNosepiece.GetItemData(nSel);
			if (lValue > 0) {
				HRESULT hr = S_OK;
				try {
					INikonLv* pNikonLv = m_Microscope.GetInterface();
					CComPtr<INosepiece> pNosepiece = NULL;
					if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
						_com_raise_error(hr);

					CWaitCursor w;
					if (FAILED(hr = pNosepiece->EnableInterlock()))
						_com_raise_error(hr);
					if (FAILED(hr = m_Microscope.ExecSelectCommand(AccessoryNosepiece, lValue)))
						_com_raise_error(hr);

					if (pNosepiece)
						pNosepiece.Release();
				}
				catch (_com_error/* &e*/) {
					if (!RetryNosepieceCommand(2))
						UpdateNosepiece();
//					AfxMessageBox(_T("Microscope error!"));
				}
			}
			else
				UpdateNosepiece();
		}
	}
}
void CLvSampleDlg::OnBnClickedButtonNosepiecePrev()
{
	MoveNosepiece(0);
}
void CLvSampleDlg::OnBnClickedButtonNosepieceNext()
{
	MoveNosepiece(1);
}
// Cube
void CLvSampleDlg::OnCbnSelchangeComboCube()
{
	if (m_Microscope.m_bCubeMounted) {
		int nSel = m_cboCube.GetCurSel();
		if (nSel >= 0) {
			long lValue = (long)m_cboCube.GetItemData(nSel);
			if (lValue > 0) {
				HRESULT hr = S_OK;
				try {
					INikonLv* pNikonLv = m_Microscope.GetInterface();
					CComPtr<IFilterBlockCassette> pFilterBlockCassette = NULL;
					if (FAILED(hr = pNikonLv->get_FilterBlockCassette(&pFilterBlockCassette)))
						_com_raise_error(hr);

					CWaitCursor w;
					if (FAILED(hr = pFilterBlockCassette->EnableInterlock()))
						_com_raise_error(hr);
					if (FAILED(hr = m_Microscope.ExecSelectCommand(AccessoryFilterBlockCassette, lValue)))
						_com_raise_error(hr);

					if (pFilterBlockCassette)
						pFilterBlockCassette.Release();
				}
				catch (_com_error/* &e*/) {
					AfxMessageBox(_T("Microscope error!"));
				}
			}
			else
				UpdateCube();
		}
	}
}
void CLvSampleDlg::OnBnClickedButtonCubePrev()
{
	MoveCube(0);
}
void CLvSampleDlg::OnBnClickedButtonCubeNext()
{
	MoveCube(1);
}
// DIA
void CLvSampleDlg::OnBnClickedCheckDia()
{
	CWaitCursor w;
	BOOL bIsOn = m_chkDia.GetCheck() == BST_CHECKED ? TRUE : FALSE;
	m_Microscope.ExecCommand(AccessoryDiaLamp, bIsOn ? 0 : 1);
}
void CLvSampleDlg::OnBnClickedButtonDiaDec()
{
	CWaitCursor w;
	m_Microscope.ExecCommand(AccessoryDiaLampVoltage, 0);
}
void CLvSampleDlg::OnBnClickedButtonDiaInc()
{
	CWaitCursor w;
	m_Microscope.ExecCommand(AccessoryDiaLampVoltage, 1);
}
void CLvSampleDlg::OnBnClickedButtonDiaPreset()
{
	CWaitCursor w;
	m_Microscope.ExecCommand(AccessoryDiaLampVoltage, 2);
}
// Z Drive
void CLvSampleDlg::OnBnClickedButtonZDriveU()
{
	CWaitCursor w;
	m_Microscope.ExecZDriveSetSpeed(Speed6);
	m_Microscope.ExecCommand(AccessoryZDrive, 1);
}
void CLvSampleDlg::OnBnClickedButtonZDriveD()
{
	CWaitCursor w;
	m_Microscope.ExecZDriveSetSpeed(Speed6);
	m_Microscope.ExecCommand(AccessoryZDrive, -1);
}
void CLvSampleDlg::OnBnClickedButtonZDriveUu()
{
	CWaitCursor w;
	SetZDriveSpeedByStep();
	int nMax = m_slZDrive.GetUserRangeMax();
	if (m_slZDrive.GetValuePos() + m_lZDriveStep > nMax)
		m_Microscope.ExecZDriveMoveToLimit(1);
	else
		m_Microscope.ExecCommand(AccessoryZDrive, m_lZDriveStep);
}
void CLvSampleDlg::OnBnClickedButtonZDriveDd()
{
	CWaitCursor w;
	SetZDriveSpeedByStep();
	int nMin = m_slZDrive.GetUserRangeMin();
	if (m_slZDrive.GetValuePos() - m_lZDriveStep < nMin)
		m_Microscope.ExecZDriveMoveToLimit(0);
	else
		m_Microscope.ExecCommand(AccessoryZDrive, -m_lZDriveStep);
}
LRESULT CLvSampleDlg::OnHoldButtonDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam) {
	case IDC_BUTTON_ZDRIVE_MAX:
		SetZDriveSpeedContinuous();
		m_Microscope.ExecZDriveMoveContinuous(1);
		break;
	case IDC_BUTTON_ZDRIVE_MIN:
		SetZDriveSpeedContinuous();
		m_Microscope.ExecZDriveMoveContinuous(0);
		break;
	default:
		break;
	}
	return 0;
}
LRESULT CLvSampleDlg::OnHoldButtonUp(WPARAM wParam, LPARAM lParam)
{
	switch (lParam) {
	case IDC_BUTTON_ZDRIVE_MAX:
	case IDC_BUTTON_ZDRIVE_MIN:
		m_Microscope.ExecZDriveAbort();
		break;
	default:
		break;
	}
	return 0;
}
void CLvSampleDlg::OnBnClickedButtonZDriveChange()
{
	CZDriveRangeDlg dlg(&m_Microscope);
	if (dlg.DoModal() == IDOK) {
		UpdateZDriveRange();
		UpdateZDrivePos();
	}
}
void CLvSampleDlg::OnBnClickedCheckZDriveALF()
{
	CWaitCursor w;
	BOOL bIsInterlock = m_chkAlf.GetCheck() == BST_CHECKED ? TRUE : FALSE;
	m_Microscope.SetZDriveInterlock(bIsInterlock);
	UpdateZDriveInterlockMode();
}
void CLvSampleDlg::OnCbnSelchangeComboZDriveStep()
{
	int nSel = m_cboZDriveStep.GetCurSel();
	if (nSel >= 0)
		m_lZDriveStep = (long)m_cboZDriveStep.GetItemData(nSel);
}
void CLvSampleDlg::OnCbnSelchangeComboZDriveSearch()
{
	int nSel = m_cboZDriveSearch.GetCurSel();
	if (nSel >= 0)
		m_eAfSearch = (EnumAfSearch)m_cboZDriveSearch.GetItemData(nSel);
}
void CLvSampleDlg::OnCbnSelchangeComboZDriveSpeed()
{
	int nSel = m_cboZDriveSpeed.GetCurSel();
	if (nSel >= 0)
		m_eZDriveSpeed = (EnumSpeed)m_cboZDriveSpeed.GetItemData(nSel);
}
void CLvSampleDlg::OnBnClickedButtonZEscape()
{
	CWaitCursor w;
	BOOL bIsEscape = m_chkZEscape.GetCheck() == BST_CHECKED ? TRUE : FALSE;
	m_Microscope.ExecZDriveSetSpeed(Speed1);
	if (FAILED(m_Microscope.ExecZEscape(!bIsEscape))) {
		AfxMessageBox(IDS_MESSAGE_NOT_ESCAPE);
	}
	UpdateZEscape();
}
void CLvSampleDlg::OnBnClickedButtonAF()
{
	if (m_bSupportedAF) {
		CWaitCursor w;
		m_Microscope.ExecZDriveAFStateChange(m_chkAF.GetCheck() == BST_CHECKED ? AfWait : AfRun);
		UpdateZDriveAF();
	}
}
void CLvSampleDlg::OnBnClickedButtonSearch()
{
	if (m_bSupportedAF) {
		CWaitCursor w;
		GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(FALSE);
		m_Microscope.ExecZDriveSearchAF(m_eAfSearch);
//		UpdateZDriveAF();
	}
}
void CLvSampleDlg::OnBnClickedButtonStopSearch()
{
	if (m_bSupportedAF) {
		CWaitCursor w;
		GetDlgItem(IDC_BUTTON_STOPSEARCH)->EnableWindow(FALSE);
		m_Microscope.ExecZDriveStopAF();
//		UpdateZDriveAF();
	}
}
// Aperture
void CLvSampleDlg::OnBnClickedButtonApertureDec()
{
	CWaitCursor w;
	m_Microscope.ExecCommand(AccessoryEpiApertureStop, 0);
}
void CLvSampleDlg::OnBnClickedButtonApertureInc()
{
	CWaitCursor w;
	m_Microscope.ExecCommand(AccessoryEpiApertureStop, 1);
}
void CLvSampleDlg::OnBnClickedButtonAperturePreset()
{
	CWaitCursor w;
	m_Microscope.ExecCommand(AccessoryEpiApertureStop, 2);
}
// DIC
void CLvSampleDlg::OnBnClickedButtonDICDec()
{
	CWaitCursor w;
	m_Microscope.ExecCommand(AccessoryDICPrism, 0);
}
void CLvSampleDlg::OnBnClickedButtonDICInc()
{
	CWaitCursor w;
	m_Microscope.ExecCommand(AccessoryDICPrism, 1);
}
void CLvSampleDlg::OnBnClickedButtonDICCalibration()
{
	CWaitCursor w;
	m_Microscope.ExecCommand(AccessoryDICPrism, 2);
	UpdateDICCalibration();
	UpdateDICValue();
}
void CLvSampleDlg::OnBnClickedButtonDICClear()
{
	CWaitCursor w;
	m_Microscope.ExecCommand(AccessoryDICPrism, 3);
	UpdateDICCalibration();
	UpdateDICValue();
}
void CLvSampleDlg::OnCbnSelchangeComboDIC()
{
	CWaitCursor w;
	m_cboDIC.PutValue();
}
// Interlock
void CLvSampleDlg::OnCbnSelchangeComboInterlock()
{
	int nSel = m_cboInterlock.GetCurSel();
	CWaitCursor w;
	if (nSel == 0)
		m_Microscope.SetInterlock(FALSE);
	else if (nSel == 1)
		m_Microscope.SetInterlock(TRUE);
	UpdateInterlock();
}

// Menu command
// View
void CLvSampleDlg::OnViewStandard()
{
	SetViewExpand(FALSE);
}
void CLvSampleDlg::OnViewExpand()
{
	SetViewExpand(TRUE);
}
// File
void CLvSampleDlg::OnFileSelectDevice()
{
	CDeviceDlg dlg(&m_Microscope);
	if (dlg.DoModal() == IDOK) {
		long lIndex = dlg.GetSelectedDeviceIndex();
		if (lIndex >= 0) {
			HRESULT hr = S_OK;
			try {
				INikonLv* pNikonLv = m_Microscope.GetInterface();
				CWaitCursor w;
				if ((BOOL)pNikonLv->Device->Connected == TRUE) {
					if (FAILED(hr = DisconnectMipParam()))
						_com_raise_error(hr);
					if (FAILED(hr = m_Microscope.Disconnect()))
						_com_raise_error(hr);
				}

				if (FAILED(hr = m_Microscope.ConnectSelectedDevice(lIndex)))
					_com_raise_error(hr);
				if (hr == S_OK) {
					if (FAILED(hr = ConnectMipParam()))
						_com_raise_error(hr);
				}
			}
			catch (_com_error/* &e*/) {
				AfxMessageBox(_T("Microscope error!"));
			}
		}
	}
}
void CLvSampleDlg::OnFileCommand()
{
	CCommandDlg dlg(&m_Microscope);
	dlg.DoModal();
}
// Setting
void CLvSampleDlg::OnSettingInitialize()
{
	HRESULT hr = S_OK;
	INikonLv* pNikonLv = m_Microscope.GetInterface();

	if (m_Microscope.m_bZDriveMounted) {
	BOOL bIsEscape = FALSE;
		try {
			CComPtr<IZDrive> pZDrive = NULL;
			if (FAILED(hr = pNikonLv->get_ZDrive(&pZDrive)))
				_com_raise_error(hr);

			if ((EnumStatus)((long)pZDrive->IsZEscape) == StatusTrue) {
				AfxMessageBox(IDS_MESSAGE_ESCAPING_ZDRIVE);
				bIsEscape = TRUE;
			}
			if (pZDrive)
				pZDrive.Release();
		}
		catch (_com_error/* &e*/) {
			AfxMessageBox(_T("Microscope error!"));
		}
		if (bIsEscape)
			return;
	}

	long lIndex = pNikonLv->Device->Index;
	try {
		if ((BOOL)pNikonLv->Device->Connected == TRUE) {
			if (FAILED(hr = DisconnectMipParam()))
				_com_raise_error(hr);
			if (FAILED(hr = m_Microscope.Disconnect()))
				_com_raise_error(hr);
		}
		CSetupDlg dlg(&m_Microscope);
		dlg.DoModal();

		CWaitCursor w;
		if (FAILED(hr = pNikonLv->Devices->Refresh()))
			_com_raise_error(hr);

		if (FAILED(hr = m_Microscope.ConnectSelectedDevice(lIndex)))
			_com_raise_error(hr);
		if (hr == S_OK) {
			if (FAILED(hr = ConnectMipParam()))
				_com_raise_error(hr);
		}
		else {
			AfxMessageBox(IDS_MESSAGE_NO_RESPONSE_FROM_DEVICE);
		}
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}
void CLvSampleDlg::OnSettingInterlock()
{
	CUserObservationDlg dlg(&m_Microscope);
	dlg.DoModal();
	UpdateObservationInformation();
	UpdateObservationMode();
	UpdateApertureEnable();
	UpdateDICEnable();
	UpdateDICValue();
}
void CLvSampleDlg::OnSettingOffset()
{
	if (AfxMessageBox(IDS_MESSAGE_USER_OFFSET_INITIALIZED, MB_YESNO) == IDYES) {
		CWaitCursor w;
		HRESULT hr = S_OK;

		try {
			INikonLv* pNikonLv = m_Microscope.GetInterface();

			CComPtr<IPresets> pPresets = NULL;
			CComPtr<INosepiece> pNosepiece = NULL;

			if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
				_com_raise_error(hr);

			if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
				_com_raise_error(hr);

			VARIANT val;
			IMipParameterPtr spiParam;
			if (FAILED(hr = pPresets->get_ObservationMode(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);

			for (int i = 1; i <= spiParam->EnumSet->Count; i++) {
				EnumObservationMode eMode = (EnumObservationMode)spiParam->EnumSet->GetItem(i).RawValue.lVal;
				if (eMode != ObservationModeNone && eMode != ObservationModeUnknown) {
					for (int j = (int)pNosepiece->LowerLimit; j <= (int)pNosepiece->UpperLimit; j++) {
						if (m_Microscope.m_bDiaLampMounted)
							pPresets->InitializeMemorizedPresetsEx(AccessoryDiaLampVoltage, eMode, j);
						if (m_Microscope.m_bEpiLampMounted || m_Microscope.m_bFiberMounted)
							pPresets->InitializeMemorizedPresetsEx(AccessoryEpiLampVoltage, eMode, j);
						if (m_Microscope.m_bApertureMounted && (eMode != DarkField))
							pPresets->InitializeMemorizedPresetsEx(AccessoryEpiApertureStop, eMode, j);
						if (m_Microscope.m_bDicMounted)
							pPresets->InitializeMemorizedPresetsEx(AccessoryDICPrismShift, eMode, j);
					}
				}
			}

			if (spiParam)
				spiParam.Release();
			if (pPresets)
				pPresets.Release();
			if (pNosepiece)
				pNosepiece.Release();
		}
		catch (_com_error/* &e*/) {
			AfxMessageBox(_T("Microscope error!"));
		}
	}
}
void CLvSampleDlg::OnSettingControl()
{
	CControlDlg dlg(&m_Microscope);
	dlg.DoModal();
}
void CLvSampleDlg::OnSettingFocus()
{
	CCompensationDlg dlg(&m_Microscope);
	dlg.DoModal();
	UpdateZDriveAF();
	UpdateZDriveRange();
	UpdateZDrivePos();
}
void CLvSampleDlg::OnSettingSoftLimit()
{
	CSoftLimitDlg dlg(&m_Microscope);
	dlg.DoModal();
	UpdateZDriveAF();
	UpdateZDriveRange();
	UpdateZDrivePos();
}
void CLvSampleDlg::OnDicCalibration()
{
	CWaitCursor w;
	m_Microscope.ExecCommand(AccessoryDICPrism, 2);
	UpdateDICCalibration();
	UpdateDICValue();
}
void CLvSampleDlg::OnDicClear()
{
	CWaitCursor w;
	m_Microscope.ExecCommand(AccessoryDICPrism, 3);
	UpdateDICCalibration();
	UpdateDICValue();
}
void CLvSampleDlg::OnSettingAFOffset()
{
	CAfOffsetDlg dlg(&m_Microscope);
	dlg.DoModal();
}
// Status
void CLvSampleDlg::OnStatusNosepiese()
{
	CInfoDlg dlg(&m_Microscope, E_Nosepiece);
	dlg.DoModal();
}
void CLvSampleDlg::OnStatusCube()
{
	CInfoDlg dlg(&m_Microscope, E_Cube);
	dlg.DoModal();
}
void CLvSampleDlg::OnStatusInterlock()
{
	CInfoDlg dlg(&m_Microscope, E_Interlock);
	dlg.DoModal();
}
void CLvSampleDlg::OnStatusUserObservation()
{
	CInfoDlg dlg(&m_Microscope, E_UserObservation);
	dlg.DoModal();
}
void CLvSampleDlg::OnStatusEpiDia()
{
	CInfoDlg dlg(&m_Microscope, E_EpiDia);
	dlg.DoModal();
}
void CLvSampleDlg::OnStatusZDrive()
{
	CInfoDlg dlg(&m_Microscope, E_ZDrive);
	dlg.DoModal();
}
void CLvSampleDlg::OnStatusDic()
{
	CInfoDlg dlg(&m_Microscope, E_Dic);
	dlg.DoModal();
}
void CLvSampleDlg::OnStatusSystem()
{
	CInfoDlg dlg(&m_Microscope, E_System);
	dlg.DoModal();
}
// Help
void CLvSampleDlg::OnHelpIndex()
{
	LPWSTR lpPath = NULL;
	lpPath = (LPWSTR)malloc(sizeof(LPWSTR) * 255);
	if (lpPath) {
		HRESULT hr = S_OK;
		if (SUCCEEDED(hr = SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, NULL, lpPath))) {
			CString str, strPath, strName;
			strPath.Format(_T("%s"), lpPath);
			strName.LoadString(IDS_NAME_HELP);
			if (strPath.Right(1) == '\\')
				str.Format(_T("%sNikon\\LV-Series\\Help\\%s"), strPath, strName);
			else
				str.Format(_T("%s\\Nikon\\LV-Series\\Help\\%s"), strPath, strName);
			::ShellExecute(m_hWnd, _T("Open"), str, NULL, _T("C:\\"), 1);
		}
		free(lpPath);
	}
}
