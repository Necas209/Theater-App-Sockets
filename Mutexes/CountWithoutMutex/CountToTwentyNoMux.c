#include <windows.h>
#include <stdio.h>

#define THREADCOUNT 4

DWORD WINAPI CountToTwenty(LPVOID);

int main(void)
{
    HANDLE aThread[THREADCOUNT] = { NULL };
    DWORD ThreadID;
    int i;

    
    // Create worker threads

    for (i = 0; i < THREADCOUNT; i++)
    {
        aThread[i] = CreateThread(
            NULL,       // default security attributes
            0,          // default stack size
            (LPTHREAD_START_ROUTINE)CountToTwenty,
            NULL,       // no thread function arguments
            0,          // default creation flags
            &ThreadID); // receive thread identifier

        if (aThread[i] == NULL)
        {
            printf("CreateThread error: %d\n", GetLastError());
            return 1;
        }
    }

    // Wait for all threads to terminate

    WaitForMultipleObjects(THREADCOUNT, aThread, TRUE, INFINITE);

    // Close thread 

    for (i = 0; i < THREADCOUNT; i++)
        CloseHandle(aThread[i]);

    return 0;
}

DWORD WINAPI CountToTwenty(LPVOID lpParam)
{
    // lpParam not used in this example
    UNREFERENCED_PARAMETER(lpParam);

    DWORD dwCount = 0;
    int i;

    // Request ownership of mutex.

    while (++dwCount < 10)
    {
    // TODO: Count to 20
       
        printf("\nThread %d , %d contagem:\n", GetCurrentThreadId(), dwCount);
        for (i = 1; i <= 10; i++)
        {
            printf(" Thread %d: %d |", GetCurrentThreadId(), i);
            Sleep(500 * ((int)rand() % 2));
        }
        
    }
    return 0;
}