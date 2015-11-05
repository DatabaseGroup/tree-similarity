#ifndef LBLTREE_H
#define LBLTREE_H

class LblTree {
	protected:
		int id;

	public:
		LblTree() : id(0) { }
		LblTree(int id) : id(id) { };
		virtual ~LblTree() { };

		const int getId() const;
};

#endif // LBLTREE_H