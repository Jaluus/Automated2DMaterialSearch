// PropStatic.cpp : 実装ファイル
//

#include "stdafx.h"
#include "LvSample.h"
#include "PropStatic.h"

// CPropEdit

IMPLEMENT_DYNAMIC(CPropStatic, CStatic)

CPropStatic::CPropStatic()
	: m_nUnit(1)
	, m_nDec(1)
	, m_vtDisp(VT_BSTR)
{
	m_Source.m_pProperty = NULL;
}

CPropStatic::~CPropStatic()
{
}

BEGIN_MESSAGE_MAP(CPropStatic, CStatic)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_PAREVENT, OnParamEvent)
END_MESSAGE_MAP()

// CPropStatic メッセージ ハンドラ

void CPropStatic::OnDestroy()
{
	CStatic::OnDestroy();

	Disconnect();
}

LRESULT CPropStatic::OnParamEvent(WPARAM wParam, LPARAM lParam)
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

// CPropStatic

BOOL CPropStatic::Connect(IDispatch *piParameter)
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

void CPropStatic::Disconnect()
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

BOOL CPropStatic::InfoChanged()
{
	HRESULT hr = S_OK;

	try {
		m_Source.ResetInfoChangedFlag();

		ATLASSERT(m_Source);
	}
	catch (_com_error/* &e*/) {
		return FALSE;
	}
	return TRUE;
}

BOOL CPropStatic::ValueChanged()
{
	try {
		m_Source.ResetValueChangedFlag();

		if (m_Source.m_pProperty != NULL) {
			VARIANT result;
			::VariantInit(&result);

			HRESULT hr = m_Source->get_RawValue(&result);
			if (FAILED(hr)) {
				if (hr == DISP_E_EXCEPTION)
					_com_raise_error(result.lVal);
				else
					_com_raise_error(hr);
			}

			CString strDisp;
			strDisp.Empty();
			if (m_vtDisp == VT_BSTR) {
				BSTR bstr;
				hr = m_Source->get_DisplayString(&bstr);
				if (!FAILED(hr))
					strDisp.Format(_T("%s"), bstr);
			}
			else {
				if (result.vt == VT_I4) {
					if (m_vtDisp == VT_R8)
						strDisp.Format(_T("%.*lf"), m_nDec, (double)result.lVal / (double)m_nUnit);
					else
						strDisp.Format(_T("%d"), result.lVal / m_nUnit);
				}
				else if (result.vt == VT_R8)
					strDisp.Format(_T("%.*lf"), m_nDec, result.dblVal);
			}
			if (!strDisp.IsEmpty())
				SetWindowText(strDisp);
		}
	}
	catch (_com_error/* &e*/) {
		return FALSE;
	}
	return TRUE;
}

BOOL CPropStatic::GetValue(VARIANT* pVal)
{
	try {
		if (m_Source.m_pProperty != NULL) {
			HRESULT hr = m_Source->get_RawValue(pVal);
			if (FAILED(hr)) 
				_com_raise_error(hr);
			return TRUE;
		}
		return FALSE;
	}
	catch (_com_error/* &e*/) {
		return FALSE;
	}
}
