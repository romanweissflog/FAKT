/* main.c */

#include <stdio.h>
#include <stdlib.h>
#include "bool.h"

#include "demo.h"

int main(int argc, char* argv[])
{
    BOOL ok = (argc > 1);

    if (ok)
    {
        ok = main_c(argv[1]);
    }
    else
    {
        printf("Usage: demo.exe tablename.dbf\n");
    }
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
