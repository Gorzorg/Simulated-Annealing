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
    int line_count= -1;
    if (format == "matrix"){
        while(CSV_FILE){
            V.push_back( vector<float>() );
            line_count++;
            getline(CSV_FILE, S, '\n');
            size_t current_position=0, next_comma;
            while( current_position != string::npos){
                next_comma = S.find_first_of(',', current_position);
                V[line_count].push_back(stof( S.substr(current_position, next_comma - current_position ) ));
                current_position = next_comma +1;
            }
        }
        int L = V.size();
        for(int c=0; c<L; c++){
            if(V[c].size() != L){
                V = vector< vector<float> > ();
                V.push_back( vector<float> (-1) );
                return V;
            }
        }
        return V;
    }
}