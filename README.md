# mrhc
Module of Remote Host Computing

## os
```
$ cat /etc/lsb-release
DISTRIB_ID=Ubuntu
DISTRIB_RELEASE=16.04
DISTRIB_CODENAME=xenial
DISTRIB_DESCRIPTION="Ubuntu 16.04.6 LTS"
$ uname -a
Linux ubuntu-xenial 4.4.0-166-generic #195-Ubuntu SMP Tue Oct 1 09:35:25 UTC 2019 x86_64 x86_64 x86_64 GNU/Linux
```

## build environment
```
$ sudo apt install libpcre3-dev
$ sudo apt install build-essential
```

## build apache
```
$ tar xzvf httpd-2.4.41.tar.gz
$ cd httpd-2.4.41
$ CFLAGS="-g" ./configure # add -g option for gdb debug
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

## build apr
```
$ tar xzvf apr-1.7.0.tar.gz
$ cd apr-1.7.0
$ CFLAGS="-g" ./configure
$ make
$ sudo make install
```

## build apr-util
```
$ tar xzvf apr-util-1.6.1.tar.gz
$ cd apr-util-1.6.1
$ CFLAGS="-g" ./configure --with-apr=/usr/local/apr
$ make
$ sudo make install
$ sudo ln -s /usr/local/apr/bin/apu-1-config /usr/bin/apu-1-config
```

## init apache module
```
$ /usr/local/apache2/bin/apxs -g -n mrhc
$ cd mrhc
$ /usr/local/apache2/bin/apxs -c mod_mrhc.c
$ sudo /usr/local/apache2/bin/apxs -i -a -c mod_mrhc.c 
```

## apache config
```
$ sudo vi /usr/local/apache2/conf/httpd.conf
```

```
LoadModule hoge_module        modules/mod_mrhc.so
<IfModule mrhc_module>
  LogFormat "%h %l %u %t \"%r\" %>s %b \"%{mrhc_log}n\"" common
  <Location /mrhc>
    SetHandler mrhc
  </Location>
</IfModule>
```

```
$ /usr/local/apache2/bin/httpd -t

Syntax OK
$ sudo /usr/local/apache2/bin/httpd -k graceful
```

## oepncv
```
$ sudo apt install libopencv-dev
```

## gdb
``
`
$ sudo gdb /usr/local/apache2/bin/httpd
(gdb) b mrhc_handler
Function "mrhc_handler" not defined.
Make breakpoint pending on future shared library load? (y or [n]) y
Breakpoint 1 (mrhc_handler) pending.
(gdb) r -X # run by single process
Starting program: /usr/local/apache2/bin/httpd -X
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
warning: Temporarily disabling breakpoints for unloaded shared library "/usr/local/apache2/modules/mod_mrhc.so"
[New Thread 0x7ffff340e700 (LWP 24571)]
[New Thread 0x7ffff2c0d700 (LWP 24572)]
[New Thread 0x7ffff240c700 (LWP 24573)]
[New Thread 0x7ffff1c0b700 (LWP 24574)]
[New Thread 0x7ffff140a700 (LWP 24575)]
[New Thread 0x7ffff0c09700 (LWP 24576)]
[New Thread 0x7fffebfff700 (LWP 24577)]
[New Thread 0x7fffeb7fe700 (LWP 24578)]
[New Thread 0x7fffeaffd700 (LWP 24579)]
[New Thread 0x7fffea7fc700 (LWP 24580)]
[New Thread 0x7fffe9ffb700 (LWP 24581)]
[New Thread 0x7fffe97fa700 (LWP 24582)]
[New Thread 0x7fffe8ff9700 (LWP 24583)]
[New Thread 0x7fffe87f8700 (LWP 24584)]
[New Thread 0x7fffe7ff7700 (LWP 24585)]
[New Thread 0x7fffe77f6700 (LWP 24586)]
[New Thread 0x7fffe6ff5700 (LWP 24587)]
[New Thread 0x7fffe67f4700 (LWP 24588)]
[New Thread 0x7fffe5ff3700 (LWP 24589)]
[New Thread 0x7fffe57f2700 (LWP 24590)]
[New Thread 0x7fffe4ff1700 (LWP 24591)]
[New Thread 0x7fffe47f0700 (LWP 24592)]
[New Thread 0x7fffe3fef700 (LWP 24593)]
[New Thread 0x7fffe37ee700 (LWP 24594)]
[New Thread 0x7fffe2fed700 (LWP 24595)]
[New Thread 0x7fffe27ec700 (LWP 24596)]
[New Thread 0x7fffe1feb700 (LWP 24597)]
[Thread 0x7ffff340e700 (LWP 24571) exited]
```

```
$ wget http://[ip]/mrhc
```

```
[Switching to Thread 0x7ffff2c0d700 (LWP 24572)]

Thread 3 "httpd" hit Breakpoint 1, mrhc_handler (r=0x7ffff7ef70a0) at mod_mrhc.c:48
48          if (strcmp(r->handler, "mrhc")) {
(gdb)
(gdb) set print pretty on
```

## how to mrhc
```
$ cd mrhc
$ make
$ sudo make install
$ sudo /usr/local/apache2/bin/httpd -k graceful
```

or

```
$ sudo make reload
```

## vnc server
```
$ sudo apt install ubuntu-desktop
$ sudo apt install vnc4server
$ vncpasswd
Password:
Verify:
$ vncserver :1 -rfbport 6624
$ vncserver -kill :1
```

In some circumstances, the desktop screen may be gray.  
Add settings for that situation.  
(see: https://askubuntu.com/questions/800302/vncserver-grey-screen-ubuntu-16-04-lts)  
```
$ echo -e "gnome-panel &\ngnome-settings-daemon &\nmetacity &\nnautilus &" >> ~/.vnc/xstartup
```
