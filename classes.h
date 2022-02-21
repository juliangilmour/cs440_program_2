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
	// const int BLOCK_OVERHEAD = 18;
	// int block_size;

    vector<int> pageDirectory;  // Where pageDirectory[h(id)] gives page index of block
                                // can scan to pages using index*PAGE_SIZE as offset (using seek function)
    vector<int> block_sizes;
    
    int numBuckets; // n
	int nextAvailableBlock;
	// int sizeOfAllRecords;
    int i;
    int numRecords; // Records in index
    priority_queue<int> nextFreeBlock; // Next page to write to
	string fName;
    // fstream index_record_file;

	//our 3 buffers
	Block buffer1;// buffer2;
    Block buffer2;
	string buffer3;
	//////////////

    // Insert new record into index
    void insertRecord(Record record) {

        // No records written to index yet
        if (numRecords == 0) {
            // Initialize index with first blocks (start with 2)
			numBuckets = 2;
			nextAvailableBlock = 2;
			// sizeOfAllRecords = 0;

			pageDirectory.resize(2);
            block_sizes.resize(pageDirectory.size());
			pageDirectory[0] = 0;
			pageDirectory[1] = 1;
        }
		
		int m = hashId(record.id);//hash value
        int location_of_page;
        int record_size = 19 + strlen(record.name.c_str()) + strlen(record.bio.c_str());//16->19 because 3 delimiters
        
		//check if it goes in a bucket we are already using
		if(m <= numBuckets - 1){
			location_of_page = pageDirectory[m];;
		}
			//m is a ghost bucket
		else{
			m = m%(int)pow(2, i-1);
			location_of_page = pageDirectory[m];//shift 1 bit over for ghosts BOO!
		}
        while(1){
			//hash target is in an existing bucket
			
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
				break;
            }
			//does not fit, go to overflow
			///if overflow exists
			if(buffer1.overflow){
				location_of_page = buffer1.overflow;
				// continue;
			}
			//overflow does not exist yet
			else{
				//there are available blocks to overflow into (should be empty and initialized)
				if(nextFreeBlock.size()){
					//buffer1 overflow is updated with block from pq then returned to file
					buffer1.overflow = nextFreeBlock.top();
					nextFreeBlock.pop();
					buffer3 = buffer1.encode();
					putBackInBlock(buffer3, location_of_page);

					//next iteration is at the overflow block
					location_of_page = buffer1.overflow;
					// continue;
				}
				//create a new block to overflow into (should be empty and uninitialized)
				else{
					//put set overflow as a new block at the end of the file thne return to file
					buffer1.overflow = nextAvailableBlock;
					nextAvailableBlock++;
					buffer3 = buffer1.encode();
					// block_sizes[location_of_page] = buffer3.length();
					block_sizes.push_back(0);
					putBackInBlock(buffer3, location_of_page);
					//will try to insert at overflow next iteration
					location_of_page = buffer1.overflow;
				}

		    }

        }
		
		//SPLIT
        
		//get size
		int fileSize = 0;
		for (int i = 0; i < nextAvailableBlock; i++){
			fileSize += block_sizes[i];
		}

        // check size for 70%
		if((float)fileSize/(float)(numBuckets * PAGE_SIZE) >= .7){
			int n = numBuckets; //hash value of new bucket
            
			if(nextFreeBlock.size()){
				pageDirectory.push_back(nextFreeBlock.top());
				nextFreeBlock.pop();
			}
			else{
				pageDirectory.push_back(nextAvailableBlock);
				nextAvailableBlock++;
			}
			numBuckets++;
			int n1 = n%(int)pow(2, i-1);//hash value of bucket that was used when this was a ghost bucket

            // retrieve pagelocation[n1]
            bool going = true;
    // Go over this for all of the overflow blocks from n1 also
            while(going){
                buffer1 = getBlockFromRecord(pageDirectory[n1]);
                // Used to go over the overflows

                

                
                // rehash all  records in that block storing them in the correct locations they need to go.
                for(int j = 1; j < buffer1.numRecords; j++){
                    // Gets record to rehash id
                    Record temp = buffer1.records[j];
                    // Move if record was in a ghost block
                    if( hashId(temp.id) == n){
                        // Removes the record from buffer1
                        buffer1.records.erase(buffer1.records.begin()+j);
                        buffer1.numRecords--;
                        // add to a new block then write it
                        buffer2.records.push_back(temp);
                        // For after deleting staying in place so that it can check the record that moved down one place
                        j--;
                    }
                }
                // Moved all records changes to overflow
                int temp_overflow = buffer1.overflow;
                if (!buffer1.overflow) {
                    going = false;
                    
                }
                else{
                    n1 = buffer1.overflow;
                }
                
                // Check for overflows being empty and if so revert them back to availible block

                


            }
            // Rewrite buffer1 and buffer2
            // turn both into string
            // Old Block being put back
            buffer3 = buffer1.encode();
            // Location 
            block_sizes[n1] = buffer3.length();
            location_of_page = pageDirectory[n1];
            putBackInBlock(buffer3, location_of_page);

            // New block put in
            buffer3 = buffer2.encode();
            // Location 
            // Should be the end
            block_sizes[n] = buffer3.length();
            location_of_page = pageDirectory[n];
            putBackInBlock(buffer3, location_of_page);





		}
        

    }
    
    int hashId(int id){
		i = ceil(log2(float(numBuckets)));
		return (id%(int)pow(2,i));
		// return (id%(int)pow(2,16) && ((1 << i) - 1));
	} 


    Block getBlockFromRecord(int loc){
		fstream index_record_file;
		index_record_file.open("EmployeeIndex.csv", fstream::in);
        // if(loc == -1) return Block();
        // Was getting error when compiling, 
        // index_record_file.seekg(0, loc * PAGE_SIZE);
        index_record_file.seekg(loc * PAGE_SIZE);
        char iobuffer[4097] = {0};
		// iobuffer[4096] = 0;
        index_record_file.read(iobuffer, PAGE_SIZE);
        if(iobuffer[0] == 0){
			// Block newBlock = Block();
			index_record_file.close();
            return Block();
        }
		index_record_file.close();
        return Block(string(iobuffer));
    }

    void putBackInBlock(string output, int loc){
		fstream index_record_file;
		index_record_file.open(fName, ios::in | ios::out);
		index_record_file.clear();
        index_record_file.seekp(loc * PAGE_SIZE, ios::beg);
		int helpme = index_record_file.tellp();
		// index_record_file << output;
        char iobuffer[4097] = {0};
		// iobuffer[4096] = 0;
        strcpy(iobuffer, output.c_str());
        index_record_file.write(iobuffer, PAGE_SIZE);
		index_record_file.close();
		return;
    }

public:
    LinearHashIndex(string indexFileName) {
        numBuckets = 0;
        i = 1;
        numRecords = 0;
        int block_size = PAGE_SIZE;
        fName = indexFileName;
		ofstream output(fName);
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
    //     int m = hashId(id);

    //     if(m <= numBuckets - 1){
	// 		location_of_page = pageDirectory[m];;
	// 	}
	// 		//m is a ghost bucket
	// 	else{
	// 		m = m%(int)pow(2, i-1);
	// 		location_of_page = pageDirectory[m];//shift 1 bit over for ghosts BOO!
	// 	}

    // }

	
};