#include <stdio.h>
#include <string.h>
#include "wingetopt.h"


#define NULL    0
#define EOF    (-1)
#define ERR(s, c)    if(opterr){\
    char errbuf[2];\
    errbuf[0] = c; errbuf[1] = '\n';\
    fputs(argv[0], stderr);\
    fputs(s, stderr);\
	fputc(c, stderr);}


int    opterr = 1;
int    optind = 1;
int    optopt;
char    *optarg;

int getopt(int argc, char **argv, char *opts)
{
    static int ind = 1;
    register int c;
    register char *cp;
    
    

    if(ind == 1){
        while(optind < argc){
            if(argv[optind][0] != '-')optind++;
            else break;
        }

        if(optind >= argc ||
            argv[optind][0] != '-' || argv[optind][1] == '\0')
            return(EOF);
        else if(strcmp(argv[optind], "--") == NULL) {
            optind++;
            return(EOF);
        }
        optopt = c = argv[optind][ind];
        if(c == ':' || (cp=strchr(opts, c)) == NULL) {
            ERR(": illegal option -- ", c);
            if(argv[optind][++ind] == '\0') {
                optind++;
                ind = 1;
            }
            return('?');
        }
        if(*++cp == ':') {
            if(argv[optind][ind+1] != '\0')
                optarg = &argv[optind++][ind+1];
            else if(++optind >= argc) {
                ERR(": option requires an argument -- ", c);
                ind = 1;
                return('?');
            } else
                optarg = argv[optind++];
            ind = 1;
        } else {
            if(argv[optind][++ind] == '\0') {
                ind = 1;
                optind++;
            }
            optarg = NULL;
        }
        return(c);
    }

    return -1;
}