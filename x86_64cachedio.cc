#include "x86_64cachedio.hh"


#define SLOTSIZE    4096
#define SLOTS       10



// cclose(f)
//    Close the cfile `f` and release all its resources.

int cclose(cfile* f) {
   
    delete f;
    return 0;
}


// cread(f)
//    Read a single (unsigned) character from `f` and return it. Returns EOF
//    (which is -1) on error or end-of-file.

int cread(cfile* f){
    
   return 0;
}


// cread(f, buf, sz)
//    Read up to `sz` characters from `f` into `buf`. Returns the number of
//    characters read on success; normally this is `sz`. Returns a short
//    count, which might be zero, if the file ended before `sz` characters
//    could be read. Returns -1 if an error occurred before any characters
//    were read.

ssize_t cread(cfile* f, unsigned char* buf, size_t sz) {
    
    return 0;

    // Note: This function never returns -1 because `cread`
    // does not distinguish between error and end-of-file.
    // Your final version should return -1 if a system call indicates
    // an error.
}


// cwritec(f)
//    Write a single character `ch` to `f`. Returns 0 on success or
//    -1 on error.

int cwritec(cfile* f, int ch) {
    return 0;
}


// cwrite(f, buf, sz)
//    Write `sz` characters from `buf` to `f`. Returns the number of
//    characters written on success; normally this is `sz`. Returns -1 if
//    an error occurred before any characters were written.

ssize_t cwrite(cfile* f, const unsigned char* buf, size_t sz) {
    size_t nwritten = 0;
    return nwritten;
}


// cflush(f)
//    Forces a write of all buffered data written to `f`.
//    If `f` was opened read-only, cflush(f) may either drop all
//    data buffered for reading, or do nothing.

int cflush(cfile* f) {
    
    return 0;
}


// cseek(f, pos)
//    Change the file pointer for file `f` to `pos` bytes into the file.
//    Returns 0 on success and -1 on failure.

int cseek(cfile* f, off_t pos) {
    return 0;
    
}



// copen(filename, mode)
//    Open the file corresponding to `filename` and return its cfile *.
//    returns nullptr if it fails

cfile*  copen(const char* filename, int mode) {
    int fd = -1;
    if (filename) {
        fd = open(filename, mode, mode);
    } 
    if (fd < 0)
        return nullptr;

    cfile *FILE = new cfile(fd, mode);
    return FILE;
}


// cfilesize(f)
//    Return the size of `f` in bytes. Returns -1 if `f` does not have a
//    well-defined size (for instance, if it is a pipe).

off_t cfilesize(cfile* f) {
    struct stat s;
    int r = fstat(f->fd, &s);
    if (r >= 0 && S_ISREG(s.st_mode)) {
        return s.st_size;
    } else {
        return -1;
    }
}
