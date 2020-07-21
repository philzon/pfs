#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <ncurses.h>

#ifndef BUILD_TITLE
#define BUILD_TITLE "app"
#endif

#ifndef BUILD_VERSION
#define BUILD_VERSION "0.0.0"
#endif

class Application
{
public:
	Application(const std::vector<std::string> &options = std::vector<std::string>());
	virtual ~Application();

	int run();

private:
	void parse(const std::vector<std::string> &options);
	void usage();
	void version();
	void input(int ch);
	void draw(unsigned int left, unsigned int top, unsigned int width, unsigned int height);

	// Application state - running or not.
	bool mRunning;

	// Special options: usage and version information.
	bool mHelp;
	bool mVersion;

	// Current working directory. Will change during navigation.
	std::filesystem::directory_entry mCWD;

	// Maximum allowed parent to step back into.
	std::filesystem::path mMax;

	// Selected entry's path when exiting the application.
	std::filesystem::path mSelected;

	// List of entries retrieved.
	std::vector<std::filesystem::directory_entry> mEntries;

	// When entering a directory, store previous index so that when
	// returning back to parent directory, index will be restored.
	std::vector<unsigned int> mIndicies;

	// Current selected index/entry.
	unsigned int mIndex;

	// Persist vertical scroll when drawing.
	unsigned int mScrollV;
};

#endif // APPLICATION_HPP