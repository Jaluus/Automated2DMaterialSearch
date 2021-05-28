#pragma once

#include "MicroscopeControl.h"
#include "afxwin.h"

// CDeviceDlg �_�C�A���O

class CDeviceDlg : public CDialog
{
	DECLARE_DYNAMIC(CDeviceDlg)

public:
	CDeviceDlg(CMicroscopeControl* pMicroscope, CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~CDeviceDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_DIALOG_DEVICE };

	inline long GetSelectedDeviceIndex() { return m_bIsDeviceAvailable ? m_lDeviceIndex : -1; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CMicroscopeControl* m_pMicroscope;

private:
	CListBox m_lstDevice;
	BOOL m_bIsAvailable, m_bIsDeviceAvailable;
	long m_lDeviceIndex;

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnLbnDblclkListDevice();
};
