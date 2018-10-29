#include "btoolbar.h"

#include <commctrl.h>
#include <iostream>

void BToolBar::addCheckGroup(int bitmap, int command, bool checked)
{
    int state = TBSTATE_ENABLED;
    if(checked)
        state |= BST_CHECKED;

    buttons.push_back({bitmap, state, TBSTYLE_CHECKGROUP, command});
}

HWND BToolBar::create(HWND parent)
{
    hwnd = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 0, 0, 0,
                                parent, NULL, GetModuleHandle(NULL), NULL);

    SendMessage(hwnd, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);

    //std::cout<<buttons.size()<<std::endl;

    TBBUTTON tbb[buttons.size()];
    TBADDBITMAP tbab;

    tbab.hInst = HINST_COMMCTRL;
    tbab.nID = IDB_STD_SMALL_COLOR;

    ZeroMemory(tbb, sizeof(tbb));
    for(int i=0; i<(int)buttons.size(); i++)
    {
        tbb[i].iBitmap = buttons[i].bitmap;
        tbb[i].fsState = buttons[i].state;
        tbb[i].fsStyle = buttons[i].style;
        tbb[i].idCommand = buttons[i].command;
    }

    SendMessage(hwnd, TB_ADDBITMAP, 0, (LPARAM)&tbab);
    SendMessage(hwnd, TB_ADDBUTTONS, sizeof(tbb)/sizeof(TBBUTTON), (LPARAM)&tbb);
    return hwnd;
}

void BToolBar::adjustSize()
{
    SendMessage(hwnd, WM_SIZE, 0, 0);
}
