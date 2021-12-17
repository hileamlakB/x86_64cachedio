#include "x86_64cachedio.hh"
#include <iostream>

int main(){
    
    unsigned char buf[4000];

   
    cfile *f = copen("./files/text1meg.txt", O_RDONLY);
    size_t stat;

    int i = 0;
    profile_begin();
    while ( (stat = cread(f, buf, 4000)) != EOF){
        // cwrite(g, buf, stat);
        i++;
       
    } 
    cclose(f);
    profile_end();
}