/* 
 * Public Domain getopt header
 *
 */

#ifndef MY_GETOPT_H
#define MY_GETOPT_H

#ifdef WIN32

#ifdef __cplusplus
extern "C" {
#endif

int getopt(int argc, char * const argv[], const char *optstring);
extern char *optarg;
extern int optind, opterr, optopt;

#ifdef __cplusplus
}
#endif

#endif

#endif
