#pragma once
#ifndef _BST_
#define _BST_
#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

//-- NODE CLASS --//
class Node {
public:
	Node () {
		parent = nullptr;
		left = nullptr;
		right = nullptr;
		_k = int ();
	}
	//Left, right, and parent nodes.
	Node* parent;
	Node* left;
	Node* right;
	bool IsExternal () {
		return (left == nullptr && right == nullptr);
	}
	bool IsInternal () {
		return !IsExternal ();
	}
	bool IsRoot () {
		return (parent == nullptr);
	}
	//Key
	int _k;
};

//-- BST CLASS --//
class BinarySearchTree {
public:
	BinarySearchTree () {}
	int getSize () { return this->_Size;}
	bool empty () { return this->_Size == 0;}
	Node* getRoot () { return this->_Root; }

	//Search for and return specified node in tree, if it exists.
	Node* ToSearch (int k, Node* N) {//N==root at beginning
		if (N->IsExternal ()) {
			return N;
		}
		if (k < N->_k) {
			return ToSearch (k, N->left);
		}
		else if (k == N->_k) {
			return N;
		}
		else { //if k > N->k
			return ToSearch (k, N->right);
		}
	}

	void printTree (Node* p, int indent  = 0)
	{
		if (p != NULL) {
			if (p->right) {
				printTree (p->right, indent + 4);
			}
			if (indent) {
				cout << setw (indent) << ' ';
			}
			if (p->right) cout << "\n" << setw (indent) << ' ';
			if (p->_k != 0) {
				cout << p->_k;
			}
			if (p->left) {
				cout << setw (indent) << ' ';
				printTree (p->left, indent + 4);
			}
		}
	}

	//Insert node into tree with the specified key.
	Node* ToInsert (int k, Node* N) {//N==root at beginning
		Node* w;
		if (empty ()) {//Newly inserted node will be root.
			Node* root = new Node;
			this->_Root = root;
			w = root;
			InsertAtExternal (this->_Root, k);
		}
		else {
			w = ToSearch (k, N);//Search for appropriate node to insert new node at.
			if (w->IsInternal ()) {//if such node already exists in the tree
				return ToInsert (k, w->left);
			}
			////insert the new node at an external node on the tree
			this->InsertAtExternal (w, k);
		}
		//return the new node
		this->_Size++;
		return w;
	}

	//Deletes node from tree with the specified key, returns the parent of that node.
	void ToDelete (int k) {
		if (this->getSize () > 0) { //If tree is not empty.
			Node* v = ToSearch (k, this->getRoot ()); //find the node to delete
			Node* vParent;

			//Determine parent of node to be deleted.
			if (v->parent != NULL) {
				vParent = v->parent; //The parent node of the node to be deleted.
			}
			else {//the node to be deleted is the root node
				vParent = v->left;
			}

			if (v->IsExternal ()) {
				cout << "Such node cannot be found to be deleted from the tree." << endl;
			}
			else {
				Node* w; //node that is possibly external and child of v
				if (v->left->IsExternal ()) {///case1 if v's left child is external
					w = v->left;
				}
				else if (v->right->IsExternal ()) {////case2 if v's right child is external
					w = v->right;
				}
				else {////case3 if no children of v is external
					w = v->right; ///we can also to left if we want
					do {
						w = w->left;///find the left most child with smallest key larger than v.key
					} while (w->IsInternal ());
					Node* u = w->parent;///u is node to replace the node to be deleted
					v->_k = u->_k;
				}

				//Reduce number of nodes in tree
				this->_Size--;
				//Delete the node from the tree, using auxiliary function
				this->RemoveAboveExternal (w);
				
				//AVL DELETION AND REBALANCING CODE
				this->avlDelete( vParent);
			}
		}
		else {//The tree is empty.
			cout << "The tree is empty, nothing to be deleted." << endl;
		}

	}

	////////////////////////functions for AVL tree
	///////1. rebalance
	/*
	input: the new node inserted to the tree
	output: void, postcondition: Tree is rebalanced if not
	*/
	void ReBalance (Node* v) {
		Node* z = v;
		while (!z->IsRoot ()) {
			z = z->parent;
			if (!this->IsBalanced (z)) {
				Node* x = tallGrandchild (z);
				this->Restructure (x);
			}
		}
	}

	void avlDelete (Node* w) {
		Node* z = w; //Node where first imbalance is detected, initially set to w
		Node* x;

		//Find first imbalance node
		bool found = false;
		while(z != getRoot()){ //while not at the root node yet
			while(found == false ){ //find next imbalance
				if (z == getRoot ()) {
					break;
				}

				if (IsBalanced (z)) {
					z = z->parent;
				}
				else if(z == getRoot()) {
					found = true;
					z = getRoot();
				}
			}
			if(z != getRoot()){
				//find x node and restructure
				x = tallGrandchild(z);
				Restructure(x);
				z = z->parent;
				found = false;
			}
		}
	}

	////////2. BInsert - insertion with rebalancing
	Node* BInsert (int key) {
		Node* v = ToInsert (key, this->getRoot ());
		ReBalance (v);
		return v;
	}


private:
	Node * _Root;
	int _Size;
	/////1.auxiliary function for insert
	void InsertAtExternal (Node* v, int key) {
		if (v->IsExternal ()) {
			////assign key to the node
			v->_k = key;
			/////create children of v, so it becomes internal node
			Node* left = new Node;
			Node* right = new Node;
			v->left = left;
			v->right = right;
			left->parent = v;
			right->parent = v;
		}
		else {////if not external
			cout << "this function should insert new node on an external node";
		}
	}

	/*
	auxiliary functions needed for AVL tree rebalancing
	*/
	///1. height
	int height (Node* N) {
		if (N != nullptr) {
			if (N->IsExternal ()) {
				return 0;
			}
			else {
				int left = height (N->left);
				int right = height (N->right);
				if (left >= right) {
					return left + 1;
				}
				else {
					return right + 1;
				}
			}
		}
		else {
			cout << "node this null" << endl;
		}
	}

	/*
	remove an external node and its parent, replacing the parent with sibling
	an error occurs if the node is not external
	*/
	void RemoveAboveExternal (Node* N) {///remove the passing parameter node
		if (N->IsExternal ()) {
			///1. get sibling node
			Node* sib;
			Node* parent = N->parent;
			if (parent->left == N) {///if N is left child
				sib = parent->right;
			}
			else {
				sib = parent->left;
			}
			///2.replace parent with sibling
			////2.1 if parent is root: that means no grandparents
			if (parent->IsRoot ()) {
				this->_Root = sib;
				sib->parent = nullptr;
			}
			else {////2.2 if parent is not root: that means there is grand parent
				Node* Gparent = parent->parent;
				//need to figure out if parent is grandparent's left or right child
				if (parent == Gparent->left) {//if left
					Gparent->left = sib;
					sib->parent = Gparent;
				}
				else {
					Gparent->right = sib;
					sib->parent = Gparent;
				}
			}

			////delete the nodes after all the operations
			delete N;
			delete parent;
		}
		else {
			cout << "RemoveAboveExternal(): should be external node";
		}
	}

	////2. Isbalance-check if node is balanced or not, difference between heights of left and right can not exceed 1
	bool IsBalanced (Node* N) {
		if (N->IsExternal ()) {
			return true;
		}
		else {
			int bal = height (N->left) - height (N->right);
			return ((-1 <= bal) && (bal <= 1));
		}
	}

	////3. tallGrandchild: find the node x, which is grandchild of z, child of y, from z
	/*
	z is first unbalanced node from bottom up
	y is child of z with greater height
	x is child of y with greater height
	*/
	Node* tallGrandchild (Node* z) {
		Node* left = z->left;
		Node* right = z->right;
		if (height (left) >= height (right)) {///left is y
			Node* y = left;
			if (height (y->left) >= height (y->right)) {
				return y->left;
			}
			else {
				return y->right;
			}
		}
		else {/////right is y
			Node* y = right;
			if (height (y->left) >= height (y->right)) {
				return y->left;
			}
			else {
				return y->right;
			}
		}
	}

	////4.restructure
	/*
	input: a node x that has parent y and grandparent z
	output: void, afterward condition: tree T will be balanced AVL T
	*/
	void Restructure (Node* x) {
		////1. find y z
		Node* y = x->parent;
		Node* z = y->parent;
		////2. find their inorder-there can only 4 situations(reference from book)
		Node* a = nullptr; Node* b = nullptr; Node* c = nullptr;
		if (z->right == y && y->right == x) {
			a = z; b = y; c = x;
		}
		else if (z->left == y && y->left == x) {
			c = z; b = y; a = x;
		}
		else if (z->right == y && y->left == x) {
			a = z; b = x; c = y;
		}
		else if (z->left == y && y->right == x) {
			a = y; b = x; c = z;
		}
		////3. find the subtrees not directed from z y x (similarly only 4 cases, reference from book)
		Node* T0 = nullptr; Node* T1 = nullptr;
		Node* T2 = nullptr; Node* T3 = nullptr;
		if (a == z && b == y && c == x) {
			T0 = a->left; T1 = b->left; T2 = c->left; T3 = c->right;
		}
		else if (a == x && b == y && c == z) {
			T0 = a->left; T1 = a->right; T2 = b->right; T3 = c->right;
		}
		else if (a == z && b == x && c == y) {
			T0 = a->left; T1 = b->left; T2 = b->right; T3 = c->right;
		}
		else if (a == y && b == x && c == z) {
			T0 = a->left; T1 = b->left; T2 = b->right; T3 = c->right;
		}
		////4.1 replace the subtree rooted at z with a new subtree rooted at b
		///4.1.1 if z is not root
		if (!z->IsRoot ()) {
			if (z->parent->left == z) {
				z->parent->left = b;
			}
			else {
				z->parent->right = b;
			}
			b->parent = z->parent;
		}
		///4.1.2 if z is root
		else {
			b->parent = nullptr;
			this->_Root = b;
		}
		////4.2.1 set a be left child of b
		b->left = a;
		a->parent = b;
		a->left = nullptr;
		a->right = nullptr;
		////4.2.2 let c be right child of b
		b->right = c;
		c->parent = b;
		c->left = nullptr;
		c->right = nullptr;
		/////4.3.1 let T0, T1 be left and right subtrees of a, respectively
		a->left = T0;
		if (T0 != nullptr) {
			T0->parent = a;
		}
		a->right = T1;
		if (T1 != nullptr) {
			T1->parent = a;
		}
		/////4.3.2 let T2, T3 be left and right subtrees of c, respectively
		c->left = T2;
		if (T2 != nullptr) {
			T2->parent = c;
		}
		c->right = T3;
		if (T3 != nullptr) {
			T3->parent = c;
		}
	}
};

#endif
