#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <cstring>
#include <algorithm>

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