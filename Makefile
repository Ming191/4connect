CXX=g++
CXXFLAGS=--std=c++20 -W -Wall -O3

SRCS=main.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

c4solver:$(OBJS)
	$(CXX) $(LDFLAGS) -o c4solver $(OBJS) $(LOADLIBES) $(LDLIBS)

.depend: $(SRCS)
	$(CXX) $(CXXFLAGS) -MM $^ > ./.depend
	
include .depend

clean:
	rm -f *.o .depend c4solver


