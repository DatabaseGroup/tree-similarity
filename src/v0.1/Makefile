CC		= g++
CFLAGS	= -O3 --std=c++11

all: default

default: TreeSimilarity.cc LblTree.cc SpecializedTree.cc
	$(CC) $(CFLAGS) $? -o TreeSimilarity
	./TreeSimilarity

clean:
	rm -f TreeSimilarity
