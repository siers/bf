#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

#include "bf.h"


extern char *__progname;

int brackets_match(char* buf)
{
    int n;

    for (; *buf; buf++) {
        if (*buf == '[')
            n++;
        else if (*buf == ']') {
            if (!n)
                return -1;
            else
                n--;
        }
    }

    return n;
}

int interpret(void)
{
    char p[256], buf[4096];
    int n;

    *buf = 0;
    printf("* ");
    fflush(stdout);
    while ((n = scanf("%256s", p)) != -1) {
        strncat(buf, p, 4096);
        switch ((n = brackets_match(buf))) {
            case -1:
                *buf = 0;
                printf("Unmatched brackets, clearing buffer.\n* ");
                break;
            case 0:
                brainfuck(buf, strlen(buf));
                *buf = 0;
                printf("\n* ");
                break;
            default:
                break;
        }
        fflush(stdout);
    }
    return 0;
}

int main(int argc, char** argv)
{
    char *buf, *file;
    int c, fd = 0, size = 0;
    struct timespec start_time, end_time;

    while ((c = getopt (argc, argv, "qi")) != -1)
        switch (c) {
            case 'q':
                close(STDERR_FILENO);
                fcntl(open("/dev/null", O_WRONLY), F_DUPFD, STDERR_FILENO);
                break;
            case 'i':
                fd = -1;
                break;
            default:
                printf("Usage: %s [-qi]\n", __progname);
                return 1;
    }

    if (fd == -1) /* If fd == -1, you thou shall interpret! */
        return interpret();

    for (c = optind; c < argc; c++)
        file = argv[(unsigned int) c];

    if (!(fd = open(file, O_RDONLY)))
        return 2;
    fcntl(fd, F_SETFD, O_NONBLOCK);
    buf = malloc(BUFSIZ);
    while ((size += read(fd, buf + size, BUFSIZ)) == BUFSIZ) {
        if (!(buf = realloc(buf, size)))
                return -1;
    };

    if (!(size > 0)) {
        printf("Nothing to interpret, exiting.\n");
        return 3;
    }

    fprintf(stderr, "Interpreting started. (buf: %p; size: %i)\n", buf, size);
    clock_gettime(CLOCK_REALTIME, &start_time);
    c = brainfuck(buf, size);
    clock_gettime(CLOCK_REALTIME, &end_time);

    printf("\n");
    if (labs(end_time.tv_nsec - start_time.tv_nsec) < 50000 &&
            end_time.tv_sec - start_time.tv_sec < 2)
        fprintf(stderr, "Interpreter returned: 0x%x. Time: %li nanoseconds.\n",
                c, labs(end_time.tv_nsec - start_time.tv_nsec));
    else
        if (labs(end_time.tv_nsec - start_time.tv_nsec) < 50000000 &&
                end_time.tv_sec - start_time.tv_sec < 2)
            fprintf(stderr,
                    "Interpreter returned: 0x%x. Time: %.5f microseconds.\n",
                    c, labs(end_time.tv_nsec - start_time.tv_nsec) / 1000000.0);
        else
            fprintf(stderr, "Interpreter returned: 0x%x. Time: %.5f seconds.\n",
                    c, end_time.tv_sec - start_time.tv_sec +
                    (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0);
    return 0;
}
