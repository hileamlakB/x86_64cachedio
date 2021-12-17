#ifndef IOHH
#define IOHH
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
#include <algorithm>


#define SLOTSIZE    4096
#define SLOTS       10

// flushing types
#define SINGLE      1 // flush a single cache slot
#define FULL        2 // flush all cache slots


struct slot {

    unsigned char buf[SLOTSIZE];
    
    // state variables that decribe the current state of 
    // of the cache buffer
    off_t beg = 0, cur = 0, end = 0;
    bool written = false;

    // the maximum and minum adress this cache maps
    off_t max_pointer = 0, min_pointer = 0;

    void reset(){
        beg = cur = end = 0;
        written = false;

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

    bool is_empty(){
        return cur == 0 && end == 0;
    }

    bool is_full(){
        return cur == end;
    }
};

struct cfile{

    slot slots[SLOTS];
    std::unordered_map<off_t, int> slot_map;
    std::unordered_map<int, off_t> slot_inverse_map;
    int fd, mode;

    off_t user_ofset = 0;
    off_t file_ofset = 0;
    int last_slot = 0;
    
    cfile(int _fd, int _mode){
        fd = _fd;
        mode = _mode;
    }
    
    int find_slot(){
        if (slot_map.find(user_ofset / (SLOTSIZE)) != slot_map.end()){
            return slot_map[user_ofset / (SLOTSIZE)];
        }
        return -1;
    }

    void set_slot(){
        // maps the current user offset to the current slot
        
        if (slot_inverse_map.find(last_slot) != slot_inverse_map.end()){
            slot_map.erase(slot_inverse_map[last_slot]);
        }

        slot_map[user_ofset  / (SLOTSIZE)] = last_slot;
        slot_inverse_map[last_slot] = user_ofset  / (SLOTSIZE);
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
size_t cread(cfile *file, unsigned char *buf, size_t sz);

// cwrite - writes sz bytes from src to a file opened
// by copen
size_t cwrite(cfile *file, unsigned char *src, size_t sz);

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
// returns the number of bytes written
// if succesfull returns full cache size
int cflush(cfile *file, int type);

// cfill - refills cache from file
int cfill(cfile *file);




void profile_begin();
void profile_end();

#endif
