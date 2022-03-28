#include <string>
#include <vector>
#include <fstream>

template<typename T>
using matrix = std::vector<std::vector<T> >;

float CSV_to_edge_length( std::ifstream CSV_FILE, std::string format, int node1, int node2 ){
    // takes as input a CSV file, and two node IDs. Returns the length of the edge from node1 to node2
    // if the file is somehow corrupted (or the nodes we are trying to access do not exist), it should return -1. 
    if (format=="matrix"){
        for(int c=0; c<node1; c++){
            if(! CSV_FILE.ignore(std::numeric_limits<std::streamsize>::max(), '\n')){
                return -1;
            }
        }//ignore until row number node1
        for(int c=0; c<node2; c++){
            if(! CSV_FILE.ignore(std::numeric_limits<std::streamsize>::max(), ',')){
                return -1;
            }
        }//ignore until column number node2
        float ret;
        if(CSV_FILE >> ret){
            return ret;
        }
        else{
            return -1;
        }
    }
}


std::vector<std::vector<float> > CSV_to_graph( std::ifstream CSV_FILE, std::string format ) {
    //given a CSV file, and an encoding format, it returns a graph adjacency matrix.
    // if the file is corrupted, it returns [ [ -1] ]
    matrix<float> V = matrix<float>();
    std::string S;
    if (format == "matrix"){
        while(getline(CSV_FILE, S, '\n')){//while there is something more to read (and the line read has been successful)
            std::vector<float> current_line = std::vector<float> ();
            V.push_back( current_line );//push_back is a method provided by the vector class, which expands the vector adding the argument you provide it after the last entry.
            size_t current_position=0;
            size_t next_comma;
            while(true){
                
                next_comma = S.find_first_of(',', current_position);//find_first_of returns the first index such that S[index] == ','
                if (next_comma == std::string::npos){
                    current_line.push_back(stof(S.substr(current_position, S.size() - current_position)));
                    break;
                }
                else{
                    current_line.push_back(stof(S.substr(current_position, next_comma - current_position))); /* the back() method returns the last entry of a vector,
                    the stof function transforms a string into a float number, and the S.substr(a,b) returns the string S[a]...S[a+b-1] . */
                    current_position = next_comma +1;
                }
                
            }
        }
        int L = V.size();
        for(int c=0; c<L; c++){//this block checks if V is actually a square matrix
            if(V[c].size() != L){// if it is not, initialize V = [ [1] ] and return it
                V = matrix<float> (); //Wait, does this cause a memory leak? Try investigating the matter later on.
                V.push_back( std::vector<float> (-1) );
                return V;
            }
        }
        return V;
    }
}