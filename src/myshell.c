#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include <sys/wait.h>

/*
 * int fill_pipe(char ***pt_cmd1, char ***pt_cmd2, int count):
 *      This function returns 1 if there is a pipe symbol or zero otherwise.
 *      In addition, If a pipe symbol is present, it fills the given pt_cmd1 and pt_cmd2 with the two
 *      different commands on the sides of the pipe.
 */
int fill_pipe(char ***pt_cmd1, char ***pt_cmd2, int count) {
    while (count-- > 0 && strcmp(((*pt_cmd2)++)[0], "|") != 0);
    if (count > 0)
        ((*pt_cmd2) - 1)[0] = NULL;
    return count > 0 ? 1 : 0;
}

/*
 * static void handle_sigchld(int sig, siginfo_t *siginfo, void *context):
 *      Signal handler for SIGCHLD. It only waits for the calling child in order to
 *      delete it from the process table.
 */
static void handle_sigchld(int sig, siginfo_t *siginfo, void *context) {
    waitpid(siginfo->si_pid, NULL, 0);
}

/*
 * static void handle_sigint(int sig, siginfo_t *siginfo, void *context):
 *      Signal handler for SIGINT, essentialy it is an empty handler because we want the father to not close and
 *      its children to kill themselves.
 */
static void handle_sigint(int sig, siginfo_t *siginfo, void *context) {
    return;
}

/*
 * int print_and_return(const char *to_print, const int to_return, int is_exit):
 *      A function to make my code shorter... Instead of printing and returning/exiting, this function
 *      does them both with the given parameters.
 */
int print_and_return(const char *to_print, const int to_return, int is_exit) {
    fprintf(stderr,"%s", to_print);
    if (is_exit)
        exit(is_exit);
    return to_return;
}

/*
 * int process_arglist(int count, char **arglist):
 *      This function does exactly as specified in the instructions. It doesn't allocate any memory so every
 *      exit is clean.
 *      The parent performs both forks, so it is easier with a command with the pipe symbol.
 */
int process_arglist(int count, char **arglist) {
    char **cmd1 = arglist, **cmd2 = arglist;
    int is_background = !strcmp(arglist[count - 1], "&"), is_pipe = fill_pipe(&cmd1, &cmd2,
                                                                              count), pid_pipe[2], cmd_pipe[2];
    pid_t fork_pid, child_pid;
    if (is_background)
        arglist[count - 1] = NULL;

    if (pipe(pid_pipe) < 0)
        return print_and_return("Error: pipe creation failed.\n", 0, 0);
    if (pipe(cmd_pipe) < 0)
        return print_and_return("Error: pipe creation failed.\n", 0, 0);

    if ((fork_pid = fork()) < 0)
        return print_and_return("Error: fork() failed.\n", 0, 0);
    else if (fork_pid == 0) {
        child_pid = getpid();
        close(pid_pipe[0]);
        if (write(pid_pipe[1], &child_pid, sizeof(child_pid)) < 0)
            return print_and_return("Error: writing to pipe failed.\n", 0, 1);
        close(pid_pipe[1]);
        if (is_background)
            signal(SIGINT, SIG_IGN);
        if (!is_pipe) {
            if (execvp(arglist[0], arglist) < 0)
                return print_and_return("Error: command execution failed.\n", 0, 1);
        } else {
            dup2(cmd_pipe[1], STDOUT_FILENO);
            close(cmd_pipe[0]);
            if (execvp(cmd1[0], cmd1) < 0)
                return print_and_return("Error: left command execution failed.\n", 0, 1);
            close(cmd_pipe[1]);
        }
        exit(0);
    } else {
        if (!is_pipe)
            close(pid_pipe[1]);
        if (read(pid_pipe[0], &child_pid, sizeof(child_pid)) < 0)
            return print_and_return("Error: reading from pipe failed.\n", 0,0);
        if (!is_pipe)
            close(pid_pipe[0]);
        if (!is_background && !is_pipe)
            if (waitpid(child_pid, NULL, 0) == -1)
                return print_and_return("Error: waitpid() failed.\n", 0, 0);
        if (is_pipe) {
            if ((fork_pid = fork()) < 0)
                return print_and_return("Error: fork() failed.\n", 0, 0);
            if (fork_pid == 0) {
                child_pid = getpid();
                close(pid_pipe[0]);
                if (write(pid_pipe[1], &child_pid, sizeof(child_pid)) < 0)
                    return print_and_return("Error: writing to pipe failed.\n", 0, 1);
                close(pid_pipe[1]);
                dup2(cmd_pipe[0], STDIN_FILENO);
                close(cmd_pipe[1]);
                if (execvp(cmd2[0], cmd2) < 0)
                    return print_and_return("Error: right command execution failed.\n", 0, 1);
                exit(0);
            } else {
                if (waitpid(child_pid, NULL, 0) == -1)
                    return print_and_return("Error: waitpid() failed.\n", 0, 0);
                close(cmd_pipe[0]);
                close(cmd_pipe[1]);
                close(pid_pipe[1]);
                if (read(pid_pipe[0], &child_pid, sizeof(child_pid)) < 0)
                    return print_and_return("Error: reading from pipe failed.\n", 0,0);
                close(pid_pipe[0]);
                if (!kill(child_pid, 0) && waitpid(child_pid, NULL, 0) == -1)
                    return print_and_return("Error: waitpid() failed.\n", 0, 0);
            }
        }
    }
    return 1;
}
/*
 * int prepare(void):
 *      The prepare function only registers both signal handlers for the signals they need to take
 *      care of. The signal handlers are shown above. It returns -1 in case of sigaction() error and 0 otherwise.
 */
int prepare(void) {
    struct sigaction act_sigchld, act_sigint;
    memset(&act_sigchld, '\0', sizeof(act_sigchld));
    act_sigchld.sa_sigaction = &handle_sigchld;
    act_sigchld.sa_flags = SA_SIGINFO | SA_RESTART;
    if (sigaction(SIGCHLD, &act_sigchld, NULL) < 0)
        return print_and_return("Error with registering signal handler...\n", -1, 0);

    memset(&act_sigint, '\0', sizeof(act_sigint));
    act_sigint.sa_sigaction = &handle_sigint;
    act_sigint.sa_flags = SA_SIGINFO | SA_RESTART;
    if (sigaction(SIGINT, &act_sigint, NULL) < 0)
        return print_and_return("Error with registering signal handler...\n", -1, 0);
    return 0;
}

/*
 * int finalize(void):
 *      Does nothing, only returns 0.
 *      Didn't need to do anything because no memory allocation was made.
 */
int finalize(void) {
    return 0;
}