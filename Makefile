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

# the default target
all: mod_mrhc.so

# compile
mod_mrhc.o: mod_mrhc.cpp
	g++ -std=c++0x -c -fPIC -I$(APXS_INCLUDEDIR) -I/usr/include/apr-1.0/ -I/usr/local/apr/include/apr-1 $(APXS_CFLAGS) $(APXS_CFLAGS_SHLIB) -Wall -o $@ $<

vnc_client.o: vnc/vnc_client.cpp
	g++ -std=c++0x -c -fPIC -I$(APXS_INCLUDEDIR) -I/usr/include/apr-1.0/ -I/usr/local/apr/include/apr-1 $(APXS_CFLAGS) $(APXS_CFLAGS_SHLIB) -Wall -o $@ $<

# link
mod_mrhc.so: mod_mrhc.o vnc_client.o
	g++ -fPIC -shared -o $@ $^ $(APXS_LIBS_SHLIB)
#	g++ -fPIC -shared -o $@ $< $(APXS_LIBS_SHLIB)

# install the shared object file into Apache
install: all
	$(APXS) -i -a -n 'mrhc' mod_mrhc.so

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
	-rm -f *.so *.o *~

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
