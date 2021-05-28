#pragma once

#include "MicroscopeControl.h"
#include "PropCheck.h"
#include "PropSlider.h"
#include "PropCombo.h"
#include "afxwin.h"
#include "afxcmn.h"

// CAfOffsetDlg ダイアログ

class CAfOffsetDlg : public CDialog
{
	DECLARE_DYNAMIC(CAfOffsetDlg)

public:
	CAfOffsetDlg(CMicroscopeControl* pMicroscope, CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CAfOffsetDlg();

// ダイアログ データ
	enum { IDD = IDD_DIALOG_AF_OFFSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CMicroscopeControl* m_pMicroscope;

private:
	void UpdateRecipeInformation();
	void UpdateAfOffsetMode();
	void UpdateAfOffsetValueInformation();
	void UpdateAfOffsetValue();

	CPropCheck m_chkAfOffsetMode;
	CPropSlider m_slAfOffset;
	CPropCombo m_cboAfRecipe;
	CEdit m_edAfOffset;
	CSpinButtonCtrl m_spnAfOffset;

protected:
	afx_msg void OnDestroy();
	HRESULT ConnectMipParam();
	HRESULT DisconnectMipParam();

	void ValueChanged(WPARAM wParam);
	void InfoChanged(WPARAM wParam);

public:
	afx_msg LRESULT OnParamEvent(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckAfOffset();
	afx_msg void OnBnClickedButtonChange();
	afx_msg void OnCbnSelchangeComboAfRecipe();
};
