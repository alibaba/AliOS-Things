#ifndef _SIGNAL_H_
#define _SIGNAL_H_

typedef void (*SIG_FUNC)(int);

#define SIGHUP		 1
#define SIGINT		 2
#define SIGQUIT		 3
#define SIGILL		 4
#define SIGTRAP		 5
#define SIGABRT		 6
#define SIGEMT		 7
#define SIGFPE		 8
#define SIGKILL		 9
#define SIGBUS		10
#define SIGSEGV		11
#define SIGSYS		12
#define SIGPIPE		13
#define SIGALRM		14
#define SIGTERM		15
#define SIGURG		16
#define SIGSTOP		17
#define SIGTSTP		18
#define SIGCONT		19
#define SIGCHLD		20
#define SIGTTIN		21
#define SIGTTOU		22
#define SIGIO		23
#define SIGXCPU		24
#define SIGXFSZ		25
#define SIGVTALRM	26
#define SIGPROF		27
#define SIGWINCH	28
#define SIGINFO		29
#define SIGUSR1		30
#define SIGUSR2		31

#define SIGSCAN      32
#define SIGASSOC     33
#define SIGDISASSOC  34
#define SIGDEAUTH    35
#define SIGCSA       36

#define SIGPOLL	    SIGIO
#define SIGPWR	    SIGINFO
#define SIGIOT	    SIGABRT

extern void bk_signal(int sig_num, SIG_FUNC func);
unsigned int bk_alarm(unsigned int seconds);

#endif
// eof

