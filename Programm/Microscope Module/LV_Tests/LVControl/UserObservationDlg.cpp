// UserObservationDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "LvSample.h"
#include "UserObservationDlg.h"
#include "Util.h"

// CUserObservationDlg ダイアログ

IMPLEMENT_DYNAMIC(CUserObservationDlg, CDialog)

CUserObservationDlg::CUserObservationDlg(CMicroscopeControl* pMicroscope, CWnd* pParent /*=NULL*/)
	: CDialog(CUserObservationDlg::IDD, pParent)
	, m_pMicroscope(pMicroscope)
	, m_bEpiDia(FALSE)
	, m_bEpi(FALSE)
	, m_bDia(FALSE)
{

}

CUserObservationDlg::~CUserObservationDlg()
{
}

void CUserObservationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_OBSERVATION_MODE, m_cboObservation);
	DDX_Control(pDX, IDC_COMBO_OPTIONAL_MODE, m_cboOptional);
	DDX_Control(pDX, IDC_COMBO_NOSEPIECE, m_cboNosepiece);
	DDX_Control(pDX, IDC_RADIO_INTERLOCK_ON, m_rdInterlockOn);
	DDX_Control(pDX, IDC_RADIO_INTERLOCK_OFF, m_rdInterlockOff);
}


BEGIN_MESSAGE_MAP(CUserObservationDlg, CDialog)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_OBSERVATION_MODE, &CUserObservationDlg::OnCbnSelchangeComboObservation)
	ON_CBN_SELCHANGE(IDC_COMBO_OPTIONAL_MODE, &CUserObservationDlg::OnCbnSelchangeComboOptional)
	ON_CBN_SELCHANGE(IDC_COMBO_NOSEPIECE, &CUserObservationDlg::OnCbnSelchangeComboNosepiece)
	ON_BN_CLICKED(IDC_RADIO_INTERLOCK_ON, &CUserObservationDlg::OnBnClickedRadioInterlockOn)
	ON_BN_CLICKED(IDC_RADIO_INTERLOCK_OFF, &CUserObservationDlg::OnBnClickedRadioInterlockOff)
	ON_BN_CLICKED(IDC_BUTTON_USERMEMORY_CURRENT, &CUserObservationDlg::OnBnClickedButtonUserMemory)
	ON_BN_CLICKED(IDC_BUTTON_USERMEMORY_INIT, &CUserObservationDlg::OnBnClickedButtonUserMemoryInit)
END_MESSAGE_MAP()


// CUserObservationDlg メッセージ ハンドラ
BOOL CUserObservationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ConnectMipParam();

	return TRUE;
}
void CUserObservationDlg::OnDestroy()
{
	DisconnectMipParam();

	CDialog::OnDestroy();
}

HRESULT CUserObservationDlg::ConnectMipParam()
{
	HRESULT hr = S_OK;

	try {
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();

		int i;
		CString str;
		VARIANT val;
		IMipParameterPtr spiParam;
		EnumEpiDiaLampMode eLampMode;

		CComPtr<IPresets> pPresets = NULL;
		CComPtr<IEpiLamp> pEpiLamp = NULL;
		CComPtr<IEpiShutter> pEpiShutter = NULL;
		CComPtr<IDiaLamp> pDiaLamp = NULL;
		CComPtr<INosepiece> pNosepiece = NULL;

		// LampMode
		if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
			_com_raise_error(hr);
		eLampMode = (EnumEpiDiaLampMode)((long)pPresets->EpiDiaLampMode);

		// EPI
		if (FAILED(hr = pNikonLv->get_EpiLamp(&pEpiLamp)))
			_com_raise_error(hr);

		// Fiber(Epi Sutter)
		if (FAILED(hr = pNikonLv->get_EpiShutter(&pEpiShutter)))
			_com_raise_error(hr);

		// DIA
		if (FAILED(hr = pNikonLv->get_DiaLamp(&pDiaLamp)))
			_com_raise_error(hr);

		m_bEpiDia = (m_pMicroscope->m_bEpiLampMounted || m_pMicroscope->m_bFiberMounted) && m_pMicroscope->m_bDiaLampMounted && (eLampMode == EpiDiaEnable);
		m_bEpi = (m_pMicroscope->m_bEpiLampMounted || m_pMicroscope->m_bFiberMounted) && (eLampMode != DiaEnable);
		m_bDia = m_pMicroscope->m_bDiaLampMounted && (eLampMode != EpiEnable);

		// Observation
		if (FAILED(hr = pPresets->get_ObservationMode(&val)))
			_com_raise_error(hr);
		if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
			_com_raise_error(hr);

		m_cboObservation.ResetContent();
		for (i = 1; i <= (long)spiParam->EnumSet->Count; i++) {
			EnumObservationMode eMode = (EnumObservationMode)spiParam->EnumSet->GetItem(i).RawValue.lVal;
			if (eMode != ObservationModeNone && eMode != ObservationModeUnknown) {
				str.Format(_T("%s"), spiParam->EnumSet->GetItem(i).DisplayString);
				m_cboObservation.SetItemData(m_cboObservation.AddString(str), eMode);
			}
		}

		// Nosepiece
		if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
			_com_raise_error(hr);

		m_cboNosepiece.ResetContent();
		m_cboNosepiece.SetItemData(m_cboNosepiece.AddString(_T("--------")), 0);
		if (m_pMicroscope->m_bNosepieceMounted) {
			for (i = (long)pNosepiece->LowerLimit; i <= (long)pNosepiece->UpperLimit; i++) {
				str.Format(_T("%d"), i);
				m_cboNosepiece.SetItemData(m_cboNosepiece.AddString(str), i);
			}
		}

		UpdateObservationMode();
		UpdateOptionalInformation();
		UpdateOptionalMode();
		ControlUnknown();

		if (spiParam)
			spiParam.Release();
		if (pPresets)
			pPresets.Release();
		if (pEpiLamp)
			pEpiLamp.Release();
		if (pEpiShutter)
			pEpiShutter.Release();
		if (pDiaLamp)
			pDiaLamp.Release();
		if (pNosepiece)
			pNosepiece.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}

	return hr;
}
HRESULT CUserObservationDlg::DisconnectMipParam()
{
	return S_OK;
}

void CUserObservationDlg::UpdateObservationMode()
{
	HRESULT hr = S_OK;

	try {
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();

		VARIANT val;
		IMipParameterPtr spiParam;

		CComPtr<IPresets> pPresets = NULL;

		if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
			_com_raise_error(hr);
		if (FAILED(hr = pPresets->get_ObservationMode(&val)))
			_com_raise_error(hr);
		if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
			_com_raise_error(hr);

		EnumObservationMode eMode = (EnumObservationMode)((long)pPresets->ObservationMode);
		for (int i = 0; i < m_cboObservation.GetCount(); i++) {
			if ((EnumObservationMode)m_cboObservation.GetItemData(i) == eMode) {
				if (m_cboObservation.GetCurSel() != i) {
					m_cboObservation.SetCurSel(i);
					break;
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
void CUserObservationDlg::UpdateOptionalInformation()
{
	int nSel = m_cboObservation.GetCurSel();
	if (nSel < 0)
		return;
	EnumObservationMode eObservation = (EnumObservationMode)m_cboObservation.GetItemData(nSel);

	CString strHead;
	m_cboOptional.ResetContent();

	if (eObservation >= BrightField && eObservation <= Fluorescence2) {
		strHead = CUtil::GetObservationShortString(eObservation);
		MakeOptionalMode(strHead, ObservationModeNone);
	}
	else {
		m_cboOptional.SetItemData(m_cboOptional.AddString(CUtil::GetOptionalObservationString(OptionalObservationModeNone)), OptionalObservationModeNone);
		HRESULT hr = S_OK;
		try {
			INikonLv* pNikonLv = m_pMicroscope->GetInterface();

			VARIANT val;
			IMipParameterPtr spiParam;
			CComPtr<IPresets> pPresets = NULL;

			if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
				_com_raise_error(hr);
			if (FAILED(hr = pPresets->get_ObservationMode(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);

			for (int i = 0; i < m_cboObservation.GetCount(); i++) {
				EnumObservationMode eMode = (EnumObservationMode)m_cboObservation.GetItemData(i);
				if (eMode >= BrightField && eMode <= Fluorescence2) {
					strHead = CUtil::GetObservationShortString(eMode);
					MakeOptionalMode(strHead, eMode);
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
}
void CUserObservationDlg::UpdateOptionalMode()
{
	int nSel = m_cboObservation.GetCurSel();
	if (nSel < 0)
		return;
	EnumObservationMode eObservation = (EnumObservationMode)m_cboObservation.GetItemData(nSel);

	HRESULT hr = S_OK;
	try {
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();

		int i;
		long lVal;
		BOOL bFound = FALSE;
		EnumLampPresets eLampPresets;
		EnumOptionalObservationMode eOptional;
		EnumStatus eInterlock;
		long lNosepiece;

		CComPtr<IPresets> pPresets = NULL;
		if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
			_com_raise_error(hr);

		if (eObservation < ObservationMode1) {
			pPresets->ReadLampPresets(eObservation, &eLampPresets);
			lVal = (long)eLampPresets;

			for (i = 0; i < m_cboOptional.GetCount(); i++) {
				if (m_cboOptional.GetItemData(i) == lVal) {
					if (m_cboOptional.GetCurSel() != i) {
						m_cboOptional.SetCurSel(i);
						bFound = TRUE;
					}
					break;
				}
			}
			if (!bFound) {
				if (m_cboOptional.GetCurSel() > -1)
					m_cboOptional.SetCurSel(-1);
			}
			m_rdInterlockOn.SetCheck(BST_UNCHECKED);
			m_rdInterlockOff.SetCheck(BST_CHECKED);
			if (m_cboNosepiece.GetCurSel() != 0)
				m_cboNosepiece.SetCurSel(0);
		}
		else {
			pPresets->ReadLampPresets(eObservation, &eLampPresets);
			pPresets->ReadPresetModesEx(eObservation, &eOptional, &eInterlock, &lNosepiece);
			lVal = 100 * (long)eOptional + (long)eLampPresets;

			for (i = 0; i < m_cboOptional.GetCount(); i++) {
				if (m_cboOptional.GetItemData(i) == lVal) {
					if (m_cboOptional.GetCurSel() != i) {
						m_cboOptional.SetCurSel(i);
						bFound = TRUE;
					}
					break;
				}
			}
			if (!bFound) {
				if (m_cboOptional.GetCurSel() > -1)
					m_cboOptional.SetCurSel(-1);
				eInterlock = StatusFalse;
			}

			if (eInterlock == StatusTrue) {
				m_rdInterlockOn.SetCheck(BST_CHECKED);
				m_rdInterlockOff.SetCheck(BST_UNCHECKED);
			}
			else {
				m_rdInterlockOn.SetCheck(BST_UNCHECKED);
				m_rdInterlockOff.SetCheck(BST_CHECKED);
			}

			bFound = FALSE;
			for (i = 0; i < m_cboNosepiece.GetCount(); i++) {
				if (m_cboNosepiece.GetItemData(i) == lNosepiece) {
					if (m_cboNosepiece.GetCurSel() != i) {
						m_cboNosepiece.SetCurSel(i);
						bFound = TRUE;
					}
					break;
				}
			}
			if (!bFound) {
				if (m_cboNosepiece.GetCurSel() != 0)
					m_cboNosepiece.SetCurSel(0);
			}
		}

		if (pPresets)
			pPresets.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}
void CUserObservationDlg::SetOptionalMode()
{
	int nSel = m_cboObservation.GetCurSel();
	if (nSel < 0)
		return;
	EnumObservationMode eObservation = (EnumObservationMode)m_cboObservation.GetItemData(nSel);

	nSel = m_cboOptional.GetCurSel();
	if (nSel < 0)
		return;
	int nItemData = (int)m_cboOptional.GetItemData(nSel);
	EnumOptionalObservationMode eOptional = (EnumOptionalObservationMode)(nItemData / 100);
	EnumLampPresets eLampPresets = (EnumLampPresets)(nItemData % 100);

	nSel = m_cboNosepiece.GetCurSel();
	if (nSel < 0)
		return;
	long lNosepiece = (long)m_cboNosepiece.GetItemData(nSel);

	EnumStatus eInterlock;
	if (m_rdInterlockOn.GetCheck() == BST_CHECKED)
		eInterlock = StatusTrue;
	else
		eInterlock = StatusFalse;

	HRESULT hr = S_OK;

	try {
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();

		CComPtr<IPresets> pPresets = NULL;
		if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
			_com_raise_error(hr);

		if (eObservation < ObservationMode1) {
			if (FAILED(hr = pPresets->SetLampPresets(eObservation, eLampPresets)))
				_com_raise_error(hr);
		}
		else {
			if (FAILED(hr = pPresets->SetPresetModesEx(eObservation, eOptional, eInterlock, lNosepiece)))
				_com_raise_error(hr);
			if (FAILED(hr = pPresets->SetLampPresets(eObservation, eLampPresets)))
				_com_raise_error(hr);
		}

		if (pPresets)
			pPresets.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}
void CUserObservationDlg::MemorizePresets(BOOL bInit)
{
	int nSel = m_cboObservation.GetCurSel();
	if (nSel < 0)
		return;
	EnumObservationMode eObservation = (EnumObservationMode)m_cboObservation.GetItemData(nSel);

	HRESULT hr = S_OK;
	try {
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();

		CComPtr<IPresets> pPresets = NULL;
		CComPtr<IDiaLamp> pDiaLamp = NULL;
		CComPtr<IEpiLamp> pEpiLamp = NULL;
		CComPtr<IEpiShutter> pEpiShutter = NULL;
		CComPtr<IEpiApertureStop> pEpiApertureStop = NULL;
		CComPtr<IDICPrism> pDicPrism = NULL;

		if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
			_com_raise_error(hr);

		if (m_pMicroscope->m_bDiaLampMounted) {
			if (FAILED(hr = pNikonLv->get_DiaLamp(&pDiaLamp)))
				_com_raise_error(hr);

			if (bInit) {
				if (FAILED(hr = pPresets->InitializeMemorizedPresetsEx(AccessoryDiaLampVoltage, eObservation, 0)))
					_com_raise_error(hr);
			}
			else {
				if (FAILED(hr = pPresets->AdjustMemorizedPresetsEx(AccessoryDiaLampVoltage, eObservation, 0)))
					_com_raise_error(hr);
			}
		}

		if (m_pMicroscope->m_bEpiLampMounted || m_pMicroscope->m_bFiberMounted) {
			if (FAILED(hr = pNikonLv->get_EpiLamp(&pEpiLamp)))
				_com_raise_error(hr);
			if (FAILED(hr = pNikonLv->get_EpiShutter(&pEpiShutter)))
				_com_raise_error(hr);

			if (bInit) {
				if (FAILED(hr = pPresets->InitializeMemorizedPresetsEx(AccessoryEpiLampVoltage, eObservation, 0)))
					_com_raise_error(hr);
			}
			else {
				if (FAILED(hr = pPresets->AdjustMemorizedPresetsEx(AccessoryEpiLampVoltage, eObservation, 0)))
					_com_raise_error(hr);
			}
		}

		if (eObservation != DarkField && m_pMicroscope->m_bApertureMounted) {
			if (FAILED(hr = pNikonLv->get_EpiApertureStop(&pEpiApertureStop)))
				_com_raise_error(hr);

			if (bInit) {
				if (FAILED(hr = pPresets->InitializeMemorizedPresetsEx(AccessoryEpiApertureStop, eObservation, 0)))
					_com_raise_error(hr);
			}
			else {
				if (FAILED(hr = pPresets->AdjustMemorizedPresetsEx(AccessoryEpiApertureStop, eObservation, 0)))
					_com_raise_error(hr);
			}
		}

		if (m_pMicroscope->m_bDicMounted || IsObservationDic()) {
			if (FAILED(hr = pNikonLv->get_DICPrism(&pDicPrism)))
				_com_raise_error(hr);

			if (bInit) {
				if (FAILED(hr = pPresets->InitializeMemorizedPresetsEx(AccessoryDICPrismShift, eObservation, 0)))
					_com_raise_error(hr);
			}
			else {
				if (FAILED(hr = pPresets->AdjustMemorizedPresetsEx(AccessoryDICPrismShift, eObservation, 0)))
					_com_raise_error(hr);
			}
		}

		if (pPresets)
			pPresets.Release();
		if (pDiaLamp)
			pDiaLamp.Release();
		if (pEpiLamp)
			pEpiLamp.Release();
		if (pEpiShutter)
			pEpiShutter.Release();
		if (pEpiApertureStop)
			pEpiApertureStop.Release();
		if (pDicPrism)
			pDicPrism.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}
void CUserObservationDlg::ControlUnknown()
{
	BOOL bObservation, bOptional, bUserMemory;

	if ((EnumObservationMode)m_cboObservation.GetItemData(m_cboObservation.GetCurSel()) < ObservationMode1) {
		bObservation = FALSE;
		bOptional = TRUE;
	}
	else {
		bObservation = TRUE;
		bOptional = m_cboOptional.GetCurSel() > 0;
	}

	GetDlgItem(IDC_STATIC_INTERLOCK)->EnableWindow(bObservation && bOptional);
	GetDlgItem(IDC_RADIO_INTERLOCK_ON)->EnableWindow(bObservation && bOptional);
	GetDlgItem(IDC_RADIO_INTERLOCK_OFF)->EnableWindow(bObservation && bOptional);
	GetDlgItem(IDC_COMBO_NOSEPIECE)->EnableWindow(bObservation && bOptional);

	GetDlgItem(IDC_BUTTON_USERMEMORY_INIT)->EnableWindow(bOptional);
	bUserMemory = bOptional;
	if (m_pMicroscope->m_bDicMounted && IsObservationDic()) {
		HRESULT hr = S_OK;
		try {
			INikonLv* pNikonLv = m_pMicroscope->GetInterface();
			CComPtr<IDICPrism> pDicPrism = NULL;
			if (FAILED(hr = pNikonLv->get_DICPrism(&pDicPrism)))
				_com_raise_error(hr);

			if ((EnumStatus)((long)pDicPrism->IsOutOfPosition) == StatusTrue)
				bUserMemory = FALSE;

			if (pDicPrism)
				pDicPrism.Release();
		}
		catch (_com_error/* &e*/) {
			AfxMessageBox(_T("Microscope error!"));
		}
	}
	GetDlgItem(IDC_BUTTON_USERMEMORY_CURRENT)->EnableWindow(bUserMemory);
}
void CUserObservationDlg::MakeOptionalMode(CString strHead, EnumObservationMode eOptional)
{
	CString strName;
	strName.Format(_T("%s(%s)"), strHead, CUtil::GetLampPresetsString(LampPresetsEpiNoneDiaNone));

	int nBase = 0;
	switch (eOptional) {
	case BrightField:
		nBase = BrightFieldEpiDia * 100;
		break;
	case DarkField:
		nBase = DarkFieldEpiDia * 100;
		break;
	case DIC:
		nBase = DICEpiDia * 100;
		break;
	case Fluorescence1:
		nBase = Fluorescence1EpiDia * 100;
		break;
	case Fluorescence2:
		nBase = Fluorescence2EpiDia * 100;
		break;
	default:
		break;
	}
	m_cboOptional.SetItemData(m_cboOptional.AddString(strName), nBase + LampPresetsEpiNoneDiaNone);

	if (m_bEpiDia) {
		strName.Format(_T("%s(%s)"), strHead, CUtil::GetLampPresetsString(LampPresetsEpiOnDiaOff));
		m_cboOptional.SetItemData(m_cboOptional.AddString(strName), nBase + LampPresetsEpiOnDiaOff);
		strName.Format(_T("%s(%s)"), strHead, CUtil::GetLampPresetsString(LampPresetsEpiOffDiaOn));
		m_cboOptional.SetItemData(m_cboOptional.AddString(strName), nBase + LampPresetsEpiOffDiaOn);
	}
	if (m_bEpi) {
		strName.Format(_T("%s(%s)"), strHead, CUtil::GetLampPresetsString(LampPresetsEpiOnDiaNone));
		m_cboOptional.SetItemData(m_cboOptional.AddString(strName), nBase + LampPresetsEpiOnDiaNone);
	}
	if (m_bDia) {
		strName.Format(_T("%s(%s)"), strHead, CUtil::GetLampPresetsString(LampPresetsEpiNoneDiaOn));
		m_cboOptional.SetItemData(m_cboOptional.AddString(strName), nBase + LampPresetsEpiNoneDiaOn);
	}
	if (m_bEpiDia) {
		strName.Format(_T("%s(%s)"), strHead, CUtil::GetLampPresetsString(LampPresetsEpiOnDiaOn));
		m_cboOptional.SetItemData(m_cboOptional.AddString(strName), nBase + LampPresetsEpiOnDiaOn);
	}
}
BOOL CUserObservationDlg::IsObservationDic()
{
	BOOL bIsDIC = FALSE;
	EnumObservationMode eObservation = (EnumObservationMode)m_cboObservation.GetItemData(m_cboObservation.GetCurSel());
	long lVal = (long)m_cboOptional.GetItemData(m_cboOptional.GetCurSel());
	EnumOptionalObservationMode eOptional = (EnumOptionalObservationMode)(lVal / 100);
	if (eObservation == DIC || (eObservation >= ObservationMode1 && eOptional == DICEpiDia))
		bIsDIC = TRUE;
	return bIsDIC;
}

void CUserObservationDlg::OnCbnSelchangeComboObservation()
{
	UpdateOptionalInformation();
	UpdateOptionalMode();
	ControlUnknown();
}
void CUserObservationDlg::OnCbnSelchangeComboOptional()
{
	CWaitCursor w;
	SetOptionalMode();
	ControlUnknown();
}
void CUserObservationDlg::OnCbnSelchangeComboNosepiece()
{
	CWaitCursor w;
	SetOptionalMode();
}
void CUserObservationDlg::OnBnClickedRadioInterlockOn()
{
	if (m_rdInterlockOff.GetCheck() == BST_CHECKED) {
		CWaitCursor w;
		m_rdInterlockOff.SetCheck(BST_UNCHECKED);
		SetOptionalMode();
	}
}
void CUserObservationDlg::OnBnClickedRadioInterlockOff()
{
	if (m_rdInterlockOn.GetCheck() == BST_CHECKED) {
		CWaitCursor w;
		m_rdInterlockOn.SetCheck(BST_UNCHECKED);
		SetOptionalMode();
	}
}
void CUserObservationDlg::OnBnClickedButtonUserMemory()
{
	MemorizePresets(FALSE);
}
void CUserObservationDlg::OnBnClickedButtonUserMemoryInit()
{
	MemorizePresets(TRUE);
}
