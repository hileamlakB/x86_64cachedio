#include "x86_64cachedio.hh"
#include <iostream>

// copen
cfile*  copen(const char* filename, int mode) {
    int fd = -1;
    if (filename) {
        fd = open(filename, mode, mode);
    } 
    if (fd < 0)
        return nullptr;

    cfile *file = new cfile(fd, mode);
    return file;
}

// cclose
int cclose(cfile* f) {
    cflush(f);
    int s = close(f->fd);
    delete f;
    return s;
}

int cfill(cfile *file){

    slot *slt = &file->slots[file->last_slot];

    slt->reset();

    // Cache from both sides, to improve reverse patterns
    // try to cache SLOTSIZE/2 both forward and backward
    off_t back = std::max((uintptr_t)0, file->file_ofset - SLOTSIZE/2);

    off_t new_offset = lseek(file->fd, back, SEEK_SET);
    assert(new_offset == back); // it may not always be back

    size_t to_read = back + SLOTSIZE/2;
    size_t red = read(file->fd, slt->buf, to_read);
    if (red < 0)
        return red;
    
    if (red < file->file_ofset - back){
        // only the back ward cache is read
        return -1;
    }

    slt->end = red;
    slt->cur = file->file_ofset - new_offset;
    slt->min_pointer = new_offset; 
    slt->max_pointer = file->file_ofset + SLOTSIZE/2;

    file->user_ofset = file->file_ofset;
    file->file_ofset += SLOTSIZE / 2;
    file->slot_map[file->user_ofset] = file->last_slot;
    file->last_slot++;

    assert(slt->check_state());

    return red;
}

// cread
size_t cread(cfile *file, unsigned char *buf, size_t sz){

    uintptr_t st = file->find_slot();
    
    if (st == -1)
    {
        int r;
        if ((r = cfill(file)) == 0)
            return EOF;
        if (r < 0)
            return r;
    }
   
    st = file->find_slot();
    

    size_t to_read = std::min(sz, (size_t)SLOTSIZE);
    slot *slt = &file->slots[st];

    memcpy(buf, slt->buf + slt->beg, to_read); 
    slt->beg += to_read;
    return to_read;
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
