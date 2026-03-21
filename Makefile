all : Phase1a Phase1b

Phase1a : FLIRanalysisPhase1aCamX.cpp
	g++ -g  -Wno-unused-result $< -o $@

Phase1b : FLIRanalysisPhase1bCamX.cpp
	g++ -g  -Wno-unused-result $< -o $@


