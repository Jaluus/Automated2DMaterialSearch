// EventButton.cpp : 実装ファイル
//

#include "stdafx.h"
#include "LvSample.h"
#include "EventButton.h"

// CEventButton

IMPLEMENT_DYNAMIC(CEventButton, CButton)

CEventButton::CEventButton()
{
	m_Source.m_pProperty = NULL;
}

CEventButton::~CEventButton()
{
}

BEGIN_MESSAGE_MAP(CEventButton, CButton)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_PAREVENT, OnParamEvent)
END_MESSAGE_MAP()

// CEventButton メッセージ ハンドラ

void CEventButton::OnDestroy()
{
	CButton::OnDestroy();

	Disconnect();
}

LRESULT CEventButton::OnParamEvent(WPARAM wParam, LPARAM lParam)
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

BOOL CEventButton::Connect(IDispatch *piParameter)
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

		EnableWindow(FALSE);
	}
	catch (_com_error/* &e*/) {
		return FALSE;
	}
	return TRUE;
}

void CEventButton::Disconnect()
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

BOOL CEventButton::InfoChanged()
{
	HRESULT hr = S_OK;

	try {
		m_Source.ResetInfoChangedFlag();
	}
	catch (_com_error/* &e*/) {
		return FALSE;
	}
	return TRUE;
}

BOOL CEventButton::ValueChanged()
{
	try {
		m_Source.ResetValueChangedFlag();

		EnableWindow(TRUE);
	}
	catch (_com_error/* &e*/) {
		return FALSE;
	}
	return TRUE;
}
