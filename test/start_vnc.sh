#!/bin/sh

vncserver :1 -rfbport 6624
perl ./test/fake_vnc_server_rfb_3_3.pl &
perl ./test/fake_vnc_server_rfb_3_8.pl &
