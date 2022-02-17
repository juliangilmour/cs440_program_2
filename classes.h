#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <bitset>
#include <queue>
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
    int overflow = -1;
    vector<Record> records;
    Block();
    Block(string data){
        stringstream ss(data);
        string temp;
        getline(ss, temp, '$');
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
        string output = 
        "$" + bitset<8>(numRecords).to_string() + 
        "$" + bitset<8>(numRecords).to_string() + 
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
    fstream index_record_file; 

    // Insert new record into index
    void insertRecord(Record record) {

        // No records written to index yet
        if (numRecords == 0) {
            // Initialize index with first blocks (start with 2)
			numBlocks = 2;
			pageDirectory.resize(pow(2, i));
            block_sizes.resize(pageDirectory.size());
			index_record_file.open("EmployeeIndex.csv");
        }

        

        //1 find where its going
        // Line of EmployeeIndex
        int location_of_page = pageDirectory[hashId(record.id)];
        

        

        //2 get the "block"
        //find the spot? ie the size, check pagesize - size to see if size fits else go to overflow 
        // Gets the block by calling getBlock with the location index
        Block buffer1 = getBlockFromRecord(location_of_page);

        // Finds size of record by getting all real sizes
        int record_size = 16 + strlen(record.name.c_str()) + strlen(record.bio.c_str());
        // Looking to see there is not enough room in block to add record to it  
        // Not enough room go to over flow
            
        do{///TODO while flag_has_inserted == false

            // There is enough room to write this record to the block
            if(block_sizes[location_of_page] < PAGE_SIZE && block_sizes[location_of_page] + record_size < PAGE_SIZE){
                
                // Encodes the block
                buffer1.records.push_back(record);
                buffer1.numRecords++;
                string output_to_buffer = buffer1.encode(); 
                // Updating size of block
                block_sizes[location_of_page/PAGE_SIZE] += output_to_buffer.length();//strlen(buffer1.encode().c_str());
                // Writes encoded block
                putBackInBlock(output_to_buffer, location_of_page);
                numRecords++;
            }
            // Not enough room so need for new block
            // For do while to find the end of buffer
            location_of_page = buffer1.overflow;
            if (block_sizes[location_of_page] + record_size > PAGE_SIZE){
                Block buffer2 = getBlockFromRecord(location_of_page);
                if (numRecords == 0){
                    buffer1.overflow = nextFreeBlock[0];
                    // POP PQ
                    numBlocks++;
                }
                // if num records == 0,
                //  set last pointer to num_blocks
                //  n++
                //


            }

        }while(location_of_page != -1);

            // find space in blocksize == 0
            // get index
            // block.records.pushback(record);
            // set that to recordsize
            // fill pageDirectory[hashId(record.id)] with index
            // write data
        //Overflow
        //back to find spot
        //$1$1$tu-p-l-e-s$
        // Add record to the index in the correct block, creating overflow block if necessary
        
        // Take neccessary steps if capacity is reached


        //TODO/////////
        //dowesplit?
        //if yes, SPLIT
        ////////////////

    }
    
    int hashId(int id){
		return id%(int)pow(2, i);
		//return (id%(int)pow(2,16) && ((1 << i) - 1))
	}

    Block getBlockFromRecord(int loc){
        if(loc == -1) return Block();
        index_record_file.seekg(0, loc * PAGE_SIZE);
        char buffer[4097] = {0};
        index_record_file.read(buffer, PAGE_SIZE);
        // if(buffer[0] = 0){
        //     return Block();
        // }
        return Block(string(buffer));
    }

    void putBackInBlock(string output, int loc){
        index_record_file.seekg(0, loc * PAGE_SIZE);
        char buffer[4097] = {0};
        strcpy(buffer, output.c_str());
        index_record_file.write(buffer, PAGE_SIZE);
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
            vector<string> record;
            for(int i = 0; i < 4; i++){
                getline(ss, token, ',');
                record[i] = token;
            }
            insertRecord(Record(record));
		}
		input.close();
		// insertRecord
    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {
        
    }

	
};