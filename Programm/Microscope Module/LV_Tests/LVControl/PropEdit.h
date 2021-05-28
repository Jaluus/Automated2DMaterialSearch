#pragma once

#include "mipproplink.h"

// CPropEdit

class CPropEdit : public CEdit
{
	DECLARE_DYNAMIC(CPropEdit)

public:
	CPropEdit();
	virtual ~CPropEdit();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CMipPropLink	m_Source;			// a pointer to CMipPropLink
	BOOL			m_bFireValueChanged;
	BOOL			m_bFireInfoChanged;

public:
	BOOL PutValue();
	void Disconnect();
	BOOL Connect(IDispatch *piParameter);
	void GetValue(int* pValue, int* pLow = NULL, int* pHigh = NULL);

	inline void SetFireValueChanged(BOOL bFire) { m_bFireValueChanged = bFire; }
	inline void SetFireInfoChanged(BOOL bFire) { m_bFireInfoChanged = bFire; }

protected:
	BOOL ValueChanged();
	BOOL InfoChanged();

public:
	afx_msg void OnDestroy();
	afx_msg LRESULT OnParamEvent(WPARAM wParam = 0, LPARAM lParam = 0);
};
