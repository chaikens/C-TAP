all : Phase1aPipeOpt Phase1bPipeOpt

CXXFLAGS = -O3
#CXXFLAGS = -g

Phase1aPipeOpt : FLIRanalysisPhase1aCamXPipeOpt.cpp Makefile
	$(CXX) $(CXXFLAGS)   -Wno-unused-result $< -o $@

Phase1bPipeOpt : FLIRanalysisPhase1bCamXPipeOpt.cpp Makefile
	$(CXX) $(CXXFLAGS)   -Wno-unused-result $< -o $@
