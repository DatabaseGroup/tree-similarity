CC			= g++
CFLAGS	= -O3 --std=c++11

all: default

default: tree_similarity.cc node.h node.cc upper_bound.h upper_bound.cc string_edit_distance.h zhang_shasha.h zhang_shasha.cc 
	$(CC) $(CFLAGS) $? -o tree_similarity
	./tree_similarity

clean:
	rm -f tree_similarity
