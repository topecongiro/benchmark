#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <atomic>
#include <chrono>

int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("Usage: %s loop\n", argv[0]);
    return 0;
  }

  size_t loop = atoi(argv[1]);

  uint64_t x = 0;
  std::atomic<uint64_t> y;
  y = 0;
  uint64_t z = 3;

  double atomic_exchange_time, normal_time;

  {
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < loop; i++) {
      std::swap(x, z);
    }
    auto end = std::chrono::high_resolution_clock::now();
    normal_time = std::chrono::duration<double>(end - start).count();
  }
  {
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < loop; i++) {
      std::atomic_exchange(&y, x);
    }
    auto end = std::chrono::high_resolution_clock::now();
    atomic_exchange_time = std::chrono::duration<double>(end - start).count();
  }

  printf("Elapsed time for normal swap: %f\n", normal_time);
  printf("Elapsed time for atomic_exchange: %f\n", atomic_exchange_time);
  return 0;
}
