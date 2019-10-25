.PHONY : build lua test clean

default :
	@echo "======================================="
	@echo "Please use 'make test' command to test it.."
	@echo "Please use 'make build' command to build it.."
	@echo "Please use 'make client' command to test it.."
	@echo "Please use 'make lua' command to build lua C API.."
	@echo "======================================="

CC = cc

LIB += -llua

MICRO += -std=c99 -Wall -I. -I/usr/local/include -L. -L/usr/local/lib

WARNING += -Wall -Wno-unused-variable -Wno-unused-function

build :

	$(CC) -o aoi.so aoi.c -O3 -fPIC --shared -std=c99 -Wall

test :

	$(CC) -o atest test.c aoi.c -O0 -ggdb -std=c99 $(WARNING)

lua :

	$(CC) -o aoi.so lua_aoi.c aoi.c -O3 -fPIC --shared $(MICRO) $(LIB)

clean :

	rm -rf aoi.so atest
