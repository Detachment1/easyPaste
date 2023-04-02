#ifndef TRAY_H
#define TRAY_H

#ifdef __cplusplus
extern "C"
{
#endif

struct tray_menu;

struct tray {
  const char *icon;
  char *tooltip;
  struct tray_menu *menu;
};

struct tray_menu {
  const char *text;
  int disabled;
  int checked;
  int checkbox;

  void (*cb)(struct tray_menu *);
  void *context;

  struct tray_menu *submenu;
};

int tray_init(struct tray *tray);

int tray_loop(int blocking);

void tray_update(struct tray *tray);

void tray_exit(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* TRAY_H */
