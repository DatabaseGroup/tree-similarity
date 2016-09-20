#ifndef STACK_H
#define STACK_H

namespace data_structures {

template<class _Type>
class Stack {

private:
	_Type* data;
	unsigned int size;
	unsigned int sp;

public:
	Stack() {
		size = 64;
		data = static_cast<_Type*>(malloc(sizeof(_Type) * 64));
		sp = 0;
	}

	~Stack() {
		free(data);
	}

	void push(_Type value) {
		++sp;
		if(sp >= size) {
			size <<= 1;
			data = static_cast<_Type*>(realloc(data, sizeof(_Type) * size));
		}
		data[sp] = value;
	}

	_Type pop() { // returns value!
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
