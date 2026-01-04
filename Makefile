default: clean

build:
	gcc baseline.c -O2 -Wall -o baseline.out
	gcc sharded.c -O2 -Wall -o sharded.out

baseline: build
	for iteration in 1 2 3 ; do \
		for number in 1 2 4 8 16 32 ; do \
			./baseline.out -t $$number -r 42 >> results/baseline.txt; \
		done; \
	done

sharded: build
	for iteration in 1 2 3 ; do \
		for number in 1 2 4 8 16 32 ; do \
			./sharded.out -t $$number -r 42 >> results/sharded.txt; \
		done; \
	done

clean:
	rm -f results/*
	rm -f *.out

container:
	for iteration in 1 2 3 ; do \
		for i in 1 2; do \
		  docker run --rm --cpus="4" phb:baseline -t 4 >> results/container.txt & \
		done; \
	done; \
	wait

