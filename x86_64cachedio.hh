#ifndef IO61_HH
#define IO61_HH
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <vector>
#include <fcntl.h>
#include <cerrno>
#include <sys/stat.h>
#include <unordered_map>
#include <cstdlib>
#include <unistd.h>
#include <cstdint>


#define SLOTSIZE    4096
#define SLOTS       10

struct io61_file;

// io61_file* io61_fdopen(int fd, int mode);
// io61_file* io61_open_check(const char* filename, int mode);
// int io61_close(io61_file* f);
// off_t io61_filesize(io61_file* f);
// int io61_seek(io61_file* f, off_t pos);
// int io61_readc(io61_file* f);
// int io61_writec(io61_file* f, int ch);
// ssize_t io61_read(io61_file* f, unsigned char* buf, size_t sz);
// ssize_t io61_write(io61_file* f, const unsigned char* buf, size_t sz);
// int io61_flush(io61_file* f);
void cprofile_begin();
void cprofile_end();
struct carguments {
    size_t input_size;          // `-s` option: input size. Default SIZE_MAX
    size_t block_size;          // `-b` option: block size. Default 0
    size_t stride;              // `-t` option: stride. Default 1024
    bool lines;                 // `-l` option: read by lines. Default false
    const char* output_file;    // `-o` option: output file. Default nullptr
    const char* input_file;     // input file. Default nullptr
    std::vector<const char*> input_files;   // all input files
    std::vector<const char*> output_files;  // all output files
    const char* program_name;   // name of program
    const char* opts;           // options string

    carguments(int argc, char** argv, const char* opts);
    void usage();
};



struct slot {

    off_t half_buf = SLOTSIZE / 2;
    unsigned char buf[SLOTSIZE];
    
    // state variables that decribe the current state of 
    // of the cache buffer
    off_t beg = 0, cur = 0, end = 0;

    // the maximum and minum adress this cache maps
    off_t max_pointer = 0, min_pointer = 0;

    void reset(){
        beg = cur = end = 0;
    }

    bool check_state()
    {
        // FOR DEUGGING
        if (!(beg <= cur && cur <= end))
            return false;
        
        if (!(end - cur <= SLOTSIZE))
            return false;
        
        return true;
    }
};

struct cfile{

    slot slots[SLOTS];
    std::unordered_map<uintptr_t, int> slot_map;
    int fd, mode;
    

    cfile(int _fd, int _mode){
        fd = _fd;
        mode = _mode;
    }
    
    int find_slot(uintptr_t offset){
        if (slot_map.find(offset % SLOTSIZE) != slot_map.end()){
            return slot_map[offset % SLOTSIZE];
        }
        return -1;
    }
};

// Cached io api's

// copen - opens file at path file, and returns a cfile pointer
// if it succeds or nullptr if it fails. 
cfile *copen(const char *file, int mode);

// cclose - closes a file opened by copen
int cclose(cfile *file);


// cread - reads sz bytes from a file opened by copen
// and copies it to buf
int cread(cfile *file, char *buf, size_t sz);

// cwrite - writes sz bytes from src to a file opened
// by copen
int cwrite(cfile *file, char *src, size_t sz);

// creadc - returns one character from cache
// or EOF if file has reached the end
int creadc(cfile *file);

// cwrite - writes a byte to file opened by copen
int cwritec(cfile *file, char src);


// cseek - moves a file pointer by offset depending on the whence flag
// if whence is ...
off_t cseek(cfile *file, off_t offset, int whence);

// cfread - a wrapper around unix read. You should use this
// if file is opened with copen, it gurantees the latest process buffer
// reads, specially if file is open with both read and write 
int cfread(cfile *file, char *buf, size_t sz);
// cwrite - a wrapper around unix write. You should use this
// if file is opened with copen, it gurantees cache coherene
int cfwrite(cfile *file, char *src, size_t sz);

// cflus - writes data in cache into file
int cflush(cfile *file);

// cfill - refills cache from file
int cfill(cfile *file);

#endif
