#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <array>

std::string getClipboardText()
{
    const char cmd[] = "echo | pbpaste";
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    if (result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

int saveTextToClipboard(std::string text)
{
    std::string cmd = "echo '" + text + "' | pbcopy";
    char * cstr = new char [cmd.size() + 1];
    std::strcpy (cstr, cmd.c_str());
    return system(cstr);
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
    return 0;
}