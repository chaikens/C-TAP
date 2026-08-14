all: BabyFilter BabyFilterCenterCircle

CXXFLAGS = -O3

BabyFilter : BabyFilter.cpp BlobGetter.h MyYUV.h Makefile
	$(CXX) $(CXXFLAGS) $< -o $@ 


BabyFilterCenterCircle : BabyFilterCenterCircle.cpp BlobGetter.h MyYUV.h Makefile
	$(CXX) $(CXXFLAGS) $< -o $@ 
