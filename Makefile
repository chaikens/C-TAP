CXXFLAGS = -O3 -Wno-unused-result
#CXXFLAGS = -g 

all : Phase1a Phase1b

Phase1a : FLIRanalysisPhase1aCamX.cpp Makefile
	$(CXX) $(CXXFLAGS) $< -o $@

Phase1b : FLIRanalysisPhase1bCamX.cpp Makefile
	$(CXX) $(CXXFLAGS)  $< -o $@


