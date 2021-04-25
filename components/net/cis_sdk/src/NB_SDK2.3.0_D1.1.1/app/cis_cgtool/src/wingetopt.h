#ifndef _WINGETOPT_H_
#define _WINGETOPT_H_
    
extern int opterr;
extern int optind;
extern int optopt;
extern char *optarg;
extern int getopt(int argc, char **argv, char *opts);

#endif