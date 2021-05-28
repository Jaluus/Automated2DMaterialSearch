#pragma once

// CHoldButton

class CHoldButton : public CButton
{
	DECLARE_DYNAMIC(CHoldButton)

public:
	CHoldButton();
	virtual ~CHoldButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
