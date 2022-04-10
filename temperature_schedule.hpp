#include <math>

class log_like_temperature_schedule {
	int current_iteration;
	double current_temperature;
	double multiplicative_constant;
	double cooling_speed;

	int max_iterations;
	double final_temperature;
	bool using_max_iterations;
	bool using_final_temperature;

	void common_initialization_procedure(double multiplicative_constant, double cooling_speed) {
		this->multiplicative_constant = multiplicative_constant;
		this->cooling_speed = cooling_speed;
		current_iteration = 0;
		update_temperature();
	}

	log_like_temperature_schedule(double multiplicative_constant, double cooling_speed, int max_iterations) {
		common_initialization_procedure(multiplicative_constant, cooling_speed);

		this->max_iterations = max_iterations;
		using_max_iterations = true;
		using_final_temperature = false;
	}

	log_like_temperature_schedule(double multiplicative_constant, double cooling_speed, double final_temperature) {
		common_initialization_procedure(multiplicative_constant, cooling_speed);

		this->final_temperature = final_temperature;
		using_max_iterations = false;
		using_final_temperature = true;
	}

	log_like_temperature_schedule(double multiplicative_constant, double cooling_speed,int max_iterations, double final_temperature) {
		common_initialization_procedure(multiplicative_constant, cooling_speed);

		this->max_iterations = max_iterations;
		this->final_temperature = final_temperature;
		using_max_iterations = true;
		using_final_temperature = true;
	}

public:

	bool schedule_completed() {
		return (using_max_iterations && current_iteration > max_iterations) || (using_final_temperature && current_temperature < final_temperature);
	}

	void update_temperature() {
		current_temperature = multiplicative_constant * std::log(current_iteration / cooling_speed);
		current_iteration++;
	}

	double temperature() {
		return current_temperature;
	}
};