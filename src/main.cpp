#include <cstdio>
#include <cstring>
#include <iostream>
#include "utils.h"
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>


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
#elif TRAY_WINAPI
#define TRAY_ICON1 "../sources/icon.ico"
#define TRAY_ICON2 "../sources/icon.ico"
#endif

extern struct tray tray;
std::mutex main_easyPaste_mutex;
std::condition_variable main_easyPaste_condition;
bool easyPasteOn = true;
bool stop_flag = false;

static void toggle_cb(struct tray_menu *item)
{
  std::unique_lock<std::mutex> ul(main_easyPaste_mutex);
  printf("toggle cb\n");
  item->checked = !item->checked;
  easyPasteOn = item->checked;
  tray_update(&tray);
  if (easyPasteOn) 
  {
    main_easyPaste_condition.notify_one();
  }
}

static void quit_cb(struct tray_menu *item)
{
  (void)item;
  printf("quit cb\n");
  stop_flag = true;
  tray_exit();
  main_easyPaste_condition.notify_all();
}

static void submenu_cb(struct tray_menu *item)
{
  (void)item;
  printf("submenu: clicked on %s\n", item->text);
  tray_update(&tray);
}

void easyPaste_thread()
{
  std::string oldData = "init";
  std::string newData = "init";
  while (!stop_flag)
  {
    // std::cout << stop_flag << std::endl;
    std::unique_lock<std::mutex> ul(main_easyPaste_mutex);
    while (!easyPasteOn) {
      main_easyPaste_condition.wait(ul);
      if (stop_flag) 
      {
        return;
      }
    }
    ul.unlock();
    newData = getClipboardText();
    if (newData.empty())
    {
      oldData = newData;
    }
    if (oldData == newData)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    else
    {
      newData = removeNewLines(newData);
      saveTextToClipboard(newData);
      oldData = newData;
      std::cout << newData << std::endl;
    }
  }
  return;
}

// Test tray init
struct tray tray = {
    .icon = TRAY_ICON1,
#if TRAY_WINAPI
    .tooltip = "EasyPaste",
#endif
    .menu =
        (struct tray_menu[]){
            {.text = "easyPaste on", .checked = 1, .checkbox = 1, .cb = toggle_cb},
            {.text = "-"},
            {.text = "SubMenu",
             .disabled = 1,
             .submenu =
                 (struct tray_menu[]){
                     {.text = "FIRST", .checked = 1, .checkbox = 1, .cb = submenu_cb},
                     {.text = "SECOND",
                      .submenu =
                          (struct tray_menu[]){
                              {.text = "THIRD",
                               .submenu =
                                   (struct tray_menu[]){
                                       {.text = "7", .cb = submenu_cb},
                                       {.text = "-"},
                                       {.text = "8", .cb = submenu_cb},
                                       {.text = NULL}}},
                              {.text = "FOUR",
                               .submenu =
                                   (struct tray_menu[]){
                                       {.text = "5", .cb = submenu_cb},
                                       {.text = "6", .cb = submenu_cb},
                                       {.text = NULL}}},
                              {.text = NULL}}},
                     {.text = NULL}}},
            {.text = "-"},
            {.text = "Quit", .cb = quit_cb},
            {.text = NULL}},
};

int main()
{
  hide_console();
  if (tray_init(&tray) < 0)
  {
    printf("failed to create tray\n");
    return 1;
  }
  std::thread easyPasteThread(easyPaste_thread);
  while (tray_loop(0) == 0)
  {
    
  }
  easyPasteThread.join();
  return 0;
}
