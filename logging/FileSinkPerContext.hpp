#pragma once
#include "LoggerConsumer.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>

class FileSinkPerContext : public LogSink
{
public:
	FileSinkPerContext(const std::filesystem::path& directory)
		: baseDir{ directory }
	{
		if (!std::filesystem::exists(baseDir)) {
			std::filesystem::create_directories(baseDir);
		}
	}

	void log(const LogMessage& msg) override
	{
		const auto topLevelScope = extractTopLevelScope(msg.context);
		auto& ofs = getFileStream(topLevelScope);
		{
			auto lock = std::lock_guard<std::mutex>(fileMutex);
			ofs << formatMessage(msg) << "\n";
			ofs.flush(); // optional: flush immediately
		}
	}

private:
	std::filesystem::path baseDir;
	std::mutex fileMutex;
	std::unordered_map<std::string, std::ofstream> files;

	std::ofstream& getFileStream(const std::string& topScope)
	{
		auto [it, inserted] = files.try_emplace(topScope, baseDir / (topScope + ".log"), std::ios::app);
		return it->second;
	}

	std::string extractTopLevelScope(const std::string& context)
	{
		auto pos = context.find(" > ");
		if (pos == std::string::npos)
			return context.empty() ? "general" : context;
		return context.substr(0, pos);
	}

	std::string formatMessage(const LogMessage& msg)
	{
		std::ostringstream oss;
		oss << "[" << msg.level << "] "
			<< "[TID " << msg.threadId << "] "
			<< (msg.context.empty() ? "" : "[" + msg.context + "] ")
			<< msg.msg
			<< " (" << msg.file << ":" << msg.line << ")";
		return oss.str();
	}
};
