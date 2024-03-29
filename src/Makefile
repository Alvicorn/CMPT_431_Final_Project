ifdef USE_INT
MACRO = -DUSE_INT
endif

# compiler setup
CXX = g++
MPICXX = mpic++
CXXFLAGS = -std=c++14 -O3 $(MACRO)

COMMON = core/cxxopts.h core/get_time.h
SERIAL = all_pairs_serial
PARALLEL = all_pairs_parallel
DISTRIBUTED = all_pairs_distributed
ALL = $(SERIAL) $(PARALLEL) $(DISTRIBUTED)

all: $(ALL)

$(SERIAL): %: %.cpp 
	$(CXX) $(CXXFLAGS) -o $@ $<

$(PARALLEL): %: %.cpp 
	$(CXX) $(CXXFLAGS) -o $@ $<

$(DISTRIBUTED): %: %.cpp 
	$(MPICXX) $(CXXFLAGS) -o $@ $<

.PHONY: clean clean_windows

clean: 
	rm -f *o *obj $(ALL)

clean_windows:
	del /Q /F *.exe *.o