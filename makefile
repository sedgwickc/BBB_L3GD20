GPP=g++
CFLAGS= 
DEBUG=-Wall -g -DDEBUG
LINK= -lmraa

all: gyro

gyro:
	$(GPP) $(CFLAGS) application_L3GD20.cpp Adafruit_L3GD20.cpp -o L3GD20 \
	$(LINK)

gyro_debug:
	$(GPP) $(CFLAGS) $(DEBUG) application_L3GD20.cpp Adafruit_L3GD20.cpp \
	-o L3GD20 $(LINK)

clean:
	rm *.o L3GD20 *.log
