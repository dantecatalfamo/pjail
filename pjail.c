#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <paths.h>

// all promises with space between them plus a NUL
#define MAXLEN_PROMISE 191
#define NUMBER_PLEDGES 33
#define EXIT_INVALID_PROMISE 2
#define EXIT_NO_PROMISES 3

char *allpromises[] = {
    "stdio", "rpath", "wpath", "cpath", "dpath", "tmppath", "inet",
    "mcast", "fattr", "chown", "flock", "unix", "dns", "getpw", "sendfd",
    "recvfd", "tape", "tty", "proc", "exec", "prot_exec", "settime", "ps",
    "vminfo", "id", "pf", "route", "wroute", "audio", "video", "bpf",
    "unveil", "error", NULL
};


int validpromise(const char *);
int appendpromise(const char **, int, const char *);
void listpromises();
void usage();
void pledgefmt(char *, int, const char **, int);


int
main(int argc, char **argv, char **envp)
{
    int ch, valid, npromises;
    char pledgecat[MAXLEN_PROMISE];
    char *shell;
    const char *promises[NUMBER_PLEDGES+1] = {NULL};

    pledgecat[0] = '\0';
    npromises = 0;

    while ((ch = getopt(argc, argv, "hlp:")) != -1) {
        switch(ch) {
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
        case 'l':
            listpromises();
            exit(EXIT_SUCCESS);
        case 'p':
            valid = validpromise(optarg);
            if (!valid) {
                printf("%s is not a valid pledge\n", optarg);
                exit(EXIT_INVALID_PROMISE);
            }
            if (appendpromise(promises, NUMBER_PLEDGES, optarg)) {
                npromises++;
            }
            break;
        default:
            usage();
            exit(EXIT_FAILURE);
        }
    }

    argc -= optind;
    argv += optind;


    if (npromises == 0) {
        fprintf(stderr, "No promises\n");
        exit(EXIT_NO_PROMISES);
    }

    pledgefmt(pledgecat, MAXLEN_PROMISE, promises, npromises);
    if (pledge("stdio exec", pledgecat) != 0) {
        perror("Pledge failed");
    }

    if (argc == 0) {
        shell = getenv("SHELL");
        if (!shell) {
            shell = _PATH_BSHELL;
        }
        char *const newenvp[] = {shell, NULL};
        fprintf(stderr, "No command specified, defaulting to %s\n", shell);
        execve(shell, newenvp, envp);
    }

    execvp(argv[0], argv);
    perror("Exec failed");
    return EXIT_FAILURE;
}

void
usage()
{
    printf("pjail: [-hl] [-p pledge] [command] [args...]\n");
}

void
listpromises()
{
    char *p;
    int index;
    index = 0;
    while ((p = allpromises[index++])) {
        printf("%s\n", p);
    }
}

int
validpromise(const char *promise)
{
    int index;
    char *p;

    index = 0;
    while ((p = allpromises[index++])) {
        if ((strcmp(promise, p)) == 0) {
            return 1;
        };
    }
    return 0;
}

int
appendpromise(const char **promises, int length, const char *promise)
{
    const char *p;
    int index;
    index = 0;
    while ((index < length) && (p = promises[index])) {
        if (strcmp(p, promise) == 0) {
            return 0;
        }
        index++;
    }
    assert(index <= NUMBER_PLEDGES);
    promises[index] = promise;
    return 1;
}

void
pledgefmt(char *s, int size, const char **promises, int npromises)
{
    int i, n;
    for (i=0; i<npromises; i++) {
        n = strlcat(s, promises[i], size);
        assert(n < size);
        if (i+1 < npromises) {
            strlcat(s, " ", size);
        }
    }
}
