#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdio.h>
#include <curses.h>

void sig_winch(int signo){
        struct winsize size;
        ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
        resizeterm(size.ws_row, size.ws_col);
}


int main(){
	WINDOW * wnd;
	WINDOW * subwnd;

	initscr();
	signal(SIGWINCH, sig_winch);
	curs_set(0);
	start_color();
	refresh();
	init_pair(1, COLOR_BLUE,COLOR_GREEN);
	init_pair(2, COLOR_YELLOW, COLOR_BLUE);
	wnd = newwin(5,18,2,4);
	wattron(wnd,COLOR_PAIR(1));
	box(wnd,'|' , '=' );
	subwnd = derwin(wnd, 3, 16, 1, 1);
	wbkgd(subwnd,COLOR_PAIR(2));
	wattron(subwnd, A_BOLD);
	wprintw(subwnd, "Hello, brave new curses world!\n");
	wrefresh(subwnd);
	wrefresh(wnd);
	delwin(subwnd);
	delwin(wnd);
	wmove(stdscr, 8,1);
	printw("Press any key...");
	refresh();
	getch();
	endwin();
	exit(EXIT_SUCCESS);
}
