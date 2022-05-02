#include <fstream>
#include <iostream>
#include <cstdlib>
#include <list>
#include <cmath>

using namespace std;

struct CodeChar{

	char ch;
	float probability;
	unsigned long long code;
	int top;
};

struct CodeTable
{
	int size;
	CodeChar* codeChar;
};

struct ItemDB
{
	char author[12];
	char title[32];
	char publisher[16];
	short int year;
	short int pages;
};

struct ListItem
{
	ItemDB* data;
	ListItem * next;
};

void printHeadDataBase(ListItem* db, int offset = 0, int count = 20);


class TreeNode {
	int * keys;
	ListItem** values;

	int t;
	TreeNode** C;

	int n;
	bool leaf;

public:
	TreeNode(int t1, bool leaf1) {
		t = t1;
		leaf = leaf1;
		keys = new int[2 * t - 1];
		values = new ListItem * [2 * t - 1];

		for (int i=0; i < 2*t-1; i++)
		{
			keys[i] = 0;
			values[i] = 0;
		}

		C = new TreeNode * [2 * t];

		for (int i = 0; i < 2 * t; i++)
		{
			C[i] = 0;
		}

		n = 0;
	}

	void insertNonFull(int k, ItemDB * value) {
		int i = n - 1;

		if (leaf == true) {
			while (i >= 0 && keys[i] > k) {
				keys[i + 1] = keys[i];
				values[i + 1] = values[i];
				values[i] = 0;
				i--;
			}

			keys[i + 1] = k;

			ListItem* tmp = new ListItem;
			tmp->data = value;
			tmp->next = values[i + 1];
			values[i + 1] = tmp;

			n = n + 1;
		}
		else {
			while (i >= 0 && keys[i] > k)
				i--;

			if (C[i + 1]->n == 2 * t - 1) {
				splitChild(i + 1, C[i + 1]);

				if (keys[i + 1] < k)
					i++;
			}
			C[i + 1]->insertNonFull(k, value);
		}
	}

	void splitChild(int i, TreeNode* y)
	{
		TreeNode* z = new TreeNode(y->t, y->leaf);
		z->n = t - 1;

		for (int j = 0; j < t - 1; j++)
		{
			z->keys[j] = y->keys[j + t];
			z->values[j] = y->values[j + t];
		}


		if (y->leaf == false) {
			for (int j = 0; j < t; j++)
				z->C[j] = y->C[j + t];
		}

		y->n = t - 1;
		for (int j = n; j >= i + 1; j--)
			C[j + 1] = C[j];

		C[i + 1] = z;

		for (int j = n - 1; j >= i; j--)
		{
			keys[j + 1] = keys[j];
			values[j + 1] = values[j];
		}


		keys[i] = y->keys[t - 1];
		values[i] = y->values[t - 1];
		n = n + 1;
	}

	void traverse()
	{
		int i;
		for (i = 0; i < n; i++) {
			if (leaf == false)
				C[i]->traverse();

			if (C[i] == 0)
				return;

			for (int k=0; k<C[i]->n; k++)
			{
				printHeadDataBase(C[i]->values[k], 0, -1);
			}
		}

		if (leaf == false)
			C[i]->traverse();
	}


	TreeNode* search(int k)
	{
		int i = 0;
		while (i < n && k > keys[i])
			i++;

		if (keys[i] == k)
			return this;

		if (leaf == true)
			return NULL;

		return C[i]->search(k);
	}

	friend class BTree;
};

class BTree {
	TreeNode* root;
	int t;

	bool insertExistingList(int key, ItemDB* item)
	{
		TreeNode* node = root->search(key);

		if (node == 0)
			return false;
		
		ListItem* tmp = new ListItem;
		tmp->data = item;
		tmp->next = 0;

		for(int i=0; i < node->n; i++)
		{
			if (node->keys[i] == key)
			{
				tmp->next = node->values[i];
				node->values[i] = tmp;
				break;
			}
		}

		return true;
	}

public:
	BTree(int temp) {
		root = NULL;
		t = temp;
	}

	void traverse() {
		if (root != NULL)
			root->traverse();
	}

	ListItem * search(int k) {
		if (root == 0)
			return 0;

		TreeNode * node = root->search(k);

		if (node == 0)
			return 0;

		for (int i=0; i < node->n; i ++)
		{
			if (node->keys[i] == k)
				return node->values[i];
		}
		return 0;
	}

	void insert(int k, ItemDB * item)
	{
		if (root == NULL) {
			ListItem* tmp = new ListItem;
			tmp->data = item;
			tmp->next = 0;

			root = new TreeNode(t, true);
			root->keys[0] = k;
			root->values[0] = tmp;
			root->n = 1;
		}
		else {
			if (insertExistingList(k, item))
			{
				return;
			}

			if (root->n == 2 * t - 1) {
				TreeNode* s = new TreeNode(t, false);

				s->C[0] = root;

				s->splitChild(0, root);

				int i = 0;
				if (s->keys[0] < k)
					i++;
				s->C[i]->insertNonFull(k, item);

				root = s;
			}
			else
				root->insertNonFull(k, item);
		}
	}
};



ItemDB** toArray(ListItem* h, int size)
{
	ItemDB** arr = new ItemDB * [size];
	int pos = 0;
	for (ListItem * cur = h; cur != 0; cur = cur->next)
	{
		arr[pos] = cur->data;
		pos++;
	}
	return arr;
}


void deleteArray(ItemDB** arr)
{
	delete[] arr;
}

int getLenStr(char * str)
{
	int i = 0;
	while(str[i]!=0)
	{
		i++;
	}
	return i;
}

int cmpStr(char * str1, char * str2, int n=-1)
{
	int len1 = getLenStr(str1);
	int len2 = getLenStr(str2);
	
	int pos = 0;

	while (pos < len1 && pos < len2)
	{
		if (str1[pos] < str2[pos])
		{
			return -1;
		}
		else if(str1[pos] > str2[pos])
		{
			return 1;
		}
		pos++;
		n--;
		if (n == 0)
			return 0;
	}

	if (len1 < len2)
	{
		return -1;
	}
	else if (len1 > len2)
	{
		return 1;
	}

	return 0;
}

int cmpItemDB(ItemDB * left, ItemDB * right)
{
	int res = cmpStr(left->publisher, right->publisher);
	if (res != 0)
	{
		return res;
	}
	res = cmpStr(left->author, right->author);
	return res;
}

ListItem * merge(ListItem * h1, ListItem * h2)
{
	ListItem * t1 = 0;
	ListItem * t2 = 0;
	ListItem * temp = 0;

	// Return if the first list is empty.
	if (h1 == NULL)
		return h2;

	// Return if the Second list is empty.
	if (h2 == NULL)
		return h1;

	t1 = h1;

	// A loop to traverse the second list, to merge the nodes to h1 in sorted way.
	while (h2 != NULL)
	{
		// Taking head node of second list as t2.
		t2 = h2;

		// Shifting second list head to the next.
		h2 = h2->next;
		t2->next = NULL;

		// If the data value is lesser than the head of first list add that node at the beginning.
		if (cmpItemDB(h1->data, t2->data) == 1)
		{
			t2->next = h1;
			h1 = t2;
			t1 = h1;
			continue;
		}

		// Traverse the first list.
	flag:
		if (t1->next == NULL)
		{
			t1->next = t2;
			t1 = t1->next;
		}
		// Traverse first list until t2->data more than node's data.
		else if (cmpItemDB((t1->next)->data, t2->data) <=0 )
		{
			t1 = t1->next;
			goto flag;
		}
		else
		{
			// Insert the node as t2->data is lesser than the next node.
			temp = t1->next;
			t1->next = t2;
			t2->next = temp;
		}
	}

	// Return the head of new sorted list.
	return h1;
}

int findPosition(ItemDB** data, int size, char* key)
{
	int left = 0;
	int right = size-1;

	while (left <= right)
	{
		int mid = (left + right) / 2;
		int res = cmpStr(data[mid]->publisher, key, 3);

		if (res == 1)
		{
			right = mid - 1;
		}
		else if(res == -1)
		{
			left = mid + 1;
		}
		else
		{
			return mid;
		}
	}

	return -1;
}


ListItem* filter(ItemDB ** data, int size, char * key)
{
	ListItem * head=0;
	ListItem * tail=0;

	int pos = findPosition(data, size, key);

	if (pos == -1)
		return 0;

	int s = pos - 1;
	int e = pos + 1;
	
	head = tail = new ListItem;
	head->data = data[pos];
	head->next = 0;

	while (s>=0 && cmpStr(data[s]->publisher, key, 3) == 0)
	{
		ListItem * temp = new ListItem;
		temp->data = data[s];
		temp->next = head;
		head = temp;
		s--;
	}

	while (e < size && cmpStr(data[e]->publisher, key, 3) == 0)
	{
		ListItem* temp = new ListItem;
		temp->data = data[e];
		temp->next = 0;
		tail->next = temp;
		tail = temp;
		e++;
	}

	return head;
}


// A function implementing Merge Sort on linked list using reference.
void mergeSort(ListItem ** head)
{
	ListItem* first = 0;
	ListItem * second = 0;
	ListItem * temp = 0;
	first = *head;
	temp = *head;

	// Return if list have less than two nodes.
	if (first == NULL || first->next == NULL)
	{
		return;
	}
	else
	{
		// Break the list into two half as first and second as head of list.
		while (first->next != NULL)
		{
			first = first->next;
			if (first->next != NULL)
			{
				temp = temp->next;
				first = first->next;
			}
		}
		second = temp->next;
		temp->next = NULL;
		first = *head;
	}

	// Implementing divide and conquer approach.
	mergeSort(&first);
	mergeSort(&second);

	// Merge the two part of the list into a sorted one.      
	*head = merge(first, second);
}

ListItem * readDataBase(const char const* dbName, int countItemDB)
{
	

	ifstream in(dbName, ifstream::binary);
	ListItem* head , * tail;
	head = tail = new ListItem;
	head->next = 0;

	for(int i=0; i < countItemDB; i++)
	{
		ItemDB* item = new ItemDB;
		in.read((char*)item, sizeof(ItemDB));
		tail->next = new ListItem;
		tail->next->data = item;
		tail->next->next = 0;
		tail = tail->next;
	}

	in.close();
	ListItem* temp = head;
	head = head->next;
	delete temp;
	return head;
}

void clearMemoryDataBase(ListItem * db)
{
	ListItem* cur = db;

	while (cur!=0)
	{
		ListItem* next = cur->next;
		memset(cur->data, 0, sizeof(ItemDB));
		delete cur->data;
		delete cur;
		cur = next;
	}
}

void printItem(ItemDB* item, int num)
{
	cout << num << " ";
	cout << item->author << " ";
	cout << item->title << " ";
	cout << item->publisher << " ";
	cout << item->year << " ";
	cout << item->pages << " ";
	cout << endl;
}

void printHeadDataBase(ListItem * db, int offset, int count )
{
	int ind = 0;
	ListItem* cur = db;
	for (;cur != 0 && ind < offset; cur = cur->next)
	{
		ind++;
	}

	for (;cur!=0 && count--; cur=cur->next)
	{
		ItemDB * item = cur->data;
		printItem(item, ind + 1);
		ind++;
	}
}

int countDoubleChar(ListItem* dataBase, CodeChar * arrCounter)
{
	int count = 0;

	for (ListItem * cur = dataBase; cur!=0; cur = cur -> next)
	{
		char * arrField[] = { cur->data->author, cur->data->publisher, cur->data->title};
		int lenArrField = sizeof(arrField) / sizeof(char*);
		for (int i=0; i < lenArrField; i++)
		{
			int lenField = getLenStr(arrField[i]);
			for (int k=0; k < lenField;  k++)
			{
				count++;
				unsigned char code = (unsigned char) arrField[i][k];
				arrCounter[code].ch = arrField[i][k];
				arrCounter[code].probability += 1;
			}
		}
	}
	return count;
}

CodeTable getTableForCharFieldWithOutCode(ListItem* dataBase)
{
	const int size = 256;
	CodeChar arrCounter[size];
	CodeTable table;
	table.size = 0;

	for (int i=0; i < size; i ++)
	{
		arrCounter[i].ch = 0;
		arrCounter[i].code = 0;
		arrCounter[i].probability = 0;
		arrCounter[i].top = 0;
	}

	int count = countDoubleChar(dataBase, arrCounter);

	for (int i=0; i < size; i++)
	{
		if (arrCounter[i].probability>0)
		{
			table.size++;
			arrCounter[i].probability /= count;
		}
	}

	table.codeChar = new CodeChar[table.size];

	for (int i=0, pos =0; i < size; i ++)
	{
		if (arrCounter[i].probability > 0)
		{
			table.codeChar[pos].ch = arrCounter[i].ch;
			table.codeChar[pos].probability = arrCounter[i].probability;
			table.codeChar[pos].code = 0;
			table.codeChar[pos].top = 0;
			pos++;
		}
	}

	return table;
}

void sortByProbability(CodeTable & table)
{
	for (int j = 1; j <= table.size - 1; j++) 
	{
		for (int i = 0; i < table.size - 1; i++) 
		{
			if ((table.codeChar[i].probability) > (table.codeChar[i + 1].probability)) {
				CodeChar temp;
				temp.probability = table.codeChar[i].probability;
				temp.ch = table.codeChar[i].ch;

				table.codeChar[i].probability = table.codeChar[i + 1].probability;
				table.codeChar[i].ch = table.codeChar[i + 1].ch;

				table.codeChar[i + 1].probability = temp.probability;
				table.codeChar[i + 1].ch = temp.ch;
			}
		}
	}
}

void shannon(int l, int h, CodeTable & table)
{
	if (l >= h)
		return;

	if ((l + 1) == h) // 10(10) = 1010(2)  1010 << 1 -> 10100
	{
		table.codeChar[h].top+=1;
		table.codeChar[h].code = (table.codeChar[h].code << 1) | 0;
		table.codeChar[l].top += 1;
		table.codeChar[l].code = (table.codeChar[l].code << 1) | 1;
	}
	else 
	{
		float pack1 = 0, pack2 = 0, diff1 = 0, diff2 = 0;
		int i=0, d=0, k=0, j=0;

		for (i = l; i <= h - 1; i++)
			pack1 = pack1 + table.codeChar[i].probability;

		pack2 = pack2 + table.codeChar[h].probability;

		diff1 = pack1 - pack2;
		if (diff1 < 0)
			diff1 = diff1 * -1;
		j = 2;
		while (j != h - l + 1) {
			k = h - j;
			pack1 = pack2 = 0;
			for (i = l; i <= k; i++)
				pack1 = pack1 + table.codeChar[i].probability;
			for (i = h; i > k; i--)
				pack2 = pack2 + table.codeChar[i].probability;
			diff2 = pack1 - pack2;
			if (diff2 < 0)
				diff2 = diff2 * -1;
			if (diff2 >= diff1)
				break;
			diff1 = diff2;
			j++;
		}
		k++;

		for (i = l; i <= k; i++)
		{
			table.codeChar[i].top += 1;
			table.codeChar[i].code = (table.codeChar[i].code << 1) | 1;
		}

		for (i = k + 1; i <= h; i++)
		{
			table.codeChar[i].top += 1;
			table.codeChar[i].code = (table.codeChar[i].code << 1) | 0;
		}

		shannon(l, k, table);
		shannon(k + 1, h, table);
	}
}

void calcCodeShannonFano(CodeTable & table)
{
	sortByProbability(table);
	shannon(0, table.size-1, table);
}

ListItem* encodeDataBase(CodeTable & table, ListItem* dataBase)
{
	return 0;
}

list<int> toBitArray(CodeChar code)
{
	list<int> res;
	unsigned long long v = code.code;
	for (int i = 0; i< code.top ; i++)
	{
		res.push_front(v % 2);
		v = v / 2;
	}
	return res;
}

void showCodeTableProbabilityCharacter(CodeTable & table)
{
	for (int i=0; i < table.size; i++)
	{
		cout << table.codeChar[i].ch << ": "
			<< table.codeChar[i].probability
			<< " code: ";
		for (int k: toBitArray(table.codeChar[i]))
		{
			cout << k;
		}
		cout << endl;
	}
}

void showCodeTableProbabilityByte(CodeTable& table)
{
	for (int i = 0; i < table.size; i++)
	{
		cout << (int)(unsigned char)table.codeChar[i].ch << ": " << table.codeChar[i].probability<< endl;
	}
}

CodeTable getTableForDB(const char * dbName)
{
	const int size = 256;
	CodeChar arrCounter[size];
	CodeTable table;
	table.size = 0;
	ifstream in(dbName, ifstream::binary);

	in.seekg(0, in.end);
	int sizeFile = in.tellg();
	in.seekg(0, in.beg);
	unsigned char* data = new unsigned char[sizeFile];
	in.read((char*)data, sizeFile);
	in.close();

	for (int i = 0; i < size; i++)
	{
		arrCounter[i].ch = 0;
		arrCounter[i].code = 0;
		arrCounter[i].probability = 0;
		arrCounter[i].top = 0;
	}

	for (int i = 0; i < sizeFile; i++)
	{
		int pos = (unsigned char)data[i];
		arrCounter[pos].ch = (char)data[i];
		arrCounter[pos].probability += 1;
	}

	for (int i = 0; i < size; i++)
	{
		if (arrCounter[i].probability > 0)
		{
			table.size++;
			arrCounter[i].probability /= sizeFile;
		}
	}

	table.codeChar = new CodeChar[table.size];

	for (int i = 0, pos = 0; i < size; i++)
	{
		if (arrCounter[i].probability > 0)
		{
			table.codeChar[pos].ch = arrCounter[i].ch;
			table.codeChar[pos].probability = arrCounter[i].probability;
			table.codeChar[pos].code = 0;
			table.codeChar[pos].top = 0;
			pos++;
		}
	}
	delete[] data;
	return table;
}

double calcEntropy(CodeTable& table)
{
	double result = 0;
	for (int i=0; i<table.size; i++)
	{
		result -= table.codeChar[i].probability * log2(table.codeChar[i].probability);
	}
	return result;
}

double meanLenCode(CodeTable& table)
{
	double result = 0;
	for (int i = 0; i < table.size; i++)
	{
		result += table.codeChar[i].top;
	}
	result /= table.size;
	return result;
}

CodeChar getCodeFromTable(CodeTable& table, char ch)
{
	for (int i = 0; i < table.size; i++)
	{
		if (table.codeChar[i].ch == ch)
			return table.codeChar[i];
	}
	throw exception("Tabel has not character");
}

void encodeFile(const char * dbName, const char * encodedFile, CodeTable & table)
{
	typedef unsigned int uint;

	ifstream in(dbName, ifstream::binary);

	in.seekg(0, in.end);
	int sizeFile = in.tellg();
	in.seekg(0, in.beg);
	unsigned char* data = new unsigned char[sizeFile];
	in.read((char*)data, sizeFile);
	in.close();

	uint *  encodedData = new uint[sizeFile];
	uint encodedBit = 0;
	for (int i=0; i < sizeFile; i++)
	{
		encodedData[i] = 0;
	}

	int cellSizeBit = sizeof(uint)*8;

	for (int i = 0; i < sizeFile; i++)
	{
		CodeChar code = getCodeFromTable(table, data[i]);

		int position = encodedBit / cellSizeBit;
		int countBusyBit = encodedBit % cellSizeBit;
		int countFreeBit = cellSizeBit - countBusyBit;

		if (countFreeBit >= code.top)
		{
			int countZero = countFreeBit - code.top;
			uint mask = code.code;
			mask = mask << countZero;
			encodedData[position] |= mask;
		}
		else
		{
			int rightShift = code.top - countFreeBit;
			uint mask = code.code;
			mask = mask >> rightShift;
			uint mask2 = code.code;
			int letfShift = cellSizeBit - rightShift;
			mask2 = mask2 << letfShift;

			encodedData[position] |= mask;
			encodedData[position+1] |= mask2;
		}
		encodedBit += code.top;
	}



	int encodeByte = ((encodedBit - 1) / 8) + 1;

	ofstream fout(encodedFile, ifstream::binary);
	fout.write((char*)encodedData, encodeByte);
	fout.close();

	delete[] data;
	delete[] encodedData;
}

void menu()
{
		cout << "select action:" << endl;
		cout << "1. show top 20" << endl;
		cout << "2. sorted data" << endl;
		cout << "3. find by key" << endl;
		cout << "4. build tree" << endl;
		cout << "5. find into tree" << endl;
		cout << "6. show code table" << endl;
		cout << "7. show avg len code word" << endl;
		cout << "8. save compress file" << endl;
		cout << "0. exit" << endl;
}


int main()
{
	
	const int countItemDB = 4000;
	const char* dbName = "testBase1.dat";
	const char* encodeDbName = "testBase1_encode.dat";
	ListItem* dataBase = readDataBase(dbName, countItemDB);
	ItemDB** dataArray=0;
	ListItem* filtredData=0;
	BTree tree(2);
	CodeTable table = getTableForDB(dbName);
	calcCodeShannonFano(table);

	int action = 0;

	while (1)
	{
		menu();
		cin >> action;

		if (action == 1)
		{
			printHeadDataBase(dataBase, 0, 20);
		}
		else if (action == 2)
		{
			mergeSort(&dataBase);
		}
		else if (action == 3)
		{
			if (filtredData != 0)
			{
				clearMemoryDataBase(filtredData);
			}
			dataArray = toArray(dataBase, countItemDB);
			char key[] = "Архипов Ltd  ";
			filtredData = filter(dataArray, countItemDB, key);
			printHeadDataBase(filtredData, 0, countItemDB);
		}
		else if (action == 4)
		{
			if (filtredData != 0)
			{
				for (ListItem * tmp = filtredData; tmp!=0; tmp = tmp->next)
				{
					tree.insert(tmp->data->year, tmp->data);
				}
				tree.traverse();
			}
			else
			{
				cout << "to do 3 " << endl;
			}
		}
		else if (action == 5)
		{
			printHeadDataBase(tree.search(1990), 0, -1);
		}
		else if (action == 6)
		{

			showCodeTableProbabilityCharacter(table);
		}
		else if (action == 7)
		{
			cout << "entropy file: " << calcEntropy(table) << endl;
			cout << "mean len code: " << meanLenCode(table) << endl;
		}
		else if (action == 8)
		{
			encodeFile(dbName, encodeDbName, table);
		}
		else if (action == 0)
		{
			break;
		}
	}
	
	//ListItem * dataBase = readDataBase(dbName, countItemDB);
	////CodeTable table = getTableForCharFieldWithOutCode(dataBase);
	//CodeTable table = getTableForDB(dbName);
	//calcCodeShannonFano(table);
	//showCodeTableProbabilityCharacter(table);
	//cout << "entropy file: " << calcEntropy(table) << endl;
	//cout << "mean len code: " << meanLenCode(table) << endl;
	//encodeFile(dbName, encodeDbName, table);

	////mergeSort(&dataBase);

	////printHeadDataBase(dataBase, 0, 100);

	////ItemDB ** dataArray = toArray(dataBase, countItemDB);

	////printItem(dataArray[0], 1);
	////printItem(dataArray[1], 2);
	////printItem(dataArray[2], 3);
	////printItem(dataArray[3], 4);

	//char key[] = "Архипов Ltd  ";

	//ListItem * filtredData = filter(dataArray, countItemDB, key);

	//printHeadDataBase(filtredData, 0, countItemDB);

	/*BTree tree(2);
	for (ListItem * tmp = filtredData; tmp!=0; tmp = tmp->next)
	{
		tree.insert(tmp->data->year, tmp->data);
	}*/

	//tree.traverse();

	//printHeadDataBase(tree.search(1990), 0, -1);


	//clearMemoryDataBase(dataBase);
	//deleteArray(dataArray);

	return 0;
}