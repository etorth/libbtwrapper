# =====================================================================================
#
#       Filename: Makefile
#        Created: 10/22/2017 11:16:31
#  Last Modified: 10/23/2017 01:34:21
#
#    Description: 
#
#        Version: 1.0
#       Revision: none
#       Compiler: gcc
#
#         Author: ANHONG
#          Email: anhonghe@gmail.com
#   Organization: USTC
#
# =====================================================================================

CXX      := g++
CXXFLAGS := -std=c++11 -g -Wall -Werror -funwind-tables

.PHONY: all clean

all:
	$(CXX) -fPIC -shared $(CXXFLAGS) btwrapper.cpp -lbacktrace -o libbtwrapper.so

test:
	$(CXX) $(CXXFLAGS) sample.cpp -I. -L. -lbacktrace -lbtwrapper

clean:
	rm -rf *.so *.o *.out
