#include "Logger.h"
#include "Types.h"

#include <iostream>
#include <cstdarg>
#include <cstdio>

namespace slnp {

const char* severities[] = { "E", "W", "I", "D", "V" };

void Log(const char* file, int line, const char* fn, LogLevel level, const char* fmt, ...) {
	if (level > LOG_VERBOSE) {
		printf("E|%s:%d|%s()| Invalid log priority level: %d\n", file, line, fn, level);
		return;
	}
	if (level > LOG_INFO) // cut off level
		return;

	printf("%s|%s:%d|%s()| ", severities[level], file, line, fn);

	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);

	putc('\n', stdout);
	fflush(stdout);
}

} /* namespace slnp */
