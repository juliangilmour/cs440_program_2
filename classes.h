#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
using namespace std;

class Record {
public:
    int id, manager_id;
    std::string bio, name;

    Record(vector<string> fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    void print() {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }
};


class LinearHashIndex {

private:
    const int PAGE_SIZE = 4096;

    vector<int> pageDirectory;  // Where pageDirectory[h(id)] gives page index of block
                                // can scan to pages using index*PAGE_SIZE as offset (using seek function)
    int numBlocks; // n
    int i;
    int numRecords; // Records in index
    int nextFreePage; // Next page to write to
	int block_size;
	string fName;

    // Insert new record into index
    void insertRecord(Record record) {

        // No records written to index yet
        if (numRecords == 0) {
            // Initialize index with first blocks (start with 2)
			numBlocks = 2;
			pageDirectory.resize(pow(2, i));
			//create output file

        }

        // Add record to the index in the correct block, creating overflow block if necessary

        // Take neccessary steps if capacity is reached


    }

public:
    LinearHashIndex(string indexFileName) {
        numBlocks = 0;
        i = 1;
        numRecords = 0;
        block_size = PAGE_SIZE;
        fName = indexFileName;
    }

    // Read csv file and add records to the index
    void createFromFile(string csvFName) {
		ifstream input(csvFName);
		while(input.eof() == false){
			size_t pos = 0;
			string token, s;
			getline(input, s);
			stringstream ss(s);
			while(getline(ss, token, ',')){

			}
		}
		input.close();
		// insertRecord
    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {
        
    }

	int hashId(int id){
		return id%(int)pow(2, i);
		//return (id%(int)pow(2,16) && ((1 << i) - 1))
	}
};
