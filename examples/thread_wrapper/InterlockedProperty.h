/*
	Thread Wrapper v.2.0

	Copyright (C) 2017 by Sergey A Kryukov
	http://www.SAKryukov.org
	http://www.codeproject.com/Members/SAKryukov	

	Original publications:
	https://www.codeproject.com/Articles/1177478/Thread-Wrapper-CPP
	https://www.codeproject.com/Articles/1177869/Conveyor-Thread-Wrapper-CPP
	https://www.codeproject.com/Articles/1170503/The-Impossibly-Fast-Cplusplus-Delegates-Fixed
	https://www.codeproject.com/Tips/149540/Simple-Blocking-Queue-for-Thread-Communication-and
*/

#pragma once
#include <mutex>

template<typename T>
class interlocked_property {
public:

	interlocked_property() : interlocked_property(nullptr, nullptr) { }
	interlocked_property(const T &value) : interlocked_property(nullptr, &value) { }
	interlocked_property(std::mutex& sharedMutex) : interlocked_property(&sharedMutex, nullptr) { }
	interlocked_property(std::mutex& sharedMutex, const T &value) : interlocked_property(&sharedMutex, &value) { }

	interlocked_property& operator=(interlocked_property& other) {
		this->mutex = other.mutex;
		this->value = other.value;
		return *this;
	} //operator=

	void UseSharedMutex(std::mutex& mutex) {
		this->mutex = mutex;
	} //UseSharedMutex

	operator T() const {
		std::lock_guard<std::mutex> lock(*mutex);
		return value;
	} //operator T

	T operator=(const T &value) {
		std::lock_guard<std::mutex> lock(*mutex);
		return this->value = value;
	} //operator=

private:

	interlocked_property(std::mutex * sharedMutex, const T * value) {
		if (sharedMutex == nullptr)
			mutex = &uniqueMutex;
		else
			mutex = sharedMutex;
		if (value != nullptr) // don't use mutex to interlock value here: it could be not yet fully constructed
			this->value = *value;
	} //interlocked_property

	std::mutex uniqueMutex;
	std::mutex * mutex;
	T value;

}; /* class InterockedProperty */
