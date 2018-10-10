#ifndef SAMPLE_H_
#define SAMPLE_H_

#include "IPCTarget.h"

#include <string>

/**
 * A sample class to showcase method registration and invocation
 */
class Sample: public slnp::IPCTarget {
public:
	Sample(const std::string& str);
	virtual ~Sample();

	int sum(int a, int b);
	double sumf(double a, double b);

	const std::string& getString() const;
	void setString(const std::string& str);

	double longOperation();

	/// Reimplemented
	slnp::ResultCode invoker(const std::string& name, const std::vector<slnp::Value>& args, slnp::Value* result);

private:
	std::string _string;
};

#endif /* SAMPLE_H_ */
