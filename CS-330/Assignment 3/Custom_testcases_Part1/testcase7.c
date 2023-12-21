#include <ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{

    int pages = 4096;
    int er = 1;
    char *mm1 = mmap(NULL, pages * 2, PROT_READ, 0);
    if ((long)mm1 < 0)
    {
        // Testcase failed.
        printf("%d. Test case failed \n", er++);
        return 1;
    }
    // vm_area count should be 1.
    pmap(1);
    char *mm2 = mmap(mm1 + 4 * pages, pages, PROT_READ | PROT_WRITE, MAP_FIXED);
    if ((long)mm2 < 0)
    {
        // Testcase failed.
        printf("%d. Test case failed \n", er++);
        return 1;
    }
    pmap(1);
    char *mm3 = mmap(mm2 + 3 * pages, 3 * pages, PROT_READ, MAP_FIXED);
    if ((long)mm3 < 0)
    {
        // Testcase failed.
        printf("%d. Test case failed \n", er++);
        return 1;
    }
    pmap(1);

    int a1 = munmap(mm2 - pages, 2 * pages);
    if (a1 < 0)
    {
        printf("%d. Test case failed \n", er++);
        return 1;
    }
    pmap(1);
    int a2 = munmap(mm1 + pages, 7 * pages);
    if (a2 < 0)
    {
        printf("%d. Test case failed \n", er++);
        return 1;
    }
    pmap(1);
    char *mm4 = mmap(mm1 + 9 * pages, 7 * pages, PROT_READ | PROT_WRITE, MAP_FIXED);
    if ((long)mm4 >= 0)
    {
        // Testcase failed.
        printf("%d. Test case failed \n", er++);
        return 1;
    }
    pmap(1);
    mm4 = mmap(mm1 + 9 * pages, 7 * pages, PROT_READ | PROT_WRITE, 0);
    if ((long)mm4 < 0)
    {
        // Testcase failed.
        printf("%d. Test case failed \n", er++);
        return 1;
    }
    pmap(1);
    int res = mprotect((void *)(mm4 + 3 * pages), pages * 4, PROT_READ);
    if (res < 0)
    {
        // Testcase failed.
        printf("%d. Test case failed \n", er++);
        return 1;
    }
    pmap(1);
    int a3 = munmap(mm4, 3 * pages);
    if (a3 < 0)
    {
        // Testcase failed.
        printf("%d. Test case failed \n", er++);
        return 1;
    }
    pmap(1);
    mm4 = mmap(mm1 + pages, 2 * pages, PROT_READ, MAP_FIXED);
    if ((long)mm4 < 0)
    {
        // Testcase failed.
        printf("%d. Test case failed \n", er++);
        return 1;
    }
    pmap(1);
    res = mprotect((void *)(mm1 + 2 * pages), pages * 5, PROT_READ | PROT_WRITE);
    if (res < 0)
    {
        // Testcase failed.
        printf("%d. Test case failed \n", er++);
        return 1;
    }

    pmap(1);

    return 0;
}
