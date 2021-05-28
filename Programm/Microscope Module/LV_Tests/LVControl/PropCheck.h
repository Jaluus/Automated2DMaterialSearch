#pragma once

#include "mipproplink.h"

// CPropCheck

class CPropCheck : public CButton
{
	DECLARE_DYNAMIC(CPropCheck)

public:
	CPropCheck();
	virtual ~CPropCheck();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CMipPropLink	m_Source;			// a pointer to CMipPropLink

public:
	BOOL PutValue();
	BOOL GetValue(VARIANT* pVal);
	void Disconnect();
	BOOL Connect(IDispatch *piParameter);

protected:
	BOOL ValueChanged();
	BOOL InfoChanged();

public:
	afx_msg void OnDestroy();
	afx_msg LRESULT OnParamEvent(WPARAM wParam = 0, LPARAM lParam = 0);
};
