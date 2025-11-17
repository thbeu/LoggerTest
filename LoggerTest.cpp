#include "Logger.hpp"
#include <thread>

void workerTask(int workerId)
{
	LogScope workerScope("Worker " + std::to_string(workerId));
	LOG_INFO("Starting worker");

	{
		LogScope processing("Processing Step");
		LOG_INFO("Loading data...");
		LOG_INFO("Working...");
	}

	LOG_INFO("Worker done");
}

int main()
{
	LogScope mainScope("Main");
	LOG_INFO("Application started");

	{
		LogScope subScope("Sub");
		LOG_INFO("Setting up workers...");

		std::thread t1(workerTask, 1);
		std::thread t2(workerTask, 2);

		t1.join();
		t2.join();
	}

	LOG_INFO("Application exiting");
}
