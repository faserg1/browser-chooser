module;

#include <ranges>
#include <string>

export module Utils;

namespace utils
{
	export template <typename Character, std::ranges::range Rng>
	std::basic_string<Character> bake(Rng rng)
	{
		return std::string(&*rng.begin(), std::ranges::distance(rng));
	}


}
