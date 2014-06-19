#/bin/sh
./configure   CC=${CROSS_COMPILE}gcc --with-shared   --disable-pam  --enable-openpty   --enable-syslog    --disable-lastlog  --disable-utmp  --disable-utmpx  --disable-wtmp  --disable-wtmpx   --disable-loginfunc   --disable-pututline   --disable-pututxline   --disable-zlib   --enable-bundled-libtom --build=x86  --host=mips-linux
