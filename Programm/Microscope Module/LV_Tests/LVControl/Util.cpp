#include "StdAfx.h"
#include "Util.h"

CUtil::CUtil(void)
{
}

CUtil::~CUtil(void)
{
}

void CUtil::ResizeWindowFromClientSize(CWnd* pWnd, int cx, int cy, UINT nOption)
{
	CRect rcClient;
	pWnd->GetClientRect(&rcClient);
	CRect rcWindow;
	pWnd->GetWindowRect(&rcWindow);
	CSize szWindow(rcWindow.Width() - rcClient.Width() + cx,
				   rcWindow.Height() - rcClient.Height() + cy);
	pWnd->SetWindowPos(NULL, 0, 0, szWindow.cx, szWindow.cy, SWP_NOMOVE | SWP_NOZORDER | nOption);
}

CString CUtil::GetOnOffString(EnumStatus eStatus)
{
	if (eStatus == StatusTrue)
		return _T("On");
	else if (eStatus == StatusFalse)
		return _T("Off");

	return _T("-");
}

CString CUtil::GetInterlockString(EnumInterlockMode eInterlock)
{
	if (eInterlock == InterlockEnabled)
		return _T("On");
	else if (eInterlock == InterlockDisabled)
		return _T("Off");

	return _T("-");
}

CString CUtil::GetObservationShortString(EnumObservationMode eMode)
{
	switch (eMode) {
	case BrightField:
		return _T("BF");
	case DarkField:
		return _T("DF");
	case DIC:
		return _T("DIC");
	case Fluorescence1:
		return _T("FL1");
	case Fluorescence2:
		return _T("FL2");
	}

	return _T("-");
}

CString CUtil::GetLampPresetsString(EnumLampPresets ePreset)
{
	switch (ePreset) {
	case LampPresetsUnknown:
		return _T("Unknown");
	case LampPresetsEpiOnDiaNone:
		return _T("EPI,---");
	case LampPresetsEpiOnDiaOff:
		return _T("EPI");
	case LampPresetsEpiOnDiaOn:
		return _T("EPI+DIA");
	case LampPresetsEpiNoneDiaNone:
		return _T("---,---");
	case LampPresetsEpiNoneDiaOff:
		return _T("DIA(OFF)");
	case LampPresetsEpiNoneDiaOn:
		return _T("---,DIA");
	case LampPresetsEpiOffDiaNone:
		return _T("EPI(OFF)");
	case LampPresetsEpiOffDiaOff:
		return _T("OFF");
	case LampPresetsEpiOffDiaOn:
		return _T("DIA");
	}

	return _T("-");
}

CString CUtil::GetOptionalObservationString(EnumOptionalObservationMode eOptional)
{
	switch (eOptional) {
	case OptionalObservationModeUnknown:
		return _T("Unknown");
	case OptionalObservationModeNone:
		return _T("--------");
	case BrightFieldEpiDia:
	case BrightFieldEpi:
		return _T("BF");
	case DarkFieldEpiDia:
	case DarkFieldEpi:
		return _T("DF");
	case DICEpiDia:
	case DICEpi:
		return _T("DIC");
	case Fluorescence1EpiDia:
	case Fluorescence1Epi:
		return _T("FL1");
	case Fluorescence2EpiDia:
	case Fluorescence2Epi:
		return _T("FL2");
	case OptionalObservationDia:
		return _T("DIA");
	}

	return _T("-");
}
CString CUtil::GetIlluminantPresetsString(EnumIlluminantPresets eIlluminant)
{
	switch (eIlluminant) {
	case IlluminantPresetsUnknown:
		return _T("Unknown");
	case IlluminantPresetsEpiMemDiaMem:
		return _T("EPI+DIA");
	case IlluminantPresetsEpiMemDiaOff:
		return _T("EPI");
	case IlluminantPresetsEpiMemDiaNone:
		return _T("EPI,---");
	case IlluminantPresetsEpiOffDiaMem:
		return _T("DIA");
	case IlluminantPresetsEpiOffDiaOff:
		return _T("OFF");
	case IlluminantPresetsEpiOffDiaNone:
		return _T("EPI(OFF)");
	case IlluminantPresetsEpiNoneDiaMem:
		return _T("---,DIA");
	case IlluminantPresetsEpiNoneDiaOff:
		return _T("DIA(OFF)");
	case IlluminantPresetsEpiNoneDiaNone:
		return _T("--------");
	}

	return _T("-");
}
CString CUtil::GetAperturePresetsString(EnumAperturePresets eAperture)
{
	switch (eAperture) {
	case AperturePresetsUnknown:
		return _T("Unknown");
	case KeepingRadius:
		return _T("Keeping Radius");
	case StoredRadius:
		return _T("Stored Radius");
	case ThreeQuartersOfIris:
		return _T("Three Quarters Of Iris");
	}

	return _T("-");
}
