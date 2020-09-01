ACLOCAL_AMFLAGS=-I m4
lib_LTLIBRARIES = libzzt_engine_system.la
libzzt_engine_system_la_SOURCES = zzt_engine_system.cpp zztWorld.cpp zztBoard.cpp
libzzt_engine_system_la_CXXFLAGS = -std=c++17 ${LIBECS_CFLAGS} ${JSONCPP_CFLAGS} ${LIBTHE_SEED_CFLAGS}
libzzt_engine_system_la_LDFLAGS = -no-undefined
libzzt_engine_system_la_LIBADD = ${LIBECS_LIBS} ${JSONCPP_LIBS} ${LIBTHE_SEED_LIBS}
