// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"

#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")

CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
	m_bLayoutInited=FALSE;
} 

CMainDlg::~CMainDlg()
{
}

int CMainDlg::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	// 		MARGINS mar = {5,5,30,5};
	// 		DwmExtendFrameIntoClientArea ( m_hWnd, &mar );
	SetMsgHandled(FALSE);
	return 0;
}

void CMainDlg::OnShowWindow( BOOL bShow, UINT nStatus )
{
	if(bShow)
	{
 		AnimateHostWindow(200,AW_CENTER);
	}
}


BOOL CMainDlg::OnInitDialog( HWND hWnd, LPARAM lParam )
{
    m_bLayoutInited=TRUE;
    
    m_pTreeCtrl = FindChildByName2<STreeCtrl>(L"treeSwnd");
    SASSERT(m_pTreeCtrl);

    return 0;
}

BOOL CMainDlg::OnEventCaptureHost( EventArgs *pEvt )
{
    EventCapture *pEvt2=(EventCapture*)pEvt;
    CPoint pt = pEvt2->pt_;
    ClientToScreen(&pt);
    HWND hWnd=::WindowFromPoint(pt);
    if(hWnd == m_hWnd) return TRUE;

    SStringT str;
    str.Format(_T("0x%08x"),hWnd);
    FindChildByName(L"txt_hosthwnd")->SetWindowText(str);
    TCHAR szClassName[100];
    ::GetClassName(hWnd,szClassName,100);
    FindChildByName(L"txt_hostclass")->SetWindowText(szClassName);
    return FALSE;
}

BOOL CMainDlg::OnEventCaptureHostFinish( EventArgs *pEvt )
{
    EventCaptureFinish *pEvt2=(EventCaptureFinish*)pEvt;
    CPoint pt = pEvt2->pt_;
    ClientToScreen(&pt);
    HWND hWnd=::WindowFromPoint(pt);
    if(hWnd == m_hWnd) return TRUE;

    SStringT str;
    str.Format(_T("0x%08x"),hWnd);
    FindChildByName(L"txt_hosthwnd")->SetWindowText(str);
    TCHAR szClassName[100];
    ::GetClassName(hWnd,szClassName,100);
    FindChildByName(L"txt_hostclass")->SetWindowText(szClassName);

    m_pTreeCtrl->RemoveAllItems();
    
    LRESULT lRet=::SendMessage(hWnd,SPYMSG_SETSPY,0,(LPARAM)m_hWnd);

    if(lRet == 1)
    {
        HSTREEITEM hRoot = m_pTreeCtrl->InsertItem(_T("root"),0,1,(LPARAM)hWnd);
        EnumSwnd(hWnd,0,hRoot);
    }

    return TRUE;
}

SWNDINFO swndInfo;
void CMainDlg::EnumSwnd( HWND hHost ,DWORD dwSwnd, HSTREEITEM hItem)
{
    DWORD  dwChild = ::SendMessage(hHost,SPYMSG_SWNDENUM,dwSwnd,GSW_FIRSTCHILD);
    while(dwChild != 0)
    {
        if(0!=::SendMessage(hHost,SPYMSG_SWNDINFO,dwChild,0))
        {
            SStringW str;
            str.Format(L"swnd:%d,name:%s",swndInfo.swnd,swndInfo.szName);
            HSTREEITEM hChild=m_pTreeCtrl->InsertItem((LPCTSTR)S_CW2T(str),0,1,(LPARAM)0,hItem);
            EnumSwnd(hHost,dwChild,hChild);
        }
        dwChild = ::SendMessage(hHost,SPYMSG_SWNDENUM,dwChild,GSW_NEXTSIBLING);
    }
}

BOOL CMainDlg::OnCopyData( HWND hSend, PCOPYDATASTRUCT pCopyDataStruct )
{
    if(pCopyDataStruct->dwData != SPYMSG_SWNDINFO) return FALSE;
    SWNDINFO *pSwndInfo = (SWNDINFO*)pCopyDataStruct->lpData;
    memcpy(&swndInfo,pCopyDataStruct->lpData,sizeof(swndInfo));    
    return TRUE;
}