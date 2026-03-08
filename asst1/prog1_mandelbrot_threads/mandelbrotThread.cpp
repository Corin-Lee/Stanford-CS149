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
  int maxIterations;
  int* output;
  int threadId;
  int numThreads;
} WorkerArgs;

extern void mandelbrotSerial(float x0, float y0, float x1, float y1, int width,
                             int height, int startRow, int numRows,
                             int maxIterations, int output[]);

//
// workerThreadStart --
//
// Thread entrypoint.
void workerThreadStart(WorkerArgs* const args, int threadId) {
  // TODO FOR CS149 STUDENTS: Implement the body of the worker
  // thread here. Each thread should make a call to mandelbrotSerial()
  // to compute a part of the output image.  For example, in a
  // program that uses two threads, thread 0 could compute the top
  // half of the image and thread 1 could compute the bottom half.

  // printf("Hello world from thread %d\n", args->threadId);
  int rowsPerThread = (args->height / args->numThreads);
  int startRow = threadId * rowsPerThread;
  int numRows = (threadId != args->numThreads - 1) ? rowsPerThread
                                                   : (args->height - startRow);

  // double startTime = CycleTimer::currentSeconds();
  mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width,
                   args->height, startRow, numRows, args->maxIterations,
                   args->output);
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
