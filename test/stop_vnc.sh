#!/bin/sh

vncserver -kill :1
kill `ps aux | grep "fake_vnc_server_rfb_3_3.pl" | grep -v grep | awk '{ print $2 }'`
kill `ps aux | grep "fake_vnc_server_rfb_3_8.pl" | grep -v grep | awk '{ print $2 }'`
