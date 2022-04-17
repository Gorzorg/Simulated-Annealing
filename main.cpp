#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>

#include "permutation_helper.hpp"
#include "temperature_schedule.hpp"
#include "random_graph_generation.hpp"

using namespace std;

//vector pretty print
template<typename T>
ostream& operator<<(ostream& os, vector<T> V) {
    os << '[';
    size_t c = V.size();
    if (c) {
        c--;
        for (size_t i = 0; c--; i++) {
            os << V[i] << ", ";
        }
    }
    os << V.back() << ']';
    return os;
}


template<typename num_type, typename state_type, typename T1, typename T2, typename T3>
void simulated_annealing(T1& state_generator, T2& temperature_schedule, T3& logger) {
    //state_generator is an object which keeps in memory a state_type object, retrievable with the method current_state().
    //state_generator has to have the following:
    // a method current_cost() which outputs a num_type number of the form cost_tunction(current_state());
    // a method generate_state() which generates a new state X, which will be kept in memory in the state_generator.
    // a method update_state() which substitutes the current state with X;
    // a method cost_difference() which outputs cost_function(X) - cost_function(current_state());
    //
    //
    //temperature_schedule is an object which possesses:
    // a method schedule_completed() which outputs a bool, and which will be used to decide
    //   when to stop the annealing process;
    // a method update_temperature(), which advances the temperature schedule to the next state;
    // a method temperature(), which outputs a positive double which will be used to determine the acceptance probability of worse states.
    //   the output of temperature() should stay constant over time unless the method update_temperature() is called.
    //
    //logger is an object which, at every iteration of the simulated annealing process is fed the information about what just happened.
    // It can be a dummy object, or it can for example make computations about it and feed some information into
    // the temperature schedule or the state generator.
    //It must have a method initial_input(state_type& initial_state, num_type initial_cost, double initial_temperature) to be used once
    // to initialize its state, and it must also have 
    //a method input(bool state_changed, state_type& current_state, num_type current_cost, double last_temperature) to feed it information
    // about what happens during the annealing process. What the logger does with that information is completely up to choice.
    num_type cost_difference;
    bool state_changed;
    uniform_real_distribution<> uniform_rand_01 = uniform_real_distribution<>(0, 1);
    std::random_device rd;
    std::mt19937 RNG_for_uniform_rand_01(rd());

    logger.initial_input(state_generator.current_state(), state_generator.current_cost(), temperature_schedule.temperature());

    while (! temperature_schedule.schedule_completed()) {
        state_generator.generate_state();
        cost_difference = state_generator.cost_difference();
        state_changed = false;
        if (cost_difference <= 0 || - log(uniform_rand_01(RNG_for_uniform_rand_01)) * temperature_schedule.temperature() > cost_difference) {
            // in the if statement, the second condition is written the way it is, and not as
            // uniform_rand_01() < exp(-cost_difference()/temperature_schedule.temperature())
            // because this way no cast between num_type and double is required: the minimum requirement is that
            // num_type must be comparable with double. no need to implement exp(num_type) or to implement the cast from num_type to double.
            state_generator.update_state();
            state_changed = true;
        }
        logger.input(state_changed, state_generator.current_state(), state_generator.current_cost(), temperature_schedule.temperature());
        temperature_schedule.update_temperature();
    }
}

template<typename state_type, typename num_type>
class basic_logger {
public:
    state_type best_state;
    num_type cost_of_best_state;

    void initial_input(state_type& initial_state, num_type initial_cost, double initial_temperature){
        best_state = initial_state;
        cost_of_best_state = initial_cost;
    }

    void input(bool state_changed, state_type& current_state, num_type current_cost, double last_temperature) {
        //cout <<state_changed << "\n" << current_state << "\n" << current_cost << "\n" << last_temperature << "\n" << endl;
        if (state_changed && current_cost < cost_of_best_state) {
            best_state = current_state;

            //cout << best_state << endl;
            cost_of_best_state = current_cost;
        }
    }
};

int main(){
    //should read some graph, or create some graph.
    // this graph should have n vertices.
    size_t n = 100;
    matrix<double> graph = matrix<double>();
    path_t shortest_path = path_t();
    test_graph_with_known_shortest_path(n, graph, shortest_path);

    log_like_temperature_schedule temperature_schedule(0.5, 1, 10000000);
    permutation_generator<mt19937> permutation_generator(n, 3);
    traveling_salesman_state_generator<mt19937, double> state_generator(&graph, permutation_generator);
    basic_logger<path_t, double> logger;

    cout << "schedule length: " << temperature_schedule.schedule_length() << "\n\n";

    cout << "shortest path:\n" << shortest_path << "\ncost of shortest_path: " << cost_function(shortest_path, graph) << "\n\n";

    cout << "initial state:\n" << state_generator.current_state() << "\ncost of initial state: " << state_generator.current_cost() << "\n\n";

    simulated_annealing<double, path_t, decltype(state_generator), decltype(temperature_schedule), decltype(logger)>(state_generator, temperature_schedule, logger);

    cout << "best state:\n" << logger.best_state << "\ncost of best state: " << logger.cost_of_best_state << "\n\n";

    cout << "final state:\n" << state_generator.current_state() << "\ncost of final state: " << state_generator.current_cost() << "\n\n";

    cout << "subpaths which occur in both the best state and in the shortest path, in the same order:\n";
    path_t inverse_of_best_state{};
    invert_full_permutation(logger.best_state, inverse_of_best_state);
    std::vector<bool> visited(n, false);
    for(size_t i = 0; i < n; i++){
        if(visited[i]) continue;

        visited[i] = true;
        size_t j = inverse_of_best_state[shortest_path[i]];

        size_t continues_forward = 0;
        while(logger.best_state[(j + continues_forward + 1) % n] == shortest_path[(i + continues_forward + 1) % n]){
            continues_forward++;
            visited[(i + continues_forward) % n] = true;
        }
        size_t continues_backward = 0;
        while(logger.best_state[(j + n - continues_backward - 1) % n] == shortest_path[(i + n - continues_backward - 1) % n]){
            continues_backward++;
            visited[(i + n - continues_backward) % n] = true;
        }
        if(continues_forward || continues_backward){
            cout << "[";
            for(size_t k = continues_backward; k; k--){
                cout << shortest_path[(i + n - k) % n] << ", ";
            }
            cout << shortest_path[i];
            for(size_t k = 1; k <= continues_forward; k++){
                cout << ", " << shortest_path[(i + k) % n];
            }
            cout << "]\n";
        }
    }
}
