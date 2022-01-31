#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <cppsl/threading/jthread.h>

using JThread = cppsl::threading::JThread;

void do_work(unsigned id) {}

void f() {
  {
    std::vector<std::thread> threads;
    for (unsigned i = 0; i < 20; ++i) {
      threads.push_back(std::thread(do_work, i));
    }
    std::for_each(threads.begin(), threads.end(),
                  std::mem_fn(&std::thread::join));
  }

  std::cout << "threads finisched complete" << std::endl;

  // with jthread
  {
    std::vector<JThread> jthreads;
    for (unsigned i = 0; i < 20; ++i) {
      jthreads.push_back(JThread (do_work, i));
    }
  }

  std::cout << "jthreads finisched complete" << std::endl;
}

int main() {
  f();
}

