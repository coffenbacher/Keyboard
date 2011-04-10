#include "X11/extensions/XTest.h"

#define KEY_DOWN True
#define KEY_UP   False

#define KEYCODE_ESC 9

int main() {
  Display *dpy = XOpenDisplay(NULL);
  if (!dpy) return 1;
  XTestFakeKeyEvent(dpy, KEYCODE_ESC, KEY_DOWN, CurrentTime);
  XTestFakeKeyEvent(dpy, KEYCODE_ESC, KEY_UP, CurrentTime);
  XCloseDisplay(dpy);
  return 0;
}

