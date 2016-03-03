#ifndef CITYSIM_LOGGING_SERVICE_HPP
#define CITYSIM_LOGGING_SERVICE_HPP

#include "services.hpp"
#include "constants.hpp"

enum LogLevel {
	LOG_DEBUGGIEST,
	LOG_DEBUGGIER,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,

	LOG_UNKNOWN
};

class LoggingService : public BaseService {
public:
	LoggingService(std::ostream &stream, const LogLevel &level) : stream(stream), level(level) {
	}

	virtual void onEnable() override;

	inline void logDebuggier(const std::string &msg) {
		log(msg, LOG_DEBUGGIER);
	}

	inline void logDebuggiest(const std::string &msg) {
		log(msg, LOG_DEBUGGIEST);
	}

	inline void logDebug(const std::string &msg) {
		log(msg, LOG_DEBUG);
	}

	inline void logInfo(const std::string &msg) {
		log(msg, LOG_INFO);
	}

	inline void logWarning(const std::string &msg) {
		log(msg, LOG_WARNING);
	}

	inline void logError(const std::string &msg) {
		log(msg, LOG_ERROR);
	}

	void pushIndent();

	void popIndent();

	// no change if invalid
	void setLogLevel(const std::string &s);

private:
	LogLevel level;
	std::ostream &stream;
	std::string prefix;

	std::unordered_map<LogLevel, std::string, std::hash<int>> levels;

	virtual void log(const std::string &msg, LogLevel level);

	LogLevel logLevelFromString(const std::string &s);


};

class NullLoggingService : public LoggingService {

public:
	NullLoggingService();

private:
	virtual void log(const std::string &msg, LogLevel level) override {
	}
};
namespace Logger {
	inline void logDebuggiest(const std::string &msg) {
		Locator::locate<LoggingService>()->logDebuggiest(msg);
	}

	inline void logDebuggier(const std::string &msg) {
		Locator::locate<LoggingService>()->logDebuggier(msg);
	}

	inline void logDebug(const std::string &msg) {
		Locator::locate<LoggingService>()->logDebug(msg);
	}

	inline void logInfo(const std::string &msg) {
		Locator::locate<LoggingService>()->logInfo(msg);
	}

	inline void logWarning(const std::string &msg) {
		Locator::locate<LoggingService>()->logWarning(msg);
	}

	inline void logError(const std::string &msg) {
		Locator::locate<LoggingService>()->logError(msg);
	}

	inline void pushIndent() {
		Locator::locate<LoggingService>()->pushIndent();
	}

	inline void popIndent() {
		Locator::locate<LoggingService>()->popIndent();
	}
}
#endif
