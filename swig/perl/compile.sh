cp ../mvd.i /usr/local/include/mvdparse.h ./
swig -perl mvd.i
gcc -fpic -c mvdparse_perl.c mvd_wrap.c -Dbool=char -I/usr/lib/perl5/5.12.2/i486-linux/CORE `perl -e 'use Config; print $Config{ccflags};'`
gcc -shared -Wl,-soname,mvd -o mvd.so *.o -lperl -lmvdparse

