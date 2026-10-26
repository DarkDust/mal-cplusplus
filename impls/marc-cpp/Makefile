CXX=clang
LD=$(CXX)

CXXFLAGS=-std=c++23
LDFLAGS=-lstdc++

.PHONY: all clean

MAINS=$(wildcard step*.cpp)
TARGETS=$(MAINS:%.cpp=%)

all: $(TARGETS)

$(TARGETS): %: %.o
	$(LD) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGETS)
