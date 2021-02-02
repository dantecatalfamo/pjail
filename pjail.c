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

const char *allpromises[] = {
    "stdio", "rpath", "wpath", "cpath", "dpath", "tmppath", "inet",
    "mcast", "fattr", "chown", "flock", "unix", "dns", "getpw", "sendfd",
    "recvfd", "tape", "tty", "proc", "exec", "prot_exec", "settime", "ps",
    "vminfo", "id", "pf", "route", "wroute", "audio", "video", "bpf",
    "unveil", "error", NULL
};


int validpromise(const char *);
void appendpromise(const char **, const char *);
void invertpromises(const char **);
void listpromises(void);
void usage(void);
void pledgefmt(char *, int, const char **);


int
main(int argc, char **argv, char **envp)
{
    int ch, invert, verbose;
    char pledgestr[MAXLEN_PROMISE];
    char *shell;
    char *shellargv[2] = {NULL};
    const char *promises[NUMBER_PROMISES+1] = {NULL};

    pledgestr[0] = '\0';
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
            if (!validpromise(optarg)) {
                printf("%s is not a valid pledge\n", optarg);
                exit(EXIT_INVALID_PROMISE);
            }
            appendpromise(promises, optarg);
            break;
        default:
            usage();
            exit(EXIT_FAILURE);
        }
    }

    argc -= optind;
    argv += optind;


    if (promises[0] == NULL) {
        fprintf(stderr, "No promises\n");
        usage();
        exit(EXIT_NO_PROMISES);
    }

    if (invert == 1) {
        invertpromises(promises);
    }

    pledgefmt(pledgestr, MAXLEN_PROMISE, promises);

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
    printf("pjail: [-hvli] [-p promise] [command] [args...]\n");
}

void
listpromises(void)
{
    const char *p, **ap;
    ap = allpromises;
    while ((p = *ap++)) {
        printf("%s\n", p);
    }
}

int
validpromise(const char *promise)
{
    const char *p, **ap;
    ap = allpromises;
    while ((p = *ap++)) {
        if ((strcmp(promise, p)) == 0) {
            return 1;
        }
    }
    return 0;
}

void
appendpromise(const char **promises, const char *promise)
{
    const char *p, **op;
    op = promises;
    while ((p = *op)) {
        if (strcmp(p, promise) == 0) {
            return;
        }
        op++;
    }
    *op = promise;
    assert(op[1] == NULL);
}

void
pledgefmt(char *s, int size, const char **promises)
{
    int n;
    const char *p, **op;

    op = promises;
    while ((p = *op++)) {
        n = strlcat(s, p, size);
        assert(n < size);
        if (op[1] != NULL) {
            strlcat(s, " ", size);
        }
    }
}

void
invertpromises(const char **promises)
{
    const char *p, *u, **ap, **au, **pr;
    const char *unwanted[NUMBER_PROMISES+1] = {NULL};

    au = unwanted;
    pr = promises;
    while ((*au++ = *pr++))
        ;

    ap = allpromises;
    while ((p = *ap++)) {
        au = unwanted;
        while ((u = *au++)) {
            if (strcmp(u, p) == 0) {
                goto outer;
            }
        }
        *promises++ = p;
    outer:;
    }
    *promises = NULL;
}
