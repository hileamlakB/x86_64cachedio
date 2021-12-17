# x86_64cachedio
This simple liberary provides speciallized reading cache for x86_64 architecture. It is a speciallized replacment for the stdio and works better for some specific cases. It works for regular files, and performs better than stdio apis like `fread` with reading patterns like reverse and strided. 

Currently only the read api is implemented.

# The read api

The read api uses fully associated aligned caches to improve different access paterns.

# Future todos

- [ ] Implement spetialized cache for writing too
- [ ] A read and write cache that compromises efficeny
- [ ] A seek api
- [ ] Fuzz test
