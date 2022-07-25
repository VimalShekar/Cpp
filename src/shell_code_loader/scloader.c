/*
A quick and dirty program to load a binary file into memory and transfer control.
Transfers control using __asm {  JMP  address }
Works on Windows and Linux - x86 machine. 
If DEP is enabled, this will crash. Be sure to run on a machine without DEP. 
*/

#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>

int main(int argc, char *argv[])
{
    if(argc < 2) {
        printf("Usage: %s shellcodefile [options]", argv[0]);
        return 1;
    }

    // Get File size and allocate buffer 
    struct stat sb;
    if( stat(argv[1], &sb) == -1 ) {
        printf("\nError: Could not get file stats: %s", argv[1]);        
        return 1;
    }


    FILE *pFile = fopen( argv[1], "rb");
    if(!pFile) {
        printf("\nError: Could not open file: %s", argv[1]);
        return 1;
    }


    // allocate memory for shell code, read the file into the memory    
    void *pSC = malloc(sb.st_size);;
    fread(pSC, sb.st_size, 1, pFile);
    
    
    printf("\nPassing Control to Shellcode.");
    
    #ifdef _MSC_VER         
        __asm { 
            jmp pSC; 
        };
    #else
        asm ( 
            "jmp DWORD PTR [ebp-0x10]"  
        );
    #endif
    
    printf("\nControl returned from Shellcode.");
    free(pSC);    
    fclose(pFile);
    printf("\nExiting program");
    return 0;
}
