CC		= g++
CFLAGS	= -O3 --std=c++11

all: default

default: tree_similarity.cc lbl_tree.cc node.cc upper_bound.cc string_edit_distance.cc ted_context.cc zhang_shasha.cc 
	$(CC) $(CFLAGS) $? -o tree_similarity
	./tree_similarity

clean:
	rm -f tree_similarity
