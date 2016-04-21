#include "AlgorithmRegistration.h"

AlgorithmRegistration::AlgorithmRegistration(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
	AlgorithmRegistrar::getInstance().registerAlgorithm(algorithmFactory);
}