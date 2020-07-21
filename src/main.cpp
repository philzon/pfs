#include <string>
#include <vector>

#include "Application.hpp"

int main(int argc, char **argv)
{
	std::vector<std::string> options;

	for (int i = 0; i < argc; ++i)
		options.push_back(std::string(argv[i]));

	return Application(options).run();
}
