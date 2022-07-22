#ifndef _bzvezda_h_
#define _bzvezda_h_

#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include <cstddef>
#include <stack>
#include <queue>

using namespace std;

struct Elem {
	int max;
	int min;
	int num;
	string* keys;
	Elem** pointers;
	Elem* parent;
	Elem(int max, int min) : num(0) {
		this->max = max;
		this->min = min;
		this->keys = new string[max + 1]();
		this->pointers = new Elem * [max + 2]();
		this->parent = nullptr;
	}
	~Elem() {
		for (int i = 0; i <= num; i++)
			delete pointers[i];
		delete[] pointers;
		delete[] keys;
	}
};

class BStar {
private:
	int m;
	int numNodes;
	Elem* root;

	void clear();
	void copy(const BStar& bs);
	void move(BStar& bs);
	void swap(BStar& b1, BStar& b2);
public:
	BStar(int m = 3) : m(m) { numNodes = 0; root = nullptr; }
	BStar(const BStar& bs);
	BStar(BStar&& bs);
	~BStar();
	BStar& operator=(BStar bs);

	bool operator()(string key) const;
	friend ostream& operator<<(ostream& os, const BStar& bs);
	bool operator+=(string key);
	bool operator-=(string key);
	int operator[](string key) const;

	int getNumOfKeys(Elem* rootx) const;
	bool isLeaf(Elem* node) const;
	Elem* getParent(Elem* node, int& parInd) const;
	Elem* getInorderSucc(Elem* node) const;
	Elem* getRoot() const { return root; }

	void overflow(Elem* left, Elem* right, Elem* par, int j);
	void refraction23(Elem* left, Elem* right, Elem* par, int j);
	void refraction12();

	void loan1left(Elem* left, Elem* right, Elem* par, int j);
	void loan2left(Elem* left, Elem* mid, Elem* right, Elem* par, int j);
	void loan1right(Elem* left, Elem* right, Elem* par, int j);
	void loan2right(Elem* left, Elem* mid, Elem* right, Elem* par, int j);
	void merge32(Elem* left, Elem* mid, Elem* right, Elem* par, int j);
	void merge21(Elem* left, Elem* right, Elem* par, int j);
};

#endif // !_bzvezda_h_
