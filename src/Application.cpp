#include "Application.hpp"

Application::Application(const std::vector<std::string> &options)
{
	// If set to false later is assumed a failure.
	mRunning = true;

	mCWD = std::filesystem::directory_entry(std::filesystem::current_path());
	mMax = std::filesystem::current_path().root_path();
	mIndex = 0;
	mScrollV = 0;

	parse(options);

	// Special flags that closes the application earlier.
	if (mHelp || mVersion)
		return;

	// Initialize ncurses to stderr. Setting it to stdout causes the
	// output to be redirected if used in Bash or other scripts.
	newterm(NULL, stderr, stdin);
	keypad(stdscr, true);
	raw();
	noecho();
	curs_set(0);

	// Colors are forced for interactivity.
	if (!has_colors())
	{
		std::cout << "Your terminal does not support colors\n";
		mRunning = false;

		return;
	}

	// Initialize needed colors.
	start_color();
	use_default_colors();
	init_color(COLOR_WHITE, 1000, 1000, 1000); // Overrides default terminal color setting.
	init_pair(1, COLOR_BLACK, COLOR_WHITE); // Selected entry.
	init_pair(2, COLOR_BLACK, COLOR_BLUE); // Selected entry (directory).
	init_pair(3, COLOR_BLUE, -1); // Non-selected entry (directory).
}

Application::~Application()
{
	// End ncurses mode.
	endwin();

	if (mHelp)
		usage();

	if (mVersion)
		version();

	// Print selected entry on exit.
	if (!mSelected.empty())
		std::cout << mSelected.string() << "\n";
}

int Application::run()
{
	// Not running? Then it's a failure.
	if (!mRunning)
		return 1;

	// Exit completely when special flags are set.
	if (mHelp || mVersion)
		return 0;

	mEntries.clear();

	for (const std::filesystem::directory_entry &entry: std::filesystem::directory_iterator(mCWD))
		mEntries.push_back(entry);

	while (mRunning)
	{
		// Clear everything on screen.
		erase();

		draw(0, 0, COLS, LINES);

		// Update changes to screen.
		refresh();

		input(getch());
	}

	return 0;
}

void Application::parse(const std::vector<std::string> &options)
{
	for (unsigned int i = 1; i < options.size(); ++i)
	{
		std::string arg1 = options.at(i);
		std::string arg2 = "";

		if (i + 1 < options.size())
			arg2 = options.at(i + 1);

		if (arg1 == "-h")
		{
			mHelp = true;
			mVersion = false;
			return;
		}
		else if (arg1 == "-v")
		{
			mVersion = true;
		}
		else if (arg1 == "-m")
		{
			mMax = std::filesystem::absolute(std::filesystem::canonical(arg2));
			++i;
		}
		else
		{
			std::filesystem::path path = arg1;

			if (!std::filesystem::exists(path))
			{
				std::cout << path << " does not exist\n";
				mRunning = false;
				return;
			}

			if (!std::filesystem::is_directory(path))
			{
				std::cout << path << " is not a directory\n";
				mRunning = false;
				return;
			}

			mCWD = std::filesystem::directory_entry(std::filesystem::absolute(std::filesystem::canonical(path)));
		}
	}
}

void Application::usage()
{
	std::cout << "Usage: " << BUILD_TITLE << " [OPTIONS] [PATH]\n";
	std::cout << "\n";
	std::cout << "Interactive filesystem browser that prints the entry's absolute path\n";
	std::cout << "\n";
	std::cout << "Options:\n";
	std::cout << "  -h   Print this and quit\n";
	std::cout << "  -m   Max parent height\n";
	std::cout << "  -v   Print version and quit\n";
	std::cout << "\n";
	std::cout << "Navigation:\n";
	std::cout << "  Return   Print entry path and quit\n";
	std::cout << "  Up       Select above entry in list\n";
	std::cout << "  Down     Select below entry in list\n";
	std::cout << "  Left     Step into directory\n";
	std::cout << "  Right    Step back to parent directory\n";
	std::cout << "  Q        Quit without printing anything\n";
	std::cout << "  R        Refresh entry list\n";
	std::cout << "\n";
	std::cout << "Will default to current working directory if no path is provided\n";
}

void Application::version()
{
	std::cout << BUILD_VERSION << "\n";
}

void Application::input(int ch)
{
	// Handle user input.
	switch (ch)
	{
	// Quit application.
	case 'q':
		mRunning = false;
		break;
	// RETURN: print selected entry (if empty, selects current working directory).
	case '\n':
	case '\r':
		mSelected = mEntries.empty() ? mCWD : mEntries.at(mIndex).path();
		mRunning = false;
		break;
	// Go up in list.
	case 'k':
	case KEY_UP:
		if (mIndex == 0)
			mIndex = mEntries.size() - 1;
		else
			--mIndex;
		break;
	// Go down in list.
	case 'j':
	case KEY_DOWN:
		if (mIndex < mEntries.size() - 1)
			++mIndex;
		else
			mIndex = 0;
		break;
	// Return to parent entry.
	case 'h':
	case KEY_LEFT:
		if (!mIndicies.empty())
		{
			mIndex = mIndicies.back();
			mIndicies.pop_back();
		}
		else
		{
			// Don't reset index if we cannot go further back.
			if (mCWD.path() == mMax)
				mIndex = 0;
		}

		// Only step back into parent directory if we can go further back.
		if (mCWD.path() != mMax)
		{
			mCWD = std::filesystem::directory_entry(mCWD.path().parent_path());

			mEntries.clear();

			for (const std::filesystem::directory_entry &entry: std::filesystem::directory_iterator(mCWD))
				mEntries.push_back(entry);
		}
		break;
	// Enter into entry if it's a directory.
	case 'l':
	case KEY_RIGHT:
		if (!mEntries.empty() && mEntries.at(mIndex).is_directory())
		{
			mIndicies.push_back(mIndex);
			mCWD = mEntries.at(mIndex);
			mIndex = 0;

			mEntries.clear();

			for (const std::filesystem::directory_entry &entry: std::filesystem::directory_iterator(mCWD))
				mEntries.push_back(entry);
		}
		break;
	case 'r':
	case KEY_RESIZE:
		break;
	}
}

void Application::draw(unsigned int left, unsigned int top, unsigned int width, unsigned int height)
{
	// Print current working directory.
	attron(COLOR_PAIR(1));

	mvhline(top, left, ' ', COLS);
	mvprintw(top, left, "%s", mCWD.path().c_str());

	attroff(COLOR_PAIR(1));

	if (mEntries.empty())
	{
		mvprintw(top + 1, left, "(%s)", "empty");
		return;
	}

	height -= 1;

	if (mIndex < mScrollV)
		mScrollV = mIndex;

	if (mIndex > (mScrollV + height) - 1)
		mScrollV = (mIndex - height) + 1;

	for (unsigned int y = 0; y < height; ++y)
	{
		if (mScrollV + y + 0 > mEntries.size() - 1)
			break;

		const std::filesystem::directory_entry entry = mEntries.at(mScrollV + y);

		int color = 0;

		if (mScrollV + y == mIndex)
			color = 1;
		if (mScrollV + y == mIndex && entry.is_directory())
			color = 2;
		else if (mScrollV + y != mIndex && entry.is_directory())
			color = 3;

		attron(COLOR_PAIR(color));

		if (entry.is_directory())
			mvprintw(top + y + 1, left, "/%s", entry.path().filename().c_str());
		else
			mvprintw(top + y + 1, left, "%s", entry.path().filename().c_str());

		attroff(COLOR_PAIR(color));
	}
}
