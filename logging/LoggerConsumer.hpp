#pragma once
#include <mutex>
#include <memory>
#include <string>
#include <thread>
#include <vector>

struct LogMessage
{
	std::string level;
	std::string context;   // full TLS context
	std::string msg;
	std::string file;
	int line;
	std::thread::id threadId;
};

// --- Sink interface ---
class LogSink
{
public:
	virtual ~LogSink() = default;
	virtual void log(const LogMessage& msg) = 0;
};

// --- Log dispatcher ---
class LogDispatcher
{
public:
	void addSink(std::shared_ptr<LogSink> sink)
	{
		auto lock = std::lock_guard<std::mutex>(mutex);
		sinks.push_back(sink);
	}

	void dispatch(const LogMessage& msg)
	{
		auto lock = std::lock_guard<std::mutex>(mutex);
		for (const auto& sink : sinks)
			sink->log(msg);
	}

private:
	std::vector<std::shared_ptr<LogSink>> sinks;
	std::mutex mutex;
};

// Global dispatcher singleton
inline LogDispatcher& getDispatcher()
{
	static LogDispatcher* dispatcher = new LogDispatcher();
	return *dispatcher;
}
