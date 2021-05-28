#pragma once

class CUtil
{
public:
	CUtil(void);
	~CUtil(void);

	static void ResizeWindowFromClientSize(CWnd* pWnd, int cx, int cy, UINT nOption = 0);
	static CString GetOnOffString(EnumStatus eStatus);
	static CString GetInterlockString(EnumInterlockMode eInterlock);
	static CString GetObservationShortString(EnumObservationMode eMode);
	static CString GetLampPresetsString(EnumLampPresets ePreset);
	static CString GetOptionalObservationString(EnumOptionalObservationMode eOptional);
	static CString GetIlluminantPresetsString(EnumIlluminantPresets eIlluminant);
	static CString GetAperturePresetsString(EnumAperturePresets eAperture);
};
