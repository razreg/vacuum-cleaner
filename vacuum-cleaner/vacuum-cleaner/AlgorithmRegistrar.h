#ifndef __ALGORITHM_REGISTRAR__H_
#define __ALGORITHM_REGISTRAR__H_

#include <list>
#include <functional>
#include <memory>
#include <cassert> // TODO remove
#include <dlfcn.h>

#include "AbstractAlgorithm.h"
#include "Common.h"

class AlgorithmRegistrar {

	static Logger logger;

	list<void*> dlibs;
	list<string> algorithmNames;
	list<function<unique_ptr<AbstractAlgorithm>()>> algorithmFactories;
	
	void registerAlgorithm(function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
		instance.algorithmFactories.push_back(algorithmFactory);
	};
	
	void setNameForLastAlgorithm(const string& algorithmName) {
		assert(algorithmFactories.size() - 1 == algorithmNames.size()); // TODO handle differently
		algorithmNames.push_back(algorithmName);
	};

	static AlgorithmRegistrar instance;

public: // TODO make real singleton

	friend class AlgorithmRegistration;
	
	enum { ALGORITHM_REGISTERED_SUCCESSFULY = 0, FILE_CANNOT_BE_LOADED = -1, NO_ALGORITHM_REGISTERED = -2 };
	
	~AlgorithmRegistrar();

	int loadAlgorithm(const string& path, const string& so_file_name_without_so_suffix);

	list<unique_ptr<AbstractAlgorithm>> getAlgorithms() const {
		list<unique_ptr<AbstractAlgorithm>> algorithms;
		for (auto algorithmFactoryFunc : algorithmFactories) {
			algorithms.push_back(algorithmFactoryFunc());
		}
		return algorithms;
	}

	const list<string>& getAlgorithmNames() const {
		return algorithmNames;
	}

	size_t size() const {
		return algorithmFactories.size();
	}

	static AlgorithmRegistrar& getInstance() {
		return instance;
	}
};

#endif // __ALGORITHM_REGISTRAR__H_
