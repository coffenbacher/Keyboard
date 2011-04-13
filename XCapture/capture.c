#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
   Display *dpy; 
   int quit = 0; 
   char *s; 
   int kc;

   if((dpy = XOpenDisplay(NULL)) == NULL) { 
      perror(argv[0]); 
      exit(1); 
   } 

   XGrabKeyboard(dpy, DefaultRootWindow(dpy), 
                 False, GrabModeAsync, GrabModeAsync, CurrentTime); 

   while(!quit) { 
      XEvent ev; 

      XNextEvent(dpy, &ev); 

      switch (ev.type) { 
      case KeyPress: 

        kc = ((XKeyPressedEvent*)&ev)->keycode; 
	printf("\n%x\n", kc);
        s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0)); 

        if(s) printf("KEY: %s\n", s); 
        if(!strcmp(s, "q")) quit=1;  
	break;

     case KeyRelease: 

        kc = ((XKeyPressedEvent*)&ev)->keycode; 
        s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0)); 

        if(s) printf("KEY released: %s\n", s); 
        if(!strcmp(s, "q")) quit=1;  
     }
 
   } 

   XUngrabKeyboard(dpy, CurrentTime); 

   if (XCloseDisplay(dpy)) { 
      perror(argv[0]); 
      exit(1); 
   } 

   return 0; 
} 

