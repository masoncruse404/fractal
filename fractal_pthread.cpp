
#include <cstdlib>
#include <cmath>
#include <sys/time.h>
#include "cs43805351.h"
#include "pthread.h"

static const double Delta = 0.004;
static const double xMid =  0.2389;
static const double yMid =  0.55267;
unsigned char* pic;
unsigned char* sub_pic;

static void* fractal(void *arg)
{
  // determine work for each thread
  const long my_rank = (long)arg;
  const long beg = my_rank * range / threads;
  const long end = (my_rank + 1) * range / threads;
  // compute frames
  for (long frame = beg; frame < end; frame++) {
    const double delta = Delta * pow(0.98, frame);
    const double xMin = xMid - delta;
    const double yMin = yMid - delta;
    const double dw = 2.0 * delta / width;
    for (int row = 0; row < width; row++) {
      const double cy = yMin + row * dw;
      for (int col = 0; col < width; col++) {
        const double cx = xMin + col * dw;
        double x = cx;
        double y = cy;
        int depth = 256;
        double x2, y2;
        do {
          x2 = x * x;
          y2 = y * y;
          y = 2 * x * y + cy;
          x = x2 - y2 + cx;
          depth--;
        } while ((depth > 0) && ((x2 + y2) < 5.0));
        pic[frame * width * width + row * width + col] = (unsigned char)depth;
      }
    }
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  printf("Fractal v1.7\n");

  // check command line
  if (argc != 4) {fprintf(stderr, "usage: %s frame_width num_frames\n", argv[0]); exit(-1);}
  const int width = atoi(argv[1]);
  if (width < 10) {fprintf(stderr, "error: frame_width must be at least 10\n"); exit(-1);}
  const int frames = atoi(argv[2]);
  if (frames < 1) {fprintf(stderr, "error: num_frames must be at least 1\n"); exit(-1);}
  printf("computing %d frames of %d by %d fractal\n", frames, width, width);
  const int threads = atoi(argv[3]);
  if (threads < 1) {fprintf(stderr, "error: threads must be at least 1\n");
  
  // initialize pthread variables
  pthread_t* const handle = new pthread_t[threads - 1];

  // start time
  timeval start, end;
  gettimeofday(&start, NULL);

  // allocate picture array
  unsigned char* pic = new unsigned char[frames * width * width];
  sub_pic = (unsigned char*)malloc(frames*wdith*wdith*threads*sizeof(unsigned char));

  // start time
  timeval start, end;
  gettimeofday(&start, NULL);

  // launch threads
  for (long thread = 0; thread < threads - 1; thread++)
  {
      pthread_create(&handle[thread], NULL, fractal, (void *)thread);
  }

  // work for master
  fractal((void*)(threads - 1));

  //fractal(width, frames, pic);

  // join threads
  for (long thread = 0; thread < threads - 1; thread++)
  {
      pthread_join(handle[thread], NULL);
  }

  // end time
  gettimeofday(&end, NULL);
  const double runtime = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.0;
  printf("compute time: %.3f s\n", runtime);

  // verify result by writing frames to BMP files
  if ((width <= 256) && (frames <= 100)) {
    for (int frame = 0; frame < frames; frame++) {
      char name[32];
      sprintf(name, "fractal%d.bmp", frame + 1000);
      writeBMP(width, width, &pic[frame * width * width], name);
    }
  }

  delete [] pic;
  return 0;
}

