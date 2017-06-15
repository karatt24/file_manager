#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <dirent.h>

void sig_winch(int signo){
        struct winsize size;
        ioctl(fileno(stdout),TIOCGWINSZ,(char*)&size);
        resizeterm(size.ws_row,size.ws_col);
}



int main(){

	WINDOW *wnd1,*wnd2;
	DIR *dp;
	dp=opendir(".");
	initscr();
	signal(SIGWINCH, sig_winch);
	curs_set(TRUE);
	start_color();
        refresh();
        init_pair(1, COLOR_YELLOW, COLOR_BLUE);
        wnd1 = newwin(36,70,1,1);
	wnd2 = newwin(36,70,1,72);
	box(wnd1,'|','-');
	box(wnd2,'|','-');
	wattron(wnd1, COLOR_PAIR(1));
	wattron(wnd2, COLOR_PAIR(1));
	wrefresh(wnd1);
	wrefresh(wnd2);
	getch();

	endwin();

}












