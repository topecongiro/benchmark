#include <immintrin.h>
#include <cassert>
#include <cstdio>
#include <chrono>
#include <mutex>

template<typename Func>
double bench(Func f) {
  auto start = std::chrono::high_resolution_clock::now();
  f();
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration<double>(end - start).count();
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("Usage: %s loop\n", argv[0]);
    return 0;
  }

  size_t loop = atoi(argv[1]);

  double normal_time = bench([loop]() {
      volatile uint64_t x = 0;
      for (size_t i = 0; i < loop; i++) {
        ++x;
      }
      assert(x == loop);
    });
  double htm_time = bench([loop]() {
      volatile uint64_t x = 0;
      std::mutex mtx;
      for (size_t i = 0; i < loop; i++) {
        if (_xbegin() == _XBEGIN_STARTED) {
          ++x;
          _xend();
        } else {
          mtx.lock();
          ++x;
          mtx.unlock();
        }
      }
      assert(x == loop);
    });

  printf("Elapsed time for normal operation: %f\n", normal_time);
  printf("Elapsed time for hardware transactional memory: %f\n", htm_time);

  return 0;
}

