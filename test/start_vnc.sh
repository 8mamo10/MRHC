#!/bin/sh
expect -c "
spawn vncserver :1 -rfbport 6624
expect \"Password:\"
send -- \"testtest\n\"
expect
"
perl ./test/fake_vnc_server_rfb_3_3.pl &
perl ./test/fake_vnc_server_rfb_3_8.pl &
