CXX = g++
CXXFLAGS = -std=c++11 -O3 -pedantic -Wall -Wextra -Werror
CXXDEBUG = -std=c++11 -O0 -g -pedantic -Wall -Wextra -Werror

.PHONY: do

do: src/main.cpp
	$(CXX) $(CXXFLAGS) src/* -o proj-asa

debug: src/main.cpp
	$(CXX) $(CXXDEBUG) src/* -o proj-asa
