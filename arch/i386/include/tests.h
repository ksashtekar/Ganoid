#ifndef TESTS_H_
#define TESTS_H_

#define MAX_TESTS 10

typedef int (*testfuncptr)(int param1, int param2, void * ptr);

int ExecuteTests (int *TestIdArray);


#endif // TESTS_H_
