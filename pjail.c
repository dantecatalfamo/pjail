#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <paths.h>

/* all promises with space between them plus a NUL */
#define MAXLEN_PROMISE 191
#define NUMBER_PROMISES 33
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
void listpromises(void);
void usage(void);
void pledgefmt(char *, int, const char **, int);
int invertpromises(const char **, int);


int
main(int argc, char **argv, char **envp)
{
    int ch, valid, npromises, invert, verbose;
    char pledgestr[MAXLEN_PROMISE];
    char *shell;
    char *shellargv[2] = {NULL};
    const char *promises[NUMBER_PROMISES+1] = {NULL};

    pledgestr[0] = '\0';
    npromises = 0;
    invert = 0;
    verbose = 0;

    while ((ch = getopt(argc, argv, "hvlip:")) != -1) {
        switch(ch) {
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
        case 'l':
            listpromises();
            exit(EXIT_SUCCESS);
        case 'i':
            invert = 1;
            break;
        case 'v':
            verbose = 1;
            break;
        case 'p':
            valid = validpromise(optarg);
            if (!valid) {
                printf("%s is not a valid pledge\n", optarg);
                exit(EXIT_INVALID_PROMISE);
            }
            if (appendpromise(promises, NUMBER_PROMISES, optarg)) {
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
        usage();
        exit(EXIT_NO_PROMISES);
    }

    if (invert == 1) {
        npromises = invertpromises(promises, npromises);
    }

    pledgefmt(pledgestr, MAXLEN_PROMISE, promises, npromises);

    if (verbose == 1) {
        fprintf(stderr,"pledge string: %s\n", pledgestr);
    }

    if (pledge("stdio exec", pledgestr) != 0) {
        perror("Pledge failed");
    }

    if (argc == 0) {
        shell = getenv("SHELL");
        if (!shell) {
            shell = _PATH_BSHELL;
        }
        shellargv[0] = shell;
        fprintf(stderr, "No command specified, defaulting to %s\n", shell);
        execve(shell, shellargv, envp);
    }

    execvp(argv[0], argv);
    perror("Exec failed");
    return EXIT_FAILURE;
}

void
usage(void)
{
    printf("pjail: [-hvli] [-p pledge] [command] [args...]\n");
}

void
listpromises(void)
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
    assert(index <= NUMBER_PROMISES);
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

int
invertpromises(const char **promises, int npromises)
{
    int un, all, out, i;
    const char *unwanted[NUMBER_PROMISES+1] = {NULL};

    out = 0;

    for (i=0; i<npromises; i++) {
        unwanted[i] = promises[i];
    }

    for (all=0; all<NUMBER_PROMISES; all++) {
        for (un=0; un<npromises; un++) {
            if (strcmp(unwanted[un], allpromises[all]) == 0) {
                goto outer;
            }
        }
        promises[out] = allpromises[all];
        out++;
    outer:;
    }
    return out;
}
