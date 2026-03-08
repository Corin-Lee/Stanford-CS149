#include <stdio.h>

#include <algorithm>
#include <cstdlib>
#include <thread>

#include "CycleTimer.h"

typedef struct {
  float x0, x1;
  float y0, y1;
  unsigned int width;
  unsigned int height;
  unsigned int numThreads;
  int maxIterations;
  int* output;
  int threadId;
} WorkerArgs;

extern void mandelbrotSerial(float x0, float y0, float x1, float y1, int width,
                             int height, int startRow, int numRows,
                             int maxIterations, int output[]);

//
// workerThreadStart --
//
// Thread entrypoint.
void workerThreadStart(WorkerArgs* const args, unsigned int threadId) {
  // double startTime = CycleTimer::currentSeconds();
  for (unsigned int cur_row = threadId; cur_row < args->height;
       cur_row += args->numThreads) {
    mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width,
                     args->height, cur_row, 1, args->maxIterations,
                     args->output);
  }
  // double endTime = CycleTimer::currentSeconds();
  // double minThread = std::min(1e30, endTime - startTime);
  // printf("[mandelbrot thread %d]:\t\t[%.3f] ms\n", threadId, minThread *
  // 1000);
}

//
// MandelbrotThread --
//
// Multi-threaded implementation of mandelbrot set image generation.
// Threads of execution are created by spawning std::threads.
void mandelbrotThread(int numThreads, float x0, float y0, float x1, float y1,
                      int width, int height, int maxIterations, int output[]) {
  static constexpr int MAX_THREADS = 32;

  if (numThreads > MAX_THREADS) {
    fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
    exit(1);
  }

  std::thread workers[numThreads];
  WorkerArgs args;
  args.x0 = x0;
  args.y0 = y0;
  args.x1 = x1;
  args.y1 = y1;
  args.width = width;
  args.height = height;
  args.numThreads = numThreads;
  args.maxIterations = maxIterations;
  args.numThreads = numThreads;
  args.output = output;

  // Spawn the worker threads.  Note that only numThreads-1 std::threads
  // are created and the main application thread is used as a worker
  // as well.
  for (int i = 1; i < numThreads; i++) {
    workers[i] = std::thread(workerThreadStart, &args, i);
  }

  workerThreadStart(&args, 0);
  for (int i = 1; i < numThreads; i++) {
    workers[i].join();
  }
}
