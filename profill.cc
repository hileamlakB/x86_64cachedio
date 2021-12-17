#include "x86_64cachedio.hh"
#include <sys/time.h>
#include <sys/resource.h>
#include <cerrno>

static struct timeval tv_begin;

void profile_begin() {
    int r = gettimeofday(&tv_begin, 0);
    assert(r >= 0);
}

void profile_end() {
    struct timeval tv_end;
    struct rusage usage, cusage;

    int r = gettimeofday(&tv_end, 0);
    assert(r >= 0);
    r = getrusage(RUSAGE_SELF, &usage);
    assert(r >= 0);
    r = getrusage(RUSAGE_CHILDREN, &cusage);
    assert(r >= 0);

    timersub(&tv_end, &tv_begin, &tv_end);
    timeradd(&usage.ru_utime, &cusage.ru_utime, &usage.ru_utime);
    timeradd(&usage.ru_stime, &cusage.ru_stime, &usage.ru_stime);

    char buf[1000];
    int len = sprintf(buf, "{\"time\":%ld.%06ld, \"utime\":%ld.%06ld, \"stime\":%ld.%06ld, \"maxrss\":%ld}\n",
                      tv_end.tv_sec, (long) tv_end.tv_usec,
                      usage.ru_utime.tv_sec, (long) usage.ru_utime.tv_usec,
                      usage.ru_stime.tv_sec, (long) usage.ru_stime.tv_usec,
                      usage.ru_maxrss + cusage.ru_maxrss);

    
   
    ssize_t nwritten = write(2, buf, len);
    assert(nwritten == len);
}
