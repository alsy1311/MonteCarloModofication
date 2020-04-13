#include<iostream>
#include<thread>
#include<vector>
#include<utility>
#include <ctime>
#include <algorithm>
#include <numeric>
#include <functional>
#include <random>
#include <mutex>
std::mutex mutex; // создаем мютекс 
void f(size_t N, double& result) {
	std::lock_guard < std::mutex > lock(mutex);
	result = 0;
	unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
	static std::default_random_engine e(seed);
	std::uniform_int_distribution<int> d(0, 100);
	for (int i = 0; i < N; i++) {
		auto x = d(e);

		auto y = d(e);
		if ((x - 50) * (x - 50) + (y - 50) * (y - 50) <= 2500) {
			++result;
		}
	}
	result /= N;
}
void consistently(size_t N) {
	double result;
	f(N, result);
	std::cout << 4 * result << std::endl;
}
void parallel(size_t N) {
	const std::size_t length = N;
	const std::size_t hardware_threads = std::thread::hardware_concurrency();
	const std::size_t num_threads = hardware_threads != 0 ? hardware_threads : 2;
	std::size_t block_size = length / num_threads;
	std::vector < double > results(num_threads);
	std::vector < std::thread > threads;
	for (std::size_t i = 0; i < num_threads - 1; ++i) {
		threads.push_back(std::thread(f, block_size, std::ref(results[i])));
	}
	double result;
	f(N, std::ref(result));
	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
	std::cout << 4 * std::accumulate(results.begin(), results.end(), result) / num_threads << std::endl;
}
int main() {
	//std::thread thread(consistently,100);
	//thread.join();
	parallel(1000);
	return 0;
}