#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

Display *dis;
int screen;
Window win;
GC gc;

XEvent event;
KeySym key;
char text[255];

void init_x() {
	dis=XOpenDisplay((char *)0);
	screen=DefaultScreen(dis);

        unsigned long HEX(char v[8]) { // Even though both HEX and RGB don't get used HERE specifically and 
				       // we can't call this in any other functions, we can still include it, just in case
        int r, g, b;
	r = 16*(v[1] - 48 - (((v[1] - 48)>>4)*7)) + v[2] - 48 - (((v[2] - 48)>>4)*7);
	g = 16*(v[3] - 48 - (((v[3] - 48)>>4)*7)) + v[4] - 48 - (((v[4] - 48)>>4)*7);
	b = 16*(v[5] - 48 - (((v[5] - 48)>>4)*7)) + v[6] - 48 - (((v[6] - 48)>>4)*7);
	return b + (g<<8) + (r<<16);
	}

        unsigned long RGB(int r, int g, int b) {
	return b + (g<<8) + (r<<16);
	}

	int depth;
	unsigned int class;
	Visual *visual;
	unsigned long valuemask;
        XSetWindowAttributes *attributes;

//    	win=XCreateSimpleWindow(dis,DefaultRootWindow(dis), 0, 0, 500, 500, 5, RGB(255,255,255), HEX("#000000"));
//      who creates a "simple window"? Just read some of the documentation. Or copy what I have...
        win=XCreateWindow(dis,DefaultRootWindow(dis), 0, 0, 500, 500, 5, CopyFromParent, CopyFromParent, CopyFromParent, 0, attributes);
	XSetStandardProperties(dis,win,"Conway\'s Game of Life","cgol",None,NULL,0,NULL);

	XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask);

        gc=XCreateGC(dis, win, 0,0);

        XSetBackground(dis,gc,RGB(0,0,0));
	XSetForeground(dis,gc,HEX("#000000"));

	XClearWindow(dis, win);
	XMapRaised(dis, win);
};

void close_x() { // This doesn't get used. Not yet anyhow. If you're readin this you can delete it..
        XFreeGC(dis, gc);
	XDestroyWindow(dis,win);
	XCloseDisplay(dis);
	exit(1);
};

void redraw() { // don't know Why exactly I'm calling a function FOR a single function, but hell let's include it.
XClearWindow(dis, win);
}

void *gull(void *vargp) {

	int con, off_x, off_y, menu_h;
	con = 1;
	off_x = 0;
	off_y = 0;
	menu_h = 20;

        unsigned long HEX(char v[8]) { // supplies long integer string from set hex value; This can also be accomplished by passing RGB()
        int r, g, b;
        r = 16*(v[1] - 48 - (((v[1] - 48)>>4)*7)) + v[2] - 48 - (((v[2] - 48)>>4)*7);
        g = 16*(v[3] - 48 - (((v[3] - 48)>>4)*7)) + v[4] - 48 - (((v[4] - 48)>>4)*7);
        b = 16*(v[5] - 48 - (((v[5] - 48)>>4)*7)) + v[6] - 48 - (((v[6] - 48)>>4)*7);
        return b + (g<<8) + (r<<16);
        }

        unsigned long LHEX(char v[8]) { // supplies long integer string from set hex value; This can also be accomplished by passing RGB()
        int r, g, b;
        r = 16*(v[1] - 48 - (((v[1] - 48)>>5)*39)) + v[2] - 48 - (((v[2] - 48)>>5)*39);
        g = 16*(v[3] - 48 - (((v[3] - 48)>>5)*39)) + v[4] - 48 - (((v[4] - 48)>>5)*39);
        b = 16*(v[5] - 48 - (((v[5] - 48)>>5)*39)) + v[6] - 48 - (((v[6] - 48)>>5)*39);
        return b + (g<<8) + (r<<16);
        }

        unsigned long RGB(int r, int g, int b) { // supplies long integer string from an RGB value - i.e. RGB(255,255,255) for white;
        return b + (g<<8) + (r<<16);
        };

	unsigned long def;
	def = HEX("#FFFFFF");

	char hexin[6];

        char suprand[2];
	FILE *ftp;
	ftp = fopen("/dev/urandom","rb");
        fgets(suprand, 2, ftp);

	int width, height;

	XSetForeground(dis,gc,HEX("#000000"));
	XFillRectangle(dis,win,gc,0,0,1000,1000);
	while(1) {
	int x, y;
	x = 0;
	y = 0;
	XNextEvent(dis, &event);
		if (event.type==ButtonPress) {
		width = event.xbutton.x;
		height = event.xbutton.y;
		break;
		};
	};
	XSetForeground(dis,gc,HEX("#FFFFFF"));
	XDrawRectangle(dis,win,gc,-1,-1,width+2,height-2);

        bool b[height+1][width+1]; // "main" grid
        bool c[height+1][width+1]; // temp
	bool d[19][19];

	int std_redraw() {
		 long int i, j;
                 for (i = 0; i < height-3; i++) {
                        for (j = 0; j < width+1; j++) {
				if (b[i][j]) {
                                XSetForeground(dis,gc,def);
                                XFillRectangle(dis,win,gc,j*con-off_x,
                                i*con-off_y,con,con); // set some pixels on the window!!
                                } else {
                                XSetForeground(dis,gc,HEX("#000000"));
                                XFillRectangle(dis,win,gc,j*con-off_x,
                                i*con-off_y,con,con); // set more pixels on the window!!
                                }
                        };
                        j = 0;
                 };
	};

	int draw_random(int startx, int starty, int endx, int endy) {
		long int i, j;
		for (i = starty; i < endy; i++) { // write first grid with random data -- This can easily be replaced with a design of your choice; 
                	                    // keep in mind i corresponds to y value not x: This may be confusing due just to the way arrays are constructed
                        	            // alternatively, you may swap the i and j values to remap the grid to make it more manageable / easier to comprehend
                                	    // just make sure if you do swap these values; you do so for all for loops/grid management.
			for (j = startx; j < endx; j++) {
			b[i][j] = (rand() - (int)suprand) % 2;
			};
			j = 0;
	       	};
	};

	int draw_d(int startx, int starty) {
		long int i, j;
                for (i = 0; i < 19; i++) {
                        for (j = 0; j < 19; j++) {
                        b[starty+i][startx+j] = d[i][j];
                        };
                        j = 0;
                };
	};

	int temp_grid_set() {	
                 long int i, j, med;
                 for (i = 0; i < height-3; i++) {
                        for (j = 0; j < width+1; j++) {
                        med = abs((b[i][j-1] + b[i-1][j-1] + b[i-1][j] + b[i-1][j+1] + b[i][j+1] + b[i+1][j+1] + b[i+1][j] + b[i+1][j-1] - 3) - (2*b[i][j]) + ((b[i][j-1] + b[i-1][j-1] + b[i-1][j] + b[i-1][j+1] + b[i][j+1] + b[i+1][j+1] + b[i+1][j] + b[i+1][j-1])*b[i][j]));

                        // if your new to cgol, you're probably wondering what the fuck the above statement is. Calm yourself; it's nothing too complex.
                        // Just a mathematical statement that circumvents some if logic you would otherwise be forced to use for taking into account the
                        // past cell's state and the number of surrounding cells.

                        c[i][j] = !((med + 4 + (b[i][j] * 2)) / (5 + (b[i][j] * 4)));    // build new grid + some leftover math I didn't feel like incorperating in the previous statement
                                                                                         // for brevity's sake
                                if (b[i][j]) {                                           // read old grid
                                XSetForeground(dis,gc,def);
                                XFillRectangle(dis,win,gc,j*con-off_x,
                                i*con-off_y,con,con); // set some pixels on the window!!
                                } else {
                                XSetForeground(dis,gc,HEX("#000000"));
                                XFillRectangle(dis,win,gc,j*con-off_x,
                                i*con-off_y,con,con); // set more pixels on the window!!
                                }
                        };
                        j = 0;
                 };
	};

	int replace() {
		long int i, j, med;
		for (i = 0; i < height-3; i++) { // write main grid with temp grid data, destroying the old main grid in the process
                        for (j = 0; j < width+1; j++) {
                        b[i][j] = c[i][j];
                        };
                        j = 0;
		};
	};

	std_redraw();

	int startx, starty, endx, endy, i, j;
	while(1) {
		XNextEvent(dis, &event);
		if (event.type==ButtonPress) {
		b[event.xbutton.y][event.xbutton.x] = 1;
		std_redraw();
		};
		if (event.type==KeyPress&&
		    XLookupString(&event.xkey,text,255,&key,0)==1) {
			if (text[0]=='r') {
				while(1) {
				XNextEvent(dis, &event);
					if (event.type==ButtonPress) {
					startx = event.xbutton.x;
					starty = event.xbutton.y;
					break;
					};
				};
				while(1) {
                                XNextEvent(dis, &event);
                                        if (event.type==ButtonPress) {
                                        endx = event.xbutton.x;
                                        endy = event.xbutton.y;
					break;
                                        };
                                };
				if (startx < endx&&starty < endy) {
				draw_random(startx, starty, endx, endy);
				std_redraw();
				} else {
				draw_random(endx, endy, startx, starty);
				std_redraw();
				};
			};
			if (text[0]=='c') {
                                while(1) {
                                XNextEvent(dis, &event);
                                        if (event.type==ButtonPress) {
                                        startx = event.xbutton.x;
                                        starty = event.xbutton.y;
                                        break;
                                        };
                                };
                                draw_d(startx, starty);
                                std_redraw();
			};
			if (text[0]=='p') {
				for (i = 0; i < 19; i++) {
					for (j = 0; j < 19; j++) {
						d[i][j] = 0;
					};
					j = 0;
				};
				i = 0;
			XSetForeground(dis,gc,HEX("#000000"));
			XFillRectangle(dis,win,gc,0,0,1000,1000);
			XSetForeground(dis,gc,HEX("#FFFFFF"));
			XDrawRectangle(dis,win,gc,-1,-1,381,381);
				while(1) {
				XNextEvent(dis, &event);
					if (event.type==ButtonPress) {
						if (event.xbutton.x/20>19||event.xbutton.y/20>19) {
						std_redraw();
						break;
						} else {
						d[event.xbutton.y/20][event.xbutton.x/20] = !d[event.xbutton.y/20][event.xbutton.x/20];
						XSetForeground(dis,gc,RGB(d[event.xbutton.y/20][event.xbutton.x/20]*255,d[event.xbutton.y/20][event.xbutton.x/20]*255,d[event.xbutton.y/20][event.xbutton.x/20]*255));
						XFillRectangle(dis,win,gc,event.xbutton.x/20*20,event.xbutton.y/20*20,20,20);
						};
					};
					if (event.type==KeyPress&&
					    XLookupString(&event.xkey,text,255,&key,0)==1) {
						if (text[0]=='q') {
						std_redraw();
						break;
						}
					};
				};
			};
			if (text[0]=='y') {
			break;
			};
                        if (text[0]=='q') {
                        pthread_exit(1);
                        };
		};
	};

	while(1) {

		temp_grid_set();
		replace();

		while (XPending(dis)!=0) {
		XNextEvent(dis, &event);
	                if (event.type==KeyPress&&
	                    XLookupString(&event.xkey,text,255,&key,0)==1) {
              	         	if (text[0]=='q') {
                	                pthread_exit(1);
        	                };
				if (text[0]=='h') {
				hexin[0] = '#';
				XNextEvent(dis, &event);
				XLookupString(&event.xkey,text,255,&key,0);
				hexin[1] = text[0];
				XNextEvent(dis, &event);
				XLookupString(&event.xkey,text,255,&key,0);
				hexin[2] = text[0];
				XNextEvent(dis, &event);
				XLookupString(&event.xkey,text,255,&key,0);
				hexin[3] = text[0];
				XNextEvent(dis, &event);
				XLookupString(&event.xkey,text,255,&key,0);
				hexin[4] = text[0];
				XNextEvent(dis, &event);
				XLookupString(&event.xkey,text,255,&key,0);
				hexin[5] = text[0];
				XNextEvent(dis, &event);
				XLookupString(&event.xkey,text,255,&key,0);
				hexin[6] = text[0];
				XNextEvent(dis, &event);
				XLookupString(&event.xkey,text,255,&key,0);
				if (text[0]=='y') {
				def = LHEX(hexin);
				};
				};
			};
		        if (event.type==ButtonPress) {
                           int acon;
			   acon = event.xbutton.button;
			   if (acon==1&&con!=10) {
			   con++;
			   XSetForeground(dis,gc,HEX("#000000"));
	               	   XFillRectangle(dis, win, gc, 0, 0, 500, 500);
			   off_x = off_x + event.xbutton.x;
			   off_y = off_y + event.xbutton.y;
			   };
			   if (acon==3&&con!=1) {
			   con = con - 1;
			   XSetForeground(dis,gc,HEX("#000000"));
	                   XFillRectangle(dis, win, gc, 0, 0, 1000, 1000);
			   off_x = off_x * !!(con-1) / 2;
			   off_y = off_y * !!(con-1) / 2;
			   };
			  if (acon==5&&con!=10) { // rigid zoom
                          con++;
                          XSetForeground(dis,gc,HEX("#000000"));
                          XFillRectangle(dis, win, gc, 0, 0, 1000, 1000);
                          off_x = 0;
                          off_y = 0;
                          };
			  if (acon==4&&con!=1) {
                          con = con - 1;
                          XSetForeground(dis,gc,HEX("#000000"));
                          XFillRectangle(dis, win, gc, 0, 0, 1000, 1000);
                          off_x = 0;
                          off_y = 0;
                          };

                        };

	       	};

	}; // do it all over again!

}

int main() {

init_x(); // create the window
Status XInitThreads(void);
pthread_t gul;
pthread_create(&gul, NULL, gull, NULL);
// gull(); // D o  Y o u  W a n t  T o  P l a y  A  G a m e . . . ?
pthread_join(gul, NULL);
// ignore all the posix thread messiness. I was trying to handle XEvents in a new thread...
close_x();
};

