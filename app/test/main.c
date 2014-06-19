#include "stdio.h"
#include <string.h>
#include <lw_type.h>
#include <lw_config_api.h>

typedef struct appfuns
{
    char *name;
    int (*func)(int argc,char **argv);
}appfuns_t;
//extern int ifm_main(int argc, char **argv);
extern int ifm_main(int argc, char **argv);
extern int cfg_main(int argc, char **argv);


appfuns_t applets[]=
{
  {  "ifm",ifm_main},
  {  "cfg",cfg_main} 
};

int main(int argc, char **argv)
{    
    char *base = NULL;
    
    const appfuns_t *a;
    base = strrchr(argv[0], '/');
    base = base ? base + 1 : argv[0];
   // printf("create error\n");
    Cfginit(SLAVER);
    for (a = applets; a->name; ++a)
    {        
        if(strcmp(base, a->name) == 0)
        {
                return a->func(argc, argv);
        }
    }
    return 0;
}	
