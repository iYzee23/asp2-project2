#include "bzvezda.h"
#include <fstream>

void menu() {
	cout << "0 - End program" << endl;
	cout << "1 - Create B* tree object" << endl;
	cout << "2 - Delete B* tree object" << endl;
	cout << "3 - Find key in B* tree" << endl;
	cout << "4 - Print B* tree" << endl;
	cout << "5 - Insert key in B* tree" << endl;
	cout << "6 - Delete key in B* tree" << endl;
	cout << "7 - Find number of keys less than given in B* tree" << endl;
}

int main() {
	{
		BStar* tree = nullptr;
		int choice, ind = 1;
		int m, fail; 
		string key, ff;
		ifstream file;
		while (ind) {
			menu();
			cout << "Choice? ";
			cin >> choice;
			switch (choice) {
				case 0:
					ind = 0;
					if (tree) {
						delete tree;
						tree = nullptr;
						cout << "Tree successfully deleted" << endl;
					}
					cout << "Program ended. Have a great day! :)" << endl << endl;
					break;
				case 1:
					if (tree) cout << "Tree already exists" << endl << endl;
					else {
						cout << "m? ";
						cin >> m;
						if (m < 3 || m > 10) {
							cout << "You entered invalid m. ";
							cout << "Try again" << endl << endl;
						}
						else {
							tree = new BStar(m);
							cout << "Tree successfully created" << endl << endl;
						}
					}
					break;
				case 2:
					if (!tree) cout << "Tree doesn't exist" << endl << endl;
					else {
						delete tree;
						tree = nullptr;
						cout << "Tree successfully deleted" << endl << endl;
					}
					break;
				case 3:
					if (!tree) cout << "Tree doesn't exist" << endl << endl;
					else {
						cout << "Key? ";
						cin >> key;
						if ((*tree)(key)) cout << "Key was successfully found" << endl << endl;
						else cout << "Key doesn't exist" << endl << endl;
					}
					break;
				case 4:
					if (!tree) cout << "Tree doesn't exist" << endl << endl;
					else {
						cout << *tree;
						cout << endl << endl;
					}
					break;
				case 5:
					if (!tree) cout << "Tree doesn't exist" << endl << endl;
					else {
						cout << "0 - From file" << endl;
						cout << "1 - From standard input" << endl;
						cout << "Choice? ";
						cin >> choice;
						if (choice == 0) {
							cout << "File? ";
							cin >> ff;
							file.open(ff);
							if (file.is_open()) {
								fail = 0;
								while (file >> key) {
									if (!(*tree += key))
										fail = 1;
								}
								if (fail) {
									cout << "Some of keys are already in tree.";
									cout << endl;
								}
								cout << "Keys successfully added" << endl << endl;
								file.close();
							}
							else {
								cout << "File didn't open successfully. ";
								cout << "Try again" << endl << endl;
							}
						}
						else if (choice == 1) {
							cout << "Key? ";
							cin >> key;
							if (*tree += key) cout << "Key successfully inserted" << endl << endl;
							else cout << "Given key already exists in tree. Try again" << endl << endl;
						}
						else {
							cout << "Wrong choice. Try again";
							cout << endl << endl;
						}
					}
					break;
				case 6:
					if (!tree) cout << "Tree doesn't exist" << endl << endl;
					else {
						cout << "Key? ";
						cin >> key;
						if (*tree -= key) {
							cout << "Key successfull deleted" << endl;
							if (!tree->getRoot()) {
								delete tree;
								tree = nullptr;
								cout << "Tree successfully deleted" << endl << endl;
							}
							else cout << endl;
						}
						else cout << "Given key doesn't exist in tree. Try again" << endl << endl;
					}
					break;
				case 7:
					if (!tree) cout << "Tree doesn't exist" << endl << endl;
					else {
						cout << "Key? ";
						cin >> key;
						if ((*tree)(key)) {
							cout << "Number of keys less than given: ";
							cout << (*tree)[key] << endl << endl;
						}
						else cout << "Key doesn't exist" << endl << endl;
					}
					break;
				default:
					cout << "You entered wrong choice. ";
					cout << "Try again" << endl << endl;
					break;
			}
		}
	}
	_CrtDumpMemoryLeaks();
}