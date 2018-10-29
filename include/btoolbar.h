#ifndef BTOOLBAR_H
#define BTOOLBAR_H

#include <windows.h>
#include <vector>

struct buttonDesc
{
    int bitmap;
    int state;
    int style;
    int command;
};

class BToolBar
{
public:
    void addCheckGroup(int bitmap, int command, bool checked);
    HWND create(HWND parent);
    void adjustSize();
    HWND getHandle() { return hwnd; }

protected:

private:
    std::vector<buttonDesc> buttons;
    HWND hwnd;
};

#endif // BTOOLBAR_H
