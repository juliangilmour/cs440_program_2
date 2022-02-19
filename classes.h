#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <bitset>
#include <queue>
// Added for strlen
#include <cstring>
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

class Block {
public:
    int numRecords = 0;
    int overflow = 0;
    vector<Record> records;

    Block(){
		// int numRecords = 0;
    	// int overflow = -1;
	}

    Block(string data){
        stringstream ss(data);
        string temp;
        // getline(ss, temp, '$');//gets first delimiter and stops, null string
		getline(ss, temp, '$');//this actually gets the first part
        numRecords = stoi(temp, 0, 2);
        getline(ss, temp, '$');
        overflow = stoi(temp, 0, 2);
        for(int i = 0; i < numRecords; i++){
            vector<string> fields;
            fields.resize(4);
            getline(ss, temp, '$');
            stringstream tuple(temp);
            getline(tuple,fields[0], '#');
            getline(tuple,fields[1], '#');
            getline(tuple,fields[2], '#');
            getline(tuple,fields[3], '#');
            records.push_back(Record(fields));
        }
    }
    
    // Binary conversion
    string encode(){
		string nR = bitset<8>(numRecords).to_string();
		string oF = bitset<8>(overflow).to_string();
        string output = 
        nR + 
        "$" + oF + 
        "$";
        for(int i = 0; i < numRecords; i++){
            output = output + to_string(records[i].id) + 
            "#" + records[i].name + 
            "#" + records[i].bio + 
            "#" + to_string(records[i].manager_id) + "$";
        }
        return output;
    }
    

};


class LinearHashIndex {

private:
    const int PAGE_SIZE = 4096;
	int block_size;

    vector<int> pageDirectory;  // Where pageDirectory[h(id)] gives page index of block
                                // can scan to pages using index*PAGE_SIZE as offset (using seek function)
    vector<int> block_sizes;
    
    int numBlocks; // n
    int i;
    int numRecords; // Records in index
    priority_queue<int> nextFreeBlock; // Next page to write to
	string fName;
    // fstream index_record_file;

	//our 3 buffers
	Block buffer1;// buffer2;
	string buffer3;
	//////////////

    // Insert new record into index
    void insertRecord(Record record) {

        // No records written to index yet
        if (numRecords == 0) {
            // Initialize index with first blocks (start with 2)
			numBlocks = 2;
			pageDirectory.resize(pow(2, i));
            block_sizes.resize(pageDirectory.size());
			// index_record_file.open("EmployeeIndex.csv");
        }

        
        

        

        //2 get the "block"
        //find the spot? ie the size, check pagesize - size to see if size fits else go to overflow 
        // Gets the block by calling getBlock with the location index
        int location_of_page = pageDirectory[hashId(record.id)];
        

        // Finds size of record by getting all real sizes
        int record_size = 19 + strlen(record.name.c_str()) + strlen(record.bio.c_str());//16->19 because 3 delimiters
        // Looking to see there is not enough room in block to add record to it  
        // Not enough room go to over flow
        // bool flagInserted = false;
        while(1){
			buffer1 = getBlockFromRecord(location_of_page);
            // There is enough room to write this record to the block
            if(block_sizes[location_of_page] + record_size < PAGE_SIZE){
                
                // Encodes the block
                buffer1.records.push_back(record);
                buffer1.numRecords++;
                numRecords++;
                buffer3 = buffer1.encode();
                // Updating size of block
                block_sizes[location_of_page] = buffer3.length();//strlen(buffer1.encode().c_str());
                // Writes encoded block
                putBackInBlock(buffer3, location_of_page);
				// flagInserted = true;
				return;
            }
            // Not enough room so need for new block
            // For do while to find the end of buffer
            location_of_page = buffer1.overflow;
			if(location_of_page == 0){

				if(nextFreeBlock.size()){
					location_of_page = nextFreeBlock.top();
					nextFreeBlock.pop();
					numBlocks++;
					buffer1.overflow = location_of_page;
				}
				else{
					location_of_page = numBlocks;
					numBlocks++;
                    
                    buffer1.records.push_back(record);
                    buffer1.numRecords++;
                    numRecords++;
                    buffer3 = buffer1.encode();
                    // Updating size of block
                    block_sizes[location_of_page] = buffer3.length();//strlen(buffer1.encode().c_str());
                    // Writes encoded block
                    putBackInBlock(buffer3, location_of_page);
                    return;
				}

		    }

        }

        //TODO/////////
        //dowesplit?
        //if yes, SPLIT
        ////////////////

    }
    
    int hashId(int id){
		//return id%(int)pow(2, i);
		return (id%(int)pow(2,16) && ((1 << i) - 1));
	}

    // Getting some more errors here related to this being in the LinearHashIndex Class /////////////////////////////////////////////////////////////////////

    Block getBlockFromRecord(int loc){
		fstream index_record_file;
		index_record_file.open("EmployeeIndex.csv", fstream::in);
        // if(loc == -1) return Block();
        // Was getting error when compiling, 
        // index_record_file.seekg(0, loc * PAGE_SIZE);
        index_record_file.seekg(loc * PAGE_SIZE);
        char iobuffer[4097] = {'%'};
		iobuffer[4096] = 0;
        index_record_file.read(iobuffer, PAGE_SIZE);
        if(iobuffer[0] == '%'){
			Block newBlock = Block();
			index_record_file.close();
            return newBlock;
        }
		index_record_file.close();
        return Block(string(iobuffer));
    }

    void putBackInBlock(string output, int loc){
		fstream index_record_file;
		index_record_file.open("EmployeeIndex.csv", fstream::out);
        // Was getting error when compiling, 
        // index_record_file.seekg(0, loc * PAGE_SIZE);
        index_record_file.seekg(loc * PAGE_SIZE);
        char iobuffer[4097] = {'%'};
		iobuffer[4096] = 0;
        strcpy(iobuffer, output.c_str());
        index_record_file.write(iobuffer, PAGE_SIZE);
		index_record_file.close();
		return;
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
		string token, s;
        
		while(getline(input, s)){
			size_t pos = 0;
			// getline(input, s);
			stringstream ss(s);
            vector<string> record;
            for(int i = 0; i < 4; i++){
                getline(ss, token, ',');
				record.push_back(token);
                // record[i] = token;
            }
            insertRecord(Record(record));
		}
		input.close();
		// insertRecord
    }

    // Given an ID, find the relevant record and print it
    // Record findRecordById(int id) {

    // }

	
};