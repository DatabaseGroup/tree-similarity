CC		= g++
CFLAGS	= -O3 --std=c++11

all: default

default: TreeSimilarity.cc LblTree.cc SpecializedTree.cc Node.cc UpperBound.cc StringEditDistance.cc
	$(CC) $(CFLAGS) $? -o TreeSimilarity
	./TreeSimilarity

clean:
	rm -f TreeSimilarity
