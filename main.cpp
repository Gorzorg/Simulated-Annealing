#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>

using namespace std;

//let's try simulated annealing on traveling salesman problem.

//let's specify some shorthand type names
template<typename T>
using matrix = std::vector<std::vector<T> >;

using path_t = std::vector<size_t>;

//we specify is the cost function of a path
template<typename T>
T cost_function(path_t& path, matrix<T> graph){
    if(path.size() < 2){
        return (T)0;
    }
    size_t v1 = path.back();
    size_t v2 = path[0];
    T cost = graph[v1][v2];
    for(size_t i = path.size() - 1; i != 0;){
        v2 = path[--i];
        cost += graph[v2][v1];
        v1 = v2;
    }
    return cost;
}

//In this program we will handle permutations.
//We will use this format (we name it PERMUTATION_REPRESENTATION_FORMAT#1) to represent, given a natural number n, a permutation P on the set {0, ..., n - 1}:
// we will encode P with std::vector<size_t> V, in the way such that, for every i in {0, ..., what.size() - 1}, it holds P(i) = V[i].

//We will also handle permutations with many fixed elements.
//We will use this format (we tag it PERMUTATION_REPRESENTATION_FORMAT#2) to represent them:
// the representation will make use of (std::vector<size_t> what, std::vector<size_t> goes_where)
//They are assumed to be vectors of the same length, what is assumed to be increasing, and goes_where is assumed to satisfy PERMUTATION_REPRESENTATION_FORMAT#1.
//The permutation being represented is the only P such that, for every i in {0, ..., what.size() - 1}, it holds P(what[i]) = what[goes_where[i]],
// and such that, for every other j which is not in what, it holds P(j) = j.

void path_after_permutation(path_t& path, path_t& path_after_permutation, std::vector<size_t>& what, std::vector<size_t>& goes_where){
    //We assume (what, goes where) satisfy PERMUTATION_REPRESENTATION_FORMAT#2
    //The number of elements moved is assumed small with respect to the length of path.
    path_after_permutation = path;
    for(int i = what.size(); i-- != 0; ){
        path_after_permutation[what[i]] = path[what[goes_where[i]]];
    }
}

void invert_full_permutation(std::vector<size_t>& V, std::vector<size_t>& inverse_of_V){
    //given V satisfying PERMUTATION_REPRESENTATION_FORMAT#1, we store into inverse_of_V, again using PERMUTATION_REPRESENTATION_FORMAT#1, the permutation V^(-1)
    inverse_of_V = std::vector<size_t>(V.size(), 0);
    for(size_t i = V.size(); i-- != 0;){
        inverse_of_V[V[i]] = i;
    }
}

void invert_permutation(std::vector<size_t>& what, std::vector<size_t>& goes_where, std::vector<size_t>& inverse_goes_where){
    //given (what, goes_where) satisfying PERMUTATION_REPRESENTATION_FORMAT#2, we build inverse_goes_where in a way such that
    // (what, inverse_goes_where) represents the permutation (what, goes_where)^(-1).
    //luckily for us, the encoding format makes it easy and we only have to invert goes_where.
    invert_full_permutation(goes_where, inverse_goes_where);
    //in the end we did not use what, and this function is just an alias for invert_full_permutation.
}

void path_after_swap(path_t& path, path_t& path_after_swap, size_t a, size_t b){
    path_after_swap = path;
    path_after_swap[a] = path[b];
    path_after_swap[b] = path[a];
}

template<typename T>
T cost_difference_after_permutation(path_t& path, matrix<T>& graph, std::vector<size_t>& what, std::vector<size_t>& goes_where){
    // I AM NOT REALLY SURE ABOUT THE CORRECTNESS OF THIS FUNCTION!
    // TO BE TESTED ACCURATELY

    //(what, goes_where) is assumed to satisfy PERMUTATION_REPRESENTATION_FORMAT#2.
    //The function outputs cost_function(path o (what, goes_where)) - cost_function(path).
    //in case the permutation leaves a lot of fixed vertices, this is faster than computing cost_function(path_after_permutation(path, what, goes_where)).

    std::vector<size_t> inverse_goes_where ();
    invert_permutation(what, goes_where, inverse_goes_where);
    for(size_t i = what.size(); i-- != 0;){
        inverse_goes_where[i] = path[what[inverse_goes_where[i]]];
    }

    //we will keep two different counts to determine the difference. posdiff will represent all the contributions given by the new path in spots in which it
    // differs from the given one. negdiff will represent the contributions given by the given path in the same spots. posdiff - negdiff will be the output of this function.
    T posdiff = (T)0;
    T negdiff = (T)0;

    //we take care of the first and last iteration of the loop which follows these conditionals
    // these iteration have to take into account that the indices of path and what loop mod path.size() and what.size() respectively.
    // in order to better understand what is going on in the following conditional, it is better to first read the for loop after it.
    if(what[0] == 0 && what.back() == path.size() - 1){
        posdiff += graph[inverse_goes_where.back()][inverse_goes_where[0]];
        negdiff += graph[path.back()][path[0]];
    } else {
        if(what[0] == 0){
            posdiff += graph[path.back()][inverse_goes_where[0]];
            negdiff += graph[path.back()][path[0]];
        } else{
            posdiff += graph[path[what[0] - 1]][inverse_goes_where[0]];
            negdiff += graph[path[what[0] - 1]][path[what[0]]];
        }
        if(what.back() == path.size() - 1){
            posdiff += graph[inverse_goes_where.back()][path[0]];
            negdiff += graph[path.back()][path[0]];
        } else{
            posdiff += graph[inverse_goes_where.back()][path[what.back() + 1]];
            negdiff += graph[path[what.back()]][path[what.back() + 1]];
        }
    }

    for(size_t i = what.size() - 1; i != 0; i--){
        if(what[i - 1] == what[i] - 1){
            posdiff += graph[inverse_goes_where[i - 1]][inverse_goes_where[i]];  
        } else{
            posdiff += graph[path[what[i] - 1]][inverse_goes_where[i]];

            posdiff += graph[inverse_goes_where[i - 1]][path[what[i - 1] + 1]];
            negdiff += graph[path[what[i - 1]]][path[what[i - 1] + 1]];
        }
        negdiff += graph[path[what[i] - 1]][path[what[i]]];
    }

    return posdiff - negdiff;
}

int main(){
    return 0;
}
