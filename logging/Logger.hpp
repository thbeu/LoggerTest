#pragma once
#include "LoggerConsumer.hpp"
#include <sstream>
#include <string>
#include <thread>
#include <vector>

// --- Thread-local logging context ---
class LoggingContext
{
public:
	void push(const std::string& scope) {
		scopes.push_back(scope);
	}

	void pop() {
		if (!scopes.empty())
			scopes.pop_back();
	}

	std::string fullContext() const {
		std::ostringstream oss;
		for (size_t i = 0; i < scopes.size(); ++i) {
			oss << scopes[i];
			if (i + 1 < scopes.size())
				oss << " > ";
		}
		return oss.str();
	}

private:
	std::vector<std::string> scopes;
};

inline thread_local LoggingContext tls_context;

// --- RAII LogScope ---
class LogScope
{
public:
	explicit LogScope(const std::string& scopeName)
		: name{ scopeName }
	{
		tls_context.push(name);
	}

	~LogScope() {
		tls_context.pop();
	}

private:
	std::string name;
};

inline void logMessage(const char* level,
	const char* file,
	int line,
	const std::string& msg)
{
	LogMessage logMsg;
	logMsg.level = level;
	logMsg.file = file;
	logMsg.line = line;
	logMsg.msg = msg;
	logMsg.context = tls_context.fullContext();
	logMsg.threadId = std::this_thread::get_id();

	getDispatcher().dispatch(logMsg);
}

// --- Logging macros ---
#define LOG_INFO(msg)  logMessage("INFO",  __FILE__, __LINE__, (msg))
#define LOG_WARN(msg)  logMessage("WARN",  __FILE__, __LINE__, (msg))
#define LOG_ERROR(msg) logMessage("ERROR", __FILE__, __LINE__, (msg))
