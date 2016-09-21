#ifndef STACK_H
#define STACK_H

namespace data_structures {


/**
 *
 * Source: http://jheriko-rtw.blogspot.co.at/2009/03/stdstack-is-slow.html
 *
 * Slightly modified: added isEmpty function and commented clear function out
 *
 */
template<class _Type>
class Stack {

private:
	_Type* data;
	unsigned int size;
	unsigned int sp;

public:
	Stack() { // Constructor
		size = 64; // initial stack size, gets doubled whenever more space is needed
		data = static_cast<_Type*>(malloc(sizeof(_Type) * 64)); // allocate space for the data (stack)
		sp = 0; // the stack pointer is initially 0
	}

	~Stack() { // Destructor
		free(data); // free the space allocated by data
	}

	void push(_Type value) { // pushes data on the stack
		++sp;
		if(sp >= size) { // if it would overflow the stack
			size <<= 1; // double the stack size
			data = static_cast<_Type*>(realloc(data, sizeof(_Type) * size)); // and reallocate the space (http://www.cplusplus.com/reference/cstdlib/realloc/)
		}
		data[sp] = value; // set the data at index of sp to the passed data
	}

	_Type pop() { // returns value! not pointer
		return data[sp--];
	}

	bool isEmpty() {
		return (bool) (sp == 0);
	}

	// void clear() {
	// 	sp = 0;
	// }
};

} // namespace data_structures

#endif // STACK_H
