rm -rf php_compile
cp -r php_module php_compile
cd php_compile
phpize
aclocal
libtoolize --force
autoheader
autoconf
./configure --enable-mvd_parsing
make
make install
cd ..
