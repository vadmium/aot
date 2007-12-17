/* main.cpp for ----, by Martin Panter, created --- */
/* ^-- that is wrong */

#include <windows.h>
#include "resource.h"
#include <cstring>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

//~ #define numelem(array) (sizeof (array) / sizeof (array)[0])

const char WND_CLASS[] = "main";
void reg_class(HINSTANCE hi);
void unreg_class(HINSTANCE hi);
LRESULT CALLBACK wnd_proc(HWND hw, UINT msg, WPARAM wp, LPARAM lp);
const UINT WM_NOTIFY_ICON = WM_APP;
LRESULT on_create(HWND hw);
void on_destroy(HWND hw);
void on_close(HWND hw);
HWND create_wnd(HINSTANCE hi);
LRESULT on_notify_icon(HWND hw, LPARAM msg);
const int IDM_CANCEL = 0;
const int IDM_EXIT = IDM_CANCEL + 1;
void on_exit(HWND hw);

int WINAPI WinMain(
  HINSTANCE hi, HINSTANCE /*hi_prev*/, LPSTR /*cmd_line*/, int /*show_cmd*/
) {
  reg_class(hi);
  
  /* Create hidden window. */
  /*HWND hw = */
  create_wnd(hi);
  
  /* Message pump */
  MSG msg;
  /* Remember to quit when GetMessage gets a quit message. */
  //TODO: figure out this binary/trinary thing with getmessage return value (on error)
  while(GetMessage(&msg, NULL, 0, 0)) {
//    if(TranslateAccelerator(cal_wnd->getHWnd(), hAccel, &msg)) {
//      continue;
//    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  
  unreg_class(hi);
  
  return msg.wParam;
}

void reg_class(HINSTANCE hi) {
  WNDCLASS wc;
  wc.style = 0;
  wc.lpfnWndProc = &wnd_proc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hi;
  wc.hIcon = NULL; // TODO: might use LoadIcon later
  wc.hCursor = NULL; // TODO: might use normal cursor
  wc.hbrBackground = NULL; // TODO: might use background
  wc.lpszMenuName = NULL;
  wc.lpszClassName = WND_CLASS;
  RegisterClass(&wc);
  // TODO: check for error returns
}

void unreg_class(HINSTANCE hi) {
  UnregisterClass(WND_CLASS, hi);
  // TODO: check for error return (not much can do though)
}

HWND create_wnd(HINSTANCE hi) {
  return CreateWindow(
    WND_CLASS, /* class */
    NULL, /* caption */
    0, /* style */
    0, 0, 0, 0, /* size, position */
    NULL, /* parent/owner */
    NULL, hi, NULL
  );
  /* NULL != return */
}

LRESULT CALLBACK wnd_proc(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
  switch(msg)
  {
  case WM_CREATE:
    return on_create(hw);
  
  case WM_DESTROY:
    on_destroy(hw);
    break;
  
  case WM_CLOSE:
    on_close(hw);
    return 0;
  
  case WM_NOTIFY_ICON:
    return on_notify_icon(hw, lp);
  
  //~ case WM_COMMAND:
    //~ switch(LOWORD(wp))
    //~ {
    //~ case IDM_EXIT:
      //~ on_exit(hw);
      //~ break;
    //~ }
    //~ return 0;
  }
  
  return DefWindowProc(hw, msg, wp, lp);
}

LRESULT on_create(HWND hw) {
  NOTIFYICONDATA nid;
  nid.cbSize = sizeof nid; // TODO: can I make it smaller to support earlier versions?
  nid.hWnd = hw;
  nid.uID = 0;
  nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
  nid.uCallbackMessage = WM_NOTIFY_ICON;
  nid.hIcon = static_cast<HICON>(LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON,
  GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR));
  strcpy(nid.szTip, "Top windows"); // TODO: handle UNICODE

  Shell_NotifyIcon(NIM_ADD, &nid);
  // TODO: if this returns false, the function fails.
  
  DestroyIcon(nid.hIcon);
  //nid.uFlags &= ~NIF_ICON;
  
  return 0;
}

void on_destroy(HWND hw) {
  NOTIFYICONDATA nid;
  nid.cbSize = sizeof nid; // TODO: can I make it smaller to support earlier versions?
  nid.hWnd = hw;
  nid.uID = 0;
  nid.uFlags = 0;
  Shell_NotifyIcon(NIM_DELETE, &nid);
  // TODO: if this returns false, the function fails. (not really much can be done)

  PostQuitMessage(0);
}

void on_close(HWND hw) {
  DestroyWindow(hw);
}

struct Lister {
  int count;
  HMENU hm;
};

BOOL CALLBACK list_wnd(HWND hw, LPARAM lp) {
  Lister *l = (Lister *)lp;
  // check for: own window
  
  // invisible window?
  if(!IsWindowVisible(hw)) {
    return TRUE;
  }
  
  int title_length = GetWindowTextLength(hw);
  // zero length title
  char *title = new char[title_length + 1];
  GetWindowText(hw, title, title_length + 1);
  // TODO: check error return
  // title is "Program Manager"
  MENUITEMINFO mii;
  mii.cbSize = sizeof mii;
  mii.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_DATA;
  mii.fType = MFT_STRING;
  // if window is on top tick the item
  mii.fState = GetWindowLong(hw, GWL_EXSTYLE) & WS_EX_TOPMOST?
    MFS_CHECKED : MFS_UNCHECKED;
  mii.wID = IDM_EXIT + 1 + l->count;
  mii.dwItemData = (DWORD)hw;
  mii.dwTypeData = title; /* TODO: fix apersand & problems */

  // put the windows in in the order that we find them - that way
  // they appear in their z-order - and topmost are all at top of menu
  InsertMenuItem(l->hm, l->count, TRUE, &mii);

  ++l->count;
  delete title;
  return TRUE;
}

LRESULT on_notify_icon(HWND hw, LPARAM msg) {
  switch(msg)
  {
  case WM_RBUTTONUP:
  case WM_CONTEXTMENU:
    POINT pt_cursor;
    GetCursorPos(&pt_cursor);
    // TODO: doesn't each windows message already contain the cursor co-ordinates?
    
    // enum windows and add them to the menu
    Lister l;
    l.hm = CreatePopupMenu(); // TODO: error?
    l.count = 0;
    EnumWindows(list_wnd, (LPARAM)&l);
    // TODO: check return for errors
    
    AppendMenu(l.hm, MF_SEPARATOR, 0, 0);
    AppendMenu(l.hm, MF_STRING | MF_ENABLED, IDM_EXIT, TEXT("E&xit"));
    AppendMenu(l.hm, MF_STRING | MF_ENABLED, IDM_CANCEL, TEXT("&Cancel"));
    
    // Note calls to SetForegroundWindow and PostMessage(NULL) to work around weird menu behaviour.
    SetForegroundWindow(hw); // makes sure that menu gets keypresses, cancels immediately when clicked off
    int id = TrackPopupMenu(l.hm,
    TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON /*|
    TPM_HORPOSANIMATION | TPM_VERPOSANIMATION*/ | TPM_RETURNCMD,
      /* Doesn't look like animation constants are defined for mingw---ah, who cares, I've
      got the animation turned off in Windows anyhow. */
    pt_cursor.x, pt_cursor.y, 0, hw, NULL);
    PostMessage(hw, WM_NULL, 0, 0); // stops menu from sometimes instantly disappearing
    
    if(id > IDM_EXIT) {
      MENUITEMINFO mii;
      mii.cbSize = sizeof mii;
      mii.fMask = MIIM_DATA;
      GetMenuItemInfo(l.hm, id, FALSE, &mii);
      HWND hw = (HWND)mii.dwItemData;
      
      SetWindowPos(hw, GetWindowLong(hw, GWL_EXSTYLE) & WS_EX_TOPMOST?
        HWND_NOTOPMOST : HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
    DestroyMenu(l.hm);
    if(IDM_EXIT == id) {
      on_exit(hw);
    }
    break;
  }
  
  return 0;
}

void on_exit(HWND hw) {
  PostMessage(hw, WM_CLOSE, 0, 0);
}
