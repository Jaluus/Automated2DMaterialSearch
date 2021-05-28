// MicroscopeControl.cpp: CMicroscopeControl クラスのインプリメンテーション
//
/*! @file
 @brief MicroscopeControl CMicroscopeControl クラスのインプリメンテーション ファイル

 このファイルはMicroscopeControl.cppです。
 @author 作者 Nikon
 @date 作成日, 履歴

*/

#include "stdafx.h"
#include "LvSample.h"
#include "MicroscopeControl.h"
#include "Util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CMicroscopeControl::CMicroscopeControl()
	: m_pNikonLv(NULL)
	, m_bIsSimulation(TRUE)
	, m_bConfirm(FALSE)
	, m_bNosepieceMounted(FALSE)
	, m_bCubeMounted(FALSE)
	, m_bApertureMounted(FALSE)
	, m_bZDriveMounted(FALSE)
	, m_bEpiLampMounted(FALSE)
	, m_bDiaLampMounted(FALSE)
	, m_bFiberMounted(FALSE)
	, m_bDicMounted(FALSE)
{
}

CMicroscopeControl::~CMicroscopeControl()
{
	if (m_pNikonLv) {
		Disconnect();
		Uninitialize();
	}
}

/*!
 @brief 顕微鏡初期化処理

 @param[in] なし
 @return 初期結果値
*/
HRESULT CMicroscopeControl::Initialize()
{
	HRESULT hr = S_OK;

	try {
		hr = CoCreateInstance(CLSID_NikonLv, NULL, CLSCTX_ALL, IID_INikonLv, (void**)&m_pNikonLv);
		if (FAILED(hr))
			_com_raise_error(hr);

		long lLocked = 0;
		if (FAILED(hr = m_pNikonLv->get_IsLocked(&lLocked)))
			_com_raise_error(hr);

		if (lLocked) {
			AfxMessageBox(_T("SDK cannot be multiple started."));
			return E_FAIL;
		}
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	return hr;
}

/*!
 @brief 顕微鏡解放処理

 @param[in] なし
 @return 解放結果値
*/
HRESULT CMicroscopeControl::Uninitialize()
{
	HRESULT hr = S_OK;

	if (m_pNikonLv) {
		try {
			m_pNikonLv->Release();
			m_pNikonLv = NULL;
		}
		catch (_com_error &e) {
			hr = e.Error();
		}
	}

	return hr;
}

/*!
 @brief 顕微鏡接続処理

 @param[in] なし
 @return 接続結果値
*/
HRESULT CMicroscopeControl::Connect()
{
	HRESULT hr = S_OK;

	try {
		CComPtr<INikonLvDevices> pNikonLvDevices = NULL;
		if (FAILED(hr = m_pNikonLv->get_Devices(&pNikonLvDevices)))
			_com_raise_error(hr);

		long nCount;
		if (FAILED(hr = pNikonLvDevices->get_Count(&nCount)))
			_com_raise_error(hr);

		for (int i = nCount - 1; i >= 0; i--) {
			CComPtr<INikonLvDevice> pNikonLvDevice = NULL;
			if (FAILED(hr = pNikonLvDevices->get_Item(i, &pNikonLvDevice)))
				_com_raise_error(hr);

			long bAvailable = 0;
			if (FAILED(hr = pNikonLvDevice->get_IsAvailable(&bAvailable)))
				_com_raise_error(hr);

			if (bAvailable) {
				if (FAILED(hr = m_pNikonLv->put_Device(pNikonLvDevice)))
					_com_raise_error(hr);

				if (FAILED(hr = pNikonLvDevice->Connect()))
					_com_raise_error(hr);

				m_bIsSimulation = (i == 0);
				m_eController = (EnumController)((long)pNikonLvDevice->LvDeviceType);	// Device Type

				break;
			}
			else
				hr = MICROSCOPE_NOT_AVAILABLE;

			if (pNikonLvDevice)
				pNikonLvDevice.Release();
		}
		if (pNikonLvDevices)
			pNikonLvDevices.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	return hr;
}

/*!
 @brief 顕微鏡切断処理

 @param[in] なし
 @return 切断結果値
*/
HRESULT CMicroscopeControl::Disconnect()
{
	HRESULT hr = S_OK;

	if (m_pNikonLv) {
		try {
			CComPtr<INikonLvDevice> pNikonLvDevice = NULL;
			if (FAILED(hr = m_pNikonLv->get_Device(&pNikonLvDevice)))
				_com_raise_error(hr);

			if (FAILED(hr = pNikonLvDevice->Disconnect()))
				_com_raise_error(hr);

			if (pNikonLvDevice)
				pNikonLvDevice.Release();
		}
		catch (_com_error &e) {
			hr = e.Error();
		}
	}

	return hr;
}

HRESULT CMicroscopeControl::ConnectSelectedDevice(long lIndex)
{
	if (lIndex < 0)
		return -1;

	HRESULT hr = S_OK;

	try {
		CComPtr<INikonLvDevices> pNikonLvDevices = NULL;
		if (FAILED(hr = m_pNikonLv->get_Devices(&pNikonLvDevices)))
			_com_raise_error(hr);

		long nCount;
		if (FAILED(hr = pNikonLvDevices->get_Count(&nCount)))
			_com_raise_error(hr);

		if (lIndex < nCount) {
			CComPtr<INikonLvDevice> pNikonLvDevice = NULL;
			if (FAILED(hr = pNikonLvDevices->get_Item(lIndex, &pNikonLvDevice)))
				_com_raise_error(hr);

			long bAvailable = 0;
			if (FAILED(hr = pNikonLvDevice->get_IsAvailable(&bAvailable)))
				_com_raise_error(hr);

			if (bAvailable) {
				if (FAILED(hr = m_pNikonLv->put_Device(pNikonLvDevice)))
					_com_raise_error(hr);

				if (FAILED(hr = pNikonLvDevice->Connect()))
					_com_raise_error(hr);

				m_bIsSimulation = (lIndex == 0);
				m_eController = (EnumController)((long)pNikonLvDevice->LvDeviceType);	// Device Type
			}
			else
				hr = MICROSCOPE_NOT_AVAILABLE;

			if (pNikonLvDevice)
				pNikonLvDevice.Release();
		}
		if (pNikonLvDevices)
			pNikonLvDevices.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	return hr;
}

HRESULT CMicroscopeControl::GetName(EnumAccessory eAccessory, long lVal, CString& strName)
{
	HRESULT hr = S_OK;

	try {
		BSTR bstrName = NULL;

		switch (eAccessory) {
		case AccessoryNosepiece:
			{
				CComPtr<INosepiece> pNosepiece = NULL;
				CComPtr<IObjectives> pObjectives = NULL;

				if (FAILED(hr = m_pNikonLv->get_Nosepiece(&pNosepiece)))
					_com_raise_error(hr);

				if (FAILED(hr = pNosepiece->get_Objectives(&pObjectives)))
					_com_raise_error(hr);

				if (FAILED(hr = pObjectives->GetItem(lVal)->get_Name(&bstrName)))
					_com_raise_error(hr);

				if (pObjectives)
					pObjectives.Release();
				if (pNosepiece)
					pNosepiece.Release();
			}
			break;
		case AccessoryFilterBlockCassette:
			{
				CComPtr<IFilterBlockCassette> pFilterBlockCassette = NULL;
				CComPtr<IFilterBlocks> pFilterBlocks = NULL;

				if (FAILED(hr = m_pNikonLv->get_FilterBlockCassette(&pFilterBlockCassette)))
					_com_raise_error(hr);

				if (FAILED(hr = pFilterBlockCassette->get_FilterBlocks(&pFilterBlocks)))
					_com_raise_error(hr);

				if (FAILED(hr = pFilterBlocks->GetItem(lVal)->get_Name(&bstrName)))
					_com_raise_error(hr);

				if (pFilterBlocks)
					pFilterBlocks.Release();
				if (pFilterBlockCassette)
					pFilterBlockCassette.Release();
			}
			break;
		case AccessoryNDFilter:
			{
				CComPtr<INDFilter> pNDFilter = NULL;

				if (FAILED(hr = m_pNikonLv->get_NDFilter(&pNDFilter)))
					_com_raise_error(hr);

				if (FAILED(hr = pNDFilter->get_Name(&bstrName)))
					_com_raise_error(hr);

				if (pNDFilter)
					pNDFilter.Release();
			}
			break;
		case AccessoryDICPrism:
			{
				CComPtr<IDICPrism> pDICPrism = NULL;

				if (FAILED(hr = m_pNikonLv->get_DICPrism(&pDICPrism)))
					_com_raise_error(hr);

				if (FAILED(hr = pDICPrism->get_Name(&bstrName)))
					_com_raise_error(hr);

				if (pDICPrism)
					pDICPrism.Release();
			}
			break;
		default:
			break;
		}

		if (bstrName) {
			strName = bstrName;
			::SysFreeString(bstrName);
		}
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	return hr;
}

HRESULT CMicroscopeControl::ExecCommand(EnumAccessory eAccessory, long lParam)
{
	HRESULT hr = S_OK;
	CWaitCursor w;
	DWORD dwStart = GetTickCount();

	try {
		switch (eAccessory) {
		case AccessoryEpiLamp:
			{
				CComPtr<IEpiLamp> pEpiLamp = NULL;

				if (FAILED(hr = m_pNikonLv->get_EpiLamp(&pEpiLamp)))
					_com_raise_error(hr);

				EnumStatus eIsOn = (EnumStatus)((long)pEpiLamp->IsOn);

				switch (lParam) {
				case 0:
					if (eIsOn == StatusTrue) {
						if (FAILED(hr = pEpiLamp->Off()))
							_com_raise_error(hr);
					}
					break;
				case 1:
					if (eIsOn == StatusFalse) {
						if (FAILED(hr = pEpiLamp->On()))
							_com_raise_error(hr);
					}
					break;
				default:
					break;
				}

				if (pEpiLamp)
					pEpiLamp.Release();
			}
			break;
		case AccessoryEpiLampVoltage:
			{
				CComPtr<IEpiLamp> pEpiLamp = NULL;

				if (FAILED(hr = m_pNikonLv->get_EpiLamp(&pEpiLamp)))
					_com_raise_error(hr);

				switch (lParam) {
				case 0:
					if (FAILED(hr = pEpiLamp->Decrease()))
						_com_raise_error(hr);
					break;
				case 1:
					if (FAILED(hr = pEpiLamp->Increase()))
						_com_raise_error(hr);
					break;
				case 2:
					if (FAILED(hr = pEpiLamp->SetVoltageForCamera()))
						_com_raise_error(hr);
					break;
				default:
					break;
				}

				if (pEpiLamp)
					pEpiLamp.Release();
			}
			break;
		case AccessoryEpiShutter:
			{
				CComPtr<IEpiShutter> pEpiShutter = NULL;

				if (FAILED(hr = m_pNikonLv->get_EpiShutter(&pEpiShutter)))
					_com_raise_error(hr);

				EnumStatus eIsOpen = (EnumStatus)((long)pEpiShutter->IsOpened);

				switch (lParam) {
				case 0:
					if (eIsOpen == StatusTrue) {
						if (FAILED(hr = pEpiShutter->Close()))
							_com_raise_error(hr);
					}
					break;
				case 1:
					if (eIsOpen == StatusFalse) {
						if (FAILED(hr = pEpiShutter->Open()))
							_com_raise_error(hr);
					}
					break;
/*				case 2:
					if (eIsOpen == StatusFalse) {
						if (FAILED(hr = pEpiShutter->OpenWithTimer(100)))
							_com_raise_error(hr);
					}
					break;
*/
				default:
					break;
				}

				if (pEpiShutter)
					pEpiShutter.Release();
			}
			break;
		case AccessoryDiaLamp:
			{
				CComPtr<IDiaLamp> pDiaLamp = NULL;

				if (FAILED(hr = m_pNikonLv->get_DiaLamp(&pDiaLamp)))
					_com_raise_error(hr);

				EnumStatus eIsOn = (EnumStatus)((long)pDiaLamp->IsOn);

				switch (lParam) {
				case 0:
					if (eIsOn == StatusTrue) {
						if (FAILED(hr = pDiaLamp->Off()))
							_com_raise_error(hr);
					}
					break;
				case 1:
					if (eIsOn == StatusFalse) {
						if (FAILED(hr = pDiaLamp->On()))
							_com_raise_error(hr);
					}
					break;
				default:
					break;
				}

				if (pDiaLamp)
					pDiaLamp.Release();
			}
			break;
		case AccessoryDiaLampVoltage:
			{
				CComPtr<IDiaLamp> pDiaLamp = NULL;

				if (FAILED(hr = m_pNikonLv->get_DiaLamp(&pDiaLamp)))
					_com_raise_error(hr);

				switch (lParam) {
				case 0:
					if (FAILED(hr = pDiaLamp->Decrease()))
						_com_raise_error(hr);
					break;
				case 1:
					if (FAILED(hr = pDiaLamp->Increase()))
						_com_raise_error(hr);
					break;
				case 2:
					if (FAILED(hr = pDiaLamp->SetVoltageForCamera()))
						_com_raise_error(hr);
					break;
				default:
					break;
				}

				if (pDiaLamp)
					pDiaLamp.Release();
			}
			break;
		case AccessoryNosepiece:
			{
				CComPtr<INosepiece> pNosepiece = NULL;

				if (FAILED(hr = m_pNikonLv->get_Nosepiece(&pNosepiece)))
					_com_raise_error(hr);

				switch (lParam) {
				case 0:
					if (FAILED(hr = pNosepiece->Reverse()))
						_com_raise_error(hr);
					break;
				case 1:
					if (FAILED(hr = pNosepiece->Forward()))
						_com_raise_error(hr);
					break;
				default:
					break;
				}

				if (pNosepiece)
					pNosepiece.Release();
			}
			break;
		case AccessoryFilterBlockCassette:
			{
				CComPtr<IFilterBlockCassette> pFilterBlockCassette = NULL;

				if (FAILED(hr = m_pNikonLv->get_FilterBlockCassette(&pFilterBlockCassette)))
					_com_raise_error(hr);

				switch (lParam) {
				case 0:
					if (FAILED(hr = pFilterBlockCassette->Reverse()))
						_com_raise_error(hr);
					break;
				case 1:
					if (FAILED(hr = pFilterBlockCassette->Forward()))
						_com_raise_error(hr);
					break;
				default:
					break;
				}

				if (pFilterBlockCassette)
					pFilterBlockCassette.Release();
			}
			break;
		case AccessoryZDrive:
			{
				CComPtr<IZDrive> pZDrive = NULL;

				if (FAILED(hr = m_pNikonLv->get_ZDrive(&pZDrive)))
					_com_raise_error(hr);

				if (FAILED(hr = pZDrive->MoveRelative(lParam)))
					_com_raise_error(hr);

				if (pZDrive)
					pZDrive.Release();
			}
			break;
		case AccessoryNDFilter:
			{
				CComPtr<INDFilter> pNDFilter = NULL;

				if (FAILED(hr = m_pNikonLv->get_NDFilter(&pNDFilter)))
					_com_raise_error(hr);

				if ((int)lParam < 0) {
					if (FAILED(hr = pNDFilter->Decrease()))
						_com_raise_error(hr);
				}
				else {
					if (FAILED(hr = pNDFilter->Increase()))
						_com_raise_error(hr);
				}

				if (pNDFilter)
					pNDFilter.Release();
			}
			break;
		case AccessoryEpiApertureStop:
			{
				CComPtr<IEpiApertureStop> pEpiApertureStop = NULL;

				if (FAILED(hr = m_pNikonLv->get_EpiApertureStop(&pEpiApertureStop)))
					_com_raise_error(hr);

				switch (lParam) {
				case 0:
					if (FAILED(hr = pEpiApertureStop->Decrease()))
						_com_raise_error(hr);
					break;
				case 1:
					if (FAILED(hr = pEpiApertureStop->Increase()))
						_com_raise_error(hr);
					break;
				case 2:
					if (FAILED(hr = pEpiApertureStop->SetApertureDefaults()))
						_com_raise_error(hr);
					break;
				default:
					break;
				}

				if (pEpiApertureStop)
					pEpiApertureStop.Release();
			}
			break;
		case AccessoryDICPrism:
			{
				CComPtr<IDICPrism> pDICPrism = NULL;

				if (FAILED(hr = m_pNikonLv->get_DICPrism(&pDICPrism)))
					_com_raise_error(hr);

				switch (lParam) {
				case 0:
					if (FAILED(hr = pDICPrism->Decrease()))
						_com_raise_error(hr);
					break;
				case 1:
					if (FAILED(hr = pDICPrism->Increase()))
						_com_raise_error(hr);
					break;
				case 2:
					if (FAILED(hr = pDICPrism->AdjustDicCalibration()))
						_com_raise_error(hr);
					break;
				case 3:
					if (FAILED(hr = pDICPrism->InitializeDicCalibration()))
						_com_raise_error(hr);
					break;
				case 4:
					if (FAILED(hr = pDICPrism->GoOutOfPosition()))
						_com_raise_error(hr);
					break;
				default:
					break;
				}

				if (pDICPrism)
					pDICPrism.Release();
			}
			break;
		default:
			break;
		}
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	CString s;
	s.Format(_T("### ExecCommand Acc=%d, Param=%d, %d[ms] ###\n"), eAccessory, lParam, GetTickCount() - dwStart);
	OutputDebugString(s);

	return hr;
}

HRESULT CMicroscopeControl::ExecEpiShutterOpenWithTimer(long nTime)
{
	HRESULT hr = S_OK;
	CWaitCursor w;
	DWORD dwStart = GetTickCount();

	try {
		CComPtr<IEpiShutter> pEpiShutter = NULL;

		if (FAILED(hr = m_pNikonLv->get_EpiShutter(&pEpiShutter)))
			_com_raise_error(hr);

		EnumStatus eIsOpen = (EnumStatus)((long)pEpiShutter->IsOpened);

		if (eIsOpen == StatusFalse) {
			if (FAILED(hr = pEpiShutter->OpenWithTimer(nTime)))
				_com_raise_error(hr);
		}

		if (pEpiShutter)
			pEpiShutter.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	CString s;
	s.Format(_T("### ExecEpiShutterOpenWithTimer nTime=%d, %d[ms] ###\n"), nTime, GetTickCount() - dwStart);
	OutputDebugString(s);

	return hr;
}

HRESULT CMicroscopeControl::ExecZDriveMoveAbsolute(long lPos)
{
	HRESULT hr = S_OK;
	CWaitCursor w;
	DWORD dwStart = GetTickCount();

	try {
		CComPtr<IZDrive> pZDrive = NULL;
		if (FAILED(hr = m_pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		if (FAILED(hr = pZDrive->MoveAbsolute(lPos)))
			_com_raise_error(hr);

		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	CString s;
	s.Format(_T("### ZDriveMoveAbsolute Pos=%d, %d[ms] ###\n"), lPos, GetTickCount() - dwStart);
	OutputDebugString(s);

	return hr;
}

HRESULT CMicroscopeControl::ExecZDriveMoveToLimit(long lDirection)
{
	return ExecZDriveMoveAbsolute(lDirection == 1 ? m_lZDriveMax : m_lZDriveMin);
}

HRESULT CMicroscopeControl::ExecZDriveMoveContinuous(long lDirection)
{
	HRESULT hr = S_OK;
	CWaitCursor w;
	DWORD dwStart = GetTickCount();

	try {
		CComPtr<INikonLvDevice> pDevice = NULL;
		CComPtr<IZDrive> pZDrive = NULL;

		if (FAILED(hr = m_pNikonLv->get_Device(&pDevice)))
			_com_raise_error(hr);
		if (FAILED(hr = m_pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		if ((EnumStatus)((long)pZDrive->IsMounted) == StatusTrue) {
			if (FAILED(pDevice->put_Overlapped(TRUE)))
				_com_raise_error(hr);
			if (FAILED(hr = pZDrive->MoveAbsolute(lDirection == 1 ? m_lZDriveMax : m_lZDriveMin)))
				_com_raise_error(hr);
			if (FAILED(pDevice->put_Overlapped(FALSE)))
				_com_raise_error(hr);
		}

		if (pDevice)
			pDevice.Release();
		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	CString s;
	s.Format(_T("### ZDriveMoveContinuous Direction=%d, %d[ms] ###\n"), lDirection, GetTickCount() - dwStart);
	OutputDebugString(s);

	return hr;
}

HRESULT CMicroscopeControl::ExecZDriveAbort()
{
	HRESULT hr = S_OK;
	CWaitCursor w;
	DWORD dwStart = GetTickCount();

	try {
		CComPtr<IZDrive> pZDrive = NULL;
		if (FAILED(hr = m_pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		if (FAILED(hr = pZDrive->AbortZ()))
			_com_raise_error(hr);

		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	CString s;
	s.Format(_T("### ZDriveAbort, %d[ms] ###\n"), GetTickCount() - dwStart);
	OutputDebugString(s);

	return hr;
}

HRESULT CMicroscopeControl::ExecZDriveSetSoftwareLimit(long lDirection)
{
	HRESULT hr = S_OK;
	CWaitCursor w;
	DWORD dwStart = GetTickCount();

	try {
		CComPtr<IZDrive> pZDrive = NULL;
		if (FAILED(hr = m_pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		if (lDirection == 0) {
			if (FAILED(hr = pZDrive->CurrentToSoftwareLowerLimit()))
				_com_raise_error(hr);
			m_lZDriveMin = (long)pZDrive->SoftwareLowerLimit;
		}
		else if (lDirection == 1) {
			if (FAILED(hr = pZDrive->CurrentToSoftwareUpperLimit()))
				_com_raise_error(hr);
			m_lZDriveMax = (long)pZDrive->SoftwareUpperLimit;
		}

		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	CString s;
	s.Format(_T("### ZDriveSetSoftwareLimit Direction=%d, %d[ms] ###\n"), lDirection, GetTickCount() - dwStart);
	OutputDebugString(s);

	return hr;
}

HRESULT CMicroscopeControl::ExecZDriveInitSoftwareLimit(long lDirection)
{
	HRESULT hr = S_OK;
	CWaitCursor w;
	DWORD dwStart = GetTickCount();

	try {
		CComPtr<IZDrive> pZDrive = NULL;
		if (FAILED(hr = m_pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		if (lDirection == 0) {
			if (FAILED(hr = pZDrive->InitSoftwareLowerLimit()))
				_com_raise_error(hr);
			m_lZDriveMin = (long)pZDrive->SoftwareLowerLimit;
		}
		else if (lDirection == 1) {
			if (FAILED(hr = pZDrive->InitSoftwareUpperLimit()))
				_com_raise_error(hr);
			m_lZDriveMax = (long)pZDrive->SoftwareUpperLimit;
		}

		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	CString s;
	s.Format(_T("### ZDriveInitSoftwareLimit Direction=%d, %d[ms] ###\n"), lDirection, GetTickCount() - dwStart);
	OutputDebugString(s);

	return hr;
}

HRESULT CMicroscopeControl::ExecZEscape(BOOL bIsEscape)
{
	HRESULT hr = S_OK;
	CWaitCursor w;
	DWORD dwStart = GetTickCount();

	try {
		CComPtr<IZDrive> pZDrive = NULL;

		if (FAILED(hr = m_pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		if ((EnumStatus)((long)pZDrive->IsMounted) == StatusTrue) {
			EnumStatus eIsEscape = (EnumStatus)((long)pZDrive->IsZEscape);
			if (bIsEscape) {
				if (eIsEscape == StatusFalse) {
					if (FAILED(hr = pZDrive->ZEscape()))
						_com_raise_error(hr);
				}
			}
			else {
				if (eIsEscape == StatusTrue) {
					if (FAILED(hr = pZDrive->Refocus()))
						_com_raise_error(hr);
				}
			}
		}

		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	CString s;
	s.Format(_T("### ZDrive%s, %d[ms] ###\n"), bIsEscape ? _T("ZEscape") : _T("Refocus"), GetTickCount() - dwStart);
	OutputDebugString(s);

	return hr;
}

HRESULT CMicroscopeControl::ExecZDriveAFStateChange(EnumAf eAF)
{
	HRESULT hr = S_OK;

	if (eAF != AfRun && eAF != AfWait)
		return -1;

	try {
		CComPtr<IZDrive> pZDrive = NULL;

		if (FAILED(hr = m_pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		if ((EnumStatus)((long)pZDrive->IsMounted) == StatusTrue)
			pZDrive->Put_AF(eAF);

		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	return hr;
}
HRESULT CMicroscopeControl::ExecZDriveSearchAF(EnumAfSearch eAFSearch)
{
	HRESULT hr = S_OK;

	try {
		CComPtr<INikonLvDevice> pDevice = NULL;
		CComPtr<IZDrive> pZDrive = NULL;

		if (FAILED(hr = m_pNikonLv->get_Device(&pDevice)))
			_com_raise_error(hr);
		if (FAILED(hr = m_pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		if ((EnumStatus)((long)pZDrive->IsMounted) == StatusTrue) {
			if (FAILED(pDevice->put_Overlapped(TRUE)))
				_com_raise_error(hr);
			if (FAILED(hr = pZDrive->SearchAF(eAFSearch)))
				_com_raise_error(hr);
			if (FAILED(pDevice->put_Overlapped(FALSE)))
				_com_raise_error(hr);
		}

		if (pDevice)
			pDevice.Release();
		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	return hr;
}
HRESULT CMicroscopeControl::ExecZDriveStopAF()
{
	HRESULT hr = S_OK;

	try {
		CComPtr<IZDrive> pZDrive = NULL;

		if (FAILED(hr = m_pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		if ((EnumStatus)((long)pZDrive->IsMounted) == StatusTrue) {
			if (FAILED(hr = pZDrive->StopAF()))
				_com_raise_error(hr);
		}

		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	return hr;
}
HRESULT CMicroscopeControl::ExecZDriveSetSpeed(EnumSpeed eSpeed)
{
	HRESULT hr = S_OK;
	if (eSpeed != SpeedUnknown) {
		CWaitCursor w;
		DWORD dwStart = GetTickCount();

		try {
			CComPtr<IZDrive> pZDrive = NULL;

			if (FAILED(hr = m_pNikonLv->get_ZDrive(&pZDrive)))
				_com_raise_error(hr);

			if ((EnumStatus)((long)pZDrive->IsMounted) == StatusTrue)
				pZDrive->Put_Speed(eSpeed);

			if (pZDrive)
				pZDrive.Release();
		}
		catch (_com_error &e) {
			hr = e.Error();
		}

		CString s;
		s.Format(_T("### ZDriveSetSpeed Speed=%d, %d[ms] ###\n"), (long)eSpeed, GetTickCount() - dwStart);
		OutputDebugString(s);
	}

	return hr;
}

HRESULT CMicroscopeControl::ExecSelectCommand(EnumAccessory eAccessory, long lIndex)
{
	HRESULT hr = S_OK;
	CWaitCursor w;
	DWORD dwStart = GetTickCount();

	try {
		switch (eAccessory) {
		case AccessoryNosepiece:
			{
				CComPtr<INosepiece> pNosepiece = NULL;

				if (FAILED(hr = m_pNikonLv->get_Nosepiece(&pNosepiece)))
					_com_raise_error(hr);

				if (lIndex >= (long)pNosepiece->LowerLimit && lIndex <= (long)pNosepiece->UpperLimit)
					pNosepiece->Put_Value(lIndex);

				if (pNosepiece)
					pNosepiece.Release();
			}
			break;
		case AccessoryFilterBlockCassette:
			{
				CComPtr<IFilterBlockCassette> pFilterBlockCassette = NULL;

				if (FAILED(hr = m_pNikonLv->get_FilterBlockCassette(&pFilterBlockCassette)))
					_com_raise_error(hr);

				if (lIndex >= (long)pFilterBlockCassette->LowerLimit && lIndex <= (long)pFilterBlockCassette->UpperLimit)
					pFilterBlockCassette->Put_Value(lIndex);

				if (pFilterBlockCassette)
					pFilterBlockCassette.Release();
			}
			break;
		default:
			break;
		}
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	CString s;s.Format(_T("### ExecSelectCommand Acc=%d, Index=%d, %d[ms] ###\n"), eAccessory, lIndex, GetTickCount() - dwStart);OutputDebugString(s);

	return hr;
}

HRESULT CMicroscopeControl::GetDICCalibration(BOOL* pbIsCalibration)
{
	HRESULT hr = S_OK;

	try {
		CComPtr<IDICPrism> pDicPrism = NULL;

		if (FAILED(hr = m_pNikonLv->get_DICPrism(&pDicPrism)))
			_com_raise_error(hr);

		if ((EnumStatus)((long)pDicPrism->IsMounted) == StatusTrue) {
			EnumStatus eIsCalibration = (EnumStatus)((long)pDicPrism->IsCalibration);

			if (eIsCalibration == StatusFalse)
				*pbIsCalibration = FALSE;
			else if (eIsCalibration == StatusTrue)
				*pbIsCalibration = TRUE;
			else
				hr = -1;
		}

		if (pDicPrism)
			pDicPrism.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	return hr;
}

HRESULT CMicroscopeControl::GetDICPhaseDifference(double* pdPhaseDifference)
{
	HRESULT hr = S_OK;

	try {
		CComPtr<IDICPrism> pDicPrism = NULL;

		if (FAILED(hr = m_pNikonLv->get_DICPrism(&pDicPrism)))
			_com_raise_error(hr);

		if ((EnumStatus)((long)pDicPrism->IsMounted) == StatusTrue)
			*pdPhaseDifference = (double)pDicPrism->PhaseDifference;

		if (pDicPrism)
			pDicPrism.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	return hr;
}

HRESULT CMicroscopeControl::GetDICOutOfPosition(BOOL* pbIsOut)
{
	HRESULT hr = S_OK;

	try {
		CComPtr<IDICPrism> pDicPrism = NULL;

		if (FAILED(hr = m_pNikonLv->get_DICPrism(&pDicPrism)))
			_com_raise_error(hr);

		if ((EnumStatus)((long)pDicPrism->IsMounted) == StatusTrue) {
			EnumStatus eIsOut = (EnumStatus)((long)pDicPrism->IsOutOfPosition);

			if (eIsOut == StatusFalse)
				*pbIsOut = FALSE;
			else if (eIsOut == StatusTrue)
				*pbIsOut = TRUE;
			else
				hr = -1;
		}

		if (pDicPrism)
			pDicPrism.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	return hr;
}

HRESULT CMicroscopeControl::SetInterlock(BOOL bIsInterlock)
{
	HRESULT hr = S_OK;

	try {
		EnumInterlockMode eInterlock = bIsInterlock ? InterlockEnabled : InterlockDisabled;

		CComPtr<IPresets> pPresets = NULL;
		if (FAILED(hr = m_pNikonLv->get_Presets(&pPresets)))
			_com_raise_error(hr);

		CComPtr<IEpiLamp> pEpiLamp = NULL;
		CComPtr<IDiaLamp> pDiaLamp = NULL;
		CComPtr<IEpiApertureStop> pEpiApertureStop = NULL;
		CComPtr<IDICPrism> pDicPrism = NULL;

		// EPI
		if (FAILED(hr = m_pNikonLv->get_EpiLamp(&pEpiLamp)))
			_com_raise_error(hr);
		if ((EnumStatus)((long)pEpiLamp->IsMounted) == StatusTrue) {
			if (FAILED(hr = pPresets->SetInterlockMode(AccessoryEpiLampVoltage, eInterlock)))
				_com_raise_error(hr);
		}
		if (pEpiLamp)
			pEpiLamp.Release();

		// DIA
		if (FAILED(hr = m_pNikonLv->get_DiaLamp(&pDiaLamp)))
			_com_raise_error(hr);
		if ((EnumStatus)((long)pDiaLamp->IsMounted) == StatusTrue) {
			if (FAILED(hr = pPresets->SetInterlockMode(AccessoryDiaLampVoltage, eInterlock)))
				_com_raise_error(hr);
		}
		if (pDiaLamp)
			pDiaLamp.Release();

		// Apreture
		if (FAILED(hr = m_pNikonLv->get_EpiApertureStop(&pEpiApertureStop)))
			_com_raise_error(hr);
		if ((EnumStatus)((long)pEpiApertureStop->IsMounted) == StatusTrue) {
			if (FAILED(hr = pPresets->SetInterlockMode(AccessoryEpiApertureStop, eInterlock)))
				_com_raise_error(hr);
		}
		if (pEpiApertureStop)
			pEpiApertureStop.Release();

		// DIC
		if (FAILED(hr = m_pNikonLv->get_DICPrism(&pDicPrism)))
			_com_raise_error(hr);
		if ((EnumStatus)((long)pDicPrism->IsMounted) == StatusTrue) {
			if (FAILED(hr = pPresets->SetInterlockMode(AccessoryDICPrismShift, eInterlock)))
				_com_raise_error(hr);
		}
		if (pDicPrism)
			pDicPrism.Release();

		if (pPresets)
			pPresets.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	return hr;
}

HRESULT CMicroscopeControl::SetZDriveInterlock(BOOL bIsInterlock)
{
	HRESULT hr = S_OK;

	try {
		EnumInterlockMode eInterlock = bIsInterlock ? InterlockEnabled : InterlockDisabled;

		CComPtr<IPresets> pPresets = NULL;
		if (FAILED(hr = m_pNikonLv->get_Presets(&pPresets)))
			_com_raise_error(hr);

		// Z Drive
		CComPtr<IZDrive> pZDrive = NULL;
		if (FAILED(hr = m_pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);
		if ((EnumStatus)((long)pZDrive->IsMounted) == StatusTrue) {
			if (FAILED(hr = pPresets->SetInterlockMode(AccessoryZDrive, eInterlock)))
				_com_raise_error(hr);
		}
		if (pPresets)
			pPresets.Release();
		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	return hr;
}

HRESULT CMicroscopeControl::ReadInterlock(EnumAccessory eAccessory, BOOL* pbIsInterlock)
{
	HRESULT hr = S_OK;

	try {
		CComPtr<IPresets> pPresets = NULL;
		if (FAILED(hr = m_pNikonLv->get_Presets(&pPresets)))
			_com_raise_error(hr);

		switch (eAccessory) {
		case AccessoryEpiLampVoltage:
		case AccessoryDiaLampVoltage:
		case AccessoryEpiApertureStop:
		case AccessoryDICPrismShift:
		case AccessoryZDrive:
			{
				EnumInterlockMode eInterlock = pPresets->ReadInterlockMode(eAccessory);
				if (eInterlock == InterlockDisabled)
					*pbIsInterlock = FALSE;
				else if (eInterlock == InterlockEnabled)
					*pbIsInterlock = TRUE;
				else
					hr = -1;
			}
			break;
		default:
			hr = -1;
			break;
		}

		if (pPresets)
			pPresets.Release();
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	return hr;
}

HRESULT CMicroscopeControl::ShowSimulationWindow(HWND hWnd, BOOL bShow)
{
	HRESULT hr = S_OK;

	try {
		CString strCommand;
		strCommand.Format(_T("ShowWindow,%d,%d"), (long)hWnd, bShow ? 1 : 0);

		CComBSTR bstrCommand(strCommand);
		if (FAILED(hr = m_pNikonLv->SendData(bstrCommand.m_str)))
			_com_raise_error(hr);
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	return hr;
}

HRESULT CMicroscopeControl::SendCommand(LPCTSTR lpszCommand)
{
	HRESULT hr = S_OK;

	try {
		CComBSTR bstrCommand(lpszCommand);
		if (FAILED(hr = m_pNikonLv->SendData(bstrCommand.m_str)))
			_com_raise_error(hr);
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	return hr;
}

HRESULT CMicroscopeControl::GetCommand(LPCTSTR lpszCommand, CString& strResult)
{
	HRESULT hr = S_OK;

	try {
		CComBSTR bstrCommand(lpszCommand);
		_bstr_t bstrResult = m_pNikonLv->GetData(bstrCommand.m_str);
		strResult = CString(bstrResult.GetBSTR());
	}
	catch (_com_error &e) {
		hr = e.Error();
	}

	return hr;
}
void CMicroscopeControl::InitZDriveRange()
{
	if (!m_pNikonLv)
		return;

	HRESULT hr = S_OK;
	try {
		CComPtr<IZDrive> pZDrive = NULL;
		if (FAILED(hr = m_pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		m_lZDriveMin = (long)pZDrive->SoftwareLowerLimit;
		m_lZDriveMax = (long)pZDrive->SoftwareUpperLimit;

		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
		return;
	}
}
void CMicroscopeControl::SetZDriveMin(int nMin)
{
	if (!m_pNikonLv)
		return;

	HRESULT hr = S_OK;
	try {
		CComPtr<IZDrive> pZDrive = NULL;
		if (FAILED(hr = m_pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		if (nMin >= (long)pZDrive->SoftwareLowerLimit)
			m_lZDriveMin = nMin;

		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}
void CMicroscopeControl::SetZDriveMax(int nMax)
{
	if (!m_pNikonLv)
		return;

	HRESULT hr = S_OK;
	try {
		CComPtr<IZDrive> pZDrive = NULL;
		if (FAILED(hr = m_pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		if (nMax <= (long)pZDrive->SoftwareUpperLimit)
			m_lZDriveMax = nMax;

		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}
