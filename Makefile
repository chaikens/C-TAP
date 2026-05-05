all : Phase1a Phase1b Phase1aPipe Phase1bPipe Phase1aPipeOpt Phase1bPipeOpt

#CXXFLAGS = -O3
CXXFLAGS = -g

Phase1a : FLIRanalysisPhase1aCamX.cpp Makefile
	$(CXX) $(CXXFLAGS)  -Wno-unused-result $< -o $@

Phase1b : FLIRanalysisPhase1bCamX.cpp Makefile
	$(CXX) $(CXXFLAGS)  -Wno-unused-result $< -o $@

Phase1aPipe : FLIRanalysisPhase1aCamXPipe.cpp Makefile
	$(CXX) $(CXXFLAGS)   -Wno-unused-result $< -o $@

Phase1bPipe : FLIRanalysisPhase1bCamXPipe.cpp Makefile
	$(CXX) $(CXXFLAGS)   -Wno-unused-result $< -o $@

Phase1aPipeOpt : FLIRanalysisPhase1aCamXPipeOpt.cpp Makefile
	$(CXX) $(CXXFLAGS)   -Wno-unused-result $< -o $@

Phase1bPipeOpt : FLIRanalysisPhase1bCamXPipeOpt.cpp Makefile
	$(CXX) $(CXXFLAGS)   -Wno-unused-result $< -o $@
