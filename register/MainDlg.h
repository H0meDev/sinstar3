// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <helper/SDpiHelper.hpp>

#define RC_API 

typedef void  (RC_API  *Fun_Sinstar_InitW)(LPCWSTR pszPath);
typedef LPCWSTR (RC_API  *Fun_Sinstar_GetErrMsgW)();
typedef BOOL (RC_API  *Fun_Sinstar_ShowCaller)(BOOL bOrg);
typedef BOOL (RC_API  *Fun_Sinstar_IsRunning)();
typedef BOOL (RC_API  *Fun_Sinstar_IsUpdateIME)();
typedef BOOL (RC_API  *Fun_Sinstar_Update)();
typedef BOOL (RC_API  *Fun_Sinstar_Uninstall)();
typedef BOOL (RC_API  *Fun_Sinstar_ForceUninstall)();
typedef BOOL (RC_API  *Fun_Sinstar_Install)();
typedef BOOL (RC_API  *Fun_Sinstar_GetCurrentVer)(int wVers[4]);
typedef BOOL (RC_API  *Fun_Sinstar_PEVersionW)(LPCWSTR pszFileName, int wVers[4]);
typedef BOOL (RC_API  *Fun_Sinstar_CheckFiles)();

#define GETPROC(x,mod) (Fun_##x)GetProcAddress(mod,#x)

class CMainDlg : public SHostWnd, public SDpiHandler<CMainDlg>
{
	HMODULE m_hCore;
	Fun_Sinstar_InitW Sinstar_InitW;
	Fun_Sinstar_GetErrMsgW Sinstar_GetErrMsgW;
	Fun_Sinstar_ShowCaller Sinstar_ShowCaller;
	Fun_Sinstar_IsRunning Sinstar_IsRunning;
	Fun_Sinstar_IsUpdateIME Sinstar_IsUpdateIME;
	Fun_Sinstar_Update Sinstar_Update;
	Fun_Sinstar_Uninstall Sinstar_Uninstall;
	Fun_Sinstar_ForceUninstall Sinstar_ForceUninstall;
	Fun_Sinstar_Install Sinstar_Install;
	Fun_Sinstar_GetCurrentVer Sinstar_GetCurrentVer;
	Fun_Sinstar_PEVersionW	Sinstar_PEVersionW;
	Fun_Sinstar_CheckFiles	Sinstar_CheckFiles;
public:
	CMainDlg();
	~CMainDlg();

protected:
	//soui消息
	void OnClose();
	void OnMinimize();
	void OnAgree();

	void OnInstall();
	void OnUninstall();
	void OnForceUninstall();
	void OnUpdate();
	void OnDonate();

	EVENT_MAP_BEGIN()
		EVENT_ID_COMMAND(R.id.btn_close, OnClose)
		EVENT_ID_COMMAND(R.id.btn_min, OnMinimize)
		EVENT_ID_COMMAND(R.id.chk_agree,OnAgree)
		EVENT_ID_COMMAND(R.id.btn_install,OnInstall)
		EVENT_ID_COMMAND(R.id.btn_uninstall,OnUninstall)
		EVENT_ID_COMMAND(R.id.btn_force_uninstall,OnForceUninstall)
		EVENT_ID_COMMAND(R.id.btn_update, OnUpdate)
		EVENT_ID_COMMAND(R.id.btn_donate,OnDonate)
	EVENT_MAP_END()
		
protected:
	//HostWnd真实窗口消息处理
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

	BEGIN_MSG_MAP_EX(CMainDlg)
		CHAIN_MSG_MAP(SDpiHandler<CMainDlg>)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
};
