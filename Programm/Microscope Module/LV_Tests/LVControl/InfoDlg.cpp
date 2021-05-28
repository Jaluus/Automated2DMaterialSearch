// InfoDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "LvSample.h"
#include "InfoDlg.h"
#include "Util.h"

#define INFODLG_NULL (_T(""))
#define INFODLG_DELIMITER (m_strDelimiter)

// CInfoDlg ダイアログ

IMPLEMENT_DYNAMIC(CInfoDlg, CDialog)

CInfoDlg::CInfoDlg(CMicroscopeControl* pMicroscope, EnumInfo eInfo, CWnd* pParent /*=NULL*/)
	: CDialog(CInfoDlg::IDD, pParent)
	, m_pMicroscope(pMicroscope)
	, m_eInfo(eInfo)
{
}

CInfoDlg::~CInfoDlg()
{
}

void CInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INFO, m_lstInfo);
}


BEGIN_MESSAGE_MAP(CInfoDlg, CDialog)
END_MESSAGE_MAP()


// CInfoDlg メッセージ ハンドラ
BOOL CInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_strDelimiter.LoadString(IDS_STRING_INFODLG_DELIMITER);

	switch (m_eInfo) {
	case E_Nosepiece:
		ViewNosepiece();
		break;
	case E_Cube:
		ViewCube();
		break;
	case E_Interlock:
		ViewInterlock();
		break;
	case E_UserObservation:
		ViewUserObserbation();
		break;
	case E_EpiDia:
		ViewEpiDia();
		break;
	case E_ZDrive:
		ViewZDrive();
		break;
	case E_Dic:
		ViewDic();
		break;
	case E_System:
		ViewSystem();
		break;
	default:
		break;
	}

	return TRUE;
}

void CInfoDlg::ViewNosepiece()
{
	HRESULT hr = S_OK;
	INikonLv* pNikonLv = m_pMicroscope->GetInterface();

	try {
		CComPtr<INosepiece> pNosepiece = NULL;
		if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
			_com_raise_error(hr);

		CString str;
		int i;

		str.Format(_T("%s%s"), INFODLG_DELIMITER, pNosepiece->GetName().GetBSTR());
		m_lstInfo.AddString(str);

		str.Format(_T("Position : %d"), (long)pNosepiece->Value);
		m_lstInfo.AddString(str);

		m_lstInfo.AddString(INFODLG_NULL);
		str.Format(_T("%sRegistration Nosepiece Specification"), INFODLG_DELIMITER);
		m_lstInfo.AddString(str);
		m_lstInfo.AddString(_T("    Name                 (Series           Mag    WD /    NA Type     DIC)"));
		for (i = 1; i <= (long)pNosepiece->Objectives->Count; i++) {
			CComPtr<IObjective> pObjective = NULL;
			if (FAILED(hr = pNosepiece->Objectives->get_Item(i, &pObjective)))
				_com_raise_error(hr);

			if ((long)pObjective->Code > 0)
				str.Format(_T("%d : %-20s (%s)"), i, pObjective->GetName().GetBSTR(), pObjective->GetDescription().GetBSTR());
			else
				str.Format(_T("%d : %-20s (--------------- ---- ----- / ----- ---------- -)"), i, pObjective->GetName().GetBSTR());
			m_lstInfo.AddString(str);

			if (pObjective)
				pObjective.Release();
		}

		if (!m_pMicroscope->IsAFController()) {
			CComPtr<IDatabase> pDatabase = NULL;
			if (FAILED(hr = pNikonLv->get_Database(&pDatabase)))
				_com_raise_error(hr);

			m_lstInfo.AddString(INFODLG_NULL);
			str.Format(_T("%sSeries"), INFODLG_DELIMITER);
			m_lstInfo.AddString(str);
			for (i = 1; i <= (long)pDatabase->ObjectiveModels->Count; i++) {
				str.Format(_T("%02d : %s"), i, pDatabase->ObjectiveModels->GetItem(i)->GetName().GetBSTR());
				m_lstInfo.AddString(str);
			}

			m_lstInfo.AddString(INFODLG_NULL);
			str.Format(_T("%sType"), INFODLG_DELIMITER);
			m_lstInfo.AddString(str);
			for (i = 1; i <= (long)pDatabase->ObjectiveTypes->Count; i++) {
				str.Format(_T("%02d : %s"), i, pDatabase->ObjectiveTypes->GetItem(i)->GetName().GetBSTR());
				m_lstInfo.AddString(str);
			}

			m_lstInfo.AddString(INFODLG_NULL);
			str.Format(_T("%sSpecification"), INFODLG_DELIMITER);
			m_lstInfo.AddString(str);
			m_lstInfo.AddString(_T("      Name                 (Series           Mag    WD /    NA Type     DIC)"));
			for (i = 1; i <= (long)pDatabase->Objectives->Count; i++) {
				CComPtr<IDbObjective> pDbObjective = NULL;
				if (FAILED(hr = pDatabase->Objectives->get_Item(i, &pDbObjective)))
					_com_raise_error(hr);

				str.Format(_T("%03d : %-20s (%s)"), i, pDbObjective->GetName().GetBSTR(), pDbObjective->GetDescription().GetBSTR());
				m_lstInfo.AddString(str);

				if (pDbObjective)
					pDbObjective.Release();
			}

			if (pDatabase)
				pDatabase.Release();
		}

		if (pNosepiece)
			pNosepiece.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}

void CInfoDlg::ViewCube()
{
	HRESULT hr = S_OK;
	INikonLv* pNikonLv = m_pMicroscope->GetInterface();

	try {
		CComPtr<IFilterBlockCassette> pFilterBlockCassette = NULL;
		if (FAILED(hr = pNikonLv->get_FilterBlockCassette(&pFilterBlockCassette)))
			_com_raise_error(hr);

		CString str;
		int i;

		str.Format(_T("%s%s"), INFODLG_DELIMITER, pFilterBlockCassette->GetName().GetBSTR());
		m_lstInfo.AddString(str);

		str.Format(_T("Position : %d"), (long)pFilterBlockCassette->Value);
		m_lstInfo.AddString(str);

		m_lstInfo.AddString(INFODLG_NULL);
		str.Format(_T("%sRegistration Filter Cube Specification"), INFODLG_DELIMITER);
		m_lstInfo.AddString(str);
		for (i = 1; i <= (long)pFilterBlockCassette->UpperLimit; i++) {
			CComPtr<IFilterBlock> pFilterBlockItem = NULL;
			if (FAILED(hr = pFilterBlockCassette->FilterBlocks->get_Item(i, &pFilterBlockItem)))
				_com_raise_error(hr);

			str = GetFilterDescription(pFilterBlockItem->GetName().GetBSTR(), pFilterBlockItem->ExcitationFilterCode, pFilterBlockItem->DichroicMirrorCode, pFilterBlockItem->BarrierFilterCode);
			m_lstInfo.AddString(str);

			if (pFilterBlockItem)
				pFilterBlockItem.Release();
		}

		CComPtr<IDatabase> pDatabase = NULL;
		if (FAILED(hr = pNikonLv->get_Database(&pDatabase)))
			_com_raise_error(hr);

		m_lstInfo.AddString(INFODLG_NULL);
		str.Format(_T("%sExcitation Filter"), INFODLG_DELIMITER);
		for (i = 1; i <= (long)pDatabase->ExcitationFilters->Count; i++) {
			str.Format(_T("%03d : %s"), i, pDatabase->ExcitationFilters->GetItem(i)->GetName().GetBSTR());
			m_lstInfo.AddString(str);
		}

		m_lstInfo.AddString(INFODLG_NULL);
		str.Format(_T("%sDichroic Mirror"), INFODLG_DELIMITER);
		for (i = 1; i <= (long)pDatabase->DichroicMirrors->Count; i++) {
			str.Format(_T("%03d : %s"), i, pDatabase->DichroicMirrors->GetItem(i)->GetName().GetBSTR());
			m_lstInfo.AddString(str);
		}

		m_lstInfo.AddString(INFODLG_NULL);
		str.Format(_T("%sBarrier Filter"), INFODLG_DELIMITER);
		for (i = 1; i <= (long)pDatabase->BarrierFilters->Count; i++) {
			str.Format(_T("%03d : %s"), i, pDatabase->BarrierFilters->GetItem(i)->GetName().GetBSTR());
			m_lstInfo.AddString(str);
		}

		m_lstInfo.AddString(INFODLG_NULL);
		str.Format(_T("%sSpecification"), INFODLG_DELIMITER);
		for (i = 1; i <= (long)pDatabase->FilterBlocks->Count; i++) {
			CComPtr<IDbFilterBlock> pDbFilterBlockItem = NULL;
			if (FAILED(hr = pDatabase->FilterBlocks->get_Item(i, &pDbFilterBlockItem)))
				_com_raise_error(hr);

			CString strDescription = GetFilterDescription(pDbFilterBlockItem->GetName().GetBSTR(), pDbFilterBlockItem->ExcitationFilterCode, pDbFilterBlockItem->DichroicMirrorCode, pDbFilterBlockItem->BarrierFilterCode);
			str.Format(_T("%03d : %s"), i, strDescription);
			m_lstInfo.AddString(str);

			if (pDbFilterBlockItem)
				pDbFilterBlockItem.Release();
		}

		if (pFilterBlockCassette)
			pFilterBlockCassette.Release();
		if (pDatabase)
			pDatabase.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}

void CInfoDlg::ViewInterlock()
{
	HRESULT hr = S_OK;
	INikonLv* pNikonLv = m_pMicroscope->GetInterface();

	try {
		CComPtr<IPresets> pPresets = NULL;
		CComPtr<IEpiLamp> pEpiLamp = NULL;
		CComPtr<IDiaLamp> pDiaLamp = NULL;
		CComPtr<IEpiApertureStop> pEpiApertureStop = NULL;
		CComPtr<IDICPrism> pDicPrism = NULL;
		CComPtr<IEpiShutter> pEpiShutter = NULL;
		CComPtr<IZDrive> pZDrive = NULL;
		CComPtr<INosepiece> pNosepiece = NULL;

		if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
			_com_raise_error(hr);

		if (FAILED(hr = pNikonLv->get_EpiLamp(&pEpiLamp)))
			_com_raise_error(hr);

		if (FAILED(hr = pNikonLv->get_DiaLamp(&pDiaLamp)))
			_com_raise_error(hr);

		if (FAILED(hr = pNikonLv->get_EpiApertureStop(&pEpiApertureStop)))
			_com_raise_error(hr);

		if (FAILED(hr = pNikonLv->get_DICPrism(&pDicPrism)))
			_com_raise_error(hr);

		if (FAILED(hr = pNikonLv->get_EpiShutter(&pEpiShutter)))
			_com_raise_error(hr);

		if (FAILED(hr = pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
			_com_raise_error(hr);

		int i, j;
		CString str;

		str.Format(_T("%sInterlock Default Value"), INFODLG_DELIMITER);
		m_lstInfo.AddString(str);

		if (m_pMicroscope->m_bEpiLampMounted) {
			str.Format(_T("EPI : %.1lf%s"), pPresets->ReadDefaultPresets(AccessoryEpiLampVoltage, 0), pEpiLamp->GetUnit().GetBSTR());
			m_lstInfo.AddString(str);
		}

		if (m_pMicroscope->m_bDiaLampMounted) {
			str.Format(_T("DIA : %.1lf%s"), pPresets->ReadDefaultPresets(AccessoryDiaLampVoltage, 0), pDiaLamp->GetUnit().GetBSTR());
			m_lstInfo.AddString(str);
		}

		if (m_pMicroscope->m_bApertureMounted) {
			for (i = 1; i <= (long)pNosepiece->UpperLimit; i++) {
				str.Format(_T("Epi Aperture Stop (Nosepiece=%d) : %.1lf%s"), i, pPresets->ReadDefaultPresets(AccessoryEpiApertureStop, i), pEpiApertureStop->GetUnit().GetBSTR());
				m_lstInfo.AddString(str);
			}
		}

		if (m_pMicroscope->m_bDicMounted) {
			str.Format(_T("DIC : %.1lf%s"), pPresets->ReadDefaultPresets(AccessoryDICPrismShift, 0), pDicPrism->GetUnit().GetBSTR());
			m_lstInfo.AddString(str);
		}

		if (m_pMicroscope->m_bFiberMounted) {
			str.Format(_T("ND : %.1lf%s"), pPresets->ReadDefaultPresets(AccessoryNDFilter, 0), pEpiShutter->GetUnit().GetBSTR());
			m_lstInfo.AddString(str);
		}

		m_lstInfo.AddString(INFODLG_NULL);
		str.Format(_T("%sInterlock Mode"), INFODLG_DELIMITER);
		m_lstInfo.AddString(str);

		if (m_pMicroscope->m_bEpiLampMounted) {
			str.Format(_T("EPI (%s) : %s"), pEpiLamp->GetUnit().GetBSTR(), CUtil::GetInterlockString(pPresets->ReadInterlockMode(AccessoryEpiLampVoltage)));
			m_lstInfo.AddString(str);
		}

		if (m_pMicroscope->m_bDiaLampMounted) {
			str.Format(_T("DIA (%s) : %s"), pDiaLamp->GetUnit().GetBSTR(), CUtil::GetInterlockString(pPresets->ReadInterlockMode(AccessoryDiaLampVoltage)));
			m_lstInfo.AddString(str);
		}

		if (m_pMicroscope->m_bApertureMounted) {
			str.Format(_T("EPI Aperture Stop : %s"), CUtil::GetInterlockString(pPresets->ReadInterlockMode(AccessoryEpiApertureStop)));
			m_lstInfo.AddString(str);
		}

		if (m_pMicroscope->m_bZDriveMounted) {
			str.Format(_T("ZDrive : %s"), CUtil::GetInterlockString(pPresets->ReadInterlockMode(AccessoryZDrive)));
			m_lstInfo.AddString(str);
		}

		if (m_pMicroscope->m_bDicMounted) {
			str.Format(_T("DIC (Position) : %s"), CUtil::GetInterlockString(pPresets->ReadInterlockMode(AccessoryDICPrism)));
			m_lstInfo.AddString(str);
			str.Format(_T("DIC (Shift) : %s"), CUtil::GetInterlockString(pPresets->ReadInterlockMode(AccessoryDICPrismShift)));
			m_lstInfo.AddString(str);
		}

		m_lstInfo.AddString(INFODLG_NULL);
		str.Format(_T("%sInitial position"), INFODLG_DELIMITER);
		m_lstInfo.AddString(str);

		VARIANT val;
		IMipParameterPtr spiParam;

		if (FAILED(hr = pPresets->get_InitialMode(&val)))
			_com_raise_error(hr);
		if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
			_com_raise_error(hr);
		str.Format(_T("%d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
		m_lstInfo.AddString(str);
		m_lstInfo.AddString(INFODLG_NULL);

		if (FAILED(hr = pPresets->get_ObservationMode(&val)))
			_com_raise_error(hr);
		if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
			_com_raise_error(hr);

		for (i = 1; i <= (long)spiParam->EnumSet->Count; i++) {
			EnumObservationMode eMode = (EnumObservationMode)spiParam->EnumSet->GetItem(i).RawValue.lVal;
			if (eMode >= BrightField && eMode <= Fluorescence2) {
				str.Format(_T("%sObservation Mode (%s)"), INFODLG_DELIMITER, spiParam->EnumSet->GetItem(i).DisplayString);
				m_lstInfo.AddString(str);

				EnumLampPresets eLamp;
				if (FAILED(hr = pPresets->ReadLampPresets(eMode, &eLamp)))
					_com_raise_error(hr);
				str.Format(_T("    Lamp presets : %d(%s)"), eLamp, CUtil::GetLampPresetsString(eLamp));
				m_lstInfo.AddString(str);
				m_lstInfo.AddString(INFODLG_NULL);

				for (j = 1; j <= (long)pNosepiece->UpperLimit; j++) {
					str.Format(_T("    %sNosepiece : %d"), INFODLG_DELIMITER, j);
					m_lstInfo.AddString(str);
					double dValue;

					if (m_pMicroscope->m_bEpiLampMounted) {
						dValue = pPresets->ReadMemorizedPresets(AccessoryEpiLampVoltage, eMode, j);
						str.Format(_T("        EPI (%s) : %.1lf"), pEpiLamp->GetUnit().GetBSTR(), dValue);
						m_lstInfo.AddString(str);
						dValue = pPresets->ReadUserPresets(AccessoryEpiLampVoltage, eMode, j);
						str.Format(_T("        EPI offset (%s) : %.1lf"), pEpiLamp->GetUnit().GetBSTR(), dValue);
						m_lstInfo.AddString(str);
					}

					if (m_pMicroscope->m_bDiaLampMounted) {
						dValue = pPresets->ReadMemorizedPresets(AccessoryDiaLampVoltage, eMode, j);
						str.Format(_T("        DIA (%s) : %.1lf"), pDiaLamp->GetUnit().GetBSTR(), dValue);
						m_lstInfo.AddString(str);
						dValue = pPresets->ReadUserPresets(AccessoryDiaLampVoltage, eMode, j);
						str.Format(_T("        DIA offset (%s) : %.1lf"), pDiaLamp->GetUnit().GetBSTR(), dValue);
						m_lstInfo.AddString(str);
					}

					if (m_pMicroscope->m_bFiberMounted) {
						dValue = pPresets->ReadMemorizedPresets(AccessoryNDFilter, eMode, j);
						str.Format(_T("        ND (%%) : %.1lf"), dValue);
						m_lstInfo.AddString(str);
						dValue = pPresets->ReadUserPresets(AccessoryNDFilter, eMode, j);
						str.Format(_T("        ND offset (%%) : %.1lf"), dValue);
						m_lstInfo.AddString(str);
					}

//					if (m_pMicroscope->m_bApertureMounted) {
					if (eMode != DarkField && m_pMicroscope->m_bApertureMounted) {	// 2011.11.28
						dValue = pPresets->ReadMemorizedPresets(AccessoryEpiApertureStop, eMode, j);
						str.Format(_T("        Epi Aperture Stop : %.1lf"), dValue);
						m_lstInfo.AddString(str);
						dValue = pPresets->ReadUserPresets(AccessoryEpiApertureStop, eMode, j);
						str.Format(_T("        Epi Aperture Stop offset (%%) : %.1lf"), dValue);
						m_lstInfo.AddString(str);
					}

					if (m_pMicroscope->m_bZDriveMounted) {
						dValue = pPresets->ReadMemorizedPresets(AccessoryZDrive, eMode, j);
						if ((long)dValue < (long)pZDrive->LowerLimit)
							m_lstInfo.AddString(_T("        ZDrive : --------"));
						else {
							str.Format(_T("        ZDrive : %.2lf"), dValue);
							m_lstInfo.AddString(str);
						}
					}

					if (m_pMicroscope->m_bDicMounted) {
						dValue = pPresets->ReadMemorizedPresets(AccessoryDICPrismShift, eMode, j);
						str.Format(_T("        DIC : %.1lf"), dValue);
						m_lstInfo.AddString(str);
						dValue = pPresets->ReadUserPresets(AccessoryDICPrismShift, eMode, j);
						str.Format(_T("        DIC offset %.1lf"), dValue);
						m_lstInfo.AddString(str);
					}

					m_lstInfo.AddString(INFODLG_NULL);
				}
			}
		}

		if (spiParam)
			spiParam.Release();
		if (pPresets)
			pPresets.Release();
		if (pEpiLamp)
			pEpiLamp.Release();
		if (pDiaLamp)
			pDiaLamp.Release();
		if (pEpiApertureStop)
			pEpiApertureStop.Release();
		if (pDicPrism)
			pDicPrism.Release();
		if (pEpiShutter)
			pEpiShutter.Release();
		if (pZDrive)
			pZDrive.Release();
		if (pNosepiece)
			pNosepiece.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}

void CInfoDlg::ViewUserObserbation()
{
	HRESULT hr = S_OK;
	INikonLv* pNikonLv = m_pMicroscope->GetInterface();

	try {
		CComPtr<IPresets> pPresets = NULL;
		CComPtr<IEpiLamp> pEpiLamp = NULL;
		CComPtr<IDiaLamp> pDiaLamp = NULL;
		CComPtr<IZDrive> pZDrive = NULL;
		CComPtr<INosepiece> pNosepiece = NULL;

		if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
			_com_raise_error(hr);

		if (FAILED(hr = pNikonLv->get_EpiLamp(&pEpiLamp)))
			_com_raise_error(hr);

		if (FAILED(hr = pNikonLv->get_DiaLamp(&pDiaLamp)))
			_com_raise_error(hr);

		if (FAILED(hr = pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
			_com_raise_error(hr);

		VARIANT val;
		IMipParameterPtr spiParam;

		if (FAILED(hr = pPresets->get_ObservationMode(&val)))
			_com_raise_error(hr);
		if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
			_com_raise_error(hr);

		int i, j;
		CString str;

		for (i = 1; i <= (long)spiParam->EnumSet->Count; i++) {
			EnumObservationMode eMode = (EnumObservationMode)spiParam->EnumSet->GetItem(i).RawValue.lVal;
			if (eMode >= ObservationMode1 && eMode <= ObservationMode6) {
				str.Format(_T("%sUser Optional Mode (%s)"), INFODLG_DELIMITER, spiParam->EnumSet->GetItem(i).DisplayString);
				m_lstInfo.AddString(str);

				EnumLampPresets eLamp;
				EnumStatus eInterlock;
				EnumOptionalObservationMode eOptional;
				long lPosition;
				if (FAILED(hr = pPresets->ReadPresetModesEx(eMode, &eOptional, &eInterlock, &lPosition)))
					_com_raise_error(hr);
				if (FAILED(hr = pPresets->ReadLampPresets(eMode, &eLamp)))
					_com_raise_error(hr);
				str.Format(_T("    Optional : %d(%s)"), eOptional, CUtil::GetOptionalObservationString(eOptional));
				m_lstInfo.AddString(str);
				str.Format(_T("    Interlock : %d(%s)"), eInterlock, CUtil::GetOnOffString(eInterlock));
				m_lstInfo.AddString(str);
				str.Format(_T("    Nosepiece : %d"), lPosition);
				m_lstInfo.AddString(str);
				str.Format(_T("    Lamp presets : %d(%s)"), eLamp, CUtil::GetLampPresetsString(eLamp));
				m_lstInfo.AddString(str);
				m_lstInfo.AddString(INFODLG_NULL);

				for (j = 1; j <= (long)pNosepiece->UpperLimit; j++) {
					str.Format(_T("    %sNosepiece : %d"), INFODLG_DELIMITER, j);
					m_lstInfo.AddString(str);
					double dValue;

					if (m_pMicroscope->m_bEpiLampMounted) {
						dValue = pPresets->ReadMemorizedPresets(AccessoryEpiLampVoltage, eMode, j);
						str.Format(_T("        EPI (%s) : %.1lf"), pEpiLamp->GetUnit().GetBSTR(), dValue);
						m_lstInfo.AddString(str);
						dValue = pPresets->ReadUserPresets(AccessoryEpiLampVoltage, eMode, j);
						str.Format(_T("        EPI offset (%s) : %.1lf"), pEpiLamp->GetUnit().GetBSTR(), dValue);
						m_lstInfo.AddString(str);
					}

					if (m_pMicroscope->m_bDiaLampMounted) {
						dValue = pPresets->ReadMemorizedPresets(AccessoryDiaLampVoltage, eMode, j);
						str.Format(_T("        DIA (%s) : %.1lf"), pDiaLamp->GetUnit().GetBSTR(), dValue);
						m_lstInfo.AddString(str);
						dValue = pPresets->ReadUserPresets(AccessoryDiaLampVoltage, eMode, j);
						str.Format(_T("        DIA offset (%s) : %.1lf"), pDiaLamp->GetUnit().GetBSTR(), dValue);
						m_lstInfo.AddString(str);
					}

					if (m_pMicroscope->m_bFiberMounted) {
						dValue = pPresets->ReadMemorizedPresets(AccessoryNDFilter, eMode, j);
						str.Format(_T("        ND (%%) : %.1lf"), dValue);
						m_lstInfo.AddString(str);
						dValue = pPresets->ReadUserPresets(AccessoryNDFilter, eMode, j);
						str.Format(_T("        ND offset (%%) : %.1lf"), dValue);
						m_lstInfo.AddString(str);
					}

					if (m_pMicroscope->m_bApertureMounted) {
						dValue = pPresets->ReadMemorizedPresets(AccessoryEpiApertureStop, eMode, j);
						str.Format(_T("        Epi Aperture Stop : %.1lf"), dValue);
						m_lstInfo.AddString(str);
						dValue = pPresets->ReadUserPresets(AccessoryEpiApertureStop, eMode, j);
						str.Format(_T("        Epi Aperture Stop offset (%%) : %.1lf"), dValue);
						m_lstInfo.AddString(str);
					}

					if (m_pMicroscope->m_bZDriveMounted) {
						dValue = pPresets->ReadMemorizedPresets(AccessoryZDrive, eMode, j);
						if ((long)dValue < (long)pZDrive->LowerLimit)
							m_lstInfo.AddString(_T("        ZDrive : --------"));
						else {
							str.Format(_T("        ZDrive : %.2lf"), dValue);
							m_lstInfo.AddString(str);
						}
					}

					if (m_pMicroscope->m_bDicMounted) {
						dValue = pPresets->ReadMemorizedPresets(AccessoryDICPrismShift, eMode, j);
						str.Format(_T("        DIC : %.1lf"), dValue);
						m_lstInfo.AddString(str);
						dValue = pPresets->ReadUserPresets(AccessoryDICPrismShift, eMode, j);
						str.Format(_T("        DIC offset %.1lf"), dValue);
						m_lstInfo.AddString(str);
					}

					m_lstInfo.AddString(INFODLG_NULL);
				}
			}
		}

		if (spiParam)
			spiParam.Release();
		if (pPresets)
			pPresets.Release();
		if (pEpiLamp)
			pEpiLamp.Release();
		if (pDiaLamp)
			pDiaLamp.Release();
		if (pZDrive)
			pZDrive.Release();
		if (pNosepiece)
			pNosepiece.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}

void CInfoDlg::ViewEpiDia()
{
	HRESULT hr = S_OK;
	INikonLv* pNikonLv = m_pMicroscope->GetInterface();

	try {
		CString str;

		if (m_pMicroscope->m_bEpiLampMounted) {
			CComPtr<IEpiLamp> pEpiLamp = NULL;
			if (FAILED(hr = pNikonLv->get_EpiLamp(&pEpiLamp)))
				_com_raise_error(hr);

			str.Format(_T("%s%s"), INFODLG_DELIMITER, pEpiLamp->GetName().GetBSTR());
			m_lstInfo.AddString(str);

			VARIANT val;
			IMipParameterPtr spiParam;

			if (FAILED(hr = pEpiLamp->get_LampType(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);
			str.Format(_T("Lamp Type : %d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
			m_lstInfo.AddString(str);

			if ((EnumLampType)((long)pEpiLamp->LampType) == LampTypeLED)
				str.Format(_T("Percent : %d%s"), (long)pEpiLamp->Value, pEpiLamp->GetUnit().GetBSTR());
			else
				str.Format(_T("Voltage : %d (%.1f%s)"), (long)pEpiLamp->Value, (double)pEpiLamp->Voltage, pEpiLamp->GetUnit().GetBSTR());
			m_lstInfo.AddString(str);

			if (FAILED(hr = pEpiLamp->get_IsOn(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);
			str.Format(_T("IsOn : %d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
			m_lstInfo.AddString(str);
			m_lstInfo.AddString(INFODLG_NULL);

			if (spiParam)
				spiParam.Release();
			if (pEpiLamp)
				pEpiLamp.Release();
		}


		if (m_pMicroscope->m_bFiberMounted) {
			CComPtr<IEpiShutter> pEpiShutter = NULL;
			if (FAILED(hr = pNikonLv->get_EpiShutter(&pEpiShutter)))
				_com_raise_error(hr);

			str.Format(_T("%s%s"), INFODLG_DELIMITER, pEpiShutter->GetName().GetBSTR());
			m_lstInfo.AddString(str);

			VARIANT val;
			IMipParameterPtr spiParam;

			if (FAILED(hr = pEpiShutter->get_FiberType(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);
			str.Format(_T("Fiber Type : %d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
			m_lstInfo.AddString(str);

			if (FAILED(hr = pEpiShutter->get_IsOpened(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);
			str.Format(_T("IsOpened : %d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
			m_lstInfo.AddString(str);
			m_lstInfo.AddString(INFODLG_NULL);

			CComPtr<INDFilter> pNDFilter = NULL;
			if (FAILED(hr = pNikonLv->get_NDFilter(&pNDFilter)))
				_com_raise_error(hr);

			str.Format(_T("%sND"), INFODLG_DELIMITER);
			m_lstInfo.AddString(str);
			if (FAILED(hr = pNDFilter->get_Position(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);
			str.Format(_T("Transmission : %d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
			m_lstInfo.AddString(str);
			m_lstInfo.AddString(INFODLG_NULL);

			if (spiParam)
				spiParam.Release();
			if (pEpiShutter)
				pEpiShutter.Release();
			if (pNDFilter)
				pNDFilter.Release();
		}

		if (m_pMicroscope->m_bDiaLampMounted) {
			CComPtr<IDiaLamp> pDiaLamp = NULL;
			if (FAILED(hr = pNikonLv->get_DiaLamp(&pDiaLamp)))
				_com_raise_error(hr);

			str.Format(_T("%s%s"), INFODLG_DELIMITER, pDiaLamp->GetName().GetBSTR());
			m_lstInfo.AddString(str);

			VARIANT val;
			IMipParameterPtr spiParam;

			if (FAILED(hr = pDiaLamp->get_LampType(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);
			str.Format(_T("Lamp Type : %d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
			m_lstInfo.AddString(str);

			str.Format(_T("Voltage : %d (%.1f%s)"), (long)pDiaLamp->Value, (double)pDiaLamp->Voltage, pDiaLamp->GetUnit().GetBSTR());
			m_lstInfo.AddString(str);

			if (FAILED(hr = pDiaLamp->get_IsOn(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);
			str.Format(_T("IsOn : %d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
			m_lstInfo.AddString(str);
			m_lstInfo.AddString(INFODLG_NULL);

			if (spiParam)
				spiParam.Release();
			if (pDiaLamp)
				pDiaLamp.Release();
		}

		if (m_pMicroscope->m_bApertureMounted) {
			CComPtr<IEpiApertureStop> pEpiApertureStop = NULL;
			if (FAILED(hr = pNikonLv->get_EpiApertureStop(&pEpiApertureStop)))
				_com_raise_error(hr);

			str.Format(_T("%s%s"), INFODLG_DELIMITER, pEpiApertureStop->GetName().GetBSTR());
			m_lstInfo.AddString(str);

			str.Format(_T("Aperture Stop : %d (%.1f%s)"), (long)pEpiApertureStop->Value, (double)pEpiApertureStop->ApertureStop, pEpiApertureStop->GetUnit().GetBSTR());
			m_lstInfo.AddString(str);

			str.Format(_T("Compensation Ratio : %3d%%"), (long)((double)pEpiApertureStop->CompensationRatio * 100));
			m_lstInfo.AddString(str);

			if (pEpiApertureStop)
				pEpiApertureStop.Release();
		}

	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}

void CInfoDlg::ViewZDrive()
{
	if (m_pMicroscope->m_bZDriveMounted) {
		HRESULT hr = S_OK;
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();

		try {
			CComPtr<IZDrive> pZDrive = NULL;
			if (FAILED(hr = pNikonLv->get_ZDrive(&pZDrive)))
				_com_raise_error(hr);

			CString str;
			str.Format(_T("%s%s"), INFODLG_DELIMITER, pZDrive->GetName().GetBSTR());
			m_lstInfo.AddString(str);

			VARIANT val;
			IMipParameterPtr spiParam;

			if (FAILED(hr = pZDrive->get_ZDriveType(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);
			str.Format(_T("ZDrive Type : %d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
			m_lstInfo.AddString(str);

			if (FAILED(hr = pZDrive->get_Speed(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);
			str.Format(_T("Speed : %d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
			m_lstInfo.AddString(str);

			str.Format(_T("Position : %d (%.2f%s)"), (long)pZDrive->Value, (double)pZDrive->Position, pZDrive->GetUnit().GetBSTR());
			m_lstInfo.AddString(str);

			str.Format(_T("Software Limit : [%d]-[%d]"), (long)pZDrive->SoftwareLowerLimit, (long)pZDrive->SoftwareUpperLimit);
			m_lstInfo.AddString(str);

			if (FAILED(hr = pZDrive->get_Limitation(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);
			str.Format(_T("Limitation : %d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
			m_lstInfo.AddString(str);

			str.Format(_T("Escape : %s"), CUtil::GetOnOffString((EnumStatus)((long)pZDrive->IsZEscape)));
			m_lstInfo.AddString(str);

			if (pZDrive->IsSupportedAF) {
				if (FAILED(hr = pZDrive->get_AF(&val)))
					_com_raise_error(hr);
				if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
					_com_raise_error(hr);
				str.Format(_T("AF : %d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
				m_lstInfo.AddString(str);

				if (FAILED(hr = pZDrive->get_AfStatus(&val)))
					_com_raise_error(hr);
				if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
					_com_raise_error(hr);
				str.Format(_T("AF status : %d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
				m_lstInfo.AddString(str);

				if (FAILED(hr = pZDrive->get_IsAfSearch(&val)))
					_com_raise_error(hr);
				if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
					_com_raise_error(hr);
				str.Format(_T("AF search status : %d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
				m_lstInfo.AddString(str);

				str.Format(_T("AF offset value : %d"), (long)pZDrive->AfOffset);
				m_lstInfo.AddString(str);
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
}

void CInfoDlg::ViewDic()
{
	if (m_pMicroscope->m_bDicMounted) {
		HRESULT hr = S_OK;
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();

		try {
			CComPtr<IDICPrism> pDicPrism = NULL;
			if (FAILED(hr = pNikonLv->get_DICPrism(&pDicPrism)))
				_com_raise_error(hr);

			CString str;
			str.Format(_T("%s%s"), INFODLG_DELIMITER, pDicPrism->GetName().GetBSTR());
			m_lstInfo.AddString(str);

			str.Format(_T("Shift : %d (%.2f%s)"), (long)pDicPrism->Value, (double)pDicPrism->PhaseDifference, pDicPrism->GetUnit().GetBSTR());
			m_lstInfo.AddString(str);

			VARIANT val;
			IMipParameterPtr spiParam;

			if (FAILED(hr = pDicPrism->get_DICPosition(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);
			str.Format(_T("Position : %d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
			m_lstInfo.AddString(str);

			if (FAILED(hr = pDicPrism->get_IsOutOfPosition(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);
			str.Format(_T("IsOutOfPosition : %d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
			m_lstInfo.AddString(str);

			if (FAILED(hr = pDicPrism->get_Limitation(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);
			str.Format(_T("Limitation : %d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
			m_lstInfo.AddString(str);

			str.Format(_T("Calibration : %d"), pDicPrism->ReadDicCalibration());
			m_lstInfo.AddString(str);

			if (FAILED(hr = pDicPrism->get_IsCalibration(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);
			str.Format(_T("IsCalibration : %d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
			m_lstInfo.AddString(str);

			if (spiParam)
				spiParam.Release();
			if (pDicPrism)
				pDicPrism.Release();
		}
		catch (_com_error/* &e*/) {
			AfxMessageBox(_T("Microscope error!"));
		}
	}
}

void CInfoDlg::ViewSystem()
{
	HRESULT hr = S_OK;
	INikonLv* pNikonLv = m_pMicroscope->GetInterface();

	CString str;

	if (!m_pMicroscope->IsAFController()) {
		try {
			str.Format(_T("%sEPI/DIA Lamp Mode"), INFODLG_DELIMITER);
			m_lstInfo.AddString(str);
			CComPtr<IPresets> pPresets = NULL;
			if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
				_com_raise_error(hr);

			VARIANT val;
			IMipParameterPtr spiParam;
			if (FAILED(hr = pPresets->get_EpiDiaLampMode(&val)))
				_com_raise_error(hr);
			if (FAILED(hr = val.pdispVal->QueryInterface(&spiParam)))
				_com_raise_error(hr);
			str.Format(_T("%d (%s)"), spiParam->GetRawValue().lVal, spiParam->GetDisplayString().GetBSTR());
			m_lstInfo.AddString(str);

			if (spiParam)
				spiParam.Release();
			if (pPresets)
				pPresets.Release();

			m_lstInfo.AddString(INFODLG_NULL);
		}
		catch (_com_error/* &e*/) {
			AfxMessageBox(_T("Microscope error!"));
		}
	}

	EnumStatus eStatus;
	str.Format(_T("%sSwitch"), INFODLG_DELIMITER);
	m_lstInfo.AddString(str);
	eStatus = pNikonLv->IsSwitchEnabled();
	str.Format(_T("%d (%s)"), (int)eStatus, CUtil::GetOnOffString(eStatus));
	m_lstInfo.AddString(str);

	m_lstInfo.AddString(INFODLG_NULL);
	str.Format(_T("%sBuzzer"), INFODLG_DELIMITER);
	m_lstInfo.AddString(str);
	eStatus = pNikonLv->IsBuzzerEnabled();
	str.Format(_T("%d (%s)"), (int)eStatus, CUtil::GetOnOffString(eStatus));
	m_lstInfo.AddString(str);
}

CString CInfoDlg::GetFilterDescription(BSTR bstrName, int nEF, int nDM, int nBF)
{
	HRESULT hr = S_OK;
	INikonLv* pNikonLv = m_pMicroscope->GetInterface();

	CString str;
	str.Format(_T("%-10s"), bstrName);

	try {
		CComPtr<IDatabase> pDatabase = NULL;
		if (FAILED(hr = pNikonLv->get_Database(&pDatabase)))
			_com_raise_error(hr);

		CString strEF;
		if (nEF == 0)
			strEF = _T(" (----------, ");
		else {
			BSTR bstrEF;
			if (FAILED(hr = pDatabase->GetExcitationFilters()->GetItem(nEF)->get_Name(&bstrEF)))
				_com_raise_error(hr);
			strEF.Format(_T(" (%-10s, "), bstrEF);
		}
		str += strEF;

		CString strDM;
		if (nDM == 0)
			strDM = _T("----------, ");
		else {
			BSTR bstrDM;
			if (FAILED(hr = pDatabase->GetDichroicMirrors()->GetItem(nDM)->get_Name(&bstrDM)))
				_com_raise_error(hr);
			strDM.Format(_T("%-10s, "), bstrDM);
		}
		str += strDM;

		CString strBF;
		if (nBF == 0)
			strBF = _T("----------)");
		else {
			BSTR bstrBF;
			if (FAILED(hr = pDatabase->GetBarrierFilters()->GetItem(nBF)->get_Name(&bstrBF)))
				_com_raise_error(hr);
			strBF.Format(_T("%-10s)"), bstrBF);
		}
		str += strBF;

		if (pDatabase)
			pDatabase.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}

	return str;
}
