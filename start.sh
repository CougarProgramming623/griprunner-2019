#!/bin/bash
export LD_LIBRARY_PATH=/home/debian/wpiutil/build/libs/wpiutil/shared:/home/debian/ntcore/build/libs/ntcore/shared
/home/debian/griprunner/start-streamer.sh &
/home/debian/griprunner/process_pipeline &
