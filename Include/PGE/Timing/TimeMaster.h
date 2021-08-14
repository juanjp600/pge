#ifndef PGE_TIMEMASTER_H_INCLUDED
#define PGE_TIMEMASTER_H_INCLUDED

#include "Time.h"
#include "Timing.h"

namespace PGE {

class TimeMaster {
	friend Timing;

	public:
		String print() const;

	private:
		Time top;
		Time* current = &top;
};

}

#endif // PGE_TIMEMASTER_H_INCLUDED
