#ifndef __WIN32__

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static volatile sig_atomic_t    sigflag;

/* set nonzero by signal handler */
static sigset_t                 newmask, oldmask, zeromask;

static void sig_usr(int signo)  /* one signal handler for SIGUSR1 and SIGUSR2 */
{
	sigflag = 1;
	return;
}

void TELL_WAIT()
{
	if (signal(SIGUSR1, sig_usr) == SIG_ERR)
	{
		fprintf(stderr, "signal(SIGINT) error\n");
		exit(1);
	}

	if (signal(SIGUSR2, sig_usr) == SIG_ERR)
	{
		fprintf(stderr, "signal(SIGQUIT) error\n");
		exit(1);
	}

	sigemptyset(&zeromask);

	sigemptyset(&newmask);
	sigaddset(&newmask, SIGUSR1);
	sigaddset(&newmask, SIGUSR2);

	/* block SIGUSR1 and SIGUSR2, and save current signal mask */
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
	{
		fprintf(stderr, "SIG_BLOCK error\n");
		exit(1);
	}
}

void TELL_PARENT(pid_t pid)
{
	kill(pid, SIGUSR2);         /* tell parent we're done */
}

void WAIT_PARENT(void)
{
	while (sigflag == 0)
		sigsuspend(&zeromask);  /* and wait for parent */

	sigflag = 0;

	/* reset signal mask to original value */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
	{
		fprintf(stderr, "SIG_SETMASK error\n");
		exit(1);
	}
}

void TELL_CHILD(pid_t pid)
{
	kill(pid, SIGUSR1);                 /* tell child we're done */
}

void WAIT_CHILD(void)
{
	while (sigflag == 0)
		sigsuspend(&zeromask);  /* and wait for child */

	sigflag = 0;

	/* reset signal mask to original value */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
	{
		fprintf(stderr, "SIG_SETMASK error\n");
		exit(1);
	}
}

#endif
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
