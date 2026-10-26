CXX=clang
LD=$(CXX)

CXXFLAGS=-std=c++23 -fcxx-modules
LDFLAGS=-lstdc++

.PHONY: all clean

MAINS=$(wildcard step*.cpp)
TARGETS=$(MAINS:%.cpp=%)

all: $(TARGETS)

$(TARGETS): %: %.o mal/mal.o
	$(LD) $^ -o $@ $(LDFLAGS)

mal/mal.pcm: mal/mal.cppm mal/mal.hpp mal/linenoise.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) --precompile $< -o $@

mal/mal.o: mal/mal.pcm
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp mal/mal.pcm
	$(CXX) $(CXXFLAGS) -fmodule-file=mal=mal/mal.pcm -c $< -o $@

clean:
	rm -f *.o mal/*.o mal/*.pcm $(TARGETS)
