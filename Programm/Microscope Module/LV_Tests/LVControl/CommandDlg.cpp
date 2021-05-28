// CommandDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "LvSample.h"
#include "CommandDlg.h"

// CCommandDlg ダイアログ

IMPLEMENT_DYNAMIC(CCommandDlg, CDialog)

CCommandDlg::CCommandDlg(CMicroscopeControl* pMicroscope, CWnd* pParent /*=NULL*/)
	: CDialog(CCommandDlg::IDD, pParent)
	, m_pMicroscope(pMicroscope)
{
}

CCommandDlg::~CCommandDlg()
{
}

void CCommandDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCommandDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CCommandDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()

// CCommandDlg メッセージ ハンドラ

void CCommandDlg::OnBnClickedButtonSend()
{
	CString strCommand;
	GetDlgItem(IDC_EDIT_COMMAND)->GetWindowText(strCommand);
	GetDlgItem(IDC_EDIT_RECIEVE)->SetWindowText(_T(""));

	if (strCommand.GetLength() < 1) {
		AfxMessageBox(IDS_MESSAGE_INPUT_COMMAND);
		return;
	}

	CString strSep;
	strSep = strCommand.Left(1);

	try {
		HRESULT hr = S_OK;
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();

		if (strSep == _T("r")) {
			CWaitCursor w;
			CComBSTR bstrCommand(strCommand);
			BSTR bstrResult = NULL;
			if (FAILED(hr = pNikonLv->GetData2(bstrCommand.m_str, &bstrResult)))
				_com_raise_error(hr);
			CString strResult(bstrResult);
			GetDlgItem(IDC_EDIT_RECIEVE)->SetWindowText(strResult);
		}
		else if (strSep == _T("c") || strSep == _T("f")) {
			CWaitCursor w;
			CComBSTR bstrCommand(strCommand);
			if (FAILED(hr = pNikonLv->SendData(bstrCommand.m_str)))
				_com_raise_error(hr);
			CString strResult(pNikonLv->Device->LastResponse.GetBSTR());
			GetDlgItem(IDC_EDIT_RECIEVE)->SetWindowText(strResult);
		}
		else {
			AfxMessageBox(IDS_MESSAGE_INVALID_COMMAND);
		}
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Command error!"));
	}
}

BOOL CCommandDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		switch(pMsg->wParam) {
		case VK_RETURN:
			OnBnClickedButtonSend();
			return TRUE;
		case VK_ESCAPE:
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
