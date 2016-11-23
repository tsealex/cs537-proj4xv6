#include "types.h"
#include "stat.h"
#include "user.h"

lock_t lock;
int result = 0;

void
worker_wlock(void* arg) {
  int i;
  int max = ((int*)arg)[0];
  lock_acquire(&lock);
  int temp = result;
  for (i = 0; i < max; i++) {
    temp++;
  }
  result = temp;
  lock_release(&lock);
  exit();
}

void
worker_wolock(void* arg) {
  int i;
  int max = ((int*)arg)[0];
  int temp = result;
  for (i = 0; i < max; i++) {
    temp++;
  }
  result = temp;
  exit();
}

int
main(int argc, char **argv) {
  lock_init(&lock);
  if (argc != 3) {
    printf(1, "Usage: threadtest [threads] [times]\n");
    exit();
  }
  printf(1, "waiting for result\n");

  int* arr = malloc(sizeof(int));
  arr[0] = atoi(argv[2]);
  int i = 0;
  int num = atoi(argv[1]);

  for (i = 0; i < num; i++)
    thread_create(worker_wolock, (void*)arr);
  for (i = 0; i < num; i++)
    thread_join();
  printf(1, "final result (without lock): %d\n", result);

  result = 0;
  printf(1, "waiting for result (with lock)\n");
  for (i = 0; i < num; i++)
    thread_create(worker_wlock, (void*)arr);
  for (i = 0; i < num; i++)
    thread_join();
  printf(1, "final result (with lock): %d\n", result);

  exit();
}

