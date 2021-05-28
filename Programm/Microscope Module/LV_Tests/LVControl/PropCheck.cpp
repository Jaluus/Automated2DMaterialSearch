// PropCheck.cpp : 実装ファイル
//

#include "stdafx.h"
#include "LvSample.h"
#include "PropCheck.h"

// CPropCheck

IMPLEMENT_DYNAMIC(CPropCheck, CButton)

CPropCheck::CPropCheck()
{
	m_Source.m_pProperty = NULL;
}

CPropCheck::~CPropCheck()
{
}

BEGIN_MESSAGE_MAP(CPropCheck, CButton)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_PAREVENT, OnParamEvent)
END_MESSAGE_MAP()

// CPropCheck メッセージ ハンドラ

void CPropCheck::OnDestroy()
{
	CButton::OnDestroy();

	Disconnect();
}

LRESULT CPropCheck::OnParamEvent(WPARAM wParam, LPARAM lParam)
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
	::SendMessage(::GetParent(m_hWnd), WM_PAREVENT, (WPARAM)::GetDlgCtrlID(m_hWnd), lParam);
	return 0L;
}

// CPropCheck

BOOL CPropCheck::Connect(IDispatch *piParameter)
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

void CPropCheck::Disconnect()
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

BOOL CPropCheck::InfoChanged()
{
	HRESULT hr = S_OK;

	try {
		m_Source.ResetInfoChangedFlag();

		ATLASSERT(m_Source);

		VARIANT_BOOL bRead = VARIANT_FALSE;
		m_Source->get_IsReadOnly(&bRead);
		if (bRead == VARIANT_TRUE)
			EnableWindow(FALSE);
		else
			EnableWindow(TRUE);
	}
	catch (_com_error/* &e*/) {
		return FALSE;
	}
	return TRUE;
}

BOOL CPropCheck::ValueChanged()
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

			if (result.vt == VT_I4)
				SetCheck(result.lVal ? BST_CHECKED : BST_UNCHECKED);
		}
	}
	catch (_com_error/* &e*/) {
		return FALSE;
	}
	return TRUE;
}

BOOL CPropCheck::PutValue()
{
	HRESULT hr;
	CWaitCursor w;

	try {
		if (m_Source.m_pProperty != NULL) {
			DWORD dwStart = GetTickCount();
			int nVal = GetCheck() ? 1 : 0;
			_variant_t vtTemp = nVal;
			hr = m_Source.m_pProperty->put_RawValue(vtTemp);
			CString s;s.Format(_T("### PropCheck::PutValue ID=%d, %d[ms] ###\n"), GetDlgCtrlID(), GetTickCount() - dwStart);OutputDebugString(s);
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

BOOL CPropCheck::GetValue(VARIANT* pVal)
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
