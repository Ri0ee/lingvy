#pragma once

#include <chrono>

class Time
{
    public:
        Time() {
			time_starting_point = std::chrono::steady_clock::now();
		}
		
        ~Time() {}

        double GetTime() {
			std::chrono::duration<double> duration = std::chrono::steady_clock::now() - time_starting_point;
			return duration.count();
		}

        void StartTimeMeasure() {
			time_measure_point_1 = std::chrono::steady_clock::now();
		}
		
        void EndTimeMeasure() {
			time_measure_point_2 = std::chrono::steady_clock::now();
		}
		
        double GetTimeMeasure() {
			std::chrono::duration<double> duration = time_measure_point_2 - time_measure_point_1;
			return duration.count();
		}

    private:
        std::chrono::steady_clock::time_point time_measure_point_1;
        std::chrono::steady_clock::time_point time_measure_point_2;
        std::chrono::steady_clock::time_point time_starting_point;
};