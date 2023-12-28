#The pound sign is a comment
#Author:	Myra
#Program:  	Mission
#  Machine used: 	nogales linux C++
#  Documentation for running this program and its capabilities
#  can be found in README
#
#CC=/opt/gcc-3.4.2/bin/g++
CC=g++
#-g flag for gdb and -pg for gprof
#FLAGS=-c -O3 -g
#FLAGS=-c -O3 -m32
FLAGS=-c -O3 -lpthread
SMT_LIB = -I../../z3-master/src/api -I../../z3-master/src/api/c++
SMT_SO =  ../../z3-master/build/libz3.so
#LINKTO= -m32 -g -o  maetg
#LINKTO= -m32  -o  maetg
LINKTO=  -o  maetg
#  Note... you need to TAB before $(CC) or you will get an error
# -L$(LIBRARY)

#add -pg to below command if profiling
maetg: cover.o util.o used.o tiebreaker.o solve_smt.o main.o
	$(CC)	$(LINKTO)  cover.o  util.o  used.o tiebreaker.o main.o solve_smt.o $(SMT_SO) 
cover.o: cover.C  cover.h util.h
	$(CC)   $(FLAGS) cover.C $(SMT_LIB)
tiebreaker.o: tiebreaker.C tiebreaker.h
	$(CC)	$(FLAGS)  tiebreaker.C
util.o: util.C  util.h
	$(CC)   $(FLAGS) util.C $(SMT_LIB)
used.o: used.C  used.h
	$(CC)   $(FLAGS) used.C  $(SMT_LIB)
solve_smt.o: solve_smt.cpp
	$(CC) 	$(FLAGS) solve_smt.cpp $(SMT_LIB)
main.o: main.C cover.h util.h used.h tiebreaker.h
	$(CC) 	$(FLAGS) main.C $(SMT_LIB)

clean:
	rm -f *.o *~ core
	rm core.*

