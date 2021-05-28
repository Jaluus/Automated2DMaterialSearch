// DeviceDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "LvSample.h"
#include "DeviceDlg.h"


// CDeviceDlg ダイアログ

IMPLEMENT_DYNAMIC(CDeviceDlg, CDialog)

CDeviceDlg::CDeviceDlg(CMicroscopeControl* pMicroscope, CWnd* pParent /*=NULL*/)
	: CDialog(CDeviceDlg::IDD, pParent)
	, m_pMicroscope(pMicroscope)
	, m_bIsAvailable(FALSE)
	, m_bIsDeviceAvailable(FALSE)
	, m_lDeviceIndex(-1)
{

}

CDeviceDlg::~CDeviceDlg()
{
}

void CDeviceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DEVICE, m_lstDevice);
}


BEGIN_MESSAGE_MAP(CDeviceDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CDeviceDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDeviceDlg::OnBnClickedCancel)
	ON_LBN_DBLCLK(IDC_LIST_DEVICE, &CDeviceDlg::OnLbnDblclkListDevice)
END_MESSAGE_MAP()


// CDeviceDlg メッセージ ハンドラ
BOOL CDeviceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_lstDevice.ResetContent();

	HRESULT hr = S_OK;
	try {
		INikonLv* pNikonLv = m_pMicroscope->GetInterface();

		long lIndex = pNikonLv->Device->Index;
		long lDevice = pNikonLv->Device->DeviceIndex;

		if (FAILED(hr = pNikonLv->Devices->Refresh()))
			_com_raise_error(hr);
		for (int i = 0; i < pNikonLv->Devices->Count; i++) {
			_bstr_t bstrDeviceName = pNikonLv->Devices->Item[i]->Address;
			CString strDeviceName(bstrDeviceName.GetBSTR());
			if (strDeviceName.IsEmpty())
				m_lstDevice.AddString(_T("null"));
			else
				m_lstDevice.AddString(strDeviceName);
		}

		m_bIsAvailable = pNikonLv->Device->IsAvailable ? TRUE : FALSE;

		if (m_bIsAvailable) {
			if (lIndex >= 0 && lIndex < m_lstDevice.GetCount() &&
				((lDevice >= 0 && pNikonLv->Devices->Item[lIndex]->DeviceIndex == lDevice) || lIndex == 0))
				m_lstDevice.SetCurSel(lIndex);
			else
				m_bIsAvailable = FALSE;
		}
	}
	catch (_com_error/* &e*/) {
		AfxMessageBox(_T("Microscope error!"));
		return FALSE;
	}

	return TRUE;
}

void CDeviceDlg::OnBnClickedOk()
{
	long lIndex = m_lstDevice.GetCurSel();
	if (lIndex < 0) {
		AfxMessageBox(IDS_MESSAGE_SELECT_DEVICE);
	}
	else {
		HRESULT hr = S_OK;
		try {
			INikonLv* pNikonLv = m_pMicroscope->GetInterface();
			if (FAILED(hr = pNikonLv->Devices->Refresh()))
				_com_raise_error(hr);

			m_bIsDeviceAvailable = pNikonLv->Devices->Item[lIndex]->IsAvailable ? TRUE : FALSE;

			if (!m_bIsDeviceAvailable) {
				AfxMessageBox(IDS_MESSAGE_NO_RESPONSE_FROM_DEVICE);
			}
			else {
				m_lDeviceIndex = lIndex;
				CDialog::OnOK();
			}
		}
		catch (_com_error/* &e*/) {
			AfxMessageBox(IDS_MESSAGE_CONNECT_DEVICE_FAILED);
		}
	}
}

void CDeviceDlg::OnBnClickedCancel()
{
	if (!m_bIsAvailable)
		AfxMessageBox(IDS_MESSAGE_NOT_CONNECTED_DEVICE);
	else
		CDialog::OnCancel();
}

void CDeviceDlg::OnLbnDblclkListDevice()
{
	OnBnClickedOk();
}
