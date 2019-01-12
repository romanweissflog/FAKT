/* cdemo.c */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "bool.h"
#include "dbf.h"
#include "demo.h"

BOOL main_c(const char* filename)
{
  DBF_HANDLE handle = dbf_open(filename, NULL);
  BOOL ok = (handle != NULL);

  if (ok)
  {
     size_t count = dbf_getrecordcount(handle);
     size_t fields = dbf_getfieldcount(handle);
     size_t i, j;

     for (i = 0; i < count; i++)
     {
        dbf_setposition(handle, i);
        for (j = 0; j < fields; j++)
        {
           char temp[80] = "";
           
           dbf_getfield(handle, dbf_getfieldptr(handle, j), temp, sizeof(temp), DBF_DATA_TYPE_ANY);
           if (j) printf(";");
           printf(temp);
        }
        printf("\n");
     }
     dbf_close(&handle);
  }
   return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
