#include <iostream>
#include <fstream>
//ifstream (derived from istream), ofstream (derived from ostream), and fstream (derived from iostream). These classes do file input, output, and input/output respectively.
#include <vector>
#include <string.h>

using namespace std;



float CSV_to_edge_length( std::ifstream CSV_FILE, std::string format, int node1, int node2 ){
    // takes as input a CSV file, and two node IDs. Returns the length of the edge from node1 to node2
    // if the file is somehow corrupted, it should return -1. 
    string S;
    if (format=="matrix"){
        for(int c=0; c<node1; c++){
            if(! CSV_FILE){
                return -1;
            }
            getline(CSV_FILE, S, '\n');
        }
        for(int c=0; c<node2; c++){
            if(! CSV_FILE){
                return -1;
            }
            getline(CSV_FILE, S, ',');
        }
        if(! CSV_FILE){
            return -1;
        }
        getline(CSV_FILE, S, ',');
        return stof(S);
    }
}


vector<vector<float> > CSV_to_graph( std::ifstream CSV_FILE, std::string format ) {
    //given a CSV file, and an encoding format, it returns a graph adjacency matrix.
    // if the file is corrupted, it returns [ [ -1] ]
    vector<vector<float> > V;//maybe inefficient
    string S;
    if (format == "matrix"){
        while(CSV_FILE){//while there is something more to read
            V.push_back( vector<float>() );//push_back is a method provided by the vector class, which expands the vector adding the argument you provide it after the last entry.
            getline(CSV_FILE, S, '\n');//reads the file you CSV_FILE, until it finds a \n char, and then stores the result (excluding the \n) into the string S.
            size_t current_position=0, next_comma;
            while( current_position != string::npos){
                next_comma = S.find_first_of(',', current_position);//find_first_of returns the first index such that S[index] == ','
                V.back().push_back(stof( S.substr(current_position, next_comma - current_position ) )); /* the back() method returns the last entry of a vector,
                the stof function transforms a string into a float number, and the S.substr(a,b) returns the string S[a]...S[a+b-1] . */
                current_position = next_comma +1;
            }
        }
        int L = V.size();
        for(int c=0; c<L; c++){//this block checks if V is actually a square matrix
            if(V[c].size() != L){// if it is not, initialize V = [ [1] ] and return it
                V = vector< vector<float> > ();
                V.push_back( vector<float> (-1) );
                return V;
            }
        }
        return V;
    }
}
// Test #2