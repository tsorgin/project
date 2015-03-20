LDFLAGS = -lm
CFLAGS= -O3 -Wall
CXXFLAGS= -O3 -Wall
CC=g++
RM=rm

PROGRAM_NAME= mandelbox

$(PROGRAM_NAME): main.o print.o timing.o savebmp.o getparams.o 3d.o getcolor.o distance_est.o mandelboxde.o raymarching.o renderer.o init3D.o getframedat.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

run: $(PROGRAM_NAME)
	./$(PROGRAM_NAME)$(EXEXT) params.dat

clean:
	$(RM) -rf *.o $(PROGRAM_NAME)$(EXEEXT) *~

all-clean:
	rm -rf *.bmp log.dat
