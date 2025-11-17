#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <thread>
#include <mutex>

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

// mutex singleton
static std::mutex coutMutex;  

// Returns a reference to a mutex that is never destroyed.
// Safe to use even during static/global destructors.
inline std::mutex& logCoutMutex()
{
	static std::mutex* mtx = new std::mutex(); // never destroyed
	return *mtx;
}

// --- Core logger (no consumer yet, prints to stdout) ---
inline void logMessage(const char* level,
	const char* file,
	int line,
	const std::string& msg)
{
	auto lock = std::lock_guard<std::mutex>(logCoutMutex());

	const auto context = tls_context.fullContext();
	const auto threadId = std::this_thread::get_id();

	std::cout
		<< "[" << level << "] "
		<< "[TID " << threadId << "] "
		<< (context.empty() ? "" : ("[" + context + "] "))
		<< msg
		<< " (" << file << ":" << line << ")\n";
}

// --- Logging macros ---
#define LOG_INFO(msg)  logMessage("INFO",  __FILE__, __LINE__, (msg))
#define LOG_WARN(msg)  logMessage("WARN",  __FILE__, __LINE__, (msg))
#define LOG_ERROR(msg) logMessage("ERROR", __FILE__, __LINE__, (msg))
