#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int addr5;
int addr6;

int foo()
{
    return -1;
}
void point_at(void *p);
void foo1();
char g = 'g';
void foo2();

int secondary(int x)
{
    int addr2;
    int addr3;
    char *yos = "ree";
    int *addr4 = (int *)(malloc(50));
    int iarray[3];
    float farray[3];
    double darray[3];
    char carray[3];
    int iarray2[] = {1, 2, 3};
    char carray2[] = {'a', 'b', 'c'};
    int *iarray2Ptr = iarray2;
    char *carray2Ptr = carray2;

    printf("- &addr2: %p\n", &addr2);
    printf("- &addr3: %p\n", &addr3);
    printf("- foo: %p\n", &foo);
    printf("- &addr5: %p\n", &addr5);
    printf("Print distances:\n");
    point_at(&addr5);

    printf("Print more addresses:\n");
    printf("- &addr6: %p\n", &addr6);
    printf("- yos: %p\n", yos);
    printf("- gg: %p\n", &g);
    printf("- addr4: %p\n", addr4);
    printf("- &addr4: %p\n", &addr4);

    printf("- &foo1: %p\n", &foo1);
    printf("- &foo1: %p\n", &foo2);

    printf("Print another distance:\n");
    printf("- &foo2 - &foo1: %ld\n", (long)(&foo2 - &foo1));

    printf("Arrays Mem Layout (T1b):\n");

    /* task 1 b here */
    // Print hexadecimal values of pointers
    printf("Task1B START here: \n");

    // printf("iarray: %p\n", (void *)iarray);
    // printf("iarray+1: %p\n", (void *)(iarray + 1));

    // printf("farray: %p\n", (void *)farray);
    // printf("farray+1: %p\n", (void *)(farray + 1));

    // printf("darray: %p\n", (void *)darray);
    // printf("darray+1: %p\n", (void *)(darray + 1));

    // printf("carray: %p\n", (void *)carray);
    // printf("carray+1: %p\n", (void *)(carray + 1));

    printf("Hexadecimal values:\n");
    printf("iarray: %p\n", (void *)iarray);
    printf("iarray+1: %p\n", (void *)(iarray + 1));

    printf("farray: %p\n", (void *)farray);
    printf("farray+1: %p\n", (void *)(farray + 1));

    printf("darray: %p\n", (void *)darray);
    printf("darray+1: %p\n", (void *)(darray + 1));

    printf("carray: %p\n", (void *)carray);
    printf("carray+1: %p\n", (void *)(carray + 1));

    printf("Pointers and arrays (T1d):\n ");

    // /* task 1 d here */
    printf("Task1d start here: \n");

    //     for (int i = 0; i < 3; i++)
    //     {
    //         printf("%d", *iarray2Ptr);
    //         iarray2Ptr = iarray2Ptr + 1;
    //     }
    //     printf("\n");
    //     for (int i = 0; i < 3; i++)
    //     {
    //         printf("%d", *carray2Ptr);
    //         carray2Ptr = carray2Ptr + 1;
    //     }
    //     int *p;
    //     printf("%d \n", *p);

    //     printf("Task1d end here: \n");
    // }

    // /* task 1 d here */
    printf("Task1d start here: \n");

    // Print all values of the iarray2 using iarray2Ptr
    for (int i = 0; i < 3; i++)
    {
        printf("%d ", *iarray2Ptr);
        iarray2Ptr = iarray2Ptr + 1;
    }
    printf("\n");

    // Print all values of the carray2 using carray2Ptr
    for (int i = 0; i < 3; i++)
    {
        printf("%c ", *carray2Ptr);
        carray2Ptr = carray2Ptr + 1;
    }

    // Uninitialized pointer local variable p
    int *p;
    printf("\nUninitialized pointer p value: %p \n", (void *)p); // Print the value of the uninitialized pointer

    printf("Task1d end here: \n");
}

    int main(int argc, char **argv)
    {

        printf("Print function argument addresses:\n");

        printf("- &argc %p\n", &argc);
        printf("- argv %p\n", argv);
        printf("- &argv %p\n", &argv);

        secondary(0);

        printf("Command line arg addresses (T1e):\n");
        /* task 1 e here */
        printf("Print function argument addresses:\n");

        // Print the address and content of each command-line argument
        for (int i = 1; i < argc; i++)
        {
            printf("Argument %d: Address=%p, Content=%s\n", i, argv[i], argv[i]);
        }

        return 0;
    }

    void point_at(void *p)
    {
        int local;
        static int addr0 = 2;
        static int addr1;

        long dist1 = (size_t)&addr6 - (size_t)p;
        long dist2 = (size_t)&local - (size_t)p;
        long dist3 = (size_t)&foo - (size_t)p;

        printf("- dist1: (size_t)&addr6 - (size_t)p: %ld\n", dist1);
        printf("- dist2: (size_t)&local - (size_t)p: %ld\n", dist2);
        printf("- dist3: (size_t)&foo - (size_t)p:  %ld\n", dist3);

        /* part of task 1 a here */
        //==============Task1a==========================================================
        printf("Check long type mem size (T1a):\n");
        printf("TASK1A !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");

        // Print the actual size of dist3
        printf("The actual size of dist3: %zu bytes\n", sizeof(dist3));

        // Print the size in the code (compile-time size)
        printf("The size in the code: %zu bytes\n", sizeof(long));

        //==============Task1a==========================================================

        printf("- addr0: %p\n", &addr0);
        printf("- addr1: %p\n", &addr1);
    }

    void foo1()
    {
        printf("foo1\n");
    }

    void foo2()
    {
        printf("foo2\n");
    }