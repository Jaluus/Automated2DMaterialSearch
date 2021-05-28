
// stdafx.cpp : source file that includes just the standard includes
// LvSample.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

void ResizeWindowFromClientSize(CWnd* pWnd, int cx, int cy, UINT nOption)
{
	CRect rcClient;
	pWnd->GetClientRect(&rcClient);
	CRect rcWindow;
	pWnd->GetWindowRect(&rcWindow);
	CSize szWindow(rcWindow.Width() - rcClient.Width() + cx,
				   rcWindow.Height() - rcClient.Height() + cy);
	pWnd->SetWindowPos(NULL, 0, 0, szWindow.cx, szWindow.cy, SWP_NOMOVE | SWP_NOZORDER | nOption);
}
