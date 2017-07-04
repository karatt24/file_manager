#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <malloc.h>
#include <limits.h>

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


void screen(struct dirent **dir, int sh, WINDOW *wnd1, WINDOW *wnd2){
        int y,x,i;
	struct stat *buf;
        box(wnd1, 0, 0);
        box(wnd2, 0, 0);
        wmove(wnd1,1,1);
        for(i=0; i<sh; i++){
		if(dir[i]->d_type == DT_DIR){
			wattron(wnd1, COLOR_PAIR(2));
			wprintw(wnd1,"%s\n", dir[i]->d_name);
			wrefresh(wnd1);
			wattroff(wnd1, COLOR_PAIR(2));
			getyx(wnd1, y ,x);
			wmove(wnd2,y-1,x+1);
		}

		else{
                	wprintw(wnd1,"%s\n",dir[i]->d_name);
                	getyx(wnd1, y ,x);
                	wmove(wnd2,y-1,x+1);
		}
		if(dir[i]->d_type == DT_DIR){
                        wattron(wnd2, COLOR_PAIR(2));
                        wprintw(wnd2,"%s\n",dir[i]->d_name);
                        wrefresh(wnd2);
			wattroff(wnd2, COLOR_PAIR(2));
			getyx(wnd1, y ,x);
			wmove(wnd1, y,x+1);
                }
		else{
                	wprintw(wnd2,"%s\n",dir[i]->d_name);
			getyx(wnd1, y ,x);
                	wmove(wnd1, y,x+1);
		}
        }
        wrefresh(wnd1);
        wrefresh(wnd2);

}



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

		return y;
	}
}


int main(){

	WINDOW *win,*wnd1,*wnd2, *sub, *text;
	DIR *dp;
	struct  dirent **dir;
	int x,y, ch, sh=0,i, rc, q;
	char *str, **path, *buf, *op;
	struct stat stbuf;
	pid_t pid;
//	str=malloc(sizeof(char)*23);
	buf = malloc(PATH_MAX);
	op = malloc(PATH_MAX);
	path=malloc(sizeof(char*)*23);
	for(i=0; i<256;i++){
		path[i]=malloc(sizeof(char)*256);
	}
	dp=opendir(".");
	initscr();
	signal(SIGWINCH, sig_winch);
	curs_set(TRUE);
	start_color();
        refresh();
        init_pair(1, COLOR_YELLOW, COLOR_BLUE);
	init_pair(2, COLOR_RED, COLOR_GREEN);
        wnd1 = newwin(25,40,1,1);
	wnd2 = newwin(25,40,1,42);
	box(wnd1, 0, 0);
	box(wnd2, 0, 0);
	wbkgd(wnd1, COLOR_PAIR(1));
	wbkgd(wnd2, COLOR_PAIR(1));
	wrefresh(wnd1);
	wrefresh(wnd2);
	wmove(wnd1, 1,1);
	sh = scandir(get_current_dir_name(), &dir, 0, alphasort);
	screen(dir, sh, wnd1, wnd2);
	wmove(wnd1, 1, 1);
	wrefresh(wnd1);
	win=wnd1;
	keypad(win, TRUE);
	char *pwd;
	while(1){
		ch = wgetch(win);
		if (ch == 9){
	                if (win == wnd1){
	                        win=wnd2;
			/*	screen(dir, sh, wnd1, wnd2);*/
	                }
        	        else{
                	        win=wnd1;
			/*	screen(dir, sh, wnd1, wnd2);*/
                	}
			screen(dir, sh, wnd1, wnd2);
			wmove(win, 1,1);
                        keypad(win, TRUE);

		}

		if(ch == 10){
			getyx(win, y, x);
			y--;
			rc = stat(dir[y]->d_name, &stbuf);
			wprintw(win, "   %s \n", dir[y]->d_name);
			if(rc != 0){
				wprintw(win, "Erorr by calling stat()\n");
        			wrefresh(win);
			}
			if(S_ISDIR(stbuf.st_mode)){
		/*		getyx(win, y, x);
				y--;*/
				chdir(dir[y]->d_name);
				sh = scandir(get_current_dir_name(), &dir, 0, alphasort);
				wclear(wnd1);
				wclear(wnd2);
				screen(dir, sh, wnd1, wnd2);
				wmove(win, 1,1);
			}else{
				if(stbuf.st_mode & S_IXUSR){

					wclear(wnd1);
                                	wclear(wnd2);
					wclear(win);
					delwin(wnd1);
                                        delwin(wnd2);
                                        delwin(win);
					clear();
					endwin();

					op = "./";
					strcat(buf, op);
					strcat(buf, dir[y]->d_name);
					pid = fork();
					if(pid == 0){
						execlp(buf, buf, NULL, NULL);
					}else{
						wait();
						getchar();
					}
						initscr();
						wnd1 = newwin(25,40,1,1);
					        wnd2 = newwin(25,40,1,42);
						box(wnd1, 0, 0);
       						box(wnd2, 0, 0);
        					wbkgd(wnd1, COLOR_PAIR(1));
        					wbkgd(wnd2, COLOR_PAIR(1));
						win = wnd1;
						sh = scandir(get_current_dir_name(), &dir, 0, alphasort);
						screen(dir, sh, wnd1, wnd2);
        					wrefresh(wnd1);
        					wrefresh(wnd2);

				/*	}*/
				}
			}
		}
	/*	if (ch == KEY_F1){
			text = newwin(0, 0, 0, 0);
		}*/
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
