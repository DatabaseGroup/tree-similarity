CC		= g++
CFLAGS	= -O3 --std=c++11 -Wall -Wextra -Werror -pedantic-errors

all: default

default: tree_similarity.cc node.h custom_node.h upper_bound.h string_edit_distance.h zhang_shasha.h array_2d.h
	$(CC) $(CFLAGS) $? -o tree_similarity
	./tree_similarity "{aa{bb{gg}}{ccc{ffff}}}" "{aa{ccc{ffff}{gg}}{dd}}"

clean:
	rm -f tree_similarity
