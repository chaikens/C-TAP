
all: pugixml.o install

pugixml.o : pugixml.cpp pugiconfig.hpp pugixml.hpp
	g++  -c pugixml.cpp

install:
	cp pugixml.o pugiconfig.hpp pugixml.hpp ../.. 

.phony : all install


