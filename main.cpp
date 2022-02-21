/*
Skeleton code for linear hash indexing
*/

#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include "classes.h"
using namespace std;


int main(int argc, char* const argv[]) {

    // Create the index
	int id = 48569047;
	int n = 5;
	int m = 9;
	 int i = ceil(log2(float(n)));
	cout << i << " " << m / (int)pow(2, i-1) << endl;
    // LinearHashIndex emp_index("EmployeeIndex.csv");
    // emp_index.createFromFile("Employee.csv");
	// cout << "over" << endl;
    // int search_id = 0;
    // bool going = true;
    // int check = 0;
    
    // Loop to lookup IDs until user is ready to quit
    // while (going){
    //     cout << "Enter an ID: ";
    //     cin >> search_id;
    //     cout << "Search for " << search_id;
    //     // Area for actual Search  // 

    //     // Hash serch_id

    //     // get pageDirectory[HashedID] * Pagesize
        
    //     // Open filestream of EmployeeIndex.csv

    //     // Get block read from file

    //     // Decode

    //     // Match ID to ID from block

    //     // Return Record


    //     /////////////////////////////
    //     cout << "Do you want to quit(1: Yes, 0: No): ";
    //     cin >> check;

    //     if (check == 1){
    //         going = false;
    //     }
        
    // }

    return 0;
}
