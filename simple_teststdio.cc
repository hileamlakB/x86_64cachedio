#include "x86_64cachedio.hh"
#include <cstdio>
#include <iostream>

int main(){
    
    unsigned char buf[4000];

    // cfile *g = copen("./files/output.txt", O_RDWR|O_TRUNC|O_CREAT);
    FILE *f = fopen("./files/text1meg.txt", "r");
    size_t stat;

    int i = 0;
    profile_begin();
    while ( (stat = fread(buf, 1, 4000, f)) != 0){
        // cwrite(g, buf, stat);
        i++;
        //  std::cout<<std::string((const char *)buf, stat)<<std::endl;
    } 
    fclose(f);
    // cclose(g);
    profile_end();
}