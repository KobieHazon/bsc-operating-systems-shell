#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static void reap_children(int signal_number)
{
    (void)signal_number;
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0) {}
    errno = saved_errno;
}

static void ignore_interrupt(int signal_number)
{
    (void)signal_number;
}

static void wait_for(pid_t child)
{
    while (waitpid(child, NULL, 0) < 0) {
        if (errno == EINTR)
            continue;
        /* The SIGCHLD handler may have already reaped this child. */
        if (errno != ECHILD)
            perror("waitpid");
        break;
    }
}

static void execute(char **arguments, int background)
{
    signal(SIGINT, background ? SIG_IGN : SIG_DFL);
    signal(SIGCHLD, SIG_DFL);
    execvp(arguments[0], arguments);
    perror(arguments[0]);
    _exit(127);
}

int process_arglist(int count, char **arguments)
{
    if (count == 0)
        return 1;
    int background = strcmp(arguments[count - 1], "&") == 0;
    if (background)
        arguments[--count] = NULL;
    int split = -1;
    for (int i = 0; i < count; ++i) {
        if (strcmp(arguments[i], "&") == 0)
            goto invalid;
        if (strcmp(arguments[i], "|") == 0) {
            if (split != -1 || i == 0 || i == count - 1)
                goto invalid;
            split = i;
        }
    }
    if (count == 0)
        goto invalid;
    if (split == -1) {
        pid_t child = fork();
        if (child == 0)
            execute(arguments, background);
        if (child < 0)
            perror("fork");
        else if (!background)
            wait_for(child);
        return 1;
    }

    int descriptors[2];
    if (pipe(descriptors) < 0) {
        perror("pipe");
        return 1;
    }
    arguments[split] = NULL;
    pid_t left = fork();
    if (left == 0) {
        if (dup2(descriptors[1], STDOUT_FILENO) < 0)
            _exit(126);
        close(descriptors[0]);
        close(descriptors[1]);
        execute(arguments, background);
    }
    if (left < 0) {
        perror("fork");
        close(descriptors[0]);
        close(descriptors[1]);
        return 1;
    }
    pid_t right = fork();
    if (right == 0) {
        if (dup2(descriptors[0], STDIN_FILENO) < 0)
            _exit(126);
        close(descriptors[0]);
        close(descriptors[1]);
        execute(arguments + split + 1, background);
    }
    close(descriptors[0]);
    close(descriptors[1]);
    if (right < 0) {
        perror("fork");
        kill(left, SIGTERM);
        wait_for(left);
    } else if (!background) {
        wait_for(left);
        wait_for(right);
    }
    return 1;

invalid:
    fputs("Syntax: COMMAND [ | COMMAND ] [ & ]\n", stderr);
    return 1;
}

int prepare(void)
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    action.sa_handler = reap_children;
    if (sigaction(SIGCHLD, &action, NULL) < 0)
        return -1;
    action.sa_flags = SA_RESTART;
    action.sa_handler = ignore_interrupt;
    return sigaction(SIGINT, &action, NULL);
}

int finalize(void)
{
    return 0;
}
