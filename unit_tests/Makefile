CC = cc
#CFLAGS = -std=c99 -Wall -Werror -g -O0
CFLAGS = -std=c99 -Wall -g -O0


#
# build third-party libraries
#
SUBDIRS = 3rd-party

ifdef SUBDIRS
.PHONY : $(SUBDIRS)
$(SUBDIRS) : 
	@if [ -d $@ ]; then \
		$(MAKE) --no-print-directory --directory=$@ \
			CC=$(CC) CFLAGS="$(CFLAGS)" $(MAKECMDGOALS); \
	fi	
endif


THIRD_PARTY_DIR = 3rd-party
CMOCKA_INCLUDE_DIR = $(THIRD_PARTY_DIR)/build-Debug/include
CMOCKA_LIBRARY = $(THIRD_PARTY_DIR)/build-Debug/lib/libcmocka.a

cmocka : $(SUBDIRS)

CFLAGS += -I $(CMOCKA_INCLUDE_DIR)

#
# quick and dirty build rules to demonstrate testing, not good build
# system practices.
#

#
# library
#
LIB_SRCS = \
	I2C_temp.c 				\
	I2C_light.c 			


LIB_OBJS = $(LIB_SRCS:%.c=%.o)

LIB_HEADERS = $(LIB_SRCS:%.c=%.h)

LIB = libutils.a

TEST_SRCS =	Project1UnitTests.c

#
# tests
#
#ifeq ($(FILE), DOUBLE_LL)
#TEST_SRCS =	DoublyLinkedListUnitTest.c  
#else ifeq ($(FILE), CIRC_BUF)
#TEST_SRCS =	CircularBufferUnitTests.c  
#else ifeq ($(FILE), I2C)
#TEST_SRCS =	Project1UnitTests.c   
#endif

TEST_OBJS = $(TEST_SRCS:%.c=%.o)

TEST_EXE = test_result.out


#
# Generic rule to generate objects
#
%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(LIB) : $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^

all : $(LIB) $(EXE)

$(EXE) : $(EXE_OBJS) $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lm

test : $(TEST_EXE)
	./$(TEST_EXE)

$(TEST_EXE) : $(TEST_SRCS) $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ $(CMOCKA_LIBRARY) -lm


.PHONY : clean
clean : FORCE
	-rm -rf *~ *.o $(LIB) $(EXE) $(TEST_EXE) *.dSYM/

.PHONY : clobber
clobber : $(SUBDIRS) clean


FORCE :
