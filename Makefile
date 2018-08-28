all: main-hwloc

main-hwloc: main-with-hwloc.c
	gcc -Wall -o $@ $< -lhwloc

clean:
	rm -f main-hwloc
