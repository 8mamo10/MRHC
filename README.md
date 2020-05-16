# MRHC
Module of Remote Host Computing  

[![MRHC](https://github.com/8mamo10/mrhc/workflows/MRHC/badge.svg)](https://github.com/8mamo10/mrhc/actions?query=workflow%3AMRHC)

## what's this
MRHC is apache module that provides VNC over HTTP.  
Using apr + opencv.  

<img src="https://user-images.githubusercontent.com/562105/76103304-ce997f80-6014-11ea-9897-d07f14697cfd.png" width="320px">

MRHC reads as `Murofushi`.  
This name carries the wish and desire to throw out PC operations.  

## os
Support Ubuntu 16.04 for now. (I have not tested the operation with others)  

```
$ cat /etc/lsb-release
DISTRIB_ID=Ubuntu
DISTRIB_RELEASE=16.04
DISTRIB_CODENAME=xenial
DISTRIB_DESCRIPTION="Ubuntu 16.04.6 LTS"
$ uname -a
Linux ubuntu-xenial 4.4.0-166-generic #195-Ubuntu SMP Tue Oct 1 09:35:25 UTC 2019 x86_64 x86_64 x86_64 GNU/Linux
```

## setup build environment
```
$ cd mrhc
$ ./script/setup.sh
```

## build mrhc
```
$ cd mrhc
$ make
```

## how to mrhc
Install apache2 beforehand, like this.  
```
$ sudo apt install apache2
```

Build and install mrhc.  
```
$ cd mrhc
$ sudo make install
$ sudo make start
```

Then, access to http://[your host]/mrhc from browser.  
You're required to authenticate by basic auth.  
Please enter in the following format.  
```
username: [vnc_host]:[vnc_port]
password: [vnc_password]
```

## vnc server
```
$ sudo apt install ubuntu-desktop # optional, if you need rich gui
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

## how to test
Using real vnc server and some fake simulating script of vnc server.  
```
$ cd test
$ vncserver :1 -rfbport 6624; perl fake_vnc_server_rfb_3_3.pl &; perl fake_vnc_server_rfb_3_8.pl &;
$ make test
$ vncserver -kill :1
```
