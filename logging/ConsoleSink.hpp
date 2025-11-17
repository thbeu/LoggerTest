#pragma once
#include "LoggerConsumer.hpp"
#include <iostream>
#include <mutex>

class ConsoleSink : public LogSink
{
public:
	void log(const LogMessage& msg) override
	{
		auto lock = std::lock_guard<std::mutex>(consoleMutex);
		std::cout
			<< "[" << msg.level << "] "
			<< "[TID " << msg.threadId << "] "
			<< (msg.context.empty() ? "" : "[" + msg.context + "] ")
			<< msg.msg
			<< " (" << msg.file << ":" << msg.line << ")"
			<< std::endl;
	}

private:
	std::mutex consoleMutex;
};
