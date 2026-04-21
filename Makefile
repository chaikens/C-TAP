all : Phase1a Phase1b Phase1aPipe Phase1bPipe

CXXFLAGS = -Wno-unused-result -O3 #-g

Phase1a : FLIRanalysisPhase1aCamX.cpp Makefile
	$(CXX) $(CXXFLAGS)  -Wno-unused-result $< -o $@

Phase1b : FLIRanalysisPhase1bCamX.cpp Makefile
	$(CXX) $(CXXFLAGS)  -Wno-unused-result $< -o $@

Phase1aPipe : FLIRanalysisPhase1aCamXPipe.cpp Makefile
	$(CXX) $(CXXFLAGS)   -Wno-unused-result $< -o $@

Phase1bPipe : FLIRanalysisPhase1bCamXPipe.cpp Makefile
	$(CXX) $(CXXFLAGS)   -Wno-unused-result $< -o $@



