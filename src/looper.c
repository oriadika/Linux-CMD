#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>


void handler(int sig)
{
    printf("\nReceived Signal: %s\n", strsignal(sig));

    if (sig == SIGTSTP)
    {
        // Forward signal to the default handler
        signal(SIGTSTP, SIG_DFL);
        raise(SIGTSTP);

        // Reinstate the custom handler for SIGCONT
        signal(SIGCONT, handler);
    }
    else if (sig == SIGCONT)
    {
        // Forward signal to the default handler
        signal(SIGCONT, SIG_DFL);
        raise(SIGCONT);

        // Reinstate the custom handler for SIGTSTP
        signal(SIGTSTP, handler);
    }
    else if (sig == SIGINT)
    {
        // Forward signal to the default handler
        signal(SIGINT, SIG_DFL);
        raise(SIGINT);
    }
}


int main(int argc, char **argv)
{
	printf("Starting the program\n");
	signal(SIGINT, handler);
	signal(SIGTSTP, handler);
	signal(SIGCONT, handler);

	while (1)
		sleep(1);
	

	return 0;
}