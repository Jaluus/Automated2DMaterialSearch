// DLVSetup.h  : Microsoft Visual C++ で作成された ActiveX コントロール ラッパー クラスの宣言です。

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CDLVSetup

class CDLVSetup : public CWnd
{
protected:
	DECLARE_DYNCREATE(CDLVSetup)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x94E9E54B, 0x1F7F, 0x4B13, { 0x88, 0x3C, 0xA9, 0x3C, 0x6F, 0x55, 0x77, 0xB8 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// 属性
public:

	long GetWidth()
	{
		long result;
		GetProperty(0x3, VT_I4, (void*)&result);
		return result;
	}
	void SetWidth(long propVal)
	{
		SetProperty(0x3, VT_I4, propVal);
	}
	long GetHeight()
	{
		long result;
		GetProperty(0x4, VT_I4, (void*)&result);
		return result;
	}
	void SetHeight(long propVal)
	{
		SetProperty(0x4, VT_I4, propVal);
	}

// 操作
public:

	long Start(LPUNKNOWN pSdk)
	{
		long result;
		static BYTE parms[] = VTS_UNKNOWN ;
		InvokeHelper(0x1, DISPATCH_METHOD, VT_I4, (void*)&result, parms, pSdk);
		return result;
	}
	long End()
	{
		long result;
		InvokeHelper(0x2, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}


};
