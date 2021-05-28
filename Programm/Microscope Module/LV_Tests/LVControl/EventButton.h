#pragma once

#include "mipproplink.h"

// CEventButton

class CEventButton : public CButton
{
	DECLARE_DYNAMIC(CEventButton)

public:
	CEventButton();
	virtual ~CEventButton();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CMipPropLink	m_Source;			// a pointer to CMipPropLink

public:
	void Disconnect();
	BOOL Connect(IDispatch *piParameter);
	void ClearEvent() { EnableWindow(FALSE); }

protected:
	BOOL ValueChanged();
	BOOL InfoChanged();

public:
	afx_msg void OnDestroy();
	afx_msg LRESULT OnParamEvent(WPARAM wParam = 0, LPARAM lParam = 0);
};
