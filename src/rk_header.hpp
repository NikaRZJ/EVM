#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fstream>
#include <iomanip>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#ifndef rk_header_hpp_
#define rk_header_hpp_

struct termios stored_settings;

enum keys { // strucktura s klu4ami dlya deistvii
	key_num = 0,
	key_up = 10,
	key_down,
	key_right,
	key_left,
	key_f6,
	key_load,
	key_save,
	key_reset,
	key_tt,
	key_i,
	key_other,
	key_q,
	key_plus,
	key_minus,
	key_run,
	key_step,
	key_f5,  //accum
	key_f4, // instructionCounter
};

int rk_readkey(keys *key);
int rk_mytermsave (void);
int rk_mytermrestore (void);
int rk_mytermregime (int regime, int vtime, int vmin, int echo, int sigint);

int rk_readkey(enum keys *key) // perevod knopki v klu4
{

	rk_mytermregime(1, 0, 1, 1, 1);
	char buf[8] = { 0 };
	read(STDIN_FILENO, buf, 8);
	if (strcmp(buf, "\E[A") == 0)
		*key = key_up;
	else if (strcmp(buf, "\E[B") == 0)
		*key = key_down;
	else if (strcmp(buf, "\E[C") == 0)
		*key = key_right;
	else if (strcmp(buf, "\E[D") == 0)
		*key = key_left;
	else if (strcmp(buf, "\E[OS") == 0)
		*key = key_f4;
	else if (strcmp(buf, "\E[15~") == 0)
		*key = key_f5;
	else if (strcmp(buf, "\E[17~") == 0)
		*key = key_f6;
	else if (buf[0] == 'l')
		*key = key_load;
	else if (buf[0] == 's')
		*key = key_save;
	else if (buf[0] == 'r')
		*key = key_reset;
	else if (buf[0] == '=' or buf[0] == '+')
		*key = key_plus;
	else if (buf[0] == '-' or buf[0] == '_')
		*key = key_minus;
	else if (buf[0] == 'g')
		*key = key_run;
	else if (buf[0] == 't')
		*key = key_step;
	else if (buf[0] == 'q')
		*key = key_q;
	else
		*key = key_other;
	rk_mytermrestore();
	return 0;
}

int rk_mytermsave(void) // sohranenie sostoyaniya kanoni4eskogo rejima
{
	if (tcgetattr(0, &stored_settings)) {
		return 1;
	}

	return 0;
}

int rk_mytermrestore(void) // vosstanovlenie kanon rejima
{
	if (tcsetattr(0, TCSANOW, &stored_settings)){
		return 1;
	}

	return 0;
}

int rk_mytermregime(int regime, int vtime, int vmin, int echo, int sigint) // perehod v nekanon rejim
{
	struct termios new_settings;
	rk_mytermsave();
	new_settings = stored_settings;

	if (regime == 1) {
		new_settings.c_lflag &= (~ICANON);

		if (sigint == 1) {
			new_settings.c_lflag &= (~ISIG);
		} else if (sigint == 0) {
			new_settings.c_lflag |= ISIG;
		}

		if (echo == 1) {
			new_settings.c_lflag &= (~ECHO);
		} else if (echo == 0) {
			new_settings.c_lflag |= ECHO;
		}

		new_settings.c_cc[VMIN] = vmin;
		new_settings.c_cc[VTIME] = vtime;
	} else if (regime == 0) {
		new_settings.c_lflag |= ICANON;
	}

	tcsetattr(0, TCSANOW, &new_settings);
	return 0;
}

#endif
