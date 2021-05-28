#pragma once

//#import "C:\Program Files\Nikon\LV-Series\Bin\LVSetup.ocx" no_namespace, named_guids
#include "DLVSetup.h"
#include "MicroscopeControl.h"

// CSetupDlg �_�C�A���O

class CSetupDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetupDlg)

public:
	CSetupDlg(CMicroscopeControl* pMicroscope, CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~CSetupDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_DIALOG_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
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
