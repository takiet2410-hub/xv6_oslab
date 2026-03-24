#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

#define MAXPATH 512

void indent(int depth)
{
    for(int i = 0; i < depth; i++)
        printf("  ");
}

void tree(char *path, int depth)
{
    int fd;
    struct stat st;
    struct dirent de;
    char buf[MAXPATH];

    if((fd = open(path, 0)) < 0)
    {
        printf("tree: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0)
    {
        printf("tree: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if(st.type != T_DIR)
    {
        indent(depth);
        printf("%s\n", path);
        close(fd);
        return;
    }

    while(read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if(de.inum == 0) continue;

        char name[DIRSIZ + 1];
        memmove(name, de.name, DIRSIZ);
        name[DIRSIZ] = 0;

        if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;

        int len = strlen(path);

        if(len + 1 + strlen(name) + 1 > MAXPATH)
        {
            printf("tree: path too long\n");
            continue;
        }

        strcpy(buf, path);

        if(path[len - 1] != '/')
        {
            buf[len] = '/';
            buf[len + 1] = 0;
            len++;
        }

        strcpy(buf + len, name);

        if(stat(buf, &st) < 0)
        {
            printf("tree: cannot stat %s\n", buf);
            continue;
        }

        indent(depth);

        if(st.type == T_DIR)
        {
            printf("%s/\n", name);
            tree(buf, depth + 1);
        }
        else
        {
            printf("%s\n", name);
        }
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    char *path;
    if(argc < 2) path = ".";
    else path = argv[1];
    printf("%s/\n", path);
    tree(path, 1);
    exit(0);
}
