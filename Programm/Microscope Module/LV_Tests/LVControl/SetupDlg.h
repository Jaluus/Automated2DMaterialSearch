#pragma once

//#import "C:\Program Files\Nikon\LV-Series\Bin\LVSetup.ocx" no_namespace, named_guids
#include "DLVSetup.h"
#include "MicroscopeControl.h"

// CSetupDlg ダイアログ

class CSetupDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetupDlg)

public:
	CSetupDlg(CMicroscopeControl* pMicroscope, CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CSetupDlg();

// ダイアログ データ
	enum { IDD = IDD_DIALOG_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();

	CDLVSetup* m_pDLVSetup;
	CMicroscopeControl* m_pMicroscope;

	BOOL CreateDLVSetup();
	BOOL DestroyDLVSetup();

public:
	DECLARE_EVENTSINK_MAP()
	void EndNoticeLVSetupCtrl1();
};
