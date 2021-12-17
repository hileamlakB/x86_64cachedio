#include "x86_64cachedio.hh"
#include <iostream>

// copen
cfile*  copen(const char* filename, int mode) {
    int fd = -1;
    if (filename) {
        fd = open(filename, mode, S_IRWXU);
        if (fd == -1)
            perror("ERROR");
    } 
    if (fd < 0)
        return nullptr;

    cfile *file = new cfile(fd, mode);
    return file;
}

// cclose
int cclose(cfile* file) {
    if (!file)
        return 0;
    
    cflush(file, FULL);
    int s = close(file->fd);
    delete file;
    return s;
}

int cfill(cfile *file){

    slot *slt = &file->slots[file->last_slot];

    slt->reset();

    // Cache aligne memory to improve reverse readding patterns
    off_t x = (file->file_ofset / SLOTSIZE ) * SLOTSIZE;
    off_t new_offset = x;
    if (x != file->file_ofset)
        new_offset = lseek(file->fd, x, SEEK_SET);
    assert(new_offset == x); // check if lseek hasnt failed
    
    
    size_t red = read(file->fd, slt->buf, SLOTSIZE);
    if (red < 0)
        return red;
    
    if (red < file->file_ofset - new_offset){
        // only the back ward cache is read
        return -1;
    }

    slt->end = red;
    slt->cur = file->file_ofset - new_offset;
    slt->min_pointer = new_offset; 
    slt->max_pointer = new_offset + red;

    file->user_ofset = file->file_ofset;
    file->file_ofset = new_offset + red;
    
    file->set_slot();
    
    file->last_slot = (file->last_slot + 1) % SLOTS ;
    assert(slt->check_state());

    return red;
}

// cread
size_t cread(cfile *file, unsigned char *buf, size_t sz){

    size_t all_red = 0;
    int si = file->find_slot();
    
    if (si == -1)
    {
        if (sz > SLOTSIZE){
            int red = read(file->fd, buf, sz);
            if (red > 0)
            {
                file->file_ofset += red;
                file->user_ofset += red;
            }
            return red == 0? EOF: red;
        }
            
        int r;
        if ((r = cfill(file)) == 0)
            return EOF;
        if (r < 0)
            return r;
    }

   
    si = file->find_slot();
    slot *slt = &file->slots[si];

    if (slt->is_empty()){
        int r;
        if ((r = cfill(file)) == 0)
            return EOF;
        if (r < 0)
            return r;
    }
    
    size_t to_read = (sz < slt->end - slt->cur)? sz : slt->end - slt->cur;

    memcpy(buf, slt->buf + slt->cur, to_read); 
    sz -= to_read;
    buf += to_read;
    all_red += to_read;
    slt->cur += to_read;
    file->user_ofset += to_read;

    if (sz > SLOTSIZE){
        // after reading what was available directly read if cacheing isn't benefitial
        // / is integer division so it will tell us how money 
        // buffer sizes we will be reading
        size_t direct_read = (sz / (SLOTSIZE)) * (SLOTSIZE);
        int red = read(file->fd, buf, direct_read);
        if (red < direct_read)
            return red;
        buf += red;
        sz -= red;
        all_red += red;
        file->file_ofset += red;
        file->user_ofset += red;
    }

    if (sz)
    {
        size_t red = cread(file, buf, sz);
        if (red != EOF)
            all_red += red;
    }
        

    
    return all_red;
}

// cwrite
size_t cwrite(cfile *file, unsigned char *src, size_t sz) {
    return 0;
    
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




// cflush(f)
//    Forces a write of all buffered data written to `f`.
//    If `f` was opened read-only, cflush(f) may either drop all
//    data buffered for reading, or do nothing.

int cflush(cfile* file, int type = SINGLE) {
    if (type == SINGLE){
        int si = file->find_slot();
        if (si == -1)
            return 0;
        
        slot *slt = &file->slots[si];
        off_t new_ofset = lseek(file->fd, slt->min_pointer, SEEK_SET);
        assert(new_ofset == slt->min_pointer);
        
        
        size_t writen = write(file->fd, slt->buf, SLOTSIZE);
        assert(writen > 0);
       
        slt->cur = 0;
        file->file_ofset = slt->min_pointer + writen; 

        return writen;

    }

    if (type == FULL){
        int written = 0;
        for (auto& slot_idpair: file->slot_inverse_map){
            if (file->slots[slot_idpair.first].written){
                slot *slt = &file->slots[slot_idpair.first];
                off_t new_ofset = lseek(file->fd, slt->min_pointer, SEEK_SET);
                assert(new_ofset == slt->min_pointer);
              
                
                size_t writen = write(file->fd, slt->buf, slt->cur);
                assert(writen > 0);
                written += writen;
                slt->cur = 0;
                file->file_ofset = slt->min_pointer + writen; 
            }

        }
        
        return written;
    }

    
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
