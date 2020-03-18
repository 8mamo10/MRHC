# the used tools
APXS=/usr/local/apache2/bin/apxs
APACHECTL=/usr/local/apache2/bin/apachectl

# apxs's internal values.
APXS_CC=`$(APXS) -q CC`
APXS_TARGET=`$(APXS) -q TARGET`
APXS_CFLAGS=`$(APXS) -q CFLAGS`
APXS_SBINDIR=`$(APXS) -q SBINDIR`
APXS_CFLAGS_SHLIB=`$(APXS) -q CFLAGS_SHLIB`
APXS_INCLUDEDIR=`$(APXS) -q INCLUDEDIR`
APXS_LD_SHLIB=`$(APXS) -q LD_SHLIB`
APXS_LIBEXECDIR=`$(APXS) -q LIBEXECDIR`
APXS_LDFLAGS_SHLIB=`$(APXS) -q LDFLAGS_SHLIB`
APXS_SYSCONFDIR=`$(APXS) -q SYSCONFDIR`
APXS_LIBS_SHLIB=`$(APXS) -q LIBS_SHLIB`

# Macro
PROG=mod_mrhc.so
SRCS=mod_mrhc.cpp vnc_client.cpp d3des.cpp logger.cpp

OBJS=$(SRCS:%.cpp=%.o)
DEPS=$(SRCS:%.cpp=%.d)

CC=g++
INCLUDES=-I$(APXS_INCLUDEDIR) -I/usr/local/apr/include/apr-1 `pkg-config --cflags opencv4`
CFLAGS=$(APXS_CFLAGS) $(APXS_CFLAGS_SHLIB) -Wall -O2
LIBS=`pkg-config --libs opencv4`

# the default target
all: $(PROG)

# link
$(PROG): $(OBJS)
	$(CC) -fPIC -shared -o $@ $^ $(APXS_LIBS_SHLIB) $(LIBS)

%.o: %.cpp
	$(CC) -std=c++11 $(INCLUDES) $(CFLAGS) $< -MM -MP -MF $*.d
	$(CC) -std=c++11 -c -fPIC $(INCLUDES) $(CFLAGS) $<

include $(shell ls $(DEPS) 2>/dev/null)

# install the shared object file into Apache
install: all
	$(APXS) -i -a -n 'mrhc' $(PROG)

# display the apxs variables
check_apxs_vars:
	@echo APXS_CC $(APXS_CC);\
	echo APXS_TARGET $(APXS_TARGET);\
	echo APXS_CFLAGS $(APXS_CFLAGS);\
	echo APXS_SBINDIR $(APXS_SBINDIR);\
	echo APXS_CFLAGS_SHLIB $(APXS_CFLAGS_SHLIB);\
	echo APXS_INCLUDEDIR $(APXS_INCLUDEDIR);\
	echo APXS_LD_SHLIB $(APXS_LD_SHLIB);\
	echo APXS_LIBEXECDIR $(APXS_LIBEXECDIR);\
	echo APXS_LDFLAGS_SHLIB $(APXS_LDFLAGS_SHLIB);\
	echo APXS_SYSCONFDIR $(APXS_SYSCONFDIR);\
	echo APXS_LIBS_SHLIB $(APXS_LIBS_SHLIB)

#   cleanup
clean:
	$(RM) $(PROG) $(OBJS) $(DEPS) $(TEST_TARGET)

#   install and activate shared object by reloading Apache to
#   force a reload of the shared object file
reload: install restart

#   the general Apache start/restart/stop
#   procedures
start:
	$(APACHECTL) start
restart:
	$(APACHECTL) restart
stop:
	$(APACHECTL) stop

# for google test
TEST_DIR=./test
TEST_SRCS=$(TEST_DIR)/gtest_mrhc.cpp
TEST_OBJS=vnc_client.o logger.o d3des.o
TEST_TARGET=$(TEST_DIR)/gtest_mrhc
TEST_LIBS=$(LIBS) -lgtest -lgtest_main -lpthread
TEST_INCLUDES=$(INCLUDES) -I/usr/local/include/gtest -I./

test: $(TEST_TARGET)
$(TEST_TARGET): $(TEST_SRCS) $(OBJS)
	$(CC) $(TEST_SRCS) $(TEST_OBJS) -std=c++11 $(TEST_INCLUDES) $(TEST_LIBS) -o $(TEST_TARGET)
	$(TEST_TARGET)
