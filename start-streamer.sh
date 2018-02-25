#!/bin/bash

# Full resolution for the MS LifeCam HD-3000 is 1280x720
RESOLUTION="-r 960x544"
FPS="-f 20"
YUYV="-y"
QUALITY="-q 10" # 0 .. 10
CONTRAST="-co 5" #0 .. 10
SHARPNESS="-sh 50" # 0 .. 50
COLORBALANCE="-cb 90"
WHITEBALANCE="-wb 2800" # 2800 .. 10000
# Possible exposure values for the Microsoft LifeCam HD-3000/5000:
# 5, 9-10 (same exposure?), 19-20 (same exposure?) 39, 78, 156, 312, 625, 1250,
# 2500, 5000, 10000
# (per http://comments.gmane.org/gmane.linux.drivers.uvc.devel/5717)
EXPOSURE="-ex 9"
BACKLIGHT="-bk 0"
CHROMAGAIN="-cagc 2800" # 2800 .. 10000

OPTIONS="$RESOLUTION $FPS $QUALITY $CONTRAST $SHARPNESS \
	$COLORBALANCE $WHITEBALANCE $EXPOSURE $BACKLIGHT $CHROMAGAIN"

mjpg_streamer -i "input_uvc.so -d /dev/video0 $OPTIONS" -o "output_http.so -p 7777 -w /usr/share/mjpg-streamer/www"
