// PropEdit.cpp : 実装ファイル
//

#include "stdafx.h"
#include "LvSample.h"
#include "PropEdit.h"

// CPropEdit

IMPLEMENT_DYNAMIC(CPropEdit, CEdit)

CPropEdit::CPropEdit()
	: m_bFireValueChanged(FALSE)
	, m_bFireInfoChanged(FALSE)
{
	m_Source.m_pProperty = NULL;
}

CPropEdit::~CPropEdit()
{
}

BEGIN_MESSAGE_MAP(CPropEdit, CEdit)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_PAREVENT, OnParamEvent)
END_MESSAGE_MAP()

// CPropEdit メッセージ ハンドラ

void CPropEdit::OnDestroy()
{
	CEdit::OnDestroy();

	Disconnect();
}

LRESULT CPropEdit::OnParamEvent(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)	{
	case EMIPPAR_VALUECHANGED:
		ValueChanged();
		break;
	case EMIPPAR_INFOCHANGED:
		InfoChanged();
		ValueChanged();
		break;
	}
	return 0L;
}

// CPropEdit

BOOL CPropEdit::Connect(IDispatch *piParameter)
{
	if (m_Source.m_pProperty)
		return FALSE;

	if (piParameter == NULL)
		return FALSE;

	Disconnect();

	try {
		HRESULT hr;
		IMipParameterPtr spiParam;
		if (FAILED(hr = piParameter->QueryInterface(&spiParam)))
			return FALSE;

		m_Source.m_pProperty = spiParam;
		hr = m_Source.AdviseAsync((long)m_hWnd, WM_PAREVENT, 0);

		InfoChanged();
		ValueChanged();
	}
	catch (_com_error/* &e*/) {
		return FALSE;
	}
	return TRUE;
}

void CPropEdit::Disconnect()
{
	try {
		HRESULT hr = m_Source.UnAdviseAsync();
		if (m_Source.m_pProperty) {
			m_Source.m_pProperty->Release();
			m_Source.m_pProperty = NULL;
		}
	}
	catch (_com_error/* &e*/) {
		return;
	}
}

BOOL CPropEdit::InfoChanged()
{
	HRESULT hr = S_OK;

	try {
		m_Source.ResetInfoChangedFlag();

		ATLASSERT(m_Source);

		VARIANT_BOOL bRead = VARIANT_FALSE;
		m_Source->get_IsReadOnly(&bRead);
		if (bRead == VARIANT_TRUE)
			SetReadOnly(TRUE);
		else
			SetReadOnly(FALSE);

		if (m_bFireInfoChanged)
			GetParent()->PostMessageW(WM_MIP_INFO_CHANGED, (WPARAM)GetSafeHwnd(), (LPARAM)GetDlgCtrlID());
	}
	catch (_com_error/* &e*/) {
		return FALSE;
	}
	return TRUE;
}

BOOL CPropEdit::ValueChanged()
{
	try {
		m_Source.ResetValueChangedFlag();

		if (m_Source.m_pProperty != NULL) {
			VARIANT result;
			::VariantInit(&result);

			BSTR bstrVal = NULL;
#if 0
			HRESULT hr = m_Source->get_RawValue(&result);
#else
			HRESULT hr = m_Source->get_DisplayString(&bstrVal);
#endif
			if (FAILED(hr)) {
				if (hr == DISP_E_EXCEPTION)
					_com_raise_error(result.lVal);
				else
					_com_raise_error(hr);
			}

			CString strVal(bstrVal);
			SetWindowText(strVal);

			if (bstrVal)
				::SysFreeString(bstrVal);

			if (m_bFireValueChanged)
				GetParent()->PostMessageW(WM_MIP_VALUE_CHANGED, (WPARAM)GetSafeHwnd(), (LPARAM)GetDlgCtrlID());
		}
	}
	catch (_com_error/* &e*/) {
		return FALSE;
	}
	return TRUE;
}

BOOL CPropEdit::PutValue()
{
	HRESULT hr;
	CWaitCursor w;

	try {
		if (m_Source.m_pProperty != NULL) {
			CString strText;
			GetWindowText(strText);
			DWORD dwStart = GetTickCount();
#if 0
			int nVal = _ttoi(strText);
			_variant_t vtTemp = nVal;
			hr = m_Source.m_pProperty->put_RawValue(vtTemp);
#else
			CComBSTR bstrVal(strText);
			hr = m_Source.m_pProperty->put_DisplayString(bstrVal);
#endif
			CString s;s.Format(_T("### PropEdit::PutValue ID=%d, %d[ms] ###\n"), GetDlgCtrlID(), GetTickCount() - dwStart);OutputDebugString(s);
			if (FAILED(hr))
				_com_raise_error(hr);

			return TRUE;
		}
		return FALSE;
	}
	catch (_com_error/* &e*/) {
		ValueChanged();
		return FALSE;
	}
}

void CPropEdit::GetValue(int* pValue, int* pLow, int* pHigh)
{
	HRESULT hr = S_OK;

	if (pValue) *pValue = 0;
	if (pLow) *pLow = 0;
	if (pHigh) *pHigh = 0;

	try {
		ATLASSERT(m_Source);
		EMIPPAR_LIMITATION lt;
		m_Source->get_LimitationType(&lt);

		VARIANT result;
		::VariantInit(&result);
		HRESULT hr = m_Source->get_RawValue(&result);
		if (FAILED(hr)) {
			if (hr == DISP_E_EXCEPTION)
				_com_raise_error(result.lVal);
			else
				_com_raise_error(hr);
		}

		if (lt == EMIPPAR_ENUM) {
			struct IDisplayRawPairCollection* pEnumSet;
			hr = m_Source->get_EnumSet(&pEnumSet);
			if (FAILED(hr)) 
				_com_raise_error(hr);

			long lCount = 0;
			if (pEnumSet != NULL) {
				hr = pEnumSet->get_Count(&lCount);
				if (FAILED(hr)) 
					_com_raise_error(hr);
			}

			DisplayRawPair stDisplayRawPair;
			for (int iItem = 0; iItem < lCount; iItem++) {
				hr = pEnumSet->get_Item(iItem + 1, &stDisplayRawPair);
				if (FAILED(hr)) 
					_com_raise_error(hr);

				if (pValue) *pValue = stDisplayRawPair.RawValue.lVal;

				::SysFreeString(stDisplayRawPair.DisplayString);
				::VariantClear(&stDisplayRawPair.RawValue);
			}

			if (pLow) *pLow = 0;
			if (pHigh) *pHigh = lCount - 1;

			pEnumSet->Release();
		}
		else if (lt == EMIPPAR_RANGE) {
			if (pValue) {
				if (result.vt == VT_I4)
					*pValue = result.lVal;
			}
			if (pLow) {
				VARIANT low;
				::VariantInit(&low);

				hr = m_Source->get_RangeLowerLimit(&low);
				if (FAILED(hr)) 
					_com_raise_error(hr);

				if (low.vt == VT_I4)
					*pLow = low.lVal;
			}
			if (pHigh) {
				VARIANT high;
				::VariantInit(&high);

				hr = m_Source->get_RangeHigherLimit(&high);
				if (FAILED(hr)) 
					_com_raise_error(hr);

				if (high.vt == VT_I4)
					*pHigh = high.lVal;
			}
		}
	}
	catch (_com_error/* &e*/) {
	}
}
