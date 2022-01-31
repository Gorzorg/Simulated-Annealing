#include <iostream>
#include <fstream>
//ifstream (derived from istream), ofstream (derived from ostream), and fstream (derived from iostream). These classes do file input, output, and input/output respectively.
#include <vector>
#include <string.h>
#include <math.h>
#include <random>

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


/* now I am about to start a template. Templates are essentially a piece of code in which you leave some types unspecified.
When calling a template at a later time, you can specify the missing types according to your needs, and you can avoid rewriting
all the code every time you change the type of something you use in it */

/* attempt at implementing simulated annealing
T2 should be a function such that, given an object A of type T1, it outputs an object B of type T1.
T3 should be a function such that, given two objects A and B of type T1, it outpots a number.
T4 should be a callable object which outputs a non-negative number, i.e. the temperature we should use in the next iteration of the simulated annealing.*/
template <typename T1, typename T2, typename T3, typename T4>
T1 simulated_annealing ( T1& initial_state, T2& random_neighbor, T3& difference_cost_function, T4& temperature_scheme ){/* the amperstand after the types means "passed by reference"
think of it as passing a pointer to the object, but the compiler knows that every time you refer to it, it has to implicitly dereference it. This allows to
pass pointers to objects around as one does in python, i.e. pretending you pass the actual object. */
    auto temperature = temperature_scheme(); /*the auto keyword tells the compiler it should try deducing the appropriate type to use for this variable. I am using it here
    because I don't want to deal with details such as choosing the particular return type of temperature_scheme in advance. Similar reason when I use it elsewhere */
    T1 current_state = initial_state;
    T1 new_state;
    while(temperature != 0){// right now the stopping condition is encoded as temperature == 0, but nothing forbids adding one more parameter to this template
        new_state = random_neighbor( current_state );
        auto diff = difference_cost_function( current_state, new_state );// I assume this is cost( current_state ) - cost( new_state )
        if( diff >= 0 ){
            current_state = new_state;
        }
        else{ // else accept the new state with probability exp( diff/temperature ).
            auto treshold = uniform_real_distribution( 0, 1 );
            if (exp( diff/ temperature ) > treshold){
                current_state = new_state; 
            }
        }
        temperature = temperature_scheme(); /* in this implementation, the sequence of temperatures to be used is assumed to be predetermined. maybe in a slightly more complex
        implementation, we could make it dependent on the sequence of states visited so far */
    }
    return current_state;
}

/*
TBD:
- change the template, so that the temperature scheme accepts (current state, diff. cost function with the previous state) as input, and so that it outputs (new temperature, frequency of new temperature).
- new template for simulated annealing, in which the random neighbor and the difference in cost are computed by the same function.

Make functions for traveling salesman problems, specifically:
- a function which creates a random initial path
- a function which, given a path, provides a new random path (we will start with just switching 2 vertices), together with the diff. cost value (which implies we know the cost of the new state).
- a function such that, given ( best state so far & its cost, current state & its cost, temperature & its frequency f), iterates the simulated annealing process f times, updates the best state so far & its cost, and outputs the last state visited & its cost.
*/
