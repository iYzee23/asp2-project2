#include "bzvezda.h"

const int dimMax = 100;

BStar::BStar(const BStar& bs)
{
	copy(bs);
}

BStar::BStar(BStar&& bs)
{
	move(bs);
}

BStar::~BStar()
{
	clear();
}

BStar& BStar::operator=(BStar bs)
{
	swap(*this, bs);
	return *this;
}

bool BStar::operator()(string key) const
{
	Elem* node = root;
	int i, ind;
	while (node) {
		ind = 0;
		for (i = 0; i < node->num; i++) {
			if (node->keys[i] == key)
				return true;
			if (node->keys[i] > key) {
				node = node->pointers[i];
				ind = 1;
				break;
			}
		}
		if (!ind)
			node = node->pointers[i];
	}
	return false;
}

bool BStar::operator+=(string key)
{
	if (!root) {
		int max = 2 * floor((2 * m - 2) / 3.);
		root = new Elem(max, 1);
		root->keys[0] = key;
		root->num++;
		numNodes++;
		return true;
	}

	Elem* node = root;
	int i, j, ind;
	while (!isLeaf(node)) {
		ind = 0;
		for (i = 0; i < node->num; i++) {
			if (node->keys[i] == key)
				return false;
			else if (node->keys[i] > key) {
				node = node->pointers[i];
				ind = 1;
				break;
			}
		}
		if (!ind)
			node = node->pointers[i];
	}
	for (i = 0; i < node->num; i++)
		if (node->keys[i] == key)
			return false;
	for (i = node->num - 1; i >= 0; i--) {
		if (node->keys[i] > key) {
			node->keys[i + 1] = node->keys[i];
			node->pointers[i + 2] = node->pointers[i + 1];
		}
		else {
			node->keys[i + 1] = key;
			node->pointers[i + 2] = nullptr;
			node->num++;
			break;
		}
	}
	if (i == -1) {
		node->keys[i + 1] = key;
		node->pointers[i + 2] = nullptr;
		node->num++;
	}

	while (node->num > node->max) {
		Elem* q = node->parent;
		if (!q) {
			refraction12();
			break;
		}
		else {
			for (j = 0; j < q->num; j++)
				if (q->pointers[j] == node)
					break;
			if (j == q->num) 
				j--;
			if (node == q->pointers[j]) {
				if (q->pointers[j + 1]->num < q->pointers[j + 1]->max)
					overflow(node, q->pointers[j + 1], q, j);
				else if ((j != 0) && (q->pointers[j - 1]->num < q->pointers[j - 1]->max))
					overflow(q->pointers[j - 1], node, q, j - 1);
				else
					refraction23(node, q->pointers[j + 1], q, j);
			}
			else {
				if (q->pointers[j]->num < q->pointers[j]->max)
					overflow(q->pointers[j], node, q, j);
				else
					refraction23(q->pointers[j], node, q, j);
			}
			node = q;
		}
	}
	return true;
}

bool BStar::operator-=(string key)
{
	Elem* node = root;
	int i, j, ind;
	while (node) {
		ind = 0;
		for (i = 0; i < node->num; i++) {
			if (node->keys[i] == key) {
				if (!isLeaf(node)) {
					Elem* pom = getInorderSucc(node->pointers[i + 1]);
					node->keys[i] = pom->keys[0];
					i = 0;
					node = pom;
				}
				goto y;
			}
			else if (node->keys[i] > key) {
				node = node->pointers[i];
				ind = 1;
				break;
			}
		}
		if (!ind)
			node = node->pointers[i];
	}
	return false;
	y:
	node->num--;
	for (int k = i; k < node->num; k++) {
		node->keys[k] = node->keys[k + 1];
		node->pointers[k + 1] = node->pointers[k + 2];
	}
	while (node->num < node->min) {
		Elem* q = node->parent;
		if (!q) {
			if (!root->pointers[0]) {
				delete root;
				root = nullptr;
			}
			else {
				Elem* pom = root->pointers[0];
				root->num = pom->num;
				for (int k = 0; k < root->num; k++)
					root->keys[k] = pom->keys[k];
				for (int k = 0; k <= root->num; k++) {
					root->pointers[k] = pom->pointers[k];
					if (pom->pointers[k]) pom->pointers[k]->parent = root;
				}
				for (int k = 0; k <= pom->max + 1; k++)
					pom->pointers[k] = nullptr;
				pom->parent = nullptr;
				delete pom;
			}
			numNodes--;
			break;
		}
		else {
			for (j = 0; j < q->num; j++)
				if (q->pointers[j] == node)
					break;
			if (j == q->num)
				j--;
			if (node == q->pointers[j]) {
				if (q->pointers[j + 1]->num > q->pointers[j + 1]->min)
					loan1right(node, q->pointers[j + 1], q, j);
				else if ((j != 0) && q->pointers[j - 1]->num > q->pointers[j - 1]->min)
					loan1left(q->pointers[j - 1], node, q, j - 1);
				else if ((j != q->num - 1) && q->pointers[j + 2]->num > q->pointers[j + 2]->min)
					loan2right(node, q->pointers[j + 1], q->pointers[j + 2], q, j);
				else if ((j != 0) && (j != 1) && q->pointers[j - 2]->num > q->pointers[j - 2]->min)
					loan2left(q->pointers[j - 2], q->pointers[j - 1], node, q, j - 1);
				else if (j == 0) {
					if (q->num >= 2) merge32(node, q->pointers[j + 1], q->pointers[j + 2], q, j + 1);
					else merge21(node, q->pointers[j + 1], q, j);
				}
				else
					merge32(q->pointers[j - 1], node, q->pointers[j + 1], q, j);
			}
			else {
				if (q->pointers[j]->num > q->pointers[j]->min)
					loan1left(q->pointers[j], node, q, j);
				else if ((j != 0) && q->pointers[j - 1]->num > q->pointers[j - 1]->min)
					loan2left(q->pointers[j - 1], q->pointers[j], node, q, j);
				else if (q->num >= 2) 
					merge32(q->pointers[j - 1], q->pointers[j], node, q, j);
				else
					merge21(q->pointers[j], node, q, j);
			}
			node = q;
		}
	}
	return true;
}

int BStar::operator[](string key) const
{
	int counter = 0, i, ind;
	Elem* node = root;
	while (node) {
		ind = 0;
		for (i = 0; i < node->num; i++) {
			if (node->keys[i] == key) {
				if (node->pointers[i])
					counter += getNumOfKeys(node->pointers[i]);
				node = nullptr;
				ind = 1;
				break;
			}
			else if (node->keys[i] > key) {
				node = node->pointers[i];
				ind = 1;
				break;
			}
			counter += 1 + getNumOfKeys(node->pointers[i]);
		}
		if (!ind)
			node = node->pointers[i];
	}
	return counter;
}

int BStar::getNumOfKeys(Elem* rootx) const
{
	if (!rootx) return 0;
	int counter = 0;
	queue<Elem*> qElem;
	Elem* node = rootx;
	qElem.push(node);
	while (!qElem.empty()) {
		node = qElem.front(); qElem.pop();
		counter += node->num;
		for (int i = 0; i <= node->num; i++)
			if (node->pointers[i])
				qElem.push(node->pointers[i]);
	}
	return counter;
}

bool BStar::isLeaf(Elem* node) const
{
	for (int i = 0; i <= node->num; i++)
		if (node->pointers[i])
			return false;
	return true;
}

Elem* BStar::getParent(Elem* node, int& j) const
{
	Elem* p = root;
	Elem* q = nullptr;
	int i, ind; string key = node->keys[0];
	while (p) {
		ind = 0;
		for (i = 0; i < p->num; i++) {
			if (p->keys[i] == key) {
				p = nullptr;
				goto y;
			}
			else if (p->keys[i] > key) {
				q = p;
				j = i;
				p = p->pointers[i];
				ind = 1;
				break;
			}
		}
		if (!p->num)
			return q;
		if (!ind) {
			q = p;
			j = i - 1;
			p = p->pointers[i];
		}
	}
	y:
	return q;
}

Elem* BStar::getInorderSucc(Elem* node) const
{
	Elem* p = node;
	while (!isLeaf(p))
		p = p->pointers[0];
	return p;
}

void BStar::overflow(Elem* left, Elem* right, Elem* par, int j)
{
	Elem* pok[dimMax];
	int nPok = 0;
	for (int i = 0; i <= left->num; i++)
		pok[nPok++] = left->pointers[i];
	for (int i = 0; i <= right->num; i++)
		pok[nPok++] = right->pointers[i];

	string arr[dimMax]{par->keys[j]};
	int curr = 1;
	for (int i = 0; i < left->num; i++)
		arr[curr++] = left->keys[i];
	for (int i = 0; i < right->num; i++)
		arr[curr++] = right->keys[i];
	for (int i = 0; i < curr - 1; i++)
		for (int j = i + 1; j < curr; j++)
			if (arr[i] > arr[j])
				std::swap(arr[i], arr[j]);

	int g = curr / 2;
	par->keys[j] = arr[g];
	left->num = g; right->num = curr - g - 1;
	for (int i = 0; i < left->num; i++)
		left->keys[i] = arr[i];
	for (int i = 0; i < right->num; i++)
		right->keys[i] = arr[g + 1 + i];
	for (int i = 0; i <= left->num; i++) {
		left->pointers[i] = pok[i];
		if (pok[i]) pok[i]->parent = left;
	}
	for (int i = 0; i <= right->num; i++) {
		right->pointers[i] = pok[g + 1 + i];
		if (pok[i]) pok[i]->parent = right;
	}
}

void BStar::refraction12()
{
	Elem* pok[dimMax];
	int nPok = 0;
	for (int i = 0; i <= root->num; i++)
		pok[nPok++] = root->pointers[i];

	string arr[dimMax];
	int curr = 0;
	for (int i = 0; i < root->num; i++)
		arr[curr++] = root->keys[i];

	int g = curr / 2;
	Elem* left = new Elem(m - 1, ceil((2 * m - 1) / 3.) - 1);
	Elem* right = new Elem(m - 1, ceil((2 * m - 1) / 3.) - 1);

	left->num = g; right->num = curr - g - 1;
	for (int i = 0; i < left->num; i++)
		left->keys[i] = arr[i];
	for (int i = 0; i < right->num; i++)
		right->keys[i] = arr[g + 1 + i];
	for (int i = 0; i <= left->num; i++) {
		left->pointers[i] = pok[i];
		if (pok[i]) pok[i]->parent = left;
	}
	for (int i = 0; i <= right->num; i++) {
		right->pointers[i] = pok[g + 1 + i];
		if (pok[g + 1 + i]) pok[g + 1 + i]->parent = right;
	}

	root->num = 1;
	root->keys[0] = arr[g];
	root->pointers[0] = left;
	root->pointers[1] = right;
	left->parent = root;
	right->parent = root;

	numNodes++;
	numNodes++;
}

void BStar::loan1left(Elem* left, Elem* right, Elem* par, int j)
{
	Elem* pok = left->pointers[left->num];
	right->num++;
	for (int i = right->num; i >= 1; i--) {
		right->pointers[i] = right->pointers[i - 1];
		right->keys[i] = right->keys[i - 1];
	}
	right->pointers[0] = pok;
	if (pok) pok->parent = right;
	right->keys[0] = par->keys[j];
	par->keys[j] = left->keys[left->num - 1];
	left->num--;
}

void BStar::loan2left(Elem* left, Elem* mid, Elem* right, Elem* par, int j)
{
	Elem* pok2 = left->pointers[left->num];
	mid->num++;
	for (int i = mid->num; i >= 1; i--) {
		mid->pointers[i] = mid->pointers[i - 1];
		mid->keys[i] = mid->keys[i - 1];
	}
	mid->pointers[0] = pok2;
	if (pok2) pok2->parent = mid;
	mid->keys[0] = par->keys[j - 1];
	par->keys[j - 1] = left->keys[left->num - 1];
	left->num--;

	Elem* pok1 = mid->pointers[mid->num];
	right->num++;
	for (int i = right->num; i >= 1; i--) {
		right->pointers[i] = right->pointers[i - 1];
		right->keys[i] = right->keys[i - 1];
	}
	right->pointers[0] = pok1;
	if (pok1) pok1->parent = right;
	right->keys[0] = par->keys[j];
	par->keys[j] = mid->keys[mid->num - 1];
	mid->num--;
}

void BStar::loan1right(Elem* left, Elem* right, Elem* par, int j)
{
	Elem* pok = right->pointers[0];
	left->num++;
	left->pointers[left->num] = pok;
	if (pok) pok->parent = left;
	left->keys[left->num-1] = par->keys[j];
	par->keys[j] = right->keys[0];
	right->num--;
	for (int i = 0; i <= right->num; i++) {
		right->keys[i] = right->keys[i + 1];
		right->pointers[i] = right->pointers[i + 1];
	}
}

void BStar::loan2right(Elem* left, Elem* mid, Elem* right, Elem* par, int j)
{
	Elem* pok2 = right->pointers[0];
	mid->num++;
	mid->pointers[mid->num] = pok2;
	if (pok2) pok2->parent = mid;
	mid->keys[mid->num - 1] = par->keys[j + 1];
	par->keys[j + 1] = right->keys[0];
	right->num--;
	for (int i = 0; i <= right->num; i++) {
		right->keys[i] = right->keys[i + 1];
		right->pointers[i] = right->pointers[i + 1];
	}

	Elem* pok1 = mid->pointers[0];
	left->num++;
	left->pointers[left->num] = pok1;
	if (pok1) pok1->parent = left;
	left->keys[left->num - 1] = par->keys[j];
	par->keys[j] = mid->keys[0];
	mid->num--;
	for (int i = 0; i <= mid->num; i++) {
		mid->keys[i] = mid->keys[i + 1];
		mid->pointers[i] = mid->pointers[i + 1];
	}
}

void BStar::merge32(Elem* left, Elem* mid, Elem* right, Elem* par, int j)
{
	Elem* pok[dimMax];
	int nPok = 0;
	for (int i = 0; i <= left->num; i++)
		pok[nPok++] = left->pointers[i];
	for (int i = 0; i <= mid->num; i++)
		pok[nPok++] = mid->pointers[i];
	for (int i = 0; i <= right->num; i++)
		pok[nPok++] = right->pointers[i];

	string arr[dimMax]{ par->keys[j], par->keys[j - 1] };
	int curr = 2;
	for (int i = 0; i < left->num; i++)
		arr[curr++] = left->keys[i];
	for (int i = 0; i < mid->num; i++)
		arr[curr++] = mid->keys[i];
	for (int i = 0; i < right->num; i++)
		arr[curr++] = right->keys[i];
	for (int i = 0; i < curr - 1; i++)
		for (int j = i + 1; j < curr; j++)
			if (arr[i] > arr[j])
				std::swap(arr[i], arr[j]);

	for (int i = 0; i <= right->max + 1; i++)
		right->pointers[i] = nullptr;
	right->parent = nullptr;
	delete right;
	numNodes--;
	int g = curr / 2;
	left->num = g; mid->num = curr - g - 1;
	for (int i = 0; i < left->num; i++)
		left->keys[i] = arr[i];
	for (int i = 0; i < mid->num; i++)
		mid->keys[i] = arr[g + 1 + i];
	for (int i = 0; i <= left->num; i++) {
		left->pointers[i] = pok[i];
		if (pok[i]) pok[i]->parent = left;
	}
	for (int i = 0; i <= mid->num; i++) {
		mid->pointers[i] = pok[g + 1 + i];
		if (pok[g + 1 + i]) pok[g + 1 + i]->parent = mid;
	}

	par->keys[j - 1] = arr[g];
	for (int i = j; i < par->num; i++) {
		par->keys[i] = par->keys[i + 1];
		par->pointers[i + 1] = par->pointers[i + 2];
	}
	par->num--;
}

void BStar::merge21(Elem* left, Elem* right, Elem* par, int j)
{
	Elem* pok[dimMax];
	int nPok = 0;
	for (int i = 0; i <= left->num; i++)
		pok[nPok++] = left->pointers[i];
	for (int i = 0; i <= right->num; i++)
		pok[nPok++] = right->pointers[i];

	string arr[dimMax]{ par->keys[j] };
	int curr = 1;
	for (int i = 0; i < left->num; i++)
		arr[curr++] = left->keys[i];
	for (int i = 0; i < right->num; i++)
		arr[curr++] = right->keys[i];
	for (int i = 0; i < curr - 1; i++)
		for (int j = i + 1; j < curr; j++)
			if (arr[i] > arr[j])
				std::swap(arr[i], arr[j]);

	for (int i = 0; i <= right->max + 1; i++)
		right->pointers[i] = nullptr;
	right->parent = nullptr;
	delete right;
	numNodes--;
	left->num = curr;
	for (int i = 0; i < left->num; i++)
		left->keys[i] = arr[i];
	for (int i = 0; i <= left->num; i++) {
		left->pointers[i] = pok[i];
		if (pok[i]) pok[i]->parent = left;
	}
	for (int i = j; i < par->num; i++) {
		par->keys[i] = par->keys[i + 1];
		par->pointers[i + 1] = par->pointers[i + 2];
	}
	par->num--;
}

void BStar::refraction23(Elem* left, Elem* right, Elem* par, int j)
{
	Elem* pok[dimMax];
	int nPok = 0;
	for (int i = 0; i <= left->num; i++)
		pok[nPok++] = left->pointers[i];
	for (int i = 0; i <= right->num; i++)
		pok[nPok++] = right->pointers[i];

	string arr[dimMax]{ par->keys[j] };
	int curr = 1;
	for (int i = 0; i < left->num; i++)
		arr[curr++] = left->keys[i];
	for (int i = 0; i < right->num; i++)
		arr[curr++] = right->keys[i];
	for (int i = 0; i < curr - 1; i++)
		for (int j = i + 1; j < curr; j++)
			if (arr[i] > arr[j])
				std::swap(arr[i], arr[j]);

	int g1 = floor((2 * m - 2) / 3.);
	int g2 = g1 + 1 + floor((2 * m - 1) / 3.);
	Elem* newi = new Elem(m - 1, ceil((2 * m - 1) / 3.) - 1);
	par->keys[j] = arr[g1];

	left->num = g1; right->num = g2 - g1 - 1;
	newi->num = curr - g2 - 1;
	for (int i = 0; i < left->num; i++)
		left->keys[i] = arr[i];
	for (int i = 0; i < right->num; i++)
		right->keys[i] = arr[g1 + 1 + i];
	for (int i = 0; i < newi->num; i++)
		newi->keys[i] = arr[g2 + 1 + i];
	for (int i = 0; i <= left->num; i++) {
		left->pointers[i] = pok[i];
		if (pok[i]) pok[i]->parent = left;
	}
	for (int i = 0; i <= right->num; i++) {
		right->pointers[i] = pok[g1 + 1 + i];
		if (pok[g1 + 1 + i]) pok[g1 + 1 + i]->parent = right;
	}
	for (int i = 0; i <= newi->num; i++) {
		newi->pointers[i] = pok[g2 + 1 + i];
		if (pok[g2 + 1 + i]) pok[g2 + 1 + i]->parent = newi;
	}

	for (int i = par->num - 1; i >= 0; i--) {
		if (par->keys[i] > arr[g2]) {
			par->keys[i + 1] = par->keys[i];
			par->pointers[i + 2] = par->pointers[i + 1];
		}
		else {
			par->keys[i + 1] = arr[g2];
			par->pointers[i + 2] = newi;
			newi->parent = par;
			par->num++;
			break;
		}
	}

	numNodes++;
}

void BStar::clear()
{
	queue<Elem*> qElem;
	Elem* node = root;
	if (node) qElem.push(node);
	while (!qElem.empty()) {
		node = qElem.front(); qElem.pop();
		for (int i = 0; i <= node->num; i++)
			if (node->pointers[i]) {
				qElem.push(node->pointers[i]);
				node->pointers[i] = nullptr;
			}
		node->parent = nullptr;
		delete node;
	}
	root = nullptr;
}

void BStar::copy(const BStar& bs)
{
	m = bs.m;
	numNodes = bs.numNodes;
	root = nullptr;
	queue<Elem*> qElem;
	Elem* node = bs.root;
	qElem.push(node);
	while (!qElem.empty()) {
		node = qElem.front(); qElem.pop();
		for (int i = 0; i < node->num; i++) {
			*this += node->keys[i];
			if (node->pointers[i])
				qElem.push(node->pointers[i]);
		}
		if (node->pointers[node->num])
			qElem.push(node->pointers[node->num]);
	}
}

void BStar::move(BStar& bs)
{
	m = bs.m;
	numNodes = bs.numNodes;
	root = bs.root;
	bs.root = nullptr;
}

void BStar::swap(BStar& b1, BStar& b2)
{
	std::swap(b1.m, b2.m);
	std::swap(b1.numNodes, b2.numNodes);
	std::swap(b1.root, b2.root);
}

ostream& operator<<(ostream& os, const BStar& bs)
{
	if (!bs.getRoot()) {
		os << "Your tree is empty. ";
		os << "Add some keys first";
		os << endl;
	}
	else {
		queue<Elem*> qElem;
		queue<int> qInt;
		Elem* node = bs.root;
		int h = 0, hPrev = 0;
		qElem.push(node);
		qInt.push(h);
		while (!qElem.empty()) {
			node = qElem.front(); qElem.pop();
			h = qInt.front(); qInt.pop();
			if (h != hPrev) os << endl;
			os << "[";
			for (int i = 0; i < node->num; i++) {
				os << node->keys[i] << (i == node->num - 1 ? "] " : "|");
				if (node->pointers[i]) {
					qElem.push(node->pointers[i]);
					qInt.push(h + 1);
				}
			}
			if (node->pointers[node->num]) {
				qElem.push(node->pointers[node->num]);
				qInt.push(h + 1);
			}
			hPrev = h;
		}
	}
	return os;
}
