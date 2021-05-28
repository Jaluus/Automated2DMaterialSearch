// ZDriveRangeDlg.cpp : ŽÀ‘•ƒtƒ@ƒCƒ‹
//

#include "stdafx.h"
#include "LvSample.h"
#include "ZDriveRangeDlg.h"


// CZDriveRangeDlg dialog

IMPLEMENT_DYNAMIC(CZDriveRangeDlg, CDialog)

CZDriveRangeDlg::CZDriveRangeDlg(CMicroscopeControl* pMicroscope, CWnd* pParent /*=NULL*/)
	: CDialog(CZDriveRangeDlg::IDD, pParent)
	, m_pMicroscope(pMicroscope)
{
}

CZDriveRangeDlg::~CZDriveRangeDlg()
{
}

void CZDriveRangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CZDriveRangeDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_READ_MIN, &CZDriveRangeDlg::OnBnClickedButtonReadMin)
	ON_BN_CLICKED(IDC_BUTTON_READ_MAX, &CZDriveRangeDlg::OnBnClickedButtonReadMax)
	ON_BN_CLICKED(IDOK, &CZDriveRangeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CZDriveRangeDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CZDriveRangeDlg message handlers
BOOL CZDriveRangeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HRESULT hr = S_OK;
	try {
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();
		CComPtr<IZDrive> pZDrive = NULL;
		if (FAILED(hr = pNikonLv->get_ZDrive(&pZDrive)))
			_com_raise_error(hr);

		m_lPos = (long)pZDrive->Value;
		m_lUnit = (long)pZDrive->ValuePerUnit;
		m_lSoftwareLower = (long)pZDrive->SoftwareLowerLimit;
		m_lSoftwareUpper = (long)pZDrive->SoftwareUpperLimit;

		if (pZDrive)
			pZDrive.Release();
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
		return FALSE;
	}

	CString strMin, strMax;
	strMin.Format(_T("%.2f"), (double)m_pMicroscope->GetZDriveMin() / (double)m_lUnit);
	GetDlgItem(IDC_EDIT_RANGE_MIN)->SetWindowText(strMin);
	strMax.Format(_T("%.2f"), (double)m_pMicroscope->GetZDriveMax() / (double)m_lUnit);
	GetDlgItem(IDC_EDIT_RANGE_MAX)->SetWindowText(strMax);

	return TRUE;
}

void CZDriveRangeDlg::OnBnClickedButtonReadMin()
{
	CString strMin;
	strMin.Format(_T("%.2f"), (double)m_lPos / (double)m_lUnit);
	GetDlgItem(IDC_EDIT_RANGE_MIN)->SetWindowText(strMin);
}

void CZDriveRangeDlg::OnBnClickedButtonReadMax()
{
	CString strMax;
	strMax.Format(_T("%.2f"), (double)m_lPos / (double)m_lUnit);
	GetDlgItem(IDC_EDIT_RANGE_MAX)->SetWindowText(strMax);
}

void CZDriveRangeDlg::OnBnClickedOk()
{
	CString strMin, strMax;
	GetDlgItem(IDC_EDIT_RANGE_MIN)->GetWindowText(strMin);
	GetDlgItem(IDC_EDIT_RANGE_MAX)->GetWindowText(strMax);
	if (strMin.GetLength() > 0 && strMax.GetLength() > 0) {
		TCHAR* eMin;
		TCHAR* eMax;
		double dNewMin = _tcstod(strMin, &eMin);
		double dNewMax = _tcstod(strMax, &eMax);
		if (*eMin == '\0' && *eMax == '\0') {
			long lNewMin = (long)(dNewMin * m_lUnit);
			long lNewMax = (long)(dNewMax * m_lUnit);
			if (lNewMin >= lNewMax) {
				AfxMessageBox(IDS_MESSAGE_RANGE_BOUND);
			}
			else if (lNewMin > m_lPos || lNewMax < m_lPos) {
				CString str, strCurrent, strOutside;
				strCurrent.LoadString(IDS_MESSAGE_RANGE_CURRENT);
				strOutside.LoadString(IDS_MESSAGE_RANGE_OUTSIDE);
				str.Format(_T("%s[%.2lf]%s"), strCurrent, (double)m_lPos / (double)m_lUnit, strOutside);
				AfxMessageBox(str);
			}
			else if (lNewMin < m_lSoftwareLower || lNewMax < m_lSoftwareLower ||
					 lNewMin > m_lSoftwareUpper || lNewMax > m_lSoftwareUpper) {
				CString str, strRange1, strRange2, strRange3;
				strRange1.LoadString(IDS_MESSAGE_RANGE1);
				strRange2.LoadString(IDS_MESSAGE_RANGE2);
				strRange3.LoadString(IDS_MESSAGE_RANGE3);
				str.Format(_T("%s[%.2lf]%s[%.2lf]%s"), strRange1, (double)m_lSoftwareLower / (double)m_lUnit, strRange2, (double)m_lSoftwareUpper / (double)m_lUnit, strRange3);
				AfxMessageBox(str);
			}
			else {
				m_pMicroscope->SetZDriveMin(lNewMin);
				m_pMicroscope->SetZDriveMax(lNewMax);
				CDialog::OnOK();
			}
		}
		else {
			AfxMessageBox(IDS_MESSAGE_INVALID_FORMAT);
		}
	}
	else {
		AfxMessageBox(IDS_MESSAGE_RANGE_INPUT_VALUE);
	}
}
void CZDriveRangeDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}
