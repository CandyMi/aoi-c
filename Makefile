.PHONY : build lua test clean

default :
	@echo "======================================="
	@echo "Please use 'make lua' command to test it.."
	@echo "Please use 'make test' command to test it.."
	@echo "Please use 'make build' command to build it.."
	@echo "Please use 'make client' command to test it.."
	@echo "======================================="

CC = cc

WARNING = -Wall -Wno-unused-variable -Wno-unused-function

build :

	$(CC) -o aoi.so aoi.c -O3 -fPIC --shared -std=c99 -Wall

test :

	$(CC) -o atest test.c aoi.c -O0 -std=c99 -ggdb $(WARNING)

clean :

	rm -rf aoi.so atest
