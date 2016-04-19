#ifndef __ALGORITHM_REGISTRATION__H_
#define __ALGORITHM_REGISTRATION__H_

#include <functional>
#include <memory>

#include "AbstractAlgorithm.h"
#include "AlgorithmRegistrar.h"

// because we currently need to use c++11 (c++14 isn't supported on nova with current g++ version)
template<typename T, typename... Args>
std::unique_ptr<T> _make_unique(Args&&... args) {
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class AlgorithmRegistration {
public:
	AlgorithmRegistration(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
		AlgorithmRegistrar::getInstance().registerAlgorithm(algorithmFactory);
	};
};

#define REGISTER_ALGORITHM(class_name) AlgorithmRegistration register_me_##class_name([]{return _make_unique<class_name>();});

#endif // __ALGORITHM_REGISTRATION__H_