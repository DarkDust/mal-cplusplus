CXX=clang
LD=$(CXX)

CXXFLAGS=-std=c++23 -fcxx-modules
LDFLAGS=-lstdc++

.PHONY: all clean

MAINS=$(wildcard step*.cpp)
TARGETS=$(MAINS:%.cpp=%)

MAL_IMPLS=$(wildcard mal/*.cpp)
MAL_OBJECTS=mal/mal.o $(MAL_IMPLS:%.cpp=%.o)

all: $(TARGETS)

$(TARGETS): %: %.o $(MAL_OBJECTS)
	$(LD) $^ -o $@ $(LDFLAGS)

mal/mal.pcm: mal/mal.cppm mal/mal.hpp mal/linenoise.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) --precompile $< -o $@

mal/mal.o: mal/mal.pcm
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


%.o: %.cpp mal/mal.pcm
	$(CXX) $(CXXFLAGS) -fmodule-file=mal=mal/mal.pcm -c $< -o $@

clean:
	rm -f *.o mal/*.o mal/*.pcm $(TARGETS)
