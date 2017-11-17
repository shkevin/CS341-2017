CC = gcc 
OPT = -O3 -mfpmath=sse -g -funroll-all-loops
#OPT = -O1  -mfpmath=sse -g
CFLAGS += -Wall -std=gnu99 -DGETTIMEOFDAY $(OPT)
LDLIBS = -lgslcblas

types = naive basic row col blocked rb
targets = $(patsubst %,benchmark_%,$(types))
objects = benchmark.o $(patsubst %,dgemm_%.o,$(types))

.PHONY : default
default : all

.PHONY : all
all : $(targets) 

objects : $(objects)

benchmark_% : benchmark.o dgemm_%.o 
	$(CC) $(OPT) -o $@ $^ $(LDLIBS)

%.o : %.c
	$(CC) -c $(CFLAGS) $<

.PHONY : clean
clean:
	rm -f $(targets) $(objects) *.stdout

%.o : %.c
	$(CC) -c $(CFLAGS) $<
