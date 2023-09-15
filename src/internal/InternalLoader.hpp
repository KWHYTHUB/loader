#pragma once

#include <loader/Log.hpp>
#include <vector>
#include <unordered_map>
#include <utils/Result.hpp>
#include <Loader.hpp>
#include "FileWatcher.hpp"
#include <mutex>

USE_SAPPHIRE_NAMESPACE();

/**
 * Internal extension of Loader for private information
 * @class InternalLoader
 */
class InternalLoader : public Loader {
protected:
	std::vector<LogPtr*> m_logQueue;
	std::vector<std::function<void(void)>> m_gdThreadQueue;
	mutable std::mutex m_gdThreadMutex;
	bool m_platformConsoleReady = false;

	InternalLoader();
	~InternalLoader();
public:
	static InternalLoader* get();

	bool setup();

	bool loadHooks();

	void queueInGDThread(std::function<void SAPPHIRE_CALL(void)> func);
	void executeGDThreadQueue();

	bool platformConsoleReady() const;
	void queueConsoleMessage(LogPtr*);
	void setupPlatformConsole();
	void awaitPlatformConsole();
	void closePlatformConsole();
	static void platformMessageBox(const char* title, const char* info);

	friend int sapphireEntry(void* platformData);
};
