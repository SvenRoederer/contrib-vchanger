#!/bin/sh
#
#  vchanger-launch-mount.sh ( vchanger v.1.0.2 ) 2017-12-05
#
#  This script is used to run the vchanger-mount-uuid.sh script as
#  a detached process and immediately exit. This is to prevent delays
#  when invoked by a udev rule. 
#
VCHANGER_MOUNT=/usr/libexec/vchanger/vchanger-mount-uuid.sh
nohup $VCHANGER_MOUNT $1 </dev/null >/dev/null 2>&1 &
