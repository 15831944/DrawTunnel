#pragma once

#ifndef SPIN_MUTEX
#define SPIN_MUTEX

#include <atomic>
//ͨ��std::atomicʵ�ֵ�������
class spin_mutex {
	std::atomic<bool> flag = ATOMIC_VAR_INIT(false);
public:
	spin_mutex() = default;
	spin_mutex(const spin_mutex&) = delete;
	spin_mutex& operator= (const spin_mutex&) = delete;
	void lock() {
		bool expected = false;
		while (!flag.compare_exchange_strong(expected, true))
			expected = false;
	}
	void unlock() {
		flag.store(false);
	}
};

#endif // !SPIN_MUTEX