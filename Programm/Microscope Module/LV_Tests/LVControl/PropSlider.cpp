// PropSlider.cpp : 実装ファイル
//

#include "stdafx.h"
#include "LvSample.h"
#include "PropSlider.h"

// CPropSlider

IMPLEMENT_DYNAMIC(CPropSlider, CSliderCtrl)

CPropSlider::CPropSlider()
	: m_nOffset(0)
	, m_dOffset(0.)
	, m_dStep(1.)
	, m_bTracking(FALSE)
	, m_bReverse(FALSE)
	, m_bUserRange(FALSE)
	, m_vtDisp(VT_BSTR)
	, m_strUnit(_T(""))
	, m_bDispUnit(FALSE)
	, m_nUnit(1)
	, m_nDec(1)
	, m_ltProp(EMIPPAR_RANGE)
	, m_vtProp(VT_I4)
{
	m_Source.m_pProperty = NULL;
}

CPropSlider::~CPropSlider()
{
}

BEGIN_MESSAGE_MAP(CPropSlider, CSliderCtrl)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_PAREVENT, OnParamEvent)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_NOTIFY(TTN_NEEDTEXT, 0, OnNeedText)
END_MESSAGE_MAP()

// CPropSlider メッセージ ハンドラ

void CPropSlider::OnDestroy()
{
	CSliderCtrl::OnDestroy();

	Disconnect();
}

LRESULT CPropSlider::OnParamEvent(WPARAM wParam, LPARAM lParam)
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

// CPropSlider

BOOL CPropSlider::Connect(IDispatch *piParameter)
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

void CPropSlider::Disconnect()
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

BOOL CPropSlider::InfoChanged()
{
	HRESULT hr = S_OK;

	try {
		m_Source.ResetInfoChangedFlag();

		ATLASSERT(m_Source);
		EMIPPAR_LIMITATION lt;
		m_Source->get_LimitationType(&lt);

		VARIANT_BOOL bRead = VARIANT_FALSE;
		m_Source->get_IsReadOnly(&bRead);
		if (bRead == VARIANT_TRUE)
			EnableWindow(FALSE);
		else
			EnableWindow(TRUE);

		m_ltProp = lt;

		if (lt == EMIPPAR_ENUM) {
			struct IDisplayRawPairCollection* pEnumSet;
			hr = m_Source->get_EnumSet(&pEnumSet);
			if (FAILED(hr)) 
				_com_raise_error(hr);

			long lCount = 0;
			if (pEnumSet != NULL) {
				hr = pEnumSet->get_Count(&lCount);
				m_vtProp = pEnumSet->GetDataType();
				if (FAILED(hr)) 
					_com_raise_error(hr);
			}

			SetRange(0, lCount - 1);
			SetLineSize(1);
			SetPageSize(1);

			pEnumSet->Release();
		}
		else if (lt == EMIPPAR_RANGE) {
			VARIANT low, high;
			hr = m_Source->get_RangeLowerLimit(&low);
			if (FAILED(hr)) 
				_com_raise_error(hr);
			hr = m_Source->get_RangeHigherLimit(&high);
			if (FAILED(hr)) 
				_com_raise_error(hr);
			m_vtProp = low.vt;

			if (low.vt == VT_I4) {
				if (m_bUserRange) {
					if (m_nUserMin < low.lVal)
						m_nUserMin = low.lVal;
					if (m_nUserMax > high.lVal)
						m_nUserMax = high.lVal;
				}
				else {
					m_nUserMin = low.lVal;
					m_nUserMax = high.lVal;
				}

				if (m_nUserMin < 0 || m_bReverse)
					m_nOffset = -m_nUserMin;
				else
					m_nOffset = 0;

				SetRange(m_nUserMin + m_nOffset, m_nUserMax + m_nOffset);

				long lRange = m_nUserMax - m_nUserMin;
				long lPage = 1, lLine = 1;

				if (lRange >= 10000) {
					lPage = __max(1, lRange / 100); // 1%
					lLine = __max(1, lPage / 10); // 0.1%
				}
				else if (lRange >= 1000) {
					lPage = __max(1, lRange / 20); // 5%
					lLine = __max(1, lPage / 10); // 0.5%
				}
				else {
					lPage = __max(1, lRange / 10); // 10%
					lLine = __max(1, lPage / 10); // 1%
				}

				SetPageSize(lPage);
				SetLineSize(lLine);
			}
			else if (low.vt == VT_R8) {
				if (low.dblVal < 0.)
					m_dOffset = -low.dblVal;
				else
					m_dOffset = 0.;

				VARIANT inc;
				hr = m_Source->get_RangeIncrement(&inc);
				if (FAILED(hr)) 
					_com_raise_error(hr);
				m_dStep = inc.dblVal;
				if (m_dStep == 0.)
					m_dStep = 0.001;
				if (m_dStep < 0.)
					m_dStep = -m_dStep;

				SetRange((int)((low.dblVal + m_dOffset) / m_dStep), (int)((high.dblVal + m_dOffset) / m_dStep));

				long lRange = (long)((high.dblVal - low.dblVal) / inc.dblVal);
				long lPage = 1, lLine = 1;

				if (lRange >= 10000) {
					lPage = __max(1, lRange / 100); // 1%
					lLine = __max(1, lPage / 10); // 0.1%
				}
				else if (lRange >= 1000) {
					lPage = __max(1, lRange / 20); // 5%
					lLine = __max(1, lPage / 10); // 0.5%
				}
				else {
					lPage = __max(1, lRange / 10); // 10%
					lLine = __max(1, lPage / 10); // 1%
				}

				SetPageSize(lPage);
				SetLineSize(lLine);
			}
		}
	}
	catch (_com_error/* &e*/) {
		return FALSE;
	}
	return TRUE;
}

BOOL CPropSlider::ValueChanged()
{
	try {
		m_Source.ResetValueChangedFlag();

		if (m_Source.m_pProperty != NULL) {
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
				if (result.vt == VT_I4) {
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

						long lEnumVal = stDisplayRawPair.RawValue.lVal;

						::SysFreeString(stDisplayRawPair.DisplayString);
						::VariantClear(&stDisplayRawPair.RawValue);

						if (lEnumVal == result.lVal) {
							SetPos(iItem);

							break;
						}
					}

					pEnumSet->Release();
				}
			}
			else if (lt == EMIPPAR_RANGE) {
				if (result.vt == VT_I4) {
					if (m_bReverse) {
						int nMin, nMax;
						GetRange(nMin, nMax);
						SetPos(nMax - ((result.lVal + m_nOffset) - nMin));
					}
					else
						SetPos(result.lVal + m_nOffset);
				}
				else if (result.vt == VT_R8) {
					if (m_bReverse) {
						int nMin, nMax;
						GetRange(nMin, nMax);
						SetPos(nMax - ((int)((result.dblVal + m_dOffset) / m_dStep) - nMin));
					}
					else
						SetPos((int)((result.dblVal + m_dOffset) / m_dStep));
				}
			}
		}
	}
	catch (_com_error/* &e*/) {
		return FALSE;
	}
	return TRUE;
}

void CPropSlider::SetReverse(BOOL bRev, BOOL bInfoChanged)
{
	m_bReverse = bRev;
	if (bInfoChanged && m_Source.m_pProperty) {
		InfoChanged();
		ValueChanged();
	}
}

int CPropSlider::GetValuePos()
{
	if (m_bReverse) {
		int nMin, nMax;
		GetRange(nMin, nMax);
		return (nMax - (GetPos() - nMin)) - m_nOffset;
	}
	return GetPos() - m_nOffset;
}

BOOL CPropSlider::SetUserRange(int nUserMin, int nUserMax, BOOL bUserRange)
{
	if (nUserMax < nUserMin)
		return FALSE;

	if (m_Source.m_pProperty) {
		if (!bUserRange && m_bUserRange) {
			m_bUserRange = bUserRange;
			InfoChanged();
			ValueChanged();
			return TRUE;
		}

		HRESULT hr = S_OK;
		VARIANT low, high;
		try {
			hr = m_Source->get_RangeLowerLimit(&low);
			if (FAILED(hr)) 
				_com_raise_error(hr);
			hr = m_Source->get_RangeHigherLimit(&high);
			if (FAILED(hr)) 
				_com_raise_error(hr);
		}
		catch (_com_error/* &e*/) {
			return FALSE;
		}

		if (low.vt == VT_I4) {
			if (nUserMin >= low.lVal && nUserMax <= high.lVal) {
				m_bUserRange = bUserRange;
				m_nUserMin = nUserMin;
				m_nUserMax = nUserMax;
				InfoChanged();
				ValueChanged();
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CPropSlider::PutValue()
{
	HRESULT hr;
	CWaitCursor w;

	try {
		if (m_Source.m_pProperty != NULL) {
			int nVal = GetPos();

			EMIPPAR_LIMITATION lt;
			hr = m_Source->get_LimitationType(&lt);
			if (FAILED(hr)) 
				_com_raise_error(hr);

			EMIPPAR_VARENUM vt;
			hr = m_Source->get_DataType(&vt);
			if (FAILED(hr)) 
				_com_raise_error(hr);

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
						CString s;s.Format(_T("### PropSlider::PutValue ID=%d, %d[ms] ###\n"), GetDlgCtrlID(), GetTickCount() - dwStart);OutputDebugString(s);
						if (FAILED(hr))
							_com_raise_error(hr);
						break;
					}
				}
			}
			else if (lt == EMIPPAR_RANGE) {
				DWORD dwStart = GetTickCount();
				_variant_t vtTemp;
				if (vt == VT_I4) {
					if (m_bReverse) {
						int nMin, nMax;
						GetRange(nMin, nMax);
						vtTemp = (nMax - (nVal - nMin)) - m_nOffset;
					}
					else
						vtTemp = nVal - m_nOffset;
				}
				else if (vt == VT_R8) {
					if (m_bReverse) {
						int nMin, nMax;
						GetRange(nMin, nMax);
						vtTemp = ((nMax - (nVal - nMin)) * m_dStep) - m_dOffset;
					}
					else
						vtTemp = (nVal * m_dStep) - m_dOffset;
				}
				hr = m_Source.m_pProperty->put_RawValue(vtTemp);
				CString s;s.Format(_T("### PropSlider::PutValue ID=%d, %d[ms] ###\n"), GetDlgCtrlID(), GetTickCount() - dwStart);OutputDebugString(s);
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

BOOL CPropSlider::GetValue(VARIANT* pVal)
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

BOOL CPropSlider::GetDisplayString(CString* pstr)
{
	try {
		if (m_Source.m_pProperty != NULL) {
			BSTR bstrDisp = NULL;
			HRESULT hr = m_Source->get_DisplayString(&bstrDisp);
			if (FAILED(hr))
				_com_raise_error(hr);
			*pstr = CString(bstrDisp);
			if (bstrDisp != NULL) {
				::SysFreeString(bstrDisp);
				bstrDisp = NULL;
			}
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

void CPropSlider::SetToolTipText(int nId, BOOL bActivate)
{
	CString sText;
	// load string resource
	sText.LoadString(nId);
	// If string resource is not empty
	if (sText.IsEmpty() == FALSE)
		SetToolTipText(&sText, bActivate);
}

void CPropSlider::SetToolTipText(LPCTSTR szText, BOOL bActivate)
{
	// We cannot accept NULL pointer
	if (szText == NULL) return;

	CString sText(szText);
	// If string resource is not empty
	if (sText.IsEmpty() == FALSE)
		SetToolTipText(&sText, bActivate);
}

void CPropSlider::SetToolTipText(CString *spText, BOOL bActivate)
{
	// We cannot accept NULL pointer
	if (spText == NULL) return;

	// Initialize ToolTip
	InitToolTip();

	// If there is no tooltip defined then add it
	if (m_ToolTip.GetToolCount() == 0) {
		CRect rectBtn;
		GetClientRect(rectBtn);
		m_ToolTip.AddTool(this, (LPCTSTR)*spText, rectBtn, 1);
	}

	// Set text for tooltip
	m_ToolTip.UpdateTipText((LPCTSTR)*spText, this, 1);
	m_ToolTip.Activate(bActivate);
}

void CPropSlider::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL) {
		// Create ToolTip control
		m_ToolTip.Create(this);
		// Create inactive
		m_ToolTip.Activate(FALSE);
	}
}

void CPropSlider::ActivateTooltip(BOOL bActivate)
{
	// If there is no tooltip then do nothing
	if (m_ToolTip.GetToolCount() == 0) return;

	// Activate tooltip
	m_ToolTip.Activate(bActivate);
}

BOOL CPropSlider::PreTranslateMessage(MSG* pMsg)
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);
	return CSliderCtrl::PreTranslateMessage(pMsg);
}

void CPropSlider::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking) {
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}
	CSliderCtrl::OnMouseMove(nFlags, point);
}

LRESULT CPropSlider::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTracking = FALSE;
	Invalidate();
	return 0;
}

LRESULT CPropSlider::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
	Invalidate();
	return 0;
}

CString CPropSlider::MakeToolTipText()
{
	CString strTooltip;
	strTooltip.Empty();
	int nVal = GetValuePos();
	if (m_ltProp == EMIPPAR_ENUM) {
		if (m_vtProp == VT_I4) {
			strTooltip.Format(_T("%d"), nVal);
		}
	}
	else if (m_ltProp == EMIPPAR_RANGE) {
		if (m_vtDisp == VT_BSTR) {
/*			BSTR bstr;
			HRESULT hr = m_Source->get_DisplayString(&bstr);
			if (!FAILED(hr))
				strTooltip.Format(_T("%s"), bstr);*/
		}
		else {
			if (m_vtProp == VT_I4) {
				if (m_vtDisp == VT_R8)
					strTooltip.Format(_T("%.*lf"), m_nDec, (double)nVal / (double)m_nUnit);
				else
					strTooltip.Format(_T("%d"), nVal / m_nUnit);
			}
//			else if (m_vtProp == VT_R8)
//				strTooltip.Format(_T("%.*lf"), m_nDec, val.dblVal);
			if (m_bDispUnit)
				strTooltip += m_strUnit;
		}
	}
	return strTooltip;
}
void CPropSlider::OnNeedText(NMHDR* pNMHDR, LRESULT* pResult)
{
	TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;

	if (pTTT->uFlags & TTF_IDISHWND) {
		int ID = ::GetDlgCtrlID((HWND)pTTT->hdr.idFrom);
		CString strToolTip = MakeToolTipText();
		if (!strToolTip.IsEmpty())
			lstrcpy(pTTT->szText, strToolTip);
	}
}
