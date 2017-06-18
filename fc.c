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

int screen(DIR *dp, WINDOW *wnd1, WINDOW *wnd2){
        int y,x,i=0;
        DIR *dpp=opendir(".");
        struct  dirent *dir;
        wclear(wnd1);
        wclear(wnd2);
        box(wnd1,'|','-');
        box(wnd2,'|','-');
        wmove(wnd1,1,1);
        while((dir=readdir(dpp)) != NULL){
                wprintw(wnd1,"%s\n",dir->d_name);
                getyx(wnd1,y,x);
                wmove(wnd2,y-1,x+1);
                wprintw(wnd2,"%s\n",dir->d_name);
                wmove(wnd1, y,x+1);
                i++;
        }
        wrefresh(wnd1);
        wrefresh(wnd2);
        closedir(dpp);
        return i;
}
/*
int key_tab(DIR *dp, WINDOW *win,WINDOW *wnd1, WINDOW *wnd2, int ch){
	int sh;
	if (ch == 9)
                if (win == wnd1){
                        win=wnd2;
                        sh=screen(dp, wnd1, wnd2);
                        wmove(win, 1,1);
                        keypad(win, TRUE);
                }
                else{
                        win=wnd1;
                        sh=screen(dp, wnd1, wnd2);
                        wmove(win, 1,1);
                        keypad(win, TRUE);
                }
	return sh;
}
*/
void key_move(WINDOW *wnd, int ch, int sh){
	int x,y;
	switch(ch){
		case KEY_UP:
			getyx(wnd, y, x);
			if (y > 0){
				y--;
				wmove(wnd, y, x);
			}
			break;
		case KEY_DOWN:
			getyx(wnd, y, x);
			if(y < sh){
				y++;
				wmove(wnd, y, x);
			}
			break;
		case 10:
			getyx(wnd, y,x);
			wprintw(wnd, "WAAAAW");
			break;
	}
}


int main(){

	WINDOW *win,*wnd1,*wnd2;
	DIR *dp;
	struct  dirent *dir;
	int x,y, ch, sh=0;
	dp=opendir(".");
	initscr();
	signal(SIGWINCH, sig_winch);
	curs_set(TRUE);
	start_color();
        refresh();
        init_pair(1, COLOR_YELLOW, COLOR_BLUE);
        wnd1 = newwin(25,40,1,1);
	wnd2 = newwin(25,40,1,42);
	box(wnd1,'|','-');
	box(wnd2,'|','-');
	wbkgd(wnd1, COLOR_PAIR(1));
	wbkgd(wnd2, COLOR_PAIR(1));
	wrefresh(wnd1);
	wrefresh(wnd2);
	wmove(wnd1, 1,1);
/*	while((dir=readdir(dp)) != NULL){
		wprintw(wnd1,"%s\n",dir->d_name);
		getyx(wnd1,y,x);
		wmove(wnd2,y-1,x+1);
		wprintw(wnd2,"%s\n",dir->d_name);
                wmove(wnd1, y,x+1);
		sh++;
	}*/
//	wrefresh(wnd2);

	sh=screen(dp, wnd1, wnd2);
	wmove(wnd1, 1, 1);
	wrefresh(wnd1);
	win=wnd1;
	keypad(win, TRUE);
	while(1){

		ch = wgetch(win);
//		sh=key_tab(dp, win, wnd1, wnd2, ch);
		if (ch == 9)
	                if (win == wnd1){
	                        win=wnd2;
				sh=screen(dp, wnd1, wnd2);
	                        wmove(win, 1,1);
				keypad(win, TRUE);
	                }
        	        else{
                	        win=wnd1;
				sh=screen(dp, wnd1, wnd2);
                	        wmove(win, 1,1);
				keypad(win, TRUE);
                	}

		key_move(win, ch, sh);
		wrefresh(win);
		if (ch == 27){
//			system("clear");
			delwin(wnd1);
			delwin(wnd2);
			endwin();
			exit(EXIT_SUCCESS);
		}
	}

//	getch();
	system("clear");
	endwin();

}












