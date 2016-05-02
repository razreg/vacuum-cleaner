#ifndef __ALGORITHM_REGISTRATION__H_
#define __ALGORITHM_REGISTRATION__H_

#include <functional>
#include <memory>
#include "Direction.h" // TODO is this ok? was not here originally
#include "AbstractAlgorithm.h" // TODO is this ok? was not here originally

class AlgorithmRegistration {
public:
	AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()>);
};

#define REGISTER_ALGORITHM(class_name) AlgorithmRegistration register_me_##class_name([]{return make_unique<class_name>();});

#endif // __ALGORITHM_REGISTRATION__H_
