OBJS =	main.o

OPT = -O3
CXX = clang++
CXXFLAGS = $(OPT) -std=c++1y -stdlib=libc++

streams: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o streams

.phony: clean
clean:
	rm -f *.o

.phony: run
run: streams
	time ./streams
