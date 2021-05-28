#pragma once

#include "MicroscopeControl.h"

// CZDriveRangeDlg dialog

class CZDriveRangeDlg : public CDialog
{
	DECLARE_DYNAMIC(CZDriveRangeDlg)

public:
	CZDriveRangeDlg(CMicroscopeControl* pMicroscope, CWnd* pParent = NULL);
	virtual ~CZDriveRangeDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ZDRIVE_RANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CMicroscopeControl* m_pMicroscope;

public:
	void OnBnClickedButtonReadMin();
	void OnBnClickedButtonReadMax();
	void OnBnClickedOk();
	void OnBnClickedCancel();

private:
	// Z Drive
	long m_lPos;
	long m_lUnit;
	long m_lSoftwareLower;
	long m_lSoftwareUpper;
};
