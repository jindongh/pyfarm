// Copyright (C) 2007 Andrey Gruber (aka lamer)

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "linklabel.h"

#define	LL_CLASS		"_agLinkLabel_"
#define	LL_CLASSW		L"_agLinkLabelW_"

LRESULT CALLBACK LLabel_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void LLabel_OnPaint(HWND hwnd);
static void LLabel_OnDestroy(HWND hwnd);
static BOOL LLabel_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
static void LLabel_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
static void LLabel_OnSetText(HWND hwnd, LPCTSTR lpszText);
static void SetLLText(HWND hwnd, char * lpText);
static void SetLLTextW(HWND hwnd, wchar_t * lpText);

static BOOL			m_Registered = FALSE, m_RegisteredW = FALSE;

static BOOL RegisterLinkLable(HINSTANCE hInstance){
	
	WNDCLASSEX		wcx;

	ZeroMemory(&wcx, sizeof(wcx));
	wcx.cbSize = sizeof(wcx);
	wcx.cbWndExtra = 8;
	wcx.hCursor = LoadCursor(NULL, IDC_HAND);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.hInstance = hInstance;
	wcx.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcx.lpszClassName = LL_CLASS;
	wcx.lpfnWndProc = LLabel_WndProc;
	m_Registered = (BOOL)RegisterClassEx(&wcx);
	if(!m_Registered)
		if(GetLastError() == ERROR_CLASS_ALREADY_EXISTS)
			m_Registered = TRUE;
	return m_Registered;
}

static BOOL RegisterLinkLableW(HINSTANCE hInstance){
	
	WNDCLASSEXW		wcx;

	ZeroMemory(&wcx, sizeof(wcx));
	wcx.cbSize = sizeof(wcx);
	wcx.cbWndExtra = 8;
	wcx.hCursor = LoadCursor(NULL, IDC_HAND);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.hInstance = hInstance;
	wcx.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcx.lpszClassName = LL_CLASSW;
	wcx.lpfnWndProc = LLabel_WndProc;
	m_RegisteredW = (BOOL)RegisterClassExW(&wcx);
	if(!m_RegisteredW)
		if(GetLastError() == ERROR_CLASS_ALREADY_EXISTS)
			m_RegisteredW = TRUE;
	return m_RegisteredW;
}

HWND CreateLinkLable(HWND hParent, HINSTANCE hInstance, char * lpText, int x, int y, int w, int h, int id){
	HWND		hwnd;

	if(!m_Registered)
		if(!RegisterLinkLable(hInstance))
			return NULL;
	hwnd = CreateWindowEx(0, LL_CLASS, lpText, WS_CHILD | WS_VISIBLE, x, y, w, h, hParent, (HMENU)id, hInstance, NULL);
	if(!hwnd)
		return NULL;
	SetLLText(hwnd, lpText);
	return hwnd;
}

HWND CreateLinkLableW(HWND hParent, HINSTANCE hInstance, wchar_t * lpText, int x, int y, int w, int h, int id){
	HWND		hwnd;

	if(!m_RegisteredW)
		if(!RegisterLinkLableW(hInstance))
			return NULL;
	hwnd = CreateWindowExW(0, LL_CLASSW, lpText, WS_CHILD | WS_VISIBLE, x, y, w, h, hParent, (HMENU)id, hInstance, NULL);
	if(!hwnd)
		return NULL;
	SetLLTextW(hwnd, lpText);
	return hwnd;
}

static void SetLLText(HWND hwnd, char * lpText){
	char		* pText;
	SIZE		sz;
	HDC			hdc;
	int			state;
	HFONT		hFont;

	pText = (char *)GetWindowLongPtr(hwnd, 0);
	if(pText)
		free(pText);
	pText = (char *)calloc(strlen(lpText) + 1, sizeof(char));
	strcpy(pText, lpText);
	SetWindowLongPtr(hwnd, 0, (LONG_PTR)pText);
	hdc = GetDC(hwnd);
	state = SaveDC(hdc);
	hFont = CreateFontIndirect((LPLOGFONT)GetWindowLongPtr(hwnd, 4));
	SelectObject(hdc, hFont);
	GetTextExtentPoint32(hdc, lpText, strlen(lpText), &sz);
	RestoreDC(hdc, state);
	ReleaseDC(hwnd, hdc);
	DeleteObject(hFont);
	SetWindowPos(hwnd, 0, 0, 0, sz.cx, sz.cy, SWP_NOMOVE | SWP_SHOWWINDOW);
}

static void SetLLTextW(HWND hwnd, wchar_t * lpText){
	wchar_t		* pText;
	SIZE		sz;
	HDC			hdc;
	int			state;
	HFONT		hFont;

	pText = (wchar_t *)GetWindowLongPtrW(hwnd, 0);
	if(pText)
		free(pText);
	pText = (wchar_t *)calloc(wcslen(lpText) + 1, sizeof(wchar_t));
	wcscpy(pText, lpText);
	SetWindowLongPtrW(hwnd, 0, (LONG_PTR)pText);
	hdc = GetDC(hwnd);
	state = SaveDC(hdc);
	hFont = CreateFontIndirectW((LPLOGFONTW)GetWindowLongPtrW(hwnd, 4));
	SelectObject(hdc, hFont);
	GetTextExtentPoint32W(hdc, lpText, wcslen(lpText), &sz);
	RestoreDC(hdc, state);
	ReleaseDC(hwnd, hdc);
	DeleteObject(hFont);
	SetWindowPos(hwnd, 0, 0, 0, sz.cx, sz.cy, SWP_NOMOVE | SWP_SHOWWINDOW);
}

LRESULT CALLBACK LLabel_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_PAINT, LLabel_OnPaint);
	HANDLE_MSG (hwnd, WM_DESTROY, LLabel_OnDestroy);
	HANDLE_MSG (hwnd, WM_CREATE, LLabel_OnCreate);
	HANDLE_MSG (hwnd, WM_LBUTTONUP, LLabel_OnLButtonUp);
	HANDLE_MSG (hwnd, WM_SETTEXT, LLabel_OnSetText);

	default: return DefWindowProc (hwnd, msg, wParam, lParam);
	}
}

static void LLabel_OnSetText(HWND hwnd, LPCTSTR lpszText)
{
	if(IsWindowUnicode(hwnd))
		SetLLTextW(hwnd, (wchar_t *)lpszText);
	else
		SetLLText(hwnd, (char *)lpszText);
}

static BOOL LLabel_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	LOGFONTW		*plfW;
	LOGFONT			*plf;
	HDC				hdc;

	hdc = GetDC(hwnd);
	if(IsWindowUnicode(hwnd)){
		plfW = calloc(1, sizeof(LOGFONTW));
		plfW->lfCharSet = 1;
		plfW->lfUnderline = TRUE;
		plfW->lfHeight = -MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		wcscpy(plfW->lfFaceName, L"MS Sans Serif");
		SetWindowLongPtrW(hwnd, 4, (LONG_PTR)plfW);
	}
	else{
		plf = calloc(1, sizeof(LOGFONT));
		plf->lfCharSet = 1;
		plf->lfUnderline = TRUE;
		plf->lfHeight = -MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		strcpy(plf->lfFaceName, "MS Sans Serif");
		SetWindowLongPtr(hwnd, 4, (LONG_PTR)plf);
	}
	ReleaseDC(hwnd, hdc);
	return TRUE;
}

static void LLabel_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	NMHDR		nmh;

	nmh.hwndFrom = hwnd;
	nmh.idFrom = GetDlgCtrlID(hwnd);
	nmh.code = NM_CLICK;
	if(IsWindowUnicode(hwnd))
		SendMessageW(GetParent(hwnd), WM_NOTIFY, (WPARAM)nmh.idFrom, (LPARAM)&nmh);
	else
		SendMessage(GetParent(hwnd), WM_NOTIFY, (WPARAM)nmh.idFrom, (LPARAM)&nmh);
}

static void LLabel_OnDestroy(HWND hwnd)
{
	if(IsWindowUnicode(hwnd)){
		free((void *)GetWindowLongPtrW(hwnd, 0));
		free((void *)GetWindowLongPtrW(hwnd, 4));
	}
	else{
		free((void *)GetWindowLongPtr(hwnd, 0));
		free((void *)GetWindowLongPtr(hwnd, 4));
	}
}

static void LLabel_OnPaint(HWND hwnd)
{
	PAINTSTRUCT		ps;
	RECT			rc;
	int				state;
	HFONT			hFont;

	BeginPaint(hwnd, &ps);
	GetClientRect(hwnd, &rc);
	state = SaveDC(ps.hdc);
	SetBkMode(ps.hdc, TRANSPARENT);
	hFont = CreateFontIndirectW((LPLOGFONTW)GetWindowLongPtrW(hwnd, 4));
	SelectObject(ps.hdc, hFont);
	SetTextColor(ps.hdc, RGB(0, 0, 255));
	if(IsWindowUnicode(hwnd))
		DrawTextW(ps.hdc, (wchar_t *)GetWindowLongPtrW(hwnd, 0), -1, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	else
		DrawText(ps.hdc, (char *)GetWindowLongPtr(hwnd, 0), -1, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	RestoreDC(ps.hdc, state);
	DeleteObject(hFont);
	EndPaint(hwnd, &ps);
}

