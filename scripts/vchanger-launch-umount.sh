#!/bin/sh
#
#  vchanger-launch-umount.sh ( vchanger v.1.0.2 ) 2017-12-05
#
#  This script is used to run the vchanger-umount-uuid.sh script in
#  another [background] process launched by the at command in order
#  to prevent delays when invoked by a udev rule. 
#
VCHANGER_UMOUNT=/usr/libexec/vchanger/vchanger-umount-uuid.sh
nohup $VCHANGER_UMOUNT $1 </dev/null >/dev/null 2>&1 &
