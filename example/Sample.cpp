#include "Sample.h"

#include "Macros.h"

Sample::Sample(const std::string& str) :
		IPCTarget("Sample"), _string(str) {
}

Sample::~Sample() {
}

int Sample::sum(int a, int b) {
	return a + b;
}

double Sample::sumf(double a, double b) {
	return a + b;
}

const std::string& Sample::getString() const {
	return _string;
}

void Sample::setString(const std::string& str) {
	_string = str;
}

double Sample::longOperation() {
	double total = 0;
	srand(42);
	for (int i = 0; i < 50000000; i++) {
		double r1 = ((double) rand()) / ((double) (RAND_MAX));
		double r2 = ((double) rand()) / ((double) (RAND_MAX));
		total += r1 + r2;
	}
	return total;
}

// Match class name
// Match method and signature
// execute method with given arguments
slnp::ResultCode Sample::invoker(const std::string& name, const std::vector<slnp::Value>& args, slnp::Value* result) {
	SLNP_GET_INTEGER(sum, "ii", sum(SLNP_INTEGER_VALUE(0), SLNP_INTEGER_VALUE(1)));
	SLNP_GET_FLOAT(sumf, "ff", sumf(SLNP_FLOAT_VALUE(0), SLNP_FLOAT_VALUE(1)));
	SLNP_GET_STRING(getString, "", getString().c_str());
	SLNP_INVOKE_FN(setString, "s", setString(SLNP_STRING_VALUE(0)));
	SLNP_GET_FLOAT(heavy, "", longOperation());
	return slnp::ResultMethodNotFound;
}

