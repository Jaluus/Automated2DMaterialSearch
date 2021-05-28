// MicroscopeControl.h: CMicroscopeControl クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MICROSCOPECONTROL_H__9D314996_ED55_41B5_9FB6_20AF99EB59D2__INCLUDED_)
#define AFX_MICROSCOPECONTROL_H__9D314996_ED55_41B5_9FB6_20AF99EB59D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MICROSCOPE_NOT_AVAILABLE ((HRESULT)0x100L);

/*! @file
 @brief MicroscopeInfo.h クラス ヘッダー ファイル

 このファイルは MicroscopeInfo.h です。
 @author 作者 Nikon
 @date 作成日, 履歴

*/
/*!
 @brief CMicroscopeControl 顕微鏡制御クラス
 ()

*/
class CMicroscopeControl  
{
public:
	CMicroscopeControl();
	virtual ~CMicroscopeControl();

	HRESULT Connect();
	HRESULT Disconnect();
	HRESULT Uninitialize();
	HRESULT Initialize();

	HRESULT ConnectSelectedDevice(long lIndex);

	inline INikonLv* GetInterface() { return m_pNikonLv; }
	// Device type
	inline BOOL IsAFController() { return m_eController == AF_Controller; }
	inline BOOL IsLVSupportObservationMode() { return (m_eController == LV_Controller || m_eController == E_Controller); }
	inline BOOL IsLV150NAController() { return m_eController == LV150NA_Controller; }
	inline BOOL IsLVINADController() { return m_eController == LVINAD_Controller; }
	inline BOOL IsLVNController() { return IsLV150NAController() || IsLVINADController(); }
	// Z Drive range
	inline int GetZDriveMin() { return m_lZDriveMin; }
	inline int GetZDriveMax() { return m_lZDriveMax; }
	void InitZDriveRange();
	void SetZDriveMin(int nMin);
	void SetZDriveMax(int nMax);

	HRESULT GetName(EnumAccessory eAccessory, long lVal, CString& strName);
	HRESULT ExecCommand(EnumAccessory eAccessory, long lParam);
	HRESULT ExecEpiShutterOpenWithTimer(long nTime);
	HRESULT ExecZDriveMoveAbsolute(long lPos);
	HRESULT ExecZDriveMoveToLimit(long lDirection);
	HRESULT ExecZDriveMoveContinuous(long lDirection);
	HRESULT ExecZDriveAbort();
	HRESULT ExecZDriveSetSoftwareLimit(long lDirection);
	HRESULT ExecZDriveInitSoftwareLimit(long lDirection);
	HRESULT ExecZEscape(BOOL bIsEscape);
	HRESULT ExecZDriveAFStateChange(EnumAf eAF);
	HRESULT ExecZDriveSearchAF(EnumAfSearch eAFSearch);
	HRESULT ExecZDriveStopAF();
	HRESULT ExecZDriveSetSpeed(EnumSpeed eSpeed);
	HRESULT ExecSelectCommand(EnumAccessory eAccessory, long lIndex);
	HRESULT GetDICCalibration(BOOL* pbIsCalibration);
	HRESULT GetDICPhaseDifference(double* pdPhaseDifference);
	HRESULT GetDICOutOfPosition(BOOL* pbIsOut);
	HRESULT SetInterlock(BOOL bIsInterlock);
	HRESULT SetZDriveInterlock(BOOL bIsInterlock);
	HRESULT ReadInterlock(EnumAccessory eAccessory, BOOL* pbIsInterlock);

	HRESULT ShowSimulationWindow(HWND hWnd, BOOL bShow);

	HRESULT SendCommand(LPCTSTR lpszCommand);
	HRESULT GetCommand(LPCTSTR lpszCommand, CString& strResult);

	// Control type
	EnumController m_eController;
	// Simulation
	BOOL m_bIsSimulation;
	// Nosepiece confirm
	BOOL m_bConfirm;
	// Device mounted
	BOOL m_bEpiLampMounted;
	BOOL m_bFiberMounted;
	BOOL m_bNDFilterMounted;
	BOOL m_bNosepieceMounted;
	BOOL m_bCubeMounted;
	BOOL m_bDiaLampMounted;
	BOOL m_bZDriveMounted;
	BOOL m_bApertureMounted;
	BOOL m_bDicMounted;

protected:
	INikonLv* m_pNikonLv;
	// Z Drive range
	long m_lZDriveMin;
	long m_lZDriveMax;
};

#endif // !defined(AFX_MICROSCOPECONTROL_H__9D314996_ED55_41B5_9FB6_20AF99EB59D2__INCLUDED_)
