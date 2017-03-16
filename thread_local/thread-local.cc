#include <cstdio>
#include <chrono>
#include <thread>

thread_local int x = 0;

int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("Usage: %s loop\n", argv[0]);
    return 0;
  }

  size_t loop = atoi(argv[1]);
  printf("loop: %zd\n", loop);

  double tl_time, normal_time;
  {
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < loop; i++) {
      ++x;
    }
    auto end = std::chrono::high_resolution_clock::now();
    tl_time = std::chrono::duration<double>(end - start).count();
  }

  {
    volatile int y = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < loop; i++) {
      ++y;
    }
    auto end = std::chrono::high_resolution_clock::now();
    normal_time = std::chrono::duration<double>(end - start).count();
  }

  printf("Elapsed time for accessing a thread_local variable: %f\n", tl_time);
  printf("Elapsed time for accessing a local variable: %f\n", normal_time);
  return 0;
}
