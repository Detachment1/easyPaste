#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <array>
#include <ApplicationServices/ApplicationServices.h>
#include <thread>
#include <chrono>
#include "utils.h"
#include <functional>

int pressKeyBoard(CGEventFlags flags, CGKeyCode keyCode) {
    CGEventSourceRef source = CGEventSourceCreate(kCGEventSourceStateCombinedSessionState);
    if (!source)
    {
        std::cout << "create CGEventSourceRef failed" << std::endl;
        return -1;
    }
    CGEventRef pressDown = CGEventCreateKeyboardEvent(source, keyCode, true);
    if (!pressDown)
    {
        std::cout << "create CGEventRef failed" << std::endl;
        return -1;
    }
    CGEventSetFlags(pressDown, flags);
    CGEventRef pressUp = CGEventCreateKeyboardEvent(source, keyCode, false);
    if (!pressUp)
    {
        std::cout << "create CGEventRef failed" << std::endl;
        return -1;
    }
    CGEventPost(kCGAnnotatedSessionEventTap, pressDown);
    CGEventPost(kCGAnnotatedSessionEventTap, pressUp);

    CFRelease(pressUp);
    CFRelease(pressDown);
    CFRelease(source);
    return 0;
}

CGEventRef filterKeyBoardEvent(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *info) {
    EasyPaste *easyPaste = (EasyPaste *) info;

    // only focus on key down
    if (type != kCGEventKeyDown)
        return event;
    // get keycode.
    CGKeyCode keycode = (CGKeyCode) CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    if (keycode != (CGKeyCode)8)
    {
        return event;
    }
    // get prefix key flag
    CGEventFlags f = CGEventGetFlags(event);
//    std::cout << keycode << std::endl;
//    std::cout << f << std::endl;
    CGEventFlags removeNewLineFlag;
    CGEventFlags simpleCopyFlag;
    if (easyPaste->copyType == 0)
    {
        removeNewLineFlag = 1048840;
        simpleCopyFlag = 1310985;
    }
    else if (easyPaste->copyType == 1)
    {
        removeNewLineFlag = 1310985;
        simpleCopyFlag = 1048840;
    }
    if (f == removeNewLineFlag)
    {
        if (pressKeyBoard((CGEventFlags) 1048840, (CGKeyCode)8) != 0)
        {
            std::cout << "can not press command + c" << std::endl;
        }
        std::string newData = getClipboardText();
        std::cout << newData << std::endl;
        newData = removeNewLines(newData);
        std::cout << newData << std::endl;
        saveTextToClipboard(newData);
        return NULL;
    }
    else if (f == simpleCopyFlag)
    {
        CGEventSetFlags(event, (CGEventFlags)1048840);
        CGEventSetIntegerValueField(event, kCGKeyboardEventKeycode, (int64_t) 8);
    }

    // Set the modified keycode field in the event.
//    CGEventSetIntegerValueField(
//            event, kCGKeyboardEventKeycode, (int64_t) keycode);

    // We must return the event for it to be useful.
    return event;
}

void observeStopEvent(CFRunLoopObserverRef observer, CFRunLoopActivity activity, void *info) {
    EasyPaste *easyPaste = (EasyPaste *) info;
    if (easyPaste->stopFlag) {
        CFRunLoopStop(CFRunLoopGetCurrent());
    }
}

void monitorKeyBoard(EasyPaste *easyPaste) {
    // Create an event tap. Only interested in key presses down.
    CGEventMask eventMask = CGEventMaskBit(kCGEventKeyDown);
    CGEventTapOptions tapOptions = kCGEventTapOptionDefault;
    CFMachPortRef eventTap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, tapOptions, eventMask, filterKeyBoardEvent, easyPaste);
    if (!eventTap) {
        fprintf(stderr, "failed to create event tap\n");
        exit(1);
    }
    // Create a run loop observer
    CFRunLoopObserverContext *context = new CFRunLoopObserverContext();
    context->info = easyPaste;
    CFRunLoopObserverRef runLoopObserver = CFRunLoopObserverCreate(kCFAllocatorDefault, kCFRunLoopAllActivities, true, 0, observeStopEvent, context);
    // Create a run loop source.
    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    // Add source.
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopDefaultMode);
    // Add observer
    CFRunLoopAddObserver(CFRunLoopGetCurrent(), runLoopObserver, kCFRunLoopDefaultMode);
    // Enable the event tap.
    CGEventTapEnable(eventTap, true);
    // CFRunLoopRun();
    //CFRunLoopRunInMode(kCFRunLoopDefaultMode, 10, false);
    bool done = false;
    do
    {
        SInt32 result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 1, false);
        if ((result == kCFRunLoopRunStopped) || (result == kCFRunLoopRunFinished))
            done = true;
    }
    while(!done);
    CFRelease(eventTap);
    CFRelease(runLoopObserver);
    CFRelease(runLoopSource);
    delete context;
    std::cout << "finish releasing memory for monitorKeyBoard thread" << std::endl;
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
    return 0;
}

std::string getClipboardText() {
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

int saveTextToClipboard(std::string text) {
    std::string cmd = "echo '" + text + "' | pbcopy";
    char* charCmd = new char[cmd.size() + 1];
    std::strcpy(charCmd, cmd.c_str());
    int res = system(charCmd);
    // clear memory
    free(charCmd);
    return res;
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
    // to do for mac system
    return 0;
}



