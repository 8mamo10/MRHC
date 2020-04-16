#!/bin/sh

sudo apt update
sudo apt install expect
sudo apt install vnc4server
expect -c "
spawn vncpasswd
expect \"Password:\"
send -- \"testtest\n\"
expect \"Verify:\"
send -- \"testtest\n\"
"
