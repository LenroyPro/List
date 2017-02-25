#include <string>
#include <utility>

using namespace std;
/*
changes have been made in 
virtual bool update(const string& key, const TYPE& value);
removed Sort function
virtual ~SimpleTable();
virtual const SimpleTable& operator=(const SimpleTable& other);
*/

template <class TYPE>
class Table{
public:
	Table(){}
	virtual bool update(const string& key, const TYPE& value)=0;
	virtual bool remove(const string& key)=0;
	virtual bool find(const string& key, TYPE& value)=0;
	virtual ~Table(){}
};

template <class TYPE>
class SimpleTable:public Table<TYPE>{

	struct Record{
		TYPE data_;
		string key_;
		Record(const string& key, const TYPE& data){
			key_=key;
			data_=data;
		}

	};

	Record** records_;   
	int max_;           
	int size_;          
	int search(const string& key);
	void grow();
public:
	SimpleTable(int maxExpected);
	SimpleTable(const SimpleTable& other);
	SimpleTable(SimpleTable&& other);
	virtual bool update(const string& key, const TYPE& value);
	virtual bool remove(const string& key);
	virtual bool find(const string& key, TYPE& value);
	virtual const SimpleTable& operator=(const SimpleTable& other);
	virtual const SimpleTable& operator=(SimpleTable&& other);
	virtual ~SimpleTable();
};



template <class TYPE>
int SimpleTable<TYPE>::search(const string& key){
	int rc=-1;
	for(int i=0;i<size_;i++){
		if(records_[i]->key_==key){
			rc=i;
		}
	}
	return rc;
}


template <class TYPE>
void SimpleTable<TYPE>::grow(){
	Record** newArray=new Record*[max_+1];
	max_=max_+1;
	for(int i=0;i<size_;i++){
		newArray[i]=records_[i];
	}
	delete [] records_;
	records_=newArray;
}


template <class TYPE>
SimpleTable<TYPE>::SimpleTable(int maxExpected): Table<TYPE>(){
	records_=new Record*[maxExpected];
	max_=maxExpected;
	size_=0;
}

template <class TYPE>
SimpleTable<TYPE>::SimpleTable(const SimpleTable<TYPE>& other){
	records_=new Record*[other.max_];
	max_=other.max_;
	size_=0;
	for(int i=0;i<other.size_;i++){
		update(other.records_[i]->key_,other.records_[i]->data_);
	}
}
template <class TYPE>
SimpleTable<TYPE>::SimpleTable(SimpleTable<TYPE>&& other){
	size_=other.size_;
	max_=other.max_;
	records_=other.records_;
	other.records_=nullptr;
	other.size_=0;
	other.max_=0;
}

/// I removed the sort function since it served no purpose.

template <class TYPE>
bool SimpleTable<TYPE>::update(const string& key, const TYPE& value){
	int idx=search(key);
	if(idx==-1){
		if(size_ == max_){
			grow();
		}
		records_[size_++]=new Record(key,value);
		
	}
	else{
		records_[idx]->data_=value;
	}
	return true;
}

template <class TYPE>
bool SimpleTable<TYPE>::remove(const string& key){
	int idx=search(key);
	if(idx!=-1){
		delete records_[idx];
		for(int i=idx;i<size_-1;i++){
			records_[i]=records_[i+1];
		}
		size_--;
		return true;
	}
	else{
		return false;
	}
}

template <class TYPE>
bool SimpleTable<TYPE>::find(const string& key, TYPE& value){
	int idx=search(key);
	if(idx==-1)
		return false;
	else{
		value=records_[idx]->data_;
		return true;
	}
}

//removed a for loop 
template <class TYPE>
const SimpleTable<TYPE>& SimpleTable<TYPE>::operator=(const SimpleTable<TYPE>& other){
	if(this!=&other){
		if(records_)
			delete [] records_;
		
		records_=new Record*[other.max_];
		max_=other.max_;
		size_=0;
		for(int i=0;i<other.size_;i++){
			update(other.records_[i]->key_,other.records_[i]->data_);
		}

	}

	return *this;
}
template <class TYPE>
const SimpleTable<TYPE>& SimpleTable<TYPE>::operator=(SimpleTable<TYPE>&& other){
	swap(records_,other.records_);
	swap(size_,other.size_);
	swap(max_,other.max_);
	return *this;
}
// just remove records as a whole with out looping
template <class TYPE>
SimpleTable<TYPE>::~SimpleTable(){
	
	delete [] records_;
	
}


















template <class TYPE>
class HashTable:public Table<TYPE>{

public:
	// this is the record that holds the value and key.
	struct Record{
		string key_;
		TYPE value_;
		Record* next_;
		Record(const string key, TYPE value, Record*next = nullptr){
			key_ = key;
			value_ = value;
			next_ = next;
		}
		Record(){
			key_ = "";
			value_ = 0;
			next_ = nullptr;

		}	
	}; 

	
	Record** table_;
	Record** tablenew_;
	int numrecord_;
	int tablesize_;
	int maxsize_;
	int lastid;
	std::hash<std::string> myhashfunction;
	HashTable(int maxExpected);
	HashTable(const HashTable& other);
	HashTable(HashTable&& other);
	virtual bool update(const string& key, const TYPE& value);
	virtual bool remove(const string& key);
	virtual bool find(const string& key, TYPE& value);
	virtual const HashTable& operator=(const HashTable& other);
	virtual const HashTable& operator=(HashTable&& other);
	virtual ~HashTable();
};


template <class TYPE>
HashTable<TYPE>::HashTable(int maxExpected): Table<TYPE>(){
	table_ = new Record*[maxExpected];

	
	for(int i=0; i<maxExpected; i++){

		table_[i] =  nullptr;

	}
	tablesize_ = maxExpected;
	numrecord_ = 0;

}

template <class TYPE>
HashTable<TYPE>::HashTable(const HashTable<TYPE>& other){
	
	tablesize_ = other.tablesize_; 
	table_ = new Record* [tablesize_];

	for (int i=0; i<tablesize_; i++)
	{	
		Record** p = &table_[i];
		Record* n =  other.table_[i];

		*p = nullptr;
		while (n)
		{
			Record* c = new Record(*n); // Record copy constructor, should set n->next to null
			*p = c;
			n=n->next_;
			p=&c->next_;

		}
	}

}

template <class TYPE>
HashTable<TYPE>::HashTable(HashTable<TYPE>&& other){
	tablesize_ = other.tablesize_;
	table_ = other.table_;
	other.table_ = nullptr;
	other.tablesize_ = 0;

}

template <class TYPE>
bool HashTable<TYPE>::update(const string& key, const TYPE& value){
std::size_t  h1 = myhashfunction(key);// gives me a hash value
int hash = h1 % tablesize_;
TYPE tempvalue;
bool found = true;
found = find(key, tempvalue);

// if the value is found then there is no need to updated the value;
if (found){
	
	Record* newEntry = table_[hash];

	// looks through the whole chain in search for the entry value to change
	while (newEntry != nullptr && newEntry->key_ != key){
		newEntry = newEntry->next_;

	}

	newEntry->value_ = value;
	return true;

}
else{

	if (tablesize_ == numrecord_){
		return false;
	} 
	else{
		//This checks if there is any value at the hash location if it is null then it will insert, else it will chian a value below.
		if (table_[hash] == nullptr) {

			table_[hash] = new Record();
			table_[hash]->key_ = key;
			table_[hash]->value_ = value;
			numrecord_++;

			return true;

		}

		Record* entry = table_[hash];

		while (entry->next_ != nullptr)
			entry = entry->next_;

		entry->next_ = new Record(key,value);
		return true;



	}
}

}

template <class TYPE>
bool HashTable<TYPE>::remove(const string& key){
	std::size_t  h1 = myhashfunction(key);
	int hash = h1 % tablesize_;
// this whole if statment will search the whole chain until the key is found and removed by checking each of the head of the chain and moving down.
	if (table_[hash] != nullptr) {
		Record* prevEntry = nullptr;
		Record* entry = table_[hash];
                  // checks the next enrtry until it hit the end of chain or key is found and the enery holds the prev entry
		while (entry->next_ != nullptr && entry->key_ != key) {
			prevEntry = entry;
			entry = entry->next_;
		}
		if (entry->key_ == key) {
                  	// this will set the hash value to the next entry else it will just set the prev enerty to equal the next enerty of the next item in list;
			if (prevEntry == nullptr) {
				Record* nextEntry = entry->next_;
				delete entry;
				table_[hash] = nextEntry;
				return true;
			} else {
				Record* next = entry->next_;
				delete entry;
				prevEntry->next_ = next ;
				return true;
			}
		}
	}

}

template <class TYPE>
bool HashTable<TYPE>::find(const string& key, TYPE& value){
	std::size_t  h1 = myhashfunction(key);
	int hash = h1 % tablesize_;
	if (table_[hash] == nullptr)
		return false;

	else {
		Record* entry = table_[hash];
		while (entry != nullptr && entry->key_ != key)
			entry = entry->next_;																							
		if (entry == nullptr)
			return false;
		else{ 
			value = entry->value_;
			return true;
		}

	}
}

template <class TYPE>
const HashTable<TYPE>& HashTable<TYPE>::operator=(const HashTable<TYPE>& other){

	if (this != &other)
	{
		delete [] table_ ;
		table_ = new Record*[other.tablesize_];
		for(int i = 0 ; i < other.tablesize_ ; i++)
			table_[i] = other.table_[i] ;
		tablesize_ = other.tablesize_;
	}

	return *this;
}

template <class TYPE>
const HashTable<TYPE>& HashTable<TYPE>::operator=(HashTable<TYPE>&& other){
	if (this != &other)
	{
		delete [] table_ ;
		table_ = new Record*[other.tablesize_];
		for(int i = 0 ; i < other.tablesize_ ; i++)
			table_[i] = other.table_[i] ;
		tablesize_ = other.tablesize_;
	}
	return *this;
}
template <class TYPE>
HashTable<TYPE>::~HashTable(){
	delete[] table_;
}
