#pragma once

#include "MicroscopeControl.h"

// CCommandDlg ダイアログ

class CCommandDlg : public CDialog
{
	DECLARE_DYNAMIC(CCommandDlg)

public:
	CCommandDlg(CMicroscopeControl* pMicroscope, CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CCommandDlg();

// ダイアログ データ
	enum { IDD = IDD_DIALOG_COMMAND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()

	CMicroscopeControl* m_pMicroscope;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonSend();
};
