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
GENERATOR = input_generator
CXX_UNIT_TESTS = tests/test_serial_utils
ALL = $(SERIAL) $(PARALLEL) $(DISTRIBUTED) $(GENERATOR)


all: $(ALL)

$(SERIAL): %: %.cpp 
	$(CXX) $(CXXFLAGS) -o $@ $<

$(PARALLEL): %: %.cpp 
	$(CXX) $(CXXFLAGS) -o $@ $<

$(DISTRIBUTED): %: %.cpp 
	$(MPICXX) $(CXXFLAGS) -o $@ $<

$(GENERATOR): %: %.cpp 
	$(CXX) $(CXXFLAGS) -o $@ $<

$(CXX_UNIT_TESTS): %: %.cpp
		$(CXX) $(CXXFLAGS) -o $@ $<


tests: $(CXX_UNIT_TESTS) $(SERIAL)
	python tests/tests.py

.PHONY: clean clean_windows

clean: 
	rm -f *o *obj $(ALL)

clean_windows:
	del /Q /F *.exe *.o