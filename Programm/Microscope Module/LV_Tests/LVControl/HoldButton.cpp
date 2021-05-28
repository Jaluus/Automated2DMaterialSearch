// HoldButton.cpp : 実装ファイル
//

#include "stdafx.h"
#include "LvSample.h"
#include "HoldButton.h"

// CHoldButton

IMPLEMENT_DYNAMIC(CHoldButton, CButton)

CHoldButton::CHoldButton()
{
}

CHoldButton::~CHoldButton()
{
}

BEGIN_MESSAGE_MAP(CHoldButton, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CHoldButton メッセージ ハンドラ

void CHoldButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	CButton::OnLButtonDown(nFlags, point);
	SetCapture();
	::SendMessage(GetParent()->GetSafeHwnd(), WM_HOLD_BUTTON_DOWN, (WPARAM)GetSafeHwnd(), (LPARAM)GetDlgCtrlID());
}

void CHoldButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	CButton::OnLButtonUp(nFlags, point);
	ReleaseCapture();
	::SendMessage(GetParent()->GetSafeHwnd(), WM_HOLD_BUTTON_UP, (WPARAM)GetSafeHwnd(), (LPARAM)GetDlgCtrlID());
}
