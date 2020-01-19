# mrhc

## os
```
$ uname -a
Linux ubuntu-xenial 4.4.0-166-generic #195-Ubuntu SMP Tue Oct 1 09:35:25 UTC 2019 x86_64 x86_64 x86_64 GNU/Linux
```

## build environment
```
$ sudo apt install libaprutil1-dev
$ sudo apt install libpcre3-dev
$ sudo apt install build-essential
```

## build apache
```
$ ./configure
$ make
$ sudo make install
```

## run apache
```
$ /usr/local/apache2/bin/httpd -v
Server version: Apache/2.4.41 (Unix)
Server built:   Jan 19 2020 20:20:22
$ sudo /usr/local/apache2/bin/httpd -k start

```

## init apache module
```
$ /usr/local/apache2/bin/apxs -g -n mrhc
$ cd mrhc
$ /usr/local/apache2/bin/apxs -c mod_mrhc.c
$ sudo /usr/local/apache2/bin/apxs -i -a mod_mrhc.la
```

## apache config
```
$ sudo vi /usr/local/apache2/conf/httpd.conf
```

```
LoadModule hoge_module        modules/mod_mrhc.so

<Location /mrhc>
  SetHandler mrhc
</Location>
```

```
$ /usr/local/apache2/bin/httpd -t

Syntax OK
$ sudo /usr/local/apache2/bin/httpd -k graceful
```
