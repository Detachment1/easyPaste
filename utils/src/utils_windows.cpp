#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <cstring>
#include <algorithm>
#include <utils.h>
#include <thread>
#include <atomic>

DWORD thread_id;

LRESULT CALLBACK filterKeyBoardEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode != HC_ACTION)
    {
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }
    KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *) lParam;
    DWORD newVkCode;

    char wParamStr[16];
    char vkStr[16] = "";

    if (wParam == WM_KEYDOWN)
        strcpy(wParamStr, "KEYDOWN");
    else if (wParam == WM_KEYUP)
        strcpy(wParamStr, "KEYUP");
    else if (wParam == WM_SYSKEYDOWN)
        strcpy(wParamStr, "SYSKEYDOWN");
    else if (wParam == WM_SYSKEYUP)
        strcpy(wParamStr, "SYSKEYUP");
    else
        strcpy(wParamStr, "UNKNOWN");

    if (p->vkCode == 10)
        strcpy(vkStr, "<LF>");
    else if (p->vkCode == 13)
        strcpy(vkStr, "<CR>");
    else
        vkStr[0] = p->vkCode;

    // printf("%d - %s - %x (%s) - %d - %lu\n",
    //        nCode, wParamStr, p->vkCode, vkStr, p->scanCode, p->time);

    // use self-defined extrainfo to identify the last injected event
    // because we can not get the actual finish time of the copy event, therefore, use this extrainfo to get the closed event
    if ((p->flags & LLKHF_INJECTED) != 0 && p->dwExtraInfo == 0x01)
    {
        PostMessage(NULL, 0x400, wParam, lParam);
    }

    if (p->vkCode == 'C' && (p->flags & LLKHF_INJECTED) == 0 && (GetAsyncKeyState(VK_CONTROL) & (1 << 15)) && (GetAsyncKeyState(VK_MENU) & (1 << 15)) && wParam == WM_KEYDOWN)
    {
        INPUT inputs[8] = {};

        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wScan = 0;
        inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
        inputs[0].ki.time = 0;
        inputs[0].ki.dwExtraInfo = 0;
        inputs[0].ki.wVk = VK_MENU;

        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wScan = 0;
        inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
        inputs[1].ki.time = 0;
        inputs[1].ki.dwExtraInfo = 0;
        inputs[1].ki.wVk = VK_CONTROL;

        inputs[2].type = INPUT_KEYBOARD;
        inputs[2].ki.wScan = 0;
        inputs[2].ki.dwFlags = 0;
        inputs[2].ki.time = 0;
        inputs[2].ki.dwExtraInfo = 0;
        inputs[2].ki.wVk = VK_CONTROL;

        inputs[3].type = INPUT_KEYBOARD;
        inputs[3].ki.wScan = 0;
        inputs[3].ki.dwFlags = 0;
        inputs[3].ki.time = 0;
        inputs[3].ki.dwExtraInfo = 0;
        inputs[3].ki.wVk = 'C';

        inputs[4].type = INPUT_KEYBOARD;
        inputs[4].ki.wScan = 0;
        inputs[4].ki.dwFlags = KEYEVENTF_KEYUP;
        inputs[4].ki.time = 0;
        inputs[4].ki.dwExtraInfo = 0;
        inputs[4].ki.wVk = 'C';

        inputs[5].type = INPUT_KEYBOARD;
        inputs[5].ki.wScan = 0;
        inputs[5].ki.dwFlags = KEYEVENTF_KEYUP;
        inputs[5].ki.time = 0;
        inputs[5].ki.dwExtraInfo = 0;
        inputs[5].ki.wVk = VK_CONTROL;

        inputs[6].type = INPUT_KEYBOARD;
        inputs[6].ki.wScan = 0;
        inputs[6].ki.dwFlags = 0;
        inputs[6].ki.time = 0;
        inputs[6].ki.dwExtraInfo = 0;
        inputs[6].ki.wVk = VK_CONTROL;

        inputs[7].type = INPUT_KEYBOARD;
        inputs[7].ki.wScan = 0;
        inputs[7].ki.dwFlags = 0;
        inputs[7].ki.time = 0;
        inputs[7].ki.dwExtraInfo = 0x01;
        inputs[7].ki.wVk = VK_MENU;

        UINT uSent = SendInput(8, inputs, sizeof(INPUT));
        if (uSent != 8)
        {
            printf("fail to press key\n");
        }
        return 1;
    }
    else if (p->vkCode == 'C' && (p->flags & LLKHF_INJECTED) == 0 && (GetAsyncKeyState(VK_CONTROL) & (1 << 15)) && wParam == WM_KEYDOWN)
    {
        PostMessage(NULL, 0x401, wParam, lParam);
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void monitorKeyBoard(EasyPaste *easyPaste) {
    thread_id = GetCurrentThreadId();
    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, filterKeyBoardEvent, NULL, 0);
    //RegisterHotKey(NULL, 100, MOD_CONTROL | MOD_ALT , 'C');
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) != 0)
    {
        if ((msg.message == 0x400 && easyPaste->copyType == 1) || (msg.message == 0x401 && easyPaste->copyType == 0))
        {
            //It is very important to make sure the copy event has finished. Do not remove sleep()!
            Sleep(500);
            std::string newData = getClipboardText();
            std::cout << newData << std::endl;
            if (newData != "")
            {
                newData = removeNewLines(newData);
                saveTextToClipboard(newData);
                std::cout << getClipboardText() << std::endl;
            }
        }
        else if (msg.message == 0x500)
        {
            printf("finish loop\n");
            break;
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    UnhookWindowsHookEx(hook);
}

EasyPaste::EasyPaste(int copyType, int pasteBufferNumber = 5) {
    this->copyType = copyType;
    this->pasteBufferNumber = pasteBufferNumber;
    this->stopFlag = false;
}

void EasyPaste::startLoopThread() {
    std::thread loopThread(monitorKeyBoard, this);
    loopThread.join();
}

int EasyPaste::stopLoopThread() {
    this->stopFlag = true;
    PostThreadMessageW(thread_id, 0x500, 0, 0);
    return 0;
}

std::string getClipboardText()
{
    // Try opening the clipboard
    if (! OpenClipboard(nullptr)) {
        printf("open clipboard error\n");
        return "";
    }
    // Get handle of clipboard object for ANSI text
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr) {
        printf("get handle error\n");
        // Release the clipboard
        CloseClipboard();
        return "";
    }
    // Lock the handle to get the actual text pointer
    char * pszText = static_cast<char*>( GlobalLock(hData) );
    if (pszText == nullptr) {
        printf("get text in clipboard error\n");
        // Release the lock
        GlobalUnlock( hData );
        // Release the clipboard
        CloseClipboard();
        return "";
    }
    // Save text in a string class instance
    std::string text( pszText );

    // Release the lock
    GlobalUnlock( hData );

    // Release the clipboard
    CloseClipboard();

    return text;
}

int saveTextToClipboard(std::string text)
{
    // Try opening the clipboard
    if (! OpenClipboard(nullptr)) {
        printf("open clipboard error\n");
        return 1;
    }
    char* c_text = new char [text.size() + 1];
    std::strcpy (c_text, text.c_str());
    const size_t len = strlen(c_text) + 1;
    HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(hMem), c_text, len);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    // Release the lock
    GlobalUnlock(hMem);
    // Release the clipboard
    CloseClipboard();
    return 0;
}

bool _bothAreSpaces(char l, char r) {
    return (l == r) && (l == ' ');
}

std::string removeNewLines(std::string text) {
    std::replace(text.begin(), text.end(), '\r', ' ');
    std::replace(text.begin(), text.end(), '\n', ' ');
    std::string::iterator new_end = std::unique(text.begin(), text.end(), _bothAreSpaces);
    text.erase(new_end, text.end());
    return text;
}

int hide_console() {
    ShowWindow( GetConsoleWindow(), SW_HIDE );
    return 0;
}