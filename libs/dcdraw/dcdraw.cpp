#include "dcdraw.h"

#include <string>

namespace DCDraw
{
	static std::string path_prefix;

	void SetPathPrefix(const std::string &prefix)
	{
		path_prefix = prefix;
	}
	
	const std::string &GetPathPrefix()
	{
		return path_prefix;
	}
}

