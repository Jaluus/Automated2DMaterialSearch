#pragma once

#include "MicroscopeControl.h"

// CCommandDlg �_�C�A���O

class CCommandDlg : public CDialog
{
	DECLARE_DYNAMIC(CCommandDlg)

public:
	CCommandDlg(CMicroscopeControl* pMicroscope, CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~CCommandDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_DIALOG_COMMAND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()

	CMicroscopeControl* m_pMicroscope;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonSend();
};
