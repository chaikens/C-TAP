all : Phase1a Phase1b Phase1aPipe

Phase1a : FLIRanalysisPhase1aCamX.cpp Makefile
	g++ -g  -Wno-unused-result $< -o $@

Phase1b : FLIRanalysisPhase1bCamX.cpp Makefile
	g++ -g  -Wno-unused-result $< -o $@

Phase1aPipe : FLIRanalysisPhase1aCamXPipe.cpp Makefile
	g++ -g  -Wno-unused-result $< -o $@



