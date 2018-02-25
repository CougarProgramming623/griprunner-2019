CFLAGS=-Werror -Wall -Wno-psabi
OBJS=GripPipeline.o process_pipeline.o
LIBS=-lcurl -lopencv_core -l opencv_imgproc -lopencv_highgui -lntcore -lwpiutil -L ~/ntcore/build/libs/ntcore/shared -L ~/wpiutil/build/libs/wpiutil/shared
INC=-I ~/ntcore/src/main/native/include -I ~/wpiutil/src/main/native/include -I /usr/include/llvm-3.9

GXX=g++ $(CFLAGS) $(INC)

.PHONY: all clean

all: process_pipeline

%.o: %.cpp
	$(GXX) -c -o $@ $<

process_pipeline: $(OBJS)
	$(GXX) $(LIBS) -o $@ $(OBJS)

clean:
	rm -f *.o process_pipeline
	
