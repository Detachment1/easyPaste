#include <cstdio>
#include <cstring>
#include <iostream>
#include "utils.h"
#include <thread>


#if defined(_WIN32) || defined(_WIN64)
#define TRAY_WINAPI 1
#elif defined(__linux__) || defined(linux) || defined(__linux)
#define TRAY_APPINDICATOR 1
#elif defined(__APPLE__) || defined(__MACH__)
#define TRAY_APPKIT 1
#endif

#include "tray.h"

#if TRAY_APPINDICATOR
#define TRAY_ICON1 "indicator-messages"
#define TRAY_ICON2 "indicator-messages-new"
#elif TRAY_APPKIT
#define TRAY_ICON1 "../sources/icon.png"
#define TRAY_ICON2 "../sources/icon.png"
#define COPY_TYPE_0_TEXT "remove new lines by command + c"
#define COPY_TYPE_1_TEXT "remove new lines by ctrl + command + c"
#elif TRAY_WINAPI
#define TRAY_ICON1 "../sources/icon.ico"
#define TRAY_ICON2 "../sources/icon.ico"
#define COPY_TYPE_0_TEXT "remove new lines by ctrl + c"
#define COPY_TYPE_1_TEXT "remove new lines by ctrl + alt + c"
#endif

extern struct tray tray;
EasyPaste* easyPaste = new EasyPaste(0, 5);

static void toggle_cb(struct tray_menu *item) {
    (void)item;
    printf("change copy type\n");
    if (easyPaste->copyType == 0)
    {
        easyPaste->copyType = 1;
        item->text = COPY_TYPE_1_TEXT;
    }
    else
    {
        easyPaste->copyType = 0;
        item->text = COPY_TYPE_0_TEXT;
    }
    tray_update(&tray);
}

static void quit_cb(struct tray_menu *item) {
    (void) item;
    printf("quit\n");
    easyPaste->stopLoopThread();
}

static void submenu_cb(struct tray_menu *item) {
    (void) item;
    printf("submenu: clicked on %s\n", item->text);
    tray_update(&tray);
}

struct tray tray = {
        .icon = TRAY_ICON1,
#if TRAY_WINAPI
        .tooltip = "EasyPaste",
#endif
        .menu =
        (struct tray_menu[]) {
                {.text = COPY_TYPE_0_TEXT, .cb = toggle_cb},//.checked = 1, .checkbox = 1,
                {.text = "-"},
                {.text = "SubMenu",
                        .disabled = 1,
                        .submenu =
                        (struct tray_menu[]) {
                                {.text = "FIRST", .checked = 1, .checkbox = 1, .cb = submenu_cb},
                                {.text = "SECOND",
                                        .submenu =
                                        (struct tray_menu[]) {
                                                {.text = "THIRD",
                                                        .submenu =
                                                        (struct tray_menu[]) {
                                                                {.text = "7", .cb = submenu_cb},
                                                                {.text = "-"},
                                                                {.text = "8", .cb = submenu_cb},
                                                                {.text = NULL}}},
                                                {.text = "FOUR",
                                                        .submenu =
                                                        (struct tray_menu[]) {
                                                                {.text = "5", .cb = submenu_cb},
                                                                {.text = "6", .cb = submenu_cb},
                                                                {.text = NULL}}},
                                                {.text = NULL}}},
                                {.text = NULL}}},
                {.text = "-"},
                {.text = "Quit", .cb = quit_cb},
                {.text = NULL}},
};


int main() {
    if (tray_init(&tray) < 0) {
        printf("failed to create tray\n");
        return 1;
    }
    std::thread easyPasteThread(&EasyPaste::startLoopThread, easyPaste);
    while (tray_loop(0) == 0) {
        if (easyPaste->stopFlag)
        {
            break;
        }
    }
    easyPasteThread.join();
    delete easyPaste;
    tray_exit();
    return 0;
}
