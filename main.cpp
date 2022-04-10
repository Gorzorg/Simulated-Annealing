#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>

#include "permutation_helper.hpp"
#include "temperature_schedule.hpp"

using namespace std;

template<typename num_type, typename state_type, typename T1, typename T2, typename T3>
void simulated_annealing(T1& state_generator, T2& temperature_schedule, T3& logger) {
    //state_generator is an object which keeps in memory a state_type object, retrievable with the method current_state().
    //state_generator has to have the following:
    // a method current_cost() which outputs a num_type number of the form cost_tunction(current_state());
    // a method update_state() which substitutes the current state with another one;
    // a method cost_difference() which, given X the state which would substitute current_state() if update_state() was executed,
    //   outputs cost_function(X) - cost_function(current_state());
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
    //The logger must have a method input(bool state_changed, state_type& current_state, num_type current_cost, double last_temperature)
    // what the logger does with that information is completely up to choice.
    state_type current_state = state_generator.initialize_state();
    state_type next_state;
    num_type cost_difference;
    bool state_changed;
    uniform_real_distribution<> uniform_rand_01 = uniform_real_distribution<>(0, 1);
    while (! temperature_schedule.schedule_completed()) {
        cost_difference = state_generator.cost_difference();
        state_changed = false;
        if (cost_difference <= 0 || - log(uniform_rand_01()) * temperature_schedule.temperature() > cost_difference) {
            // in the if statement, the second condition is written the way it is, and not as
            // uniform_rand_01() < exp(-cost_difference()/temperature_schedule.temperature())
            // because this way no cast between num_type and double is required: the minimum requirement is that
            // num_type must be comparable with double. no need to implement exp(num_type) or to implement the cast from num_type to double.
            state_generator.update_state();
            state_changed = true;
        }
        logger.input(state_changed, state_generator.current_state(), temperature_schedule.temperature());
        temperature_schedule.update_temperature();
    }
}

template<typename state_type, typename num_type>
class basic_logger {
public:
    state_type best_state;
    num_type cost_of_best_state;

    void input(bool state_changed, state_type& current_state, num_type current_cost, double last_temperature) {
        if (state_changed && current_cost < cost_of_best_state) {
            best_state = current_state;
            cost_of_best_state = current_cost;
        }
    }
};

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
    os << ']';
    return os;
}

int main(){
    //should read some graph, or create some graph.
    // this graph should have n vertices.
    log_like_temperature_schedule temperature_schedule(1, 1, 0.001);
    permutation_generator<mt19937> permutation_generator(n, max(2, n / 20));
    traveling_salesman_state_generator<mt19937, double> state_generator(graph, permutation_generator);
    basic_logger<path_t, double> logger;

    simulated_annealing(state_generator, temperature_schedule, logger);

    cout << logger.best_state;
}
