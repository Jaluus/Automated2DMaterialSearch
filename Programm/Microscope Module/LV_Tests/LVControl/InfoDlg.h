#pragma once

#include "MicroscopeControl.h"
#include "afxwin.h"

enum EnumInfo { E_Nosepiece = 1,
				E_Cube,
				E_Interlock,
				E_UserObservation,
				E_EpiDia,
				E_ZDrive,
				E_Dic,
				E_System,
};

// CInfoDlg ダイアログ

class CInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CInfoDlg)

public:
	CInfoDlg(CMicroscopeControl* pMicroscope, EnumInfo eInfo, CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CInfoDlg();

// ダイアログ データ
	enum { IDD = IDD_DIALOG_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CMicroscopeControl* m_pMicroscope;
	EnumInfo m_eInfo;

private:
	void ViewNosepiece();
	void ViewCube();
	void ViewInterlock();
	void ViewUserObserbation();
	void ViewEpiDia();
	void ViewZDrive();
	void ViewDic();
	void ViewSystem();

	CString GetFilterDescription(BSTR bstrName, int nEF, int nDM, int nBF);

	CListBox m_lstInfo;

	CString m_strDelimiter;
};
