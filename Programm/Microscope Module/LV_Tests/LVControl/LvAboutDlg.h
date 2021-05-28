#pragma once

#include "MicroscopeControl.h"

// CLvAboutDlg �_�C�A���O

class CLvAboutDlg : public CDialog
{
	DECLARE_DYNAMIC(CLvAboutDlg)

public:
	CLvAboutDlg(CMicroscopeControl* pMicroscope, CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~CLvAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_DIALOG_ABOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CMicroscopeControl* m_pMicroscope;
};
