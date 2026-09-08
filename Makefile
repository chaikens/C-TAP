all : F1a.orig F1a.all F1a.orig.backwards Phase1bPipeOpt semaphores

#CXXFLAGS = -O3
CXXFLAGS = -g

semaphores:
	$(MAKE) -C src/semaphores

F1a.orig : F1a.orig.cpp BitA.h EZbtraps.h Makefile
	$(CXX) $(CXXFLAGS)   -Wno-unused-result $< -o $@

F1a.all : F1a.all.cpp BitA.h  EZbtraps.h Makefile
	$(CXX) $(CXXFLAGS)   -Wno-unused-result $< -o $@

F1a.orig.backwards : F1a.orig.backwards.cpp BitA.h Makefile  EZbtraps.h 
	$(CXX) $(CXXFLAGS)   -Wno-unused-result $< -o $@

Phase1bPipeOpt : FLIRanalysisPhase1bCamXPipeOpt.cpp Makefile  EZbtraps.h 
	$(CXX) $(CXXFLAGS)   -Wno-unused-result $< -o $@
