#include <cassert>
#include <cstdio>

#include <chrono>
#include <iostream>
#include <thread>

void Usage(char *argv[]) {
  printf("Usage: %s loop\n", argv[0]);
}

struct alignas (64) Counter {
  int64_t body[2] = {0,0};
  double time[2];
};
static_assert(sizeof(Counter) >= 64, "sizeof(Counter) >= 64");

void Run(Counter* counter, const int id, const int loop, const bool with_false_sharing) {
  if (with_false_sharing) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < loop; i++) {
      ++counter->body[id];
    }
    auto end = std::chrono::high_resolution_clock::now();
    counter->time[id] = std::chrono::duration<double>(end - start).count();
    assert(counter->body[id] == loop);
  } else {
    int local_counter = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < loop; i++) {
      ++local_counter;
    }
    auto end = std::chrono::high_resolution_clock::now();
    counter->time[id] = std::chrono::duration<double>(end - start).count();
    assert(local_counter == loop);
  }
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    Usage(argv);
    return 0;
  }

  int loop = atoi(argv[1]);
  if (loop <= 0) {
    Usage(argv);
    return 0;
  }

  Counter c1, c2;
  std::thread t1(Run, &c1, 0, loop, false);
  std::thread t2(Run, &c1, 1, loop, false);
  t1.join();
  t2.join();
  printf("Thraed 1 without false sharing: %f\nThread 2 without false sharing: %f\n",
         c1.time[0], c1.time[1]);

  std::thread t3(Run, &c2, 0, loop, true);
  std::thread t4(Run, &c2, 1, loop, true);
  t3.join();
  t4.join();
  printf("Thread 1 with false sharing: %f\nThread 2 with false sharing: %f\n",
         c2.time[0], c2.time[1]);

  return 0;
}
