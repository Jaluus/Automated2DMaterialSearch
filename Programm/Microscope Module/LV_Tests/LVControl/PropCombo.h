#pragma once

#include "mipproplink.h"

// CPropCombo

typedef void (*FPropComboCallback)(EnumAccessory eAccessory, long lVal, CString& strName);

class CPropCombo : public CComboBox
{
	DECLARE_DYNAMIC(CPropCombo)

public:
	CPropCombo();
	virtual ~CPropCombo();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CMipPropLink		m_Source;		// a pointer to CMipPropLink
	FPropComboCallback	m_Callback;
	EnumAccessory		m_eAccessory;
	int					m_nAddUnknown;
	BOOL				m_bUseUnknownSel;
	BOOL				m_bValueChanged;

public:
	BOOL PutValue();
	BOOL GetValue(VARIANT* pVal);
	void Disconnect();
	BOOL Connect(IDispatch *piParameter);
	inline void CallbackSet(FPropComboCallback fPropComboCallback) { m_Callback = fPropComboCallback; }
	inline void SetAccessory(EnumAccessory eAccessory) { m_eAccessory = eAccessory; }
	inline BOOL GetLastError() { return m_bValueChanged; }
	inline void SetUseUnknownSel(BOOL bUse) { m_bUseUnknownSel = bUse; }

protected:
	BOOL ValueChanged();
	BOOL InfoChanged();

	void RemoveUnknown()
	{
		if (m_nAddUnknown >= 0) {
			DeleteString(m_nAddUnknown);
			m_nAddUnknown = -1;
		}
	}

public:
	afx_msg void OnDestroy();
	afx_msg LRESULT OnParamEvent(WPARAM wParam = 0, LPARAM lParam = 0);
};
