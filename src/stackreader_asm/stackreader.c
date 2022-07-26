#include<stdio.h>

unsigned int get_my_sp()
{
   asm(
    "mov eax, esp;"
    "add eax, 8;"  // 4 bytes for push ebp and 4 bytes for main()'s return address
    );
}

unsigned int get_my_bp()
{
   asm(
    "mov eax, [ebp];" 
    );
}

unsigned int get_my_caller_returnaddr()
{
   asm(  
    "mov eax, [ebp];"   // return's caller's frame pointer
    "mov eax, [eax+8]"  // return's caller's return address
    );
}

unsigned int get_byte_at(void *pAddr)
{
    return (0x0 | *((char *)pAddr));
}

unsigned int get_dword_at(void *pAddr)
{
    return *((unsigned int *)pAddr);
}


int main()
{
    int arr[10];  // just a random allocation
    int curr_StackPointer = get_my_sp();
    int curr_FramePointer = get_my_bp();
    int curr_myCallerReturnAddr = get_my_caller_returnaddr();  
    

    printf("Curr Stack Pointer: 0x%x\n", curr_StackPointer);
    printf("Curr Frame Pointer: 0x%x\n", curr_FramePointer);
    printf("Return Address: 0x%x\n", get_my_caller_returnaddr);
    
    return 0;
}
