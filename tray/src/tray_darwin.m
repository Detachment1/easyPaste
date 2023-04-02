#include <Cocoa/Cocoa.h>
#include <string.h>
#include "tray.h"


@interface AppDelegate: NSObject <NSApplicationDelegate>
    - (IBAction)menuCallback:(id)sender;
@end
@implementation AppDelegate{}
    - (IBAction)menuCallback:(id)sender
    {
        struct tray_menu *m = [[sender representedObject] pointerValue];
        if (m != NULL && m->cb != NULL) {
            m->cb(m);
        }
    }
@end

static NSApplication* app;
static NSStatusBar* statusBar;
static NSStatusItem* statusItem;

static NSMenu* _tray_menu(struct tray_menu *m) {
    NSMenu* menu = [[NSMenu alloc] init];
    [menu setAutoenablesItems:FALSE];

    for (; m != NULL && m->text != NULL; m++) {
        if (strcmp(m->text, "-") == 0) {
            [menu addItem:[NSMenuItem separatorItem]];
        } else {
            NSMenuItem* menuItem = [[NSMenuItem alloc]
                initWithTitle:[NSString stringWithUTF8String:m->text]
                action:@selector(menuCallback:)
                keyEquivalent:@""];
            [menuItem setEnabled:(m->disabled ? FALSE : TRUE)];
            [menuItem setState:(m->checked ? 1 : 0)];
            [menuItem setRepresentedObject:[NSValue valueWithPointer:m]];
            [menu addItem:menuItem];
            if (m->submenu != NULL) {
                [menu setSubmenu:_tray_menu(m->submenu) forItem:menuItem];
            }
        }
    }
    return menu;
}

int tray_init(struct tray *tray) {
    AppDelegate *delegate = [[AppDelegate alloc] init];
    app = [NSApplication sharedApplication];
    [app setDelegate:delegate];
    statusBar = [NSStatusBar systemStatusBar];
    statusItem = [statusBar statusItemWithLength:NSVariableStatusItemLength];
    tray_update(tray);
    [app activateIgnoringOtherApps:TRUE];
    return 0;
}

int tray_loop(int blocking) {
    NSDate* until = (blocking ? [NSDate distantFuture] : [NSDate distantPast]);
    NSEvent* event = [app nextEventMatchingMask:ULONG_MAX untilDate:until
        inMode:[NSString stringWithUTF8String:"kCFRunLoopDefaultMode"] dequeue:TRUE];
    if (event) {
        [app sendEvent:event];
    }
    return 0;
}

void tray_update(struct tray *tray) {
    NSImage *image = [[NSImage alloc] initWithContentsOfFile:[NSString stringWithUTF8String:tray->icon]];
    NSSize size = NSMakeSize(16, 16);
    [image setSize:NSMakeSize(16, 16)];
    statusItem.button.image = image;
    [statusItem setMenu:_tray_menu(tray->menu)];
}

void tray_exit(void) {
    [app terminate:app];
}

