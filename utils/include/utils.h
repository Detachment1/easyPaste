//
// Created by Kairan Liang on 2023/3/22.
//
#include <string>
#include <stdatomic.h>

#ifndef EASYPASTE_COPYPASTE_H
#define EASYPASTE_COPYPASTE_H

class EasyPaste {
public:
    // copy type :
    // for windows
    // 0 : removing \n when type ctrl + c, normal copy when type ctrl + alt + c
    // 1 : removing \n when type ctrl + alt + c, normal copy when type ctrl + c
    // for mac
    // 0 : removing \n when type command + c, normal copy when type ctrl + command + c
    // 1 : removing \n when type ctrl + command + c, normal copy when type command + c
    int copyType;
    // better not lager than 5
    int pasteBufferNumber;

    void startLoopThread();

    int stopLoopThread();

    EasyPaste(int copyType, int pasteBufferNumber);

    std::atomic_bool stopFlag;

};
std::string getClipboardText();
int saveTextToClipboard(std::string text);
std::string removeNewLines(std::string text);
int hide_console();

#endif //EASYPASTE_COPYPASTE_H
