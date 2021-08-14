#ifndef PGE_TIMING_H_INCLUDED
#define PGE_TIMING_H_INCLUDED

#include <chrono>
#include <unordered_map>

#include <PGE/String/Key.h>

namespace PGE {

class Timing {
	public:
		Timing(class TimeMaster& tm, const String& name);
		~Timing();

	private:
		using Clock = std::chrono::steady_clock;

		struct Time** current;
		const String name;
		const std::chrono::time_point<Clock> start;
};

}

#endif // PGE_TIMING_H_INCLUDED
