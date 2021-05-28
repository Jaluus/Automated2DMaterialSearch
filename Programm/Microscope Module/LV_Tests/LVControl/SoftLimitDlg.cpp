// SoftLimitDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "LvSample.h"
#include "SoftLimitDlg.h"
#include "ZDriveRangeDlg.h"


// CSoftLimitDlg ダイアログ

IMPLEMENT_DYNAMIC(CSoftLimitDlg, CDialog)


CSoftLimitDlg::CSoftLimitDlg(CMicroscopeControl* pMicroscope, CWnd* pParent /*=NULL*/)
	: CDialog(CSoftLimitDlg::IDD, (CWnd*)pParent)
	, m_pMicroscope(pMicroscope)
	, m_eLimitTarget(E_Upper)
	, m_lZDriveUnit(200)
	, m_lZDriveStep(20)
	, m_bSupportedAF(FALSE)
	, m_eAfSearch(AfSearchMode1)
	, m_eZDriveSpeed(Speed1)
{
}

CSoftLimitDlg::~CSoftLimitDlg()
{
}

void CSoftLimitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	// Z Drive
	DDX_Control(pDX, IDC_COMBO_ZDRIVE_TARGET, m_cboZDriveTarget);
	DDX_Control(pDX, IDC_COMBO_ZDRIVE_STEP, m_cboZDriveStep);
	DDX_Control(pDX, IDC_COMBO_ZDRIVE_SEARCH, m_cboZDriveSearch);
	DDX_Control(pDX, IDC_COMBO_ZDRIVE_SPEED, m_cboZDriveSpeed);
	DDX_Control(pDX, IDC_SLIDER_ZDRIVE, m_slZDrive);
	DDX_Control(pDX, IDC_BUTTON_ZDRIVE_U, m_btnZDriveU);
	DDX_Control(pDX, IDC_BUTTON_ZDRIVE_UU, m_btnZDriveUu);
	DDX_Control(pDX, IDC_BUTTON_ZDRIVE_D, m_btnZDriveD);
	DDX_Control(pDX, IDC_BUTTON_ZDRIVE_DD, m_btnZDriveDd);
	DDX_Control(pDX, IDC_BUTTON_ZDRIVE_MIN, m_btnZMin);
	DDX_Control(pDX, IDC_BUTTON_ZDRIVE_MAX, m_btnZMax);
	DDX_Control(pDX, IDC_BUTTON_AF, m_chkAF);
	DDX_Control(pDX, IDC_BUTTON_SEARCH, m_btnSearch);
	DDX_Control(pDX, IDC_BUTTON_STOPSEARCH, m_btnStopSearch);
}


BEGIN_MESSAGE_MAP(CSoftLimitDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
	ON_MESSAGE(WM_PAREVENT, &CSoftLimitDlg::OnParamEvent)
	// Z Drive
	ON_BN_CLICKED(IDC_BUTTON_ZDRIVE_U, &CSoftLimitDlg::OnBnClickedButtonZDriveU)
	ON_BN_CLICKED(IDC_BUTTON_ZDRIVE_UU, &CSoftLimitDlg::OnBnClickedButtonZDriveUu)
	ON_BN_CLICKED(IDC_BUTTON_ZDRIVE_D, &CSoftLimitDlg::OnBnClickedButtonZDriveD)
	ON_BN_CLICKED(IDC_BUTTON_ZDRIVE_DD, &CSoftLimitDlg::OnBnClickedButtonZDriveDd)
	ON_MESSAGE(WM_HOLD_BUTTON_DOWN, &CSoftLimitDlg::OnHoldButtonDown)
	ON_MESSAGE(WM_HOLD_BUTTON_UP, &CSoftLimitDlg::OnHoldButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_ZDRIVE_CHANGE, &CSoftLimitDlg::OnBnClickedButtonZDriveChange)
	ON_CBN_SELCHANGE(IDC_COMBO_ZDRIVE_TARGET, &CSoftLimitDlg::OnCbnSelchangeComboZDriveTarget)
	ON_CBN_SELCHANGE(IDC_COMBO_ZDRIVE_STEP, &CSoftLimitDlg::OnCbnSelchangeComboZDriveStep)
	ON_CBN_SELCHANGE(IDC_COMBO_ZDRIVE_SEARCH, &CSoftLimitDlg::OnCbnSelchangeComboZDriveSearch)
	ON_CBN_SELCHANGE(IDC_COMBO_ZDRIVE_SPEED, &CSoftLimitDlg::OnCbnSelchangeComboZDriveSpeed)
	ON_BN_CLICKED(IDC_BUTTON_AF, &CSoftLimitDlg::OnBnClickedButtonAF)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CSoftLimitDlg::OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_STOPSEARCH, &CSoftLimitDlg::OnBnClickedButtonStopSearch)
	ON_BN_CLICKED(IDC_BUTTON_SET_ZDRIVE_LIMIT, &CSoftLimitDlg::OnBnClickedButtonSetZDriveLimit)
	ON_BN_CLICKED(IDC_BUTTON_ZDRIVE_CLEAR, &CSoftLimitDlg::OnBnClickedButtonZDriveClear)
END_MESSAGE_MAP()


// CSoftLimitDlg メッセージ ハンドラ
BOOL CSoftLimitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Z Drive limit ComboBox
	m_cboZDriveTarget.ResetContent();
	CString strUpper, strLower;
	strUpper.LoadString(IDS_LIMIT_UPPER);
	strLower.LoadString(IDS_LIMIT_LOWER);
	m_cboZDriveTarget.SetItemData(m_cboZDriveTarget.AddString(strUpper), E_Upper);
	m_cboZDriveTarget.SetItemData(m_cboZDriveTarget.AddString(strLower), E_Lower);
	m_cboZDriveTarget.SetCurSel(0);

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
	
	// Connect to Microscope and MipParam
	ConnectMipParam();

	return TRUE;
}

void CSoftLimitDlg::OnDestroy()
{
	DisconnectMipParam();

	m_bmpZDriveU.DeleteObject();
	m_bmpZDriveUu.DeleteObject();
	m_bmpZDriveD.DeleteObject();
	m_bmpZDriveDd.DeleteObject();

	CDialog::OnDestroy();
}

HRESULT CSoftLimitDlg::ConnectMipParam()
{
	HRESULT hr = S_OK;

	try {
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();

		VARIANT val;

		CComPtr<IZDrive> pZDrive = NULL;

		if (FAILED(hr = pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		if (m_pMicroscope->m_bZDriveMounted) {
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

			UpdateZDriveLimit();
			UpdateZDriveRange();

			m_bSupportedAF = pZDrive->IsSupportedAF ? TRUE : FALSE;
			GetDlgItem(IDC_COMBO_ZDRIVE_SEARCH)->EnableWindow(m_bSupportedAF ? TRUE : FALSE);

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
			UpdateZDrivePos();
			UpdateZDriveEnable();
		}
		else {
			GetDlgItem(IDC_STATIC_FRM_ZDRIVE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_SLIDER_ZDRIVE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC_ZDRIVE_MIN)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC_ZDRIVE_MAX)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC_ZDRIVE_VALUE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_ZDRIVE_U)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_ZDRIVE_UU)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_ZDRIVE_D)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_ZDRIVE_DD)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_ZDRIVE_MIN)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_ZDRIVE_MAX)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC_ZDRIVE_STEP)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_COMBO_ZDRIVE_STEP)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_AF)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_SEARCH)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_STOPSEARCH)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_COMBO_ZDRIVE_SEARCH)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_COMBO_ZDRIVE_SPEED)->ShowWindow(SW_HIDE);
		}
		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}

	return hr;
}

HRESULT CSoftLimitDlg::DisconnectMipParam()
{
	// Z Drive
	m_slZDrive.Disconnect();
	m_chkAF.Disconnect();
	m_btnSearch.Disconnect();

	return S_OK;
}

void CSoftLimitDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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
			m_pMicroscope->ExecZDriveSetSpeed(Speed1);
			m_slZDrive.PutValue();
		}
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

LRESULT CSoftLimitDlg::OnParamEvent(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)	{
	case EMIPPAR_VALUECHANGED:
		ValueChanged(wParam);
		break;
	case EMIPPAR_INFOCHANGED:
//		InfoChanged(wParam);
		ValueChanged(wParam);
		break;
	}
	return 0L;
}
void CSoftLimitDlg::ValueChanged(WPARAM wParam)
{
	switch (wParam)	{
	// Z Drive
	case IDC_SLIDER_ZDRIVE:
		UpdateZDrivePos();
		break;
	case IDC_BUTTON_AF:
	case IDC_BUTTON_SEARCH:
		UpdateZDriveAF();
		break;
	default:
		break;
	}
}
void CSoftLimitDlg::InfoChanged(WPARAM wParam)
{
}

// Z Drive
void CSoftLimitDlg::UpdateZDriveEnable()
{
	BOOL bEnable = m_pMicroscope->m_bZDriveMounted && !m_pMicroscope->IsAFController();
	GetDlgItem(IDC_BUTTON_ZDRIVE_CHANGE)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_ZDRIVE_RANGE)->EnableWindow(bEnable);
}
void CSoftLimitDlg::UpdateZDriveLimit()
{
	long lLower, lUpper;

	HRESULT hr = S_OK;
	try {
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();
		CComPtr<IZDrive> pZDrive = NULL;
		if (FAILED(hr = pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		lLower = (long)pZDrive->SoftwareLowerLimit;
		lUpper = (long)pZDrive->SoftwareUpperLimit;

		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
		return;
	}

	CString strLower, strUpper;
	strLower.Format(_T("%d"), lLower / m_lZDriveUnit);
	GetDlgItem(IDC_STATIC_ZDRIVE_LOWERLIMIT)->SetWindowText(strLower);
	strUpper.Format(_T("%d"), lUpper / m_lZDriveUnit);
	GetDlgItem(IDC_STATIC_ZDRIVE_UPPERLIMIT)->SetWindowText(strUpper);
}
void CSoftLimitDlg::UpdateZDriveRange()
{
	m_slZDrive.SetUserRange(m_pMicroscope->GetZDriveMin(), m_pMicroscope->GetZDriveMax(), TRUE);
	m_slZDrive.SetTicFreq((m_pMicroscope->GetZDriveMax() - m_pMicroscope->GetZDriveMin()) / 10);
	CString strMin, strMax;
	strMin.Format(_T("%d"), m_slZDrive.GetUserRangeMin() / m_lZDriveUnit);
	GetDlgItem(IDC_STATIC_ZDRIVE_MIN)->SetWindowText(strMin);
	strMax.Format(_T("%d"), m_slZDrive.GetUserRangeMax() / m_lZDriveUnit);
	GetDlgItem(IDC_STATIC_ZDRIVE_MAX)->SetWindowText(strMax);
}
void CSoftLimitDlg::UpdateZDrivePos()
{
	VARIANT val;
	if (m_slZDrive.GetValue(&val)) {
		if (val.vt == VT_I4) {
			CString str;
			str.Format(_T("%.2lf %s"), (double)val.lVal / (double)m_lZDriveUnit, m_strZDriveUnit);
			GetDlgItem(IDC_STATIC_ZDRIVE_VALUE)->SetWindowText(str);

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
void CSoftLimitDlg::UpdateZDriveAF()
{
	if (m_bSupportedAF) {
		HRESULT hr = S_OK;
		try {
			INikonLv* pNikonLv = m_pMicroscope->GetInterface();

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
void CSoftLimitDlg::SetZDriveSpeedByStep()
{
	static const EnumSpeed aSpeed[4] = { Speed6, Speed4, Speed2, Speed1 };
	int nSel = m_cboZDriveStep.GetCurSel();
	if (nSel > -1 && nSel < 4)
		m_pMicroscope->ExecZDriveSetSpeed(aSpeed[nSel]);
}
void CSoftLimitDlg::SetZDriveSpeedContinuous()
{
	EnumSpeed eSpeed = m_eZDriveSpeed;
	if (eSpeed == Speed6) {
		if (m_pMicroscope->m_bNosepieceMounted && !m_pMicroscope->IsAFController()) {
			HRESULT hr = S_OK;
			try {
				INikonLv* pNikonLv = m_pMicroscope->GetInterface();

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
	m_pMicroscope->ExecZDriveSetSpeed(eSpeed);
}

void CSoftLimitDlg::OnBnClickedButtonZDriveU()
{
	CWaitCursor w;
	m_pMicroscope->ExecZDriveSetSpeed(Speed6);
	m_pMicroscope->ExecCommand(AccessoryZDrive, 1);
}
void CSoftLimitDlg::OnBnClickedButtonZDriveD()
{
	CWaitCursor w;
	m_pMicroscope->ExecZDriveSetSpeed(Speed6);
	m_pMicroscope->ExecCommand(AccessoryZDrive, -1);
}
void CSoftLimitDlg::OnBnClickedButtonZDriveUu()
{
	CWaitCursor w;
	SetZDriveSpeedByStep();
	int nMax = m_slZDrive.GetUserRangeMax();
	if (m_slZDrive.GetValuePos() + m_lZDriveStep > nMax)
		m_pMicroscope->ExecZDriveMoveToLimit(1);
	else
		m_pMicroscope->ExecCommand(AccessoryZDrive, m_lZDriveStep);
}
void CSoftLimitDlg::OnBnClickedButtonZDriveDd()
{
	CWaitCursor w;
	SetZDriveSpeedByStep();
	int nMin = m_slZDrive.GetUserRangeMin();
	if (m_slZDrive.GetValuePos() - m_lZDriveStep < nMin)
		m_pMicroscope->ExecZDriveMoveToLimit(0);
	else
		m_pMicroscope->ExecCommand(AccessoryZDrive, -m_lZDriveStep);
}
LRESULT CSoftLimitDlg::OnHoldButtonDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam) {
	case IDC_BUTTON_ZDRIVE_MAX:
		SetZDriveSpeedContinuous();
		m_pMicroscope->ExecZDriveMoveContinuous(1);
		break;
	case IDC_BUTTON_ZDRIVE_MIN:
		SetZDriveSpeedContinuous();
		m_pMicroscope->ExecZDriveMoveContinuous(0);
		break;
	default:
		break;
	}
	return 0;
}
LRESULT CSoftLimitDlg::OnHoldButtonUp(WPARAM wParam, LPARAM lParam)
{
	switch (lParam) {
	case IDC_BUTTON_ZDRIVE_MAX:
	case IDC_BUTTON_ZDRIVE_MIN:
		m_pMicroscope->ExecZDriveAbort();
		break;
	default:
		break;
	}
	return 0;
}
void CSoftLimitDlg::OnBnClickedButtonZDriveChange()
{
	CZDriveRangeDlg dlg(m_pMicroscope);
	if (dlg.DoModal() == IDOK)
		UpdateZDriveRange();
}
void CSoftLimitDlg::OnBnClickedButtonSetZDriveLimit()
{
	HRESULT hr = S_OK;
	CWaitCursor w;
	if (m_eLimitTarget == E_Upper)
		hr = m_pMicroscope->ExecZDriveSetSoftwareLimit(1);
	else if (m_eLimitTarget == E_Lower)
		hr = m_pMicroscope->ExecZDriveSetSoftwareLimit(0);
	if (SUCCEEDED(hr)) {
		UpdateZDriveLimit();
		UpdateZDriveRange();
	}
}
void CSoftLimitDlg::OnBnClickedButtonZDriveClear()
{
	HRESULT hr = S_OK;
	CWaitCursor w;
	if (m_eLimitTarget == E_Upper)
		hr = m_pMicroscope->ExecZDriveInitSoftwareLimit(1);
	else if (m_eLimitTarget == E_Lower)
		hr = m_pMicroscope->ExecZDriveInitSoftwareLimit(0);
	if (SUCCEEDED(hr)) {
		UpdateZDriveLimit();
		UpdateZDriveRange();
	}
}
void CSoftLimitDlg::OnCbnSelchangeComboZDriveTarget()
{
	int nSel = m_cboZDriveTarget.GetCurSel();
	if (nSel >= 0)
		m_eLimitTarget = (EnumLimit)m_cboZDriveTarget.GetItemData(nSel);
}

void CSoftLimitDlg::OnCbnSelchangeComboZDriveStep()
{
	int nSel = m_cboZDriveStep.GetCurSel();
	if (nSel >= 0)
		m_lZDriveStep = (long)m_cboZDriveStep.GetItemData(nSel);
}
void CSoftLimitDlg::OnCbnSelchangeComboZDriveSearch()
{
	int nSel = m_cboZDriveSearch.GetCurSel();
	if (nSel >= 0)
		m_eAfSearch = (EnumAfSearch)m_cboZDriveSearch.GetItemData(nSel);
}
void CSoftLimitDlg::OnCbnSelchangeComboZDriveSpeed()
{
	int nSel = m_cboZDriveSpeed.GetCurSel();
	if (nSel >= 0)
		m_eZDriveSpeed = (EnumSpeed)m_cboZDriveSpeed.GetItemData(nSel);
}
void CSoftLimitDlg::OnBnClickedButtonAF()
{
	if (m_bSupportedAF) {
		CWaitCursor w;
		m_pMicroscope->ExecZDriveAFStateChange(m_chkAF.GetCheck() == BST_CHECKED ? AfWait : AfRun);
		UpdateZDriveAF();
	}
}
void CSoftLimitDlg::OnBnClickedButtonSearch()
{
	if (m_bSupportedAF) {
		CWaitCursor w;
		GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(FALSE);
		m_pMicroscope->ExecZDriveSearchAF(m_eAfSearch);
//		UpdateZDriveAF();
	}
}
void CSoftLimitDlg::OnBnClickedButtonStopSearch()
{
	if (m_bSupportedAF) {
		CWaitCursor w;
		GetDlgItem(IDC_BUTTON_STOPSEARCH)->EnableWindow(FALSE);
		m_pMicroscope->ExecZDriveStopAF();
//		UpdateZDriveAF();
	}
}
