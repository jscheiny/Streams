OBJS =	main.o

OPT = -O3
ifeq ($(UNAME_S), Darwin)
	CXX = clang++
	CXXFLAGS = $(OPT) -std=c++11 -stdlib=libc++
else
	CXX = g++
	CXXFLAGS = $(OPT) -std=c++11
endif

streams: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o streams

.phony: clean
clean:
	rm -f *.o

.phony: run
run: streams
	time ./streams
