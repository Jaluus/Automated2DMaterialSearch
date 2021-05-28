// PropCombo.cpp : 実装ファイル
//

#include "stdafx.h"
#include "LvSample.h"
#include "PropCombo.h"

// CPropCombo

IMPLEMENT_DYNAMIC(CPropCombo, CComboBox)

CPropCombo::CPropCombo()
{
	m_Source.m_pProperty = NULL;
	m_Callback = NULL;
	m_eAccessory = AccessoryNosepiece;
	m_nAddUnknown = -1;
	m_bUseUnknownSel = TRUE;
	m_bValueChanged = FALSE;
}

CPropCombo::~CPropCombo()
{
}

BEGIN_MESSAGE_MAP(CPropCombo, CComboBox)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_PAREVENT, OnParamEvent)
END_MESSAGE_MAP()

// CPropCombo メッセージ ハンドラ

void CPropCombo::OnDestroy()
{
	CComboBox::OnDestroy();

	Disconnect();
}

LRESULT CPropCombo::OnParamEvent(WPARAM wParam, LPARAM lParam)
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

// CPropCombo

BOOL CPropCombo::Connect(IDispatch *piParameter)
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

void CPropCombo::Disconnect()
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

BOOL CPropCombo::InfoChanged()
{
	HRESULT hr = S_OK;

	try {
		m_Source.ResetInfoChangedFlag();

		ATLASSERT(m_Source);
		EMIPPAR_LIMITATION lt;
		m_Source->get_LimitationType(&lt);

		this->ResetContent();

		VARIANT_BOOL bRead = VARIANT_FALSE;
		m_Source->get_IsReadOnly(&bRead);
		if (bRead == VARIANT_TRUE)
			this->EnableWindow(FALSE);
		else
			this->EnableWindow(TRUE);

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

			long nAdd = 0;
			ResetContent();
			DisplayRawPair stDisplayRawPair;
			for (int iItem = 0; iItem < lCount; iItem++) {
				hr = pEnumSet->get_Item(iItem + 1, &stDisplayRawPair);
				if (FAILED(hr)) 
					_com_raise_error(hr);

				_variant_t vtTemp = stDisplayRawPair.RawValue;
				switch ((long)vtTemp) {
				case -1:
					break;
				default:
					SetItemData(AddString(CString(stDisplayRawPair.DisplayString)), vtTemp.lVal);
					break;
				}

				::SysFreeString(stDisplayRawPair.DisplayString);
				::VariantClear(&stDisplayRawPair.RawValue);
			}

			pEnumSet->Release();
		}
		else if (lt == EMIPPAR_RANGE) {
			VARIANT low, high, step;
			hr = m_Source->get_RangeLowerLimit(&low);
			if (FAILED(hr)) 
				_com_raise_error(hr);
			hr = m_Source->get_RangeHigherLimit(&high);
			if (FAILED(hr)) 
				_com_raise_error(hr);
			hr = m_Source->get_RangeIncrement(&step);
			if (FAILED(hr)) 
				_com_raise_error(hr);

			if (low.vt == VT_I4) {
				for (int i = low.lVal; i <= high.lVal; i+=step.lVal) {
					CString strName;
					strName.Format(_T("%d"), i);

					if (m_Callback) {
						CString strElement;
						m_Callback(m_eAccessory, i, strElement);
						if (!strElement.IsEmpty()) {
							strName += ':';
							strName += strElement;
						}
					}

					SetItemData(AddString(strName), i);
				}
			}
		}
	}
	catch (_com_error/* &e*/) {
		return FALSE;
	}
	return TRUE;
}

BOOL CPropCombo::ValueChanged()
{
	try {
		m_Source.ResetValueChangedFlag();

		RemoveUnknown();

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
			int i;
			for (i = 0; i < GetCount(); i++) {
				if (GetItemData(i) == result.lVal) {
					SetCurSel(i);
					break;
				}
			}
			if (i == GetCount()) {
				if (m_bUseUnknownSel) {
					CString str;
					str.Format(_T("%d:Unknown"), result.lVal);
					m_nAddUnknown = AddString(str);
					SetItemData(m_nAddUnknown, result.lVal);
					SetCurSel(m_nAddUnknown);
				}
				else
					SetCurSel(-1);
			}
		}
	}
	catch (_com_error/* &e*/) {
		m_bValueChanged = FALSE;
	}
	m_bValueChanged = TRUE;
	
	return m_bValueChanged;
}

BOOL CPropCombo::PutValue()
{
	HRESULT hr;
	CWaitCursor w;

	try {
		if (m_Source.m_pProperty != NULL) {
			int nVal = (int)GetItemData(GetCurSel());

			EMIPPAR_LIMITATION lt;
			m_Source->get_LimitationType(&lt);

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

				for (long i = 0; i < lCount; i++) {
					DisplayRawPair stDisplayRawPair;
					hr = pEnumSet->get_Item(i + 1, &stDisplayRawPair);
					if (FAILED(hr)) 
						_com_raise_error(hr);
					_variant_t vtTemp = stDisplayRawPair.RawValue;
					if (vtTemp.lVal == nVal) {
						DWORD dwStart = GetTickCount();
						hr = m_Source.m_pProperty->put_RawValue(vtTemp);
						CString s;s.Format(_T("### PropCombo::PutValue ID=%d, %d[ms] ###\n"), GetDlgCtrlID(), GetTickCount() - dwStart);OutputDebugString(s);
						if (FAILED(hr))
							_com_raise_error(hr);
						break;
					}
				}
			}
			else if (lt == EMIPPAR_RANGE) {
				_variant_t vtTemp = nVal;
				DWORD dwStart = GetTickCount();
				hr = m_Source.m_pProperty->put_RawValue(vtTemp);
				CString s;s.Format(_T("### PropCombo::PutValue ID=%d, %d[ms] ###\n"), GetDlgCtrlID(), GetTickCount() - dwStart);OutputDebugString(s);
				if (FAILED(hr))
					_com_raise_error(hr);
			}

			return TRUE;
		}
		return FALSE;
	}
	catch (_com_error/* &e*/) {
		ValueChanged();
		return FALSE;
	}
}

BOOL CPropCombo::GetValue(VARIANT* pVal)
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
