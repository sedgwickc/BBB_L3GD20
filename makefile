GPP=g++
CFLAGS= -lmraa
DEBUG=-Wall -g -DDEBUG

all: gyro

gyro:
	$(GPP) $(CFLAGS) application_L3GD20.cpp Adafruit_L3GD20.cpp -o L3GD20

gyro_debug:
	$(GPP) $(CFLAGS) $(DEBUG) application_L3GD20.cpp Adafruit_L3GD20.cpp -o L3GD20

clean:
	rm *.o L3GD20 *.log
