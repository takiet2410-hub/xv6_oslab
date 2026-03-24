#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAXLINE 512

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        fprintf(2, "Usage: xargs command [args...]\n");
        exit(1);
    }

    char *args[MAXARG];

    int base = 0;
    for(int i = 1; i < argc; i++)
    {
        args[base++] = argv[i];
    }

    char buf[MAXLINE];
    int len = 0;
    char c;

    while(read(0, &c, 1) == 1)
    {
        if(c == '\n')
        {
            buf[len] = 0;

            char *line = malloc(len + 1);
            if(line == 0)
            {
                fprintf(2, "malloc failed\n");
                exit(1);
            }
            strcpy(line, buf);

            args[base] = line;
            args[base + 1] = 0;

            if(fork() == 0)
            {
                exec(args[0], args);
                fprintf(2, "exec %s failed\n", args[0]);
                exit(1);
            }
            wait(0);

            len = 0; // reset buffer
        }
        else
        {
            if(len < MAXLINE - 1)
                buf[len++] = c;
        }
    }

    exit(0);
}

