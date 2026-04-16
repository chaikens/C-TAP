all : Phase1a Phase1b

Phase1a : FLIRanalysisPhase1aCamX.cpp Makefile
	g++ -g  -Wno-unused-result $< -o $@

Phase1b : FLIRanalysisPhase1bCamX.cpp Makefile
	g++ -g  -Wno-unused-result $< -o $@


