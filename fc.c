#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <malloc.h>

void sig_winch(int signo){
        struct winsize size;
        ioctl(fileno(stdout),TIOCGWINSZ,(char*)&size);
        resizeterm(size.ws_row,size.ws_col);
}

char* redact(char *path, char *buf){
        int i=0;
        path[0]='/';
        while(i != 23){
                i++;
                path[i]=buf[i-1];
        }
        return path;
}

int fill(DIR *dp, char** path){

	int i=0;
	struct  dirent *dir;
        char *buf;
        while((dir=readdir(dp)) != NULL){
                if(i > 22){
                        path=realloc(path, sizeof(char*));
                        path[i]=malloc(sizeof(char)*256);
                }

                if (dir->d_type == DT_DIR){
                        //path[0]="/";
                        buf=dir->d_name;
                        path[i]=redact(path[i], buf);
                }

                else {
			path[i]=dir->d_name;
                }
                i++;
        }

        return i;


}

void screen(char **path,int sh, WINDOW *wnd1, WINDOW *wnd2){
        int y,x,i;
        wclear(wnd1);
        wclear(wnd2);
        box(wnd1, 0, 0);
        box(wnd2, 0, 0);
        wmove(wnd1,1,1);
        for(i=0; i<sh; i++){
                wprintw(wnd1,"%s\n",path[i]);
                getyx(wnd1,y,x);
                wmove(wnd2,y-1,x+1);
                wprintw(wnd2,"%s\n",path[i]);
                wmove(wnd1, y,x+1);
                i++;
        }
        wrefresh(wnd1);
        wrefresh(wnd2);
//        closedir(dpp);
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
int key_move(WINDOW *wnd, int ch, int sh){
	int x,y,i;
	char *str, *path;
	str=malloc(sizeof(char)*23);
	path=malloc(sizeof(char)*23);
	switch(ch){
		case KEY_UP:
			getyx(wnd, y, x);
			if (y > 1){
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
/*		case 10:
			getyx(wnd, y,x);
			winnstr(wnd,str, -1);
			if (str[0] == '/'){
				wprintw(wnd, "It's not catalog!");
				break;
			}
			for(i=0; i<strlen(str); i++){
				if(str[i] == ' '){
                                        path[i]='\0';
                                        break;
                                }
                                path[i]=str[i+1];
			}
			chdir(path);
			break;*/
		return y;
	}
}


int main(){

	WINDOW *win,*wnd1,*wnd2;
	DIR *dp;
	struct  dirent *dir;
	int x,y, ch, sh=0,i;
	char *str, **path;
//	str=malloc(sizeof(char)*23);
	path=malloc(sizeof(char*)*23);
	for(i=0; i<23;i++){
		path[i]=malloc(sizeof(char)*256);
	}
	dp=opendir(".");
	initscr();
	signal(SIGWINCH, sig_winch);
	curs_set(TRUE);
	start_color();
        refresh();
        init_pair(1, COLOR_YELLOW, COLOR_BLUE);
        wnd1 = newwin(25,40,1,1);
	wnd2 = newwin(25,40,1,42);
	box(wnd1, 0, 0);
	box(wnd2, 0, 0);
	wbkgd(wnd1, COLOR_PAIR(1));
	wbkgd(wnd2, COLOR_PAIR(1));
	wrefresh(wnd1);
	wrefresh(wnd2);
	wmove(wnd1, 1,1);
	sh=fill(path, dp);
	sh=screen(path, dp, wnd1, wnd2);
	wmove(wnd1, 1, 1);
	wrefresh(wnd1);
	win=wnd1;
	keypad(win, TRUE);
	while(1){
		ch = wgetch(win);
		if (ch == 9)
	                if (win == wnd1){
	                        win=wnd2;
				sh=screen(path, dp, wnd1, wnd2);
	                        wmove(win, 1,1);
				keypad(win, TRUE);
	                }
        	        else{
                	        win=wnd1;
				sh=screen(path, dp, wnd1, wnd2);
                	        wmove(win, 1,1);
				keypad(win, TRUE);
                	}

		if(ch == 10){
//			getyx(win, y,x);
                /*	winnstr(win, str, -1);
                	if (str[0] != '/'){
		//		getyx(win, y, x);
                        	wprintw(win, "It's not catalog!\n");
                	}
			else{
		//		getyx(win, y, x);
				wprintw(win, "It's a catalog!\n");
			}
                	for(i=0; i<strlen(str); i++){
                                if(str[i] == ' '){
                                        path[i]='\0';
                                }
                                path[i]=str[i+1];
                        }*/
//                        chdir(path);
		}
	/*	if (ch == 10){
			getyx(win, y,x);
                        mvwinstr(win,y,x,str);
			wmove(win, y+1, x);
			wprintw(win," %s", str);
			wmove(win, y, x+1);
		}
*/
		y=key_move(win, ch, sh);
		wrefresh(win);
		if (ch == 27){
			delwin(win);
			delwin(wnd1);
			delwin(wnd2);
			endwin();
			exit(EXIT_SUCCESS);
		}
	}


	endwin();

}
