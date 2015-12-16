CC			= g++
CFLAGS	= -O3 --std=c++11 -Wall -Wextra -Werror -pedantic-errors

all: default

default: tree_similarity.cc node.h node.cc custom_node.h custom_node.cc upper_bound.h string_edit_distance.h zhang_shasha.h zhang_shasha.cc array_2d.h
	$(CC) $(CFLAGS) $? -o tree_similarity
	./tree_similarity

clean:
	rm -f tree_similarity
