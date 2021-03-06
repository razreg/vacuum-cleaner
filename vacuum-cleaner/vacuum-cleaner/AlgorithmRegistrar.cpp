#include "AlgorithmRegistrar.h"

using namespace std;

Logger AlgorithmRegistrar::logger = Logger("Registrar");
AlgorithmRegistrar AlgorithmRegistrar::instance;

// filename is the name of the file without the extension (only stem)
int AlgorithmRegistrar::loadAlgorithm(const string& path, const string& filename) {
	size_t size = this->size();
	void* dlib = dlopen(path.c_str(), RTLD_NOW);
	if (dlib == NULL) {
		if (logger.debugEnabled()) logger.debug("Failed to load file. Details: " + string(dlerror()));
		return FILE_CANNOT_BE_LOADED;
	}
	dlibs.emplace_back(dlib);
	if (this->size() == size) {
		if (logger.debugEnabled()) logger.debug("Failed to register algorithm");
		return NO_ALGORITHM_REGISTERED; // no algorithm registered
	}
	setNameForLastAlgorithm(filename);
	return ALGORITHM_REGISTERED_SUCCESSFULY;
}

list<unique_ptr<AbstractAlgorithm>> AlgorithmRegistrar::getAlgorithms() const {
	list<unique_ptr<AbstractAlgorithm>> algorithms;
	for (auto algorithmFactoryFunc : algorithmFactories) {
		algorithms.push_back(algorithmFactoryFunc());
	}
	return algorithms;
}

void AlgorithmRegistrar::setNameForLastAlgorithm(const string& algorithmName) {
	if (algorithmFactories.size() - 1 != algorithmNames.size()) {
		throw runtime_error("Corrupted algorithm list - the amounts of algorithm factories and algorithm names don't match");
	}
	algorithmNames.push_back(algorithmName);
}