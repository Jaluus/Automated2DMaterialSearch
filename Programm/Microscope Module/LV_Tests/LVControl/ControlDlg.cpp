// ControlDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "LvSample.h"
#include "ControlDlg.h"


// CControlDlg ダイアログ

IMPLEMENT_DYNAMIC(CControlDlg, CDialog)

CControlDlg::CControlDlg(CMicroscopeControl* pMicroscope, CWnd* pParent /*=NULL*/)
	: CDialog(CControlDlg::IDD, pParent)
	, m_pMicroscope(pMicroscope)
{

}

CControlDlg::~CControlDlg()
{
}

void CControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_NOSEPIECE_ON, m_rdNosepieceOn);
	DDX_Control(pDX, IDC_RADIO_NOSEPIECE_OFF, m_rdNosepieceOff);
	DDX_Control(pDX, IDC_CHECK_NOSEPIECE, m_chkNosepiece);
	DDX_Control(pDX, IDC_RADIO_SWITCH_ON, m_rdSwitchOn);
	DDX_Control(pDX, IDC_RADIO_SWITCH_OFF, m_rdSwitchOff);
	DDX_Control(pDX, IDC_RADIO_BUZZER_ON, m_rdBuzzerOn);
	DDX_Control(pDX, IDC_RADIO_BUZZER_OFF, m_rdBuzzerOff);
}


BEGIN_MESSAGE_MAP(CControlDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO_NOSEPIECE_ON, &CControlDlg::OnBnClickedRadioNosepieceOn)
	ON_BN_CLICKED(IDC_RADIO_NOSEPIECE_OFF, &CControlDlg::OnBnClickedRadioNosepieceOff)
	ON_BN_CLICKED(IDC_CHECK_NOSEPIECE, &CControlDlg::OnBnClickedCheckNosepiece)
	ON_BN_CLICKED(IDC_RADIO_SWITCH_ON, &CControlDlg::OnBnClickedRadioSwitchOn)
	ON_BN_CLICKED(IDC_RADIO_SWITCH_OFF, &CControlDlg::OnBnClickedRadioSwitchOff)
	ON_BN_CLICKED(IDC_RADIO_BUZZER_ON, &CControlDlg::OnBnClickedRadioBuzzerOn)
	ON_BN_CLICKED(IDC_RADIO_BUZZER_OFF, &CControlDlg::OnBnClickedRadioBuzzerOff)
END_MESSAGE_MAP()


// CControlDlg メッセージ ハンドラ
BOOL CControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateNosepiece();
	UpdateSwitch();
	UpdateBuzzer();

	m_chkNosepiece.SetCheck(m_pMicroscope->m_bConfirm ? BST_CHECKED : BST_UNCHECKED);

	return TRUE;
}

// Nosepiece
void CControlDlg::SetNosepieceControl(BOOL bEnable)
{
	if (m_pMicroscope->m_bNosepieceMounted) {
		CWaitCursor w;
		HRESULT hr = S_OK;

		try {
			INikonLv* pNikonLv = m_pMicroscope->GetInterface();
			CComPtr<INosepiece> pNosepiece = NULL;
			if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
				_com_raise_error(hr);

			pNosepiece->Put_NosepieceControl(bEnable ? ZLimit : NosepieceControlNone);
			UpdateNosepiece();

			if (pNosepiece)
				pNosepiece.Release();
		}
		catch (_com_error/* &e*/) {
			AfxMessageBox(_T("Microscope error!"));
		}
	}
}
void CControlDlg::UpdateNosepiece()
{
	if (m_pMicroscope->m_bNosepieceMounted) {
		HRESULT hr = S_OK;

		try {
			INikonLv* pNikonLv = m_pMicroscope->GetInterface();
			CComPtr<INosepiece> pNosepiece = NULL;
			if (FAILED(hr = pNikonLv->get_Nosepiece(&pNosepiece)))
				_com_raise_error(hr);

			EnumNosepieceControl eControl = (EnumNosepieceControl)((long)pNosepiece->NosepieceControl);
			if (eControl == ZLimit) {
				m_rdNosepieceOn.SetCheck(BST_CHECKED);
				m_rdNosepieceOff.SetCheck(BST_UNCHECKED);
			}
			else {
				m_rdNosepieceOn.SetCheck(BST_UNCHECKED);
				m_rdNosepieceOff.SetCheck(BST_CHECKED);
			}

			if (pNosepiece)
				pNosepiece.Release();
		}
		catch (_com_error/* &e*/) {
			AfxMessageBox(_T("Microscope error!"));
		}
	}
}
// Switch
void CControlDlg::SetSwitchControl(BOOL bEnable)
{
	CWaitCursor w;
	HRESULT hr = S_OK;

	try {
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();

		if (bEnable) {
			if (FAILED(hr = pNikonLv->EnableSwitch()))
				_com_raise_error(hr);
		}
		else {
			if (FAILED(hr = pNikonLv->DisableSwitch()))
				_com_raise_error(hr);
		}
		UpdateSwitch();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}
void CControlDlg::UpdateSwitch()
{
	INikonLv* pNikonLv = m_pMicroscope->GetInterface();

	if (pNikonLv->IsSwitchEnabled() == StatusTrue) {
		m_rdSwitchOn.SetCheck(BST_CHECKED);
		m_rdSwitchOff.SetCheck(BST_UNCHECKED);
	}
	else {
		m_rdSwitchOn.SetCheck(BST_UNCHECKED);
		m_rdSwitchOff.SetCheck(BST_CHECKED);
	}
}
// Buzzer
void CControlDlg::SetBuzzerControl(BOOL bEnable)
{
	CWaitCursor w;
	HRESULT hr = S_OK;

	try {
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();

		if (bEnable) {
			if (FAILED(hr = pNikonLv->EnableBuzzer()))
				_com_raise_error(hr);
		}
		else {
			if (FAILED(hr = pNikonLv->DisableBuzzer()))
				_com_raise_error(hr);
		}
		UpdateBuzzer();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}
void CControlDlg::UpdateBuzzer()
{
	INikonLv* pNikonLv = m_pMicroscope->GetInterface();

	if (pNikonLv->IsBuzzerEnabled() == StatusTrue) {
		m_rdBuzzerOn.SetCheck(BST_CHECKED);
		m_rdBuzzerOff.SetCheck(BST_UNCHECKED);
	}
	else {
		m_rdBuzzerOn.SetCheck(BST_UNCHECKED);
		m_rdBuzzerOff.SetCheck(BST_CHECKED);
	}
}

// Nosepiece
void CControlDlg::OnBnClickedRadioNosepieceOn()
{
	if (m_rdNosepieceOff.GetCheck() == BST_CHECKED)
		SetNosepieceControl(TRUE);
}
void CControlDlg::OnBnClickedRadioNosepieceOff()
{
	if (m_rdNosepieceOn.GetCheck() == BST_CHECKED)
		SetNosepieceControl(FALSE);
}
void CControlDlg::OnBnClickedCheckNosepiece()
{
	m_pMicroscope->m_bConfirm = m_chkNosepiece.GetCheck() == BST_CHECKED;
}
// Switch
void CControlDlg::OnBnClickedRadioSwitchOn()
{
	if (m_rdSwitchOff.GetCheck() == BST_CHECKED)
		SetSwitchControl(TRUE);
}
void CControlDlg::OnBnClickedRadioSwitchOff()
{
	if (m_rdSwitchOn.GetCheck() == BST_CHECKED)
		SetSwitchControl(FALSE);
}
// Buzzer
void CControlDlg::OnBnClickedRadioBuzzerOn()
{
	if (m_rdBuzzerOff.GetCheck() == BST_CHECKED)
		SetBuzzerControl(TRUE);
}
void CControlDlg::OnBnClickedRadioBuzzerOff()
{
	if (m_rdBuzzerOn.GetCheck() == BST_CHECKED)
		SetBuzzerControl(FALSE);
}
