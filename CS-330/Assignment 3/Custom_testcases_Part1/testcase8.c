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

    
    int res = mprotect((void *)(mm1 + 1 * pages), pages * 8, PROT_READ);
    if (res < 0)
    {
        // Testcase failed.
        printf("%d. Test case failed \n", er++);
        return 1;
    }
    pmap(1);
    

    pmap(1);

    return 0;
}
