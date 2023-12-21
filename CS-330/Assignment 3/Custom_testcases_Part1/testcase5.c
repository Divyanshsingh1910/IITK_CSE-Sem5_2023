#include <ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{

    char *addr1 = mmap(NULL, 22, PROT_READ | PROT_WRITE, 0);
    if ((long)addr1 < 0)
    {
        printf("1. TEST CASE FAILED\n");
        return 1;
    }
    // Vm_Area count should be 1
    // Expected output will have address printed. In your case address printed might be different.
    // But See the printed address, (i.e) the start and the end address of the dumped vm area is page aligned irrespective of the length provided.
    pmap(1);
    char *addr2=addr1-4096;
    // Access flag is different should create a new vm_area
    for (int i = 0; i < 5; i++)
    {
      if(i%2)  addr2 = mmap(addr2 + 4096, 4 * 4096, PROT_READ | PROT_WRITE, 0);
      else addr2 = mmap(addr2 + 4096, 4 * 4096, PROT_READ , 0);
        if ((long)addr2 < 0)
        {
            printf("2. TEST CASE FAILED\n");
            return 1;
        }
        pmap(1);
        int munmap1 = munmap(addr2, 2 * 4096);
        pmap(1);
    }

    return 0;
}