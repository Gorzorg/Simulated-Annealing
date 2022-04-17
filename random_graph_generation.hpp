#include <vector>
#include <random>
#include <iostream>

#include "permutation_helper.hpp"

void test_graph_with_known_shortest_path(size_t size, matrix<double>& graph, path_t& shortest_path){

    permutation_generator<std::mt19937> PG((size), size);
    PG.new_permutation();
    shortest_path.clear();
    //since PG.max_number_of_permutated_objects == PG.number_of_objects, PG.current_permutation_of_selected_objects is already a full permutation of {1, ..., size}
    //so we simply do
    shortest_path = PG.current_permutation_of_selected_objects;
    //instead of
    //for(size_t i = 0; i < size; i++){
    //    shortest_path.push_back(PG.currently_selected_objects[PG.current_permutation_of_selected_objects[i]]);
    //}

    std::uniform_real_distribution<> uniform_rand_01 = std::uniform_real_distribution<>(1, 2);
    std::random_device rd;
    std::mt19937 RNG_for_uniform_rand_01(rd());

    graph.clear();
    graph.reserve(size);
    for(size_t i = 0; i < size; i++){
        graph.push_back(std::vector<double>());
        graph[i].reserve(size);
        for(size_t j = 0; j < size; j++){
            graph[i].push_back(uniform_rand_01(RNG_for_uniform_rand_01));
        }
    }

    for(size_t i = 1; i < size; i++){
        graph[shortest_path[i]][shortest_path[i - 1]] = 1;
        graph[shortest_path[i - 1]][shortest_path[i]] = 1;
    }
    graph[shortest_path[0]][shortest_path.back()] = 1;
    graph[shortest_path.back()][shortest_path[0]] = 1;
}