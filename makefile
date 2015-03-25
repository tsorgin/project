# to compile with pgcc 
# CXX=pgCC
# CC=pgcc
# to compile with gcc
#CXX=g++
#CC =gcc
CXX=mpic++
CC=mpicc
CFLAGS= -std=c99 -O2
CXXFLAGS= -O2

PROGRAM_NAME= mandelbox

$(PROGRAM_NAME): main.o print.o timing.o savebmp.o getparams.o 3d.o getcolor.o distance_est.o mandelboxde.o raymarching.o renderer.o init3D.o getframedat.o writeFrameData.o Quaternion.o
	$(CXX) -o $@ $^ $(CFLAGS) $(LDFLAGS)

run-serial: $(PROGRAM_NAME)
	./$(PROGRAM_NAME)$(EXEXT) params.dat

run-16: $(PROGRAM_NAME)
	mpirun -np 16 ./$(PROGRAM_NAME)$(EXEXT) params.dat

run-32: $(PROGRAM_NAME)
	mpirun -np 32 ./$(PROGRAM_NAME)$(EXEXT) params.dat

clean:
	rm *.o $(PROGRAM_NAME)$(EXEEXT) *~
