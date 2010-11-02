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

int main(int argc, char** argv)
{
    char *buf, *file;
    int size = 0;
    int c, fd;
    struct timespec start_time, end_time;

    while ((c = getopt (argc, argv, "q")) != -1)
        switch (c) {
            case 'q':
                close(STDERR_FILENO);
                fcntl(open("/dev/null", O_WRONLY), F_DUPFD, STDERR_FILENO);
                break;
            default:
                printf("%i;Usage: %s [-q]\n", c, __progname);
                return 1;
    }

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
    if (start_time.tv_sec - end_time.tv_sec == 0
            || start_time.tv_nsec - end_time.tv_nsec < 0)
        fprintf(stderr, "Interpreter returned: 0x%x. Time: %li nanoseconds.\n",
                c, labs(start_time.tv_nsec - end_time.tv_nsec));
    else
        fprintf(stderr, "Interpreter returned: 0x%x. Time: %.5f seconds.\n",
                c, start_time.tv_sec - end_time.tv_sec +
                        (start_time.tv_nsec - end_time.tv_nsec) / 1000000000.0);
    return 0;
}
