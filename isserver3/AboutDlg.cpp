// MainDlg.cpp : implementation of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AboutDlg.h"	
	
CAboutDlg::CAboutDlg() : SHostDialog(UIRES.LAYOUT.dlg_about)
{
}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::OnClose()
{
	EndDialog(IDOK);
}

