TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../../include
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -I/usr/local/include
endif
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Linux)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Darwin)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Solaris)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), MINGW32)
	CXXFLAGS += /EHsc /GR- /MP /nologo /wd"4100" /wd"4127" /wd"4290" /wd"4355" /wd"4512"
endif
ifneq ($(COVERAGE),)
	CXXFLAGS += -fprofile-arcs -ftest-coverage
	LDFLAGS += -fprofile-arcs -ftest-coverage -lgcov
else ifneq ($(RELEASE),)
	CXXFLAGS += -O2
else
	CXXFLAGS += -g -D_DEBUG
endif



LDFLAGS += -L../../../lib/yield
ifeq ($(UNAME), FreeBSD)
	LDFLAGS += -L/usr/local/lib
endif
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Darwin)
	LIBS += -liconv
endif
ifeq ($(UNAME), FreeBSD)
	LIBS += -liconv -lintl
endif
ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -liconv -lnsl -lsocket -lm -lrt -lstdc++
endif
ifeq ($(UNAME), MINGW32)
	LIBS += -lgtestd
endif
LIBS += -lyield_fs -lyield_i18n -lyield_sockets -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/sockets -name "*.d")


O_FILE_PATHS += ../../../build/yield/sockets/datagram_socket_test.o ../../../build/yield/sockets/socket_address_test.o ../../../build/yield/sockets/stream_socket_test.o ../../../build/yield/sockets/tcp_socket_test.o ../../../build/yield/sockets/yield_sockets_test_main.o


all: ../../../bin/yield/yield_sockets_test

clean:
	$(RM) ../../../bin/yield/yield_sockets_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)

lcov: ../../../bin/yield/yield_sockets_test
	lcov --directory ../../../build/yield/sockets --zerocounters
	-../../../bin/yield/yield_sockets_test
	lcov --base-directory . --directory ../../../build/yield/sockets --capture --output-file yield.sockets_test_lcov-$(TIMESTAMP)
	mkdir yield.sockets_test_lcov_html-$(TIMESTAMP)
	genhtml -o yield.sockets_test_lcov_html-$(TIMESTAMP) yield.sockets_test_lcov-$(TIMESTAMP)
	-cp -R yield.sockets_test_lcov_html-$(TIMESTAMP) /mnt/hgfs/minorg/Desktop
	zip -qr yield.sockets_test_lcov_html-$(TIMESTAMP).zip yield.sockets_test_lcov_html-$(TIMESTAMP)/*
	rm -fr yield.sockets_test_lcov_html-$(TIMESTAMP)


../../../lib/yield/libyield_fs.a:
	$(MAKE) -C ../fs yield.fs.Makefile


../../../lib/yield/libyield_sockets.a:
	$(MAKE) -C . yield.sockets.Makefile


../../../bin/yield/yield_sockets_test: $(O_FILE_PATHS) ../../../lib/yield/libyield_fs.a ../../../lib/yield/libyield_sockets.a
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../../build/yield/sockets/datagram_socket_test.o: ../../../test/yield/sockets/datagram_socket_test.cpp
	-mkdir -p ../../../build/yield/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/datagram_socket_test.o -MD $(CXXFLAGS) ../../../test/yield/sockets/datagram_socket_test.cpp

../../../build/yield/sockets/socket_address_test.o: ../../../test/yield/sockets/socket_address_test.cpp
	-mkdir -p ../../../build/yield/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/socket_address_test.o -MD $(CXXFLAGS) ../../../test/yield/sockets/socket_address_test.cpp

../../../build/yield/sockets/stream_socket_test.o: ../../../test/yield/sockets/stream_socket_test.cpp
	-mkdir -p ../../../build/yield/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/stream_socket_test.o -MD $(CXXFLAGS) ../../../test/yield/sockets/stream_socket_test.cpp

../../../build/yield/sockets/tcp_socket_test.o: ../../../test/yield/sockets/tcp_socket_test.cpp
	-mkdir -p ../../../build/yield/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/tcp_socket_test.o -MD $(CXXFLAGS) ../../../test/yield/sockets/tcp_socket_test.cpp

../../../build/yield/sockets/yield_sockets_test_main.o: ../../../test/yield/sockets/yield_sockets_test_main.cpp
	-mkdir -p ../../../build/yield/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/yield_sockets_test_main.o -MD $(CXXFLAGS) ../../../test/yield/sockets/yield_sockets_test_main.cpp
