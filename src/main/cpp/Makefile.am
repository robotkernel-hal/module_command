ACLOCAL_AMFLAGS = -I m4 

lib_LTLIBRARIES = libmodule_command.la

libmodule_command_la_SOURCES  = command.cpp
libmodule_command_la_CXXFLAGS = -Wall -I$(srcdir) -I$(builddir) \
                                           @ROBOTKERNEL_CFLAGS@
libmodule_command_la_LDFLAGS  = -Wall -release $(PACKAGE_VERSION)

