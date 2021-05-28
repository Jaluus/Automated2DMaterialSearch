#pragma once

#include "mipproplink.h"

// CPropSlider

class CPropSlider : public CSliderCtrl
{
	DECLARE_DYNAMIC(CPropSlider)

public:
	CPropSlider();
	virtual ~CPropSlider();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CMipPropLink	m_Source;			// a pointer to CMipPropLink
	long			m_nOffset;
	double			m_dOffset;
	double			m_dStep;
	VARTYPE			m_vtDisp;
	CString			m_strUnit;
	BOOL			m_bDispUnit;
	long			m_nUnit;
	long			m_nDec;

	EMIPPAR_LIMITATION	m_ltProp;
	VARTYPE				m_vtProp;

	BOOL m_bUserRange;
	long m_nUserMin;
	long m_nUserMax;

	BOOL m_bReverse;
	BOOL m_bTracking;
	CToolTipCtrl m_ToolTip;

public:
	BOOL PutValue();
	BOOL GetValue(VARIANT* pVal);
	void Disconnect();
	BOOL Connect(IDispatch *piParameter);
	BOOL GetDisplayString(CString* pstr);

	void SetToolTipText(LPCTSTR szText, BOOL bActivate = TRUE);
	void SetToolTipText(CString* spText, BOOL bActivate = TRUE);
	void SetToolTipText(int nId, BOOL bActivate = TRUE);

	inline BOOL GetReverse() { return m_bReverse; }
	void SetReverse(BOOL bRev, BOOL bInfoChanged = FALSE);

	int GetValuePos();
	inline int GetUserRangeMin() { return m_nUserMin; }
	inline int GetUserRangeMax() { return m_nUserMax; }
	inline BOOL GetUseUserRange() { return m_bUserRange; }
	BOOL SetUserRange(int nUserMin, int nUserMax, BOOL bUserRange = FALSE);

	inline void SetUnit(long nUnit) { m_nUnit = nUnit; }
	inline void SetDecimal(long nDec) { m_nDec = nDec; }
	inline void SetDispType(VARTYPE vt) { m_vtDisp = vt; }
	inline void SetUnitString(CString strUnit) { m_strUnit = strUnit; }
	inline void SetDispUnit(BOOL bDisp) { m_bDispUnit = bDisp; }

protected:
	BOOL ValueChanged();
	BOOL InfoChanged();

	void InitToolTip();
	void ActivateTooltip(BOOL bActivate = TRUE);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString MakeToolTipText();

public:
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnParamEvent(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	void OnNeedText(NMHDR* pNMHDR, LRESULT* pResult);
};
