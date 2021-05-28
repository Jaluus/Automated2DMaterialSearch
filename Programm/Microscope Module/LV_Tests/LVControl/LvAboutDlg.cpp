// LvAboutDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "LvSample.h"
#include "LvAboutDlg.h"


// VS_VERSIONINFO
typedef struct VS_VERSIONINFO {
	WORD wLength;
	WORD wValueLength;
	WORD wType;
	WCHAR szKey[16];
	WORD Padding1[1];
	VS_FIXEDFILEINFO Value;
//	WORD Padding2[];
//	WORD Children[];
} SVersionInfo;

// CLvAboutDlg ダイアログ

IMPLEMENT_DYNAMIC(CLvAboutDlg, CDialog)

CLvAboutDlg::CLvAboutDlg(CMicroscopeControl* pMicroscope, CWnd* pParent /*=NULL*/)
	: CDialog(CLvAboutDlg::IDD, pParent)
	, m_pMicroscope(pMicroscope)
{

}

CLvAboutDlg::~CLvAboutDlg()
{
}

void CLvAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLvAboutDlg, CDialog)
END_MESSAGE_MAP()


// CLvAboutDlg メッセージ ハンドラ
BOOL CLvAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	INikonLv* pNikonLv = m_pMicroscope->GetInterface();
	CString str;

	// app version
	HRSRC hRsrc = ::FindResource(NULL, (LPCTSTR)VS_VERSION_INFO, RT_VERSION);
	HGLOBAL hGlobal = ::LoadResource(NULL, hRsrc);

	SVersionInfo *pVerData = (SVersionInfo*) ::LockResource(hGlobal);

	int nVer1 = pVerData->Value.dwFileVersionMS>>16;
	int nVer2 = pVerData->Value.dwFileVersionMS & 0xffff;
	int nVer3 = pVerData->Value.dwFileVersionLS>>16;
	int nVer4 = pVerData->Value.dwFileVersionLS & 0xffff;

	str.Format(_T("app version : %d.%d.%d.%d"), nVer1, nVer2, nVer3, nVer4);
	GetDlgItem(IDC_STATIC_APP_VERSION)->SetWindowText(str);

	::GlobalUnlock(hGlobal);

	// sdk version
	str.Format(_T("sdk version : %s"), pNikonLv->VersionOfSDK.GetBSTR());
	GetDlgItem(IDC_STATIC_SDK_VERSION)->SetWindowText(str);

	// controller name
	str.Format(_T("controller name : %s"), pNikonLv->ReadProgramName().GetBSTR());
	GetDlgItem(IDC_STATIC_CONTROLLER_NAME)->SetWindowText(str);

	// controller version
	str.Format(_T("controller version : %s"), pNikonLv->ReadVersion().GetBSTR());
	GetDlgItem(IDC_STATIC_CONTROLLER_VERSION)->SetWindowText(str);

	// check sum
	str.Format(_T("check sum : %s"), pNikonLv->ReadChecksum().GetBSTR());
	GetDlgItem(IDC_STATIC_CHECK_SUM)->SetWindowText(str);

	return TRUE;
}
