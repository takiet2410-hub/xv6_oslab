#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if(argc<2)
    {
        fprintf(2,"Usage: xargs command [args...]\n");
        exit(1);
    }
    
    if(argc>=MAXARG)
    {
        fprintf(2,"xargs: too many arguments\n");
        exit(1);
    }

    char buf[512];
    char *args[MAXARG];
    
    int i;
    int base=argc-1;

    for(i=1;i<argc;i++) args[i-1]=argv[i];
    int n=0;
    char c;

    void run()
    {
        buf[n]=0;
        args[base]=buf;
        args[base+1]=0;

        if(fork()==0)
        {
            exec(args[0], args);
            fprintf(2,"exec %s failed\n",args[0]);
            exit(1);
        }
        else wait(0);
    }

    while(read(0,&c,1)==1)
    {
        if(c=='\n')
        {
            run();
            n=0;
        }
        else 
        {
            if(n<sizeof(buf)-1)
            {
                buf[n++]=c;
            }
            else{
                run();
                n=0;
            }
        }
    }
    if(n>0) run();
    
    exit(0);
}