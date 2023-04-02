//
// Created by 梁凯然 on 2023/3/22.
//
#include <string>

#ifndef EASYPASTE_COPYPASTE_H
#define EASYPASTE_COPYPASTE_H
std::string getClipboardText();
int saveTextToClipboard(std::string text);
std::string removeNewLines(std::string text);
int hide_console();
#endif //EASYPASTE_COPYPASTE_H
