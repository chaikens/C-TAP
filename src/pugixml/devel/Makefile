demo.txt : mygenwalk CamSett.xml
	./mygenwalk CamSett.xml > demo.txt

mygenwalk : mygenwalk.cpp ../pugixml.o 
	$(CXX) -I..   mygenwalk.cpp ../pugixml.o -o mygenwalk


../pugixml.o :
	$(MAKE) -C ..
