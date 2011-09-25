#include <stdarg.h>
#include <kernel.h>
#include <ganoid/types.h>

#include <tests.h>
#include <tests/t_sprintf.h>


testfuncptr TestFunctions[MAX_TESTS] = {Test_sprintf, NULL};


int ExecuteTests (int *TestIdArray)
{
  int i;
  //  TestFunctions[0] = Test_sprintf;

  for (i = 0;TestFunctions[i];i++)
    (TestFunctions[i])(0, 0, NULL);

  return 0;
}
