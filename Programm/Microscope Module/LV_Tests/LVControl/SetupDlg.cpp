// SetupDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "LvSample.h"
#include "SetupDlg.h"


// CSetupDlg ダイアログ

IMPLEMENT_DYNAMIC(CSetupDlg, CDialog)

CSetupDlg::CSetupDlg(CMicroscopeControl* pMicroscope, CWnd* pParent /*=NULL*/)
	: CDialog(CSetupDlg::IDD, pParent)
	, m_pMicroscope(pMicroscope)
	, m_pDLVSetup(NULL)
{

}

CSetupDlg::~CSetupDlg()
{
}

void CSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetupDlg, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CSetupDlg, CDialog)
	ON_EVENT(CSetupDlg, IDC_LVSETUPCTRL1, 1, CSetupDlg::EndNoticeLVSetupCtrl1, VTS_NONE)
END_EVENTSINK_MAP()


// CSetupDlg メッセージ ハンドラ
BOOL CSetupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CoInitialize(NULL);

	CreateDLVSetup();

	return TRUE;
}
void CSetupDlg::OnDestroy()
{
	DestroyDLVSetup();

	CoUninitialize();

	CDialog::OnDestroy();
}

BOOL CSetupDlg::CreateDLVSetup()
{
	CRect rcChild;
	GetDlgItem(IDC_LVSETUPCTRL1)->GetWindowRect(rcChild);
	ScreenToClient(rcChild);

	m_pDLVSetup = new CDLVSetup();
	BOOL bSuccess = m_pDLVSetup->Create(_T("Setting"), 0, rcChild, this, IDC_LVSETUPCTRL1);
	if (m_pDLVSetup && bSuccess) {
		long lVal = m_pDLVSetup->Start(m_pMicroscope->GetInterface());
		long lWidth = m_pDLVSetup->GetWidth();
		long lHeight = m_pDLVSetup->GetHeight();
		m_pDLVSetup->SetWindowPos(NULL, 0, 0, lWidth, lHeight, 0);
		ResizeWindowFromClientSize(this, lWidth, lHeight, 0);
		m_pDLVSetup->ShowWindow(SW_SHOW);
	}
	return bSuccess;
}
BOOL CSetupDlg::DestroyDLVSetup()
{
	if (m_pDLVSetup) {
		m_pDLVSetup->End();
		m_pDLVSetup->DestroyWindow();
		m_pDLVSetup = NULL;
	}
	return TRUE;
}

void CSetupDlg::EndNoticeLVSetupCtrl1()
{
	EndDialog(TRUE);
}
