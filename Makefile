default: clean

build:
	gcc baseline.c -O2 -Wall -o baseline.out
	gcc sharded.c -O2 -Wall -o sharded.out

clean:
	rm -f results/*
	rm -f *.out

baseline:
	for iteration in 1 2 3 ; do \
		for number in 1 2 4 8 16 32 ; do \
			./baseline.out -t $$number -r 42 >> results/baseline.txt; \
		done; \
	done

baseline-N: 
	for iteration in 1 2 3 ; do \
		for number in 1 2 ; do \
			./baseline.out -t 4 -r 42 >> results/baseline-N.txt & \
		done; \
		wait; \
	done

baseline-2N: 
	for iteration in 1 2 3 ; do \
		for number in 1 2 3 4; do \
			./baseline.out -t 4 -r 42 >> results/baseline-2N.txt & \
		done; \
		wait; \
	done

baseline-4N: 
	for iteration in 1 2 3 ; do \
		for number in 1 2 3 4 5 6 7 8; do \
			./baseline.out -t 4 -r 42 >> results/baseline-4N.txt & \
		done; \
		wait; \
	done

sharded:
	for iteration in 1 2 3 ; do \
		for number in 1 2 4 8 16 32 ; do \
			./sharded.out -t $$number -r 42 >> results/sharded.txt; \
		done; \
	done

sharded-N2: 
	for iteration in 1 2 3 ; do \
		for number in 1; do \
			./sharded.out -t 4 -r 42 >> results/sharded-N2.txt & \
		done; \
		wait; \
	done

sharded-N: 
	for iteration in 1 2 3 ; do \
		for number in 1 2; do \
			./sharded.out -t 4 -r 42 >> results/sharded-N.txt & \
		done; \
		wait; \
	done

sharded-2N: 
	for iteration in 1 2 3 ; do \
		for number in 1 2 3 4; do \
			./sharded.out -t 4 -r 42 >> results/sharded-2N.txt & \
		done; \
		wait; \
	done

sharded-4N: 
	for iteration in 1 2 3 ; do \
		for number in 1 2 3 4 5 6 7 8; do \
			./sharded.out -t 4 -r 42 >> results/sharded-4N.txt & \
		done; \
		wait; \
	done

container-N:
	for iteration in 1 2 3 ; do \
		for i in 1 2; do \
		  docker run --rm --cpus="4" phb:baseline -t 4 -r 42 >> results/container-N.txt & \
		done; \
		wait; \
	done

container-2N:
	for iteration in 1 2 3 ; do \
		for i in 1 2 3 4; do \
		  docker run --rm --cpus="4" phb:baseline -t 4 -r 42 >> results/container-2N.txt & \
		done; \
		wait; \
	done

container-4N:
	for iteration in 1 2 3 ; do \
		for i in 1 2 3 4 5 6 7 8; do \
		  docker run --rm --cpus="4" phb:baseline -t 4 -r 42 >> results/container-4N.txt & \
		done; \
		wait; \
	done


N: baseline-N sharded-N container-N

2N: baseline-2N sharded-2N container-2N

4N: baseline-4N sharded-4N container-4N

all: baseline sharded N 2N 4N 
