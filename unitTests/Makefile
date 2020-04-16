CC = gcc
CFLAGS = -g -Wall -lm -I../../jTypes --coverage

all: generateTestObjCode main.c ../../jTypes/jTypes.h ../../jTypes/jTypes.c
	$(CC) $(CFLAGS) -include ../../studiousBroccoli/utils/testMallocTracker/malloc_redefinition.h main.c ../../jTypes/jTypes.c -L ../../studiousBroccoli/utils/testMallocTracker  -Wl,-rpath=$(shell pwd)/../../studiousBroccoli/utils/testMallocTracker -ltestMallocTracker -I../../studiousBroccoli/utils/testMallocTracker -o unitTests
	./unitTests
	gcov main.c

generateTestObjCode: ../templates/listTemplate.h ../templates/listTemplate.inc
	mkdir -p listHeaders listCode
	../generateList.py listHeaders listCode testobj:testobj

clean:
	rm -r unitTests *.gcov *.gcno *.gcda listCode listHeaders
