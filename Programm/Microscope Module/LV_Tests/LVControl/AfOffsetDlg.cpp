// AfOffsetDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "LvSample.h"
#include "AfOffsetDlg.h"


// CAfOffsetDlg ダイアログ

IMPLEMENT_DYNAMIC(CAfOffsetDlg, CDialog)

CAfOffsetDlg::CAfOffsetDlg(CMicroscopeControl* pMicroscope, CWnd* pParent /*=NULL*/)
	: CDialog(CAfOffsetDlg::IDD, pParent)
	, m_pMicroscope(pMicroscope)
{

}

CAfOffsetDlg::~CAfOffsetDlg()
{
}

void CAfOffsetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_AF_RECIPE, m_cboAfRecipe);
	DDX_Control(pDX, IDC_SLIDER_AF_OFFSET, m_slAfOffset);
	DDX_Control(pDX, IDC_CHECK_AF_OFFSET, m_chkAfOffsetMode);
	DDX_Control(pDX, IDC_EDIT_OFFSET, m_edAfOffset);
	DDX_Control(pDX, IDC_SPIN_OFFSET, m_spnAfOffset);
}


BEGIN_MESSAGE_MAP(CAfOffsetDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_MESSAGE(WM_PAREVENT, &CAfOffsetDlg::OnParamEvent)
	ON_BN_CLICKED(IDC_CHECK_AF_OFFSET, &CAfOffsetDlg::OnBnClickedCheckAfOffset)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, &CAfOffsetDlg::OnBnClickedButtonChange)
	ON_CBN_SELCHANGE(IDC_COMBO_AF_RECIPE, &CAfOffsetDlg::OnCbnSelchangeComboAfRecipe)
END_MESSAGE_MAP()


// CAfOffsetDlg メッセージ ハンドラ
BOOL CAfOffsetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_spnAfOffset.SetBuddy(&m_edAfOffset);

	// Connect to Microscope an MipParam
	ConnectMipParam();

	return TRUE;
}

void CAfOffsetDlg::OnDestroy()
{
	DisconnectMipParam();

	CDialog::OnDestroy();
}

HRESULT CAfOffsetDlg::ConnectMipParam()
{
	HRESULT hr = S_OK;

	try {
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();
		CComPtr<IPresets> pPresets = NULL;
		CComPtr<IZDrive> pZDrive = NULL;
		VARIANT val;

		if (FAILED(hr = pNikonLv->get_Presets(&pPresets)))
			_com_raise_error(hr);

		if (FAILED(hr = pPresets->get_AfRecipeMode(&val)))
			_com_raise_error(hr);
		if (FAILED(hr = m_cboAfRecipe.Connect(val.pdispVal)))
			_com_raise_error(hr);

		if (FAILED(hr = pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		if (FAILED(hr = pZDrive->get_AfOffsetMode(&val)))
			_com_raise_error(hr);
		if (FAILED(hr = m_chkAfOffsetMode.Connect(val.pdispVal)))
			_com_raise_error(hr);

		if (FAILED(hr = pZDrive->get_AfOffset(&val)))
			_com_raise_error(hr);
		if (FAILED(hr = m_slAfOffset.Connect(val.pdispVal)))
			_com_raise_error(hr);

		if (pPresets)
			pPresets.Release();
		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}

	UpdateRecipeInformation();
	UpdateAfOffsetMode();
	UpdateAfOffsetValueInformation();
	UpdateAfOffsetValue();

	return hr;
}

HRESULT CAfOffsetDlg::DisconnectMipParam()
{
	m_chkAfOffsetMode.Disconnect();
	m_slAfOffset.Disconnect();
	m_cboAfRecipe.Disconnect();

	return S_OK;
}

void CAfOffsetDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	BOOL bSned = FALSE;
	BOOL bUpdate = FALSE;

	switch (nSBCode) {
	case SB_LEFT:
	case SB_RIGHT:
	case SB_LINELEFT:
	case SB_LINERIGHT:
	case SB_PAGELEFT:
	case SB_PAGERIGHT:
	case SB_THUMBPOSITION:
		bSned = TRUE;
		// not break
	case SB_THUMBTRACK:
		bUpdate = TRUE;
		break;
	case SB_ENDSCROLL:
		break;
	default:
		break;
	}
	if (bSned) {
		CWaitCursor w;
		if (pScrollBar->m_hWnd == m_slAfOffset.m_hWnd)
			m_slAfOffset.PutValue();
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

LRESULT CAfOffsetDlg::OnParamEvent(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)	{
	case EMIPPAR_VALUECHANGED:
		ValueChanged(wParam);
		break;
	case EMIPPAR_INFOCHANGED:
		InfoChanged(wParam);
		ValueChanged(wParam);
		break;
	}
	return 0L;
}
void CAfOffsetDlg::ValueChanged(WPARAM wParam)
{
	switch (wParam)	{
	case IDC_CHECK_AF_OFFSET:
		UpdateAfOffsetMode();
		break;
	case IDC_SLIDER_AF_OFFSET:
		UpdateAfOffsetValue();
		break;
	case IDC_COMBO_AF_RECIPE:
		UpdateAfOffsetMode();
		UpdateAfOffsetValueInformation();
		UpdateAfOffsetValue();
		UpdateRecipeInformation();
		break;
	default:
		break;
	}
}
void CAfOffsetDlg::InfoChanged(WPARAM wParam)
{
	switch (wParam)	{
	case IDC_SLIDER_AF_OFFSET:
		UpdateAfOffsetValueInformation();
		break;
	default:
		break;
	}
}

void CAfOffsetDlg::UpdateAfOffsetMode()
{
	BOOL bEnable = FALSE;
	VARIANT val;
	if (m_chkAfOffsetMode.GetValue(&val)) {
		if (val.vt == VT_I4) {
			bEnable = ((EnumStatus)val.lVal) == StatusTrue ? TRUE : FALSE;
			m_chkAfOffsetMode.SetCheck(bEnable ? BST_CHECKED : BST_UNCHECKED);
		}
	}
	m_chkAfOffsetMode.SetCheck(bEnable ? BST_CHECKED : BST_UNCHECKED);
	GetDlgItem(IDC_SLIDER_AF_OFFSET)->EnableWindow(bEnable);
}
void CAfOffsetDlg::UpdateAfOffsetValue()
{
	VARIANT val;
	if (m_slAfOffset.GetValue(&val)) {
		if (val.vt == VT_I4) {
			CString str;
			str.Format(_T("%d"), val.lVal);
			GetDlgItem(IDC_EDIT_OFFSET)->SetWindowText(str);
		}
	}
}
void CAfOffsetDlg::UpdateRecipeInformation()
{
	VARIANT val;
	if (m_cboAfRecipe.GetValue(&val)) {
		if (val.vt == VT_I4) {
			for (int i = 0; i < m_cboAfRecipe.GetCount(); i++) {
				if (m_cboAfRecipe.GetItemData(i) == val.lVal) {
					if (m_cboAfRecipe.GetCurSel() != i)
						m_cboAfRecipe.SetCurSel(i);
					break;
				}
			}
		}
	}
}
void CAfOffsetDlg::UpdateAfOffsetValueInformation()
{
	int nMax = m_slAfOffset.GetRangeMax();
	int nMin = m_slAfOffset.GetRangeMin();
	m_slAfOffset.SetTicFreq((nMax - nMin) / 100);
	m_spnAfOffset.SetRange32(nMin, nMax);
}

void CAfOffsetDlg::OnBnClickedCheckAfOffset()
{
	HRESULT hr = S_OK;

	try {
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();

		CComPtr<IZDrive> pZDrive = NULL;
		if (FAILED(hr = pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		CWaitCursor w;
		pZDrive->Put_AfOffsetMode(m_chkAfOffsetMode.GetCheck() == BST_CHECKED ? StatusFalse : StatusTrue);

		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
	}
}

void CAfOffsetDlg::OnBnClickedButtonChange()
{
	CString strValue;
	m_edAfOffset.GetWindowText(strValue);
	if (strValue.GetLength() > 0) {
		TCHAR* e;
		long lVal = _tcstol(strValue, &e, 0);
		if (*e == '\0') {
			if (lVal < m_slAfOffset.GetRangeMin() || lVal > m_slAfOffset.GetRangeMax()) {
				AfxMessageBox(IDS_MESSAGE_OUT_OF_RANGE);
			}
			else {
				HRESULT hr = S_OK;
				try {
					INikonLv* pNikonLv = m_pMicroscope->GetInterface();

					CComPtr<IZDrive> pZDrive = NULL;
					if (FAILED(hr = pNikonLv->get_ZDrive(&pZDrive)))
						_com_raise_error(hr);

					if (lVal != (long)pZDrive->AfOffset) {
						CWaitCursor w;
						m_slAfOffset.SetPos(lVal);
						m_slAfOffset.PutValue();
						AfxMessageBox(IDS_MESSAGE_AF_OFFSET_CHANGED);
					}

					if (pZDrive)
						pZDrive.Release();
				}
				catch (_com_error/* &e*/) {
					AfxMessageBox(_T("Microscope error!"));
				}
			}
		}
		else {
			AfxMessageBox(IDS_MESSAGE_INVALID_FORMAT);
		}
	}
	else {
		AfxMessageBox(IDS_MESSAGE_INPUT_AF_OFFSET);
	}
}

void CAfOffsetDlg::OnCbnSelchangeComboAfRecipe()
{
	CWaitCursor w;
	m_cboAfRecipe.PutValue();
}
