#pragma once

#include "MicroscopeControl.h"

// CLvAboutDlg ダイアログ

class CLvAboutDlg : public CDialog
{
	DECLARE_DYNAMIC(CLvAboutDlg)

public:
	CLvAboutDlg(CMicroscopeControl* pMicroscope, CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CLvAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_DIALOG_ABOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CMicroscopeControl* m_pMicroscope;
};
