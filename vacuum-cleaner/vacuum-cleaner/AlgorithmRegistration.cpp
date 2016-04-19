#include "AlgorithmRegistration.h"

// TODO why have a cpp?
AlgorithmRegistration::AlgorithmRegistration(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
	cout << "inside AlgorithmRegistration ctor" << endl;
	AlgorithmRegistrar::getInstance().registerAlgorithm(algorithmFactory);
}