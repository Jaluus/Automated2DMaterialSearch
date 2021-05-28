#pragma once

#include "mipproplink.h"

// CPropStatic

class CPropStatic : public CStatic
{
	DECLARE_DYNAMIC(CPropStatic)

public:
	CPropStatic();
	virtual ~CPropStatic();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CMipPropLink	m_Source;			// a pointer to CMipPropLink
	long			m_nUnit;
	long			m_nDec;
	VARTYPE			m_vtDisp;

public:
	void Disconnect();
	BOOL Connect(IDispatch *piParameter);
	BOOL GetValue(VARIANT* pVal);
	inline void SetUnit(long nUnit) { m_nUnit = nUnit; }
	inline void SetDecimal(long nDec) { m_nDec = nDec; }
	inline void SetDispType(VARTYPE vt) { m_vtDisp = vt; }

protected:
	BOOL ValueChanged();
	BOOL InfoChanged();

public:
	afx_msg void OnDestroy();
	afx_msg LRESULT OnParamEvent(WPARAM wParam = 0, LPARAM lParam = 0);
};
