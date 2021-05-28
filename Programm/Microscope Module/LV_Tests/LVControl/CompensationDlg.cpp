// CompensationDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "LvSample.h"
#include "CompensationDlg.h"
#include "ZDriveRangeDlg.h"

static const DWORD stCompensation[] = { IDC_STATIC_NOSEPIECE1,
										IDC_STATIC_NOSEPIECE2,
										IDC_STATIC_NOSEPIECE3,
										IDC_STATIC_NOSEPIECE4,
										IDC_STATIC_NOSEPIECE5,
};

// CCompensationDlg ダイアログ

IMPLEMENT_DYNAMIC(CCompensationDlg, CDialog)


CCompensationDlg::CCompensationDlg(CMicroscopeControl* pMicroscope, CWnd* pParent /*=NULL*/)
	: CDialog(CCompensationDlg::IDD, pParent)
	, m_pMicroscope(pMicroscope)
	, m_lZDriveUnit(200)
	, m_lZDriveStep(20)
	, m_bSupportedAF(FALSE)
	, m_eAfSearch(AfSearchMode1)
	, m_eZDriveSpeed(Speed1)
{
}

CCompensationDlg::~CCompensationDlg()
{
}

void CCompensationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	// Z Drive
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
	// Nosepiece
	DDX_Control(pDX, IDC_BUTTON_NOSEPIECE1, m_chkNosepiece1);
	DDX_Control(pDX, IDC_BUTTON_NOSEPIECE2, m_chkNosepiece2);
	DDX_Control(pDX, IDC_BUTTON_NOSEPIECE3, m_chkNosepiece3);
	DDX_Control(pDX, IDC_BUTTON_NOSEPIECE4, m_chkNosepiece4);
	DDX_Control(pDX, IDC_BUTTON_NOSEPIECE5, m_chkNosepiece5);
}


BEGIN_MESSAGE_MAP(CCompensationDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
	ON_MESSAGE(WM_PAREVENT, &CCompensationDlg::OnParamEvent)
	// Z Drive
	ON_BN_CLICKED(IDC_BUTTON_ZDRIVE_U, &CCompensationDlg::OnBnClickedButtonZDriveU)
	ON_BN_CLICKED(IDC_BUTTON_ZDRIVE_UU, &CCompensationDlg::OnBnClickedButtonZDriveUu)
	ON_BN_CLICKED(IDC_BUTTON_ZDRIVE_D, &CCompensationDlg::OnBnClickedButtonZDriveD)
	ON_BN_CLICKED(IDC_BUTTON_ZDRIVE_DD, &CCompensationDlg::OnBnClickedButtonZDriveDd)
	ON_MESSAGE(WM_HOLD_BUTTON_DOWN, &CCompensationDlg::OnHoldButtonDown)
	ON_MESSAGE(WM_HOLD_BUTTON_UP, &CCompensationDlg::OnHoldButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_ZDRIVE_CHANGE, &CCompensationDlg::OnBnClickedButtonZDriveChange)
	ON_CBN_SELCHANGE(IDC_COMBO_ZDRIVE_STEP, &CCompensationDlg::OnCbnSelchangeComboZDriveStep)
	ON_CBN_SELCHANGE(IDC_COMBO_ZDRIVE_SEARCH, &CCompensationDlg::OnCbnSelchangeComboZDriveSearch)
	ON_CBN_SELCHANGE(IDC_COMBO_ZDRIVE_SPEED, &CCompensationDlg::OnCbnSelchangeComboZDriveSpeed)
	ON_BN_CLICKED(IDC_BUTTON_AF, &CCompensationDlg::OnBnClickedButtonAF)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CCompensationDlg::OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_STOPSEARCH, &CCompensationDlg::OnBnClickedButtonStopSearch)
	// Nosepiece
	ON_BN_CLICKED(IDC_BUTTON_NOSEPIECE1, &CCompensationDlg::OnBnClickedButtonNosepiece1)
	ON_BN_CLICKED(IDC_BUTTON_NOSEPIECE2, &CCompensationDlg::OnBnClickedButtonNosepiece2)
	ON_BN_CLICKED(IDC_BUTTON_NOSEPIECE3, &CCompensationDlg::OnBnClickedButtonNosepiece3)
	ON_BN_CLICKED(IDC_BUTTON_NOSEPIECE4, &CCompensationDlg::OnBnClickedButtonNosepiece4)
	ON_BN_CLICKED(IDC_BUTTON_NOSEPIECE5, &CCompensationDlg::OnBnClickedButtonNosepiece5)
	// Momorize
	ON_BN_CLICKED(IDC_BUTTON_NOSEPIECE_MEMORIZE, &CCompensationDlg::OnBnClickedButtonNosepieceMemorize)
	// Initialize
	ON_BN_CLICKED(IDC_BUTTON_NOSEPIECE_CLEAR, &CCompensationDlg::OnBnClickedButtonNosepieceClear)
END_MESSAGE_MAP()


// CCompensationDlg メッセージ ハンドラ
BOOL CCompensationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	// Nosepiece select button
	m_chkNosepiece[0] = &m_chkNosepiece1;
	m_chkNosepiece[1] = &m_chkNosepiece2;
	m_chkNosepiece[2] = &m_chkNosepiece3;
	m_chkNosepiece[3] = &m_chkNosepiece4;
	m_chkNosepiece[4] = &m_chkNosepiece5;

	// Connect to Microscope and MipParam
	ConnectMipParam();

	return TRUE;
}

void CCompensationDlg::OnDestroy()
{
	DisconnectMipParam();

	m_bmpZDriveU.DeleteObject();
	m_bmpZDriveUu.DeleteObject();
	m_bmpZDriveD.DeleteObject();
	m_bmpZDriveDd.DeleteObject();

	CDialog::OnDestroy();
}

HRESULT CCompensationDlg::ConnectMipParam()
{
	HRESULT hr = S_OK;

	try {
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();

		VARIANT val;

		CComPtr<IZDrive> pZDrive = NULL;
		CComPtr<INosepiece> pNosepiece = NULL;

		// Z Drive
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

		// Nosepiece
		if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
			_com_raise_error(hr);

		if (m_pMicroscope->m_bNosepieceMounted) {
			UpdateNosepiece();
			for (int i = 0; i < 5; i++)
				UpdateCompensation(i+1);
		}
		else {
			for (int i = 0; i < 5; i++) {
				m_chkNosepiece[i]->EnableWindow(FALSE);
				UpdateCompensation(i+1);
				GetDlgItem(stCompensation[i])->EnableWindow(FALSE);
			}
		}

		if (pZDrive)
			pZDrive.Release();
		if (pNosepiece)
			pNosepiece.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}

	return hr;
}

HRESULT CCompensationDlg::DisconnectMipParam()
{
	// Z Drive
	m_slZDrive.Disconnect();
	m_chkAF.Disconnect();
	m_btnSearch.Disconnect();

	return S_OK;
}

void CCompensationDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

LRESULT CCompensationDlg::OnParamEvent(WPARAM wParam, LPARAM lParam)
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
void CCompensationDlg::ValueChanged(WPARAM wParam)
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
void CCompensationDlg::InfoChanged(WPARAM wParam)
{
}

// Z Drive
void CCompensationDlg::UpdateZDriveEnable()
{
	BOOL bEnable = m_pMicroscope->m_bZDriveMounted && !m_pMicroscope->IsAFController();
	GetDlgItem(IDC_BUTTON_ZDRIVE_CHANGE)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_ZDRIVE_RANGE)->EnableWindow(bEnable);
}
void CCompensationDlg::UpdateZDriveRange()
{
	m_slZDrive.SetUserRange(m_pMicroscope->GetZDriveMin(), m_pMicroscope->GetZDriveMax(), TRUE);
	m_slZDrive.SetTicFreq((m_pMicroscope->GetZDriveMax() - m_pMicroscope->GetZDriveMin()) / 10);
	CString strMin, strMax;
	strMin.Format(_T("%d"), m_slZDrive.GetUserRangeMin() / m_lZDriveUnit);
	GetDlgItem(IDC_STATIC_ZDRIVE_MIN)->SetWindowText(strMin);
	strMax.Format(_T("%d"), m_slZDrive.GetUserRangeMax() / m_lZDriveUnit);
	GetDlgItem(IDC_STATIC_ZDRIVE_MAX)->SetWindowText(strMax);
}
void CCompensationDlg::UpdateZDrivePos()
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
void CCompensationDlg::UpdateZDriveAF()
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
void CCompensationDlg::SetZDriveSpeedByStep()
{
	static const EnumSpeed aSpeed[4] = { Speed6, Speed4, Speed2, Speed1 };
	int nSel = m_cboZDriveStep.GetCurSel();
	if (nSel > -1 && nSel < 4)
		m_pMicroscope->ExecZDriveSetSpeed(aSpeed[nSel]);
}
void CCompensationDlg::SetZDriveSpeedContinuous()
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

// Nosepiece
void CCompensationDlg::SetNosepiece(int nIndex)
{
	CWaitCursor w;
	HRESULT hr = S_OK;
	try {
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();
		CComPtr<INosepiece> pNosepiece = NULL;
		if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
			_com_raise_error(hr);

		if ((long)pNosepiece->Value != nIndex) {
			if (FAILED(hr = pNosepiece->EnableInterlock()))
				_com_raise_error(hr);
			if (FAILED(hr = m_pMicroscope->ExecSelectCommand(AccessoryNosepiece, nIndex)))
				_com_raise_error(hr);
			UpdateNosepiece();
		}

		if (pNosepiece)
			pNosepiece.Release();
	}
	catch (_com_error/* &e*/) {
		RetryNosepieceCommand(nIndex);
		UpdateNosepiece();
//		AfxMessageBox(_T("Microscope error!"));
	}
}
void CCompensationDlg::UpdateNosepiece()
{
	if (m_pMicroscope->m_bNosepieceMounted) {
		HRESULT hr = S_OK;
		try {
			INikonLv* pNikonLv = m_pMicroscope->GetInterface();
			CComPtr<INosepiece> pNosepiece = NULL;
			if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
				_com_raise_error(hr);

			long lVal = (long)pNosepiece->Value - 1;
			for (int i = 0; i < 5; i++) {
				if (i == lVal) {
					if (m_chkNosepiece[i]->GetCheck() == BST_UNCHECKED)
						m_chkNosepiece[i]->SetCheck(BST_CHECKED);
				}
				else {
					if (m_chkNosepiece[i]->GetCheck() == BST_CHECKED)
						m_chkNosepiece[i]->SetCheck(BST_UNCHECKED);
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
void CCompensationDlg::UpdateCompensation(int nIndex)
{
	if (nIndex < 1 || nIndex > 5)
		return;

	HRESULT hr = S_OK;
	try {
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();
		CComPtr<IPresets> pPresets = NULL;
		if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
			_com_raise_error(hr);

		double dVal;
		dVal = pPresets->ReadMemorizedPresets(AccessoryZDrive, ObservationModeNone, nIndex);
		CString str;
		if ((int)dVal < m_pMicroscope->GetZDriveMin())
			str = _T("------------");
		else
			str.Format(_T("%.2f%s"), dVal / (double)m_lZDriveUnit, m_strZDriveUnit);
		GetDlgItem(stCompensation[nIndex - 1])->SetWindowText(str);

		if (pPresets)
			pPresets.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}
void CCompensationDlg::RetryNosepieceCommand(int nIndex)
{
	if (nIndex < 1 || nIndex > 5)
		return;

	HRESULT hr = S_OK;
	try {
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();
		CComPtr<INosepiece> pNosepiece = NULL;
		if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
			_com_raise_error(hr);

		if (pNosepiece->IsNosepieceControlError) {
			if (m_pMicroscope->m_bConfirm) {
				if (AfxMessageBox(IDS_MESSAGE_COMPULSORILY_ROTATED, MB_YESNO) == IDYES) {
					CWaitCursor w;
					if (FAILED(hr = pNosepiece->DisableInterlock()))
						_com_raise_error(hr);
					if (FAILED(hr = m_pMicroscope->ExecSelectCommand(AccessoryNosepiece, nIndex)))
						_com_raise_error(hr);
				}
			}
			else {
				AfxMessageBox(IDS_MESSAGE_ROTATION_FAILED);
			}
		}

		if (pNosepiece)
			pNosepiece.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}
// Momorize/Initialize
void CCompensationDlg::UpdateMemorizedPresetsEx(BOOL bIsInit)
{
	if (m_pMicroscope->m_bNosepieceMounted) {
		CWaitCursor w;

		HRESULT hr = S_OK;
		try {
			INikonLv* pNikonLv = m_pMicroscope->GetInterface();
			CComPtr<IPresets> pPresets = NULL;
			CComPtr<INosepiece> pNosepiece = NULL;

			if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
				_com_raise_error(hr);
			if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
				_com_raise_error(hr);

			long lVal = (long)pNosepiece->Value;

			VARIANT val;
			IMipParameterPtr spiParam;
			if (FAILED(hr = pPresets->get_ObservationMode(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);

			for (int i = 1; i <= spiParam->EnumSet->Count; i++) {
				EnumObservationMode eMode = (EnumObservationMode)spiParam->EnumSet->GetItem(i).RawValue.lVal;
				if (eMode != ObservationModeNone && eMode != ObservationModeUnknown) {
					if (bIsInit) {
						if (FAILED(hr = pPresets->InitializeMemorizedPresetsEx(AccessoryZDrive, eMode, lVal)))
							_com_raise_error(hr);
					}
					else {
						if (FAILED(hr = pPresets->AdjustMemorizedPresetsEx(AccessoryZDrive, eMode, lVal)))
							_com_raise_error(hr);
					}
				}
			}
			UpdateCompensation(lVal);

			if (spiParam)
				spiParam.Release();
			if (pNosepiece)
				pNosepiece.Release();
			if (pPresets)
				pPresets.Release();
		}
		catch (_com_error/* &e*/) {
			AfxMessageBox(_T("Microscope error!"));
		}
	}
}

// Z Drive
void CCompensationDlg::OnBnClickedButtonZDriveU()
{
	CWaitCursor w;
	m_pMicroscope->ExecZDriveSetSpeed(Speed6);
	m_pMicroscope->ExecCommand(AccessoryZDrive, 1);
}
void CCompensationDlg::OnBnClickedButtonZDriveD()
{
	CWaitCursor w;
	m_pMicroscope->ExecZDriveSetSpeed(Speed6);
	m_pMicroscope->ExecCommand(AccessoryZDrive, -1);
}
void CCompensationDlg::OnBnClickedButtonZDriveUu()
{
	CWaitCursor w;
	SetZDriveSpeedByStep();
	int nMax = m_slZDrive.GetUserRangeMax();
	if (m_slZDrive.GetValuePos() + m_lZDriveStep > nMax)
		m_pMicroscope->ExecZDriveMoveToLimit(1);
	else
		m_pMicroscope->ExecCommand(AccessoryZDrive, m_lZDriveStep);
}
void CCompensationDlg::OnBnClickedButtonZDriveDd()
{
	CWaitCursor w;
	SetZDriveSpeedByStep();
	int nMin = m_slZDrive.GetUserRangeMin();
	if (m_slZDrive.GetValuePos() - m_lZDriveStep < nMin)
		m_pMicroscope->ExecZDriveMoveToLimit(0);
	else
		m_pMicroscope->ExecCommand(AccessoryZDrive, -m_lZDriveStep);
}
LRESULT CCompensationDlg::OnHoldButtonDown(WPARAM wParam, LPARAM lParam)
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
LRESULT CCompensationDlg::OnHoldButtonUp(WPARAM wParam, LPARAM lParam)
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
void CCompensationDlg::OnBnClickedButtonZDriveChange()
{
	CZDriveRangeDlg dlg(m_pMicroscope);
	if (dlg.DoModal() == IDOK)
		UpdateZDriveRange();
}
void CCompensationDlg::OnCbnSelchangeComboZDriveStep()
{
	int nSel = m_cboZDriveStep.GetCurSel();
	if (nSel >= 0)
		m_lZDriveStep = (long)m_cboZDriveStep.GetItemData(nSel);
}
void CCompensationDlg::OnCbnSelchangeComboZDriveSearch()
{
	int nSel = m_cboZDriveSearch.GetCurSel();
	if (nSel >= 0)
		m_eAfSearch = (EnumAfSearch)m_cboZDriveSearch.GetItemData(nSel);
}
void CCompensationDlg::OnCbnSelchangeComboZDriveSpeed()
{
	int nSel = m_cboZDriveSpeed.GetCurSel();
	if (nSel >= 0)
		m_eZDriveSpeed = (EnumSpeed)m_cboZDriveSpeed.GetItemData(nSel);
}
void CCompensationDlg::OnBnClickedButtonAF()
{
	if (m_bSupportedAF) {
		CWaitCursor w;
		m_pMicroscope->ExecZDriveAFStateChange(m_chkAF.GetCheck() == BST_CHECKED ? AfWait : AfRun);
		UpdateZDriveAF();
	}
}
void CCompensationDlg::OnBnClickedButtonSearch()
{
	if (m_bSupportedAF) {
		CWaitCursor w;
		GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(FALSE);
		m_pMicroscope->ExecZDriveSearchAF(m_eAfSearch);
//		UpdateZDriveAF();
	}
}
void CCompensationDlg::OnBnClickedButtonStopSearch()
{
	if (m_bSupportedAF) {
		CWaitCursor w;
		GetDlgItem(IDC_BUTTON_STOPSEARCH)->EnableWindow(FALSE);
		m_pMicroscope->ExecZDriveStopAF();
//		UpdateZDriveAF();
	}
}
// Nosepiece
void CCompensationDlg::OnBnClickedButtonNosepiece1()
{
	SetNosepiece(1);
}
void CCompensationDlg::OnBnClickedButtonNosepiece2()
{
	SetNosepiece(2);
}
void CCompensationDlg::OnBnClickedButtonNosepiece3()
{
	SetNosepiece(3);
}
void CCompensationDlg::OnBnClickedButtonNosepiece4()
{
	SetNosepiece(4);
}
void CCompensationDlg::OnBnClickedButtonNosepiece5()
{
	SetNosepiece(5);
}
// Momorize
void CCompensationDlg::OnBnClickedButtonNosepieceMemorize()
{
	UpdateMemorizedPresetsEx(FALSE);
}
// Initialize
void CCompensationDlg::OnBnClickedButtonNosepieceClear()
{
	UpdateMemorizedPresetsEx(TRUE);
}
