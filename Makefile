CXX = g++
CXXFLAGS = -std=c++11 -O3 -pedantic -Wall -Wextra -Werror

do: src/main.cpp
	$(CXX) $(CXXFLAGS) src/* -o proj-asa
