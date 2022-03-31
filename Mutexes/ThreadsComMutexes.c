//O objetivo deste c�digo � ensinar o acesso controlado das threads a recursos. Para isso � utilizado Mutexes, que permita controlar o acesso das threads de qualquer recurso (string, int, ficheiro...).
//Neste c�digo � controlado o acesso de uma thread e um processo, a uma vari�vel que armazena o que o utilizador escreve no terminal.
//Enquanto que o processo fica respons�vel por registar o texto na vari�vel, a thread conta o n�mero de caracteres.

//O c�digo seguinte foi adaptado do tutorial https://www.installsetupconfig.com/win32programming/threadprocesssynchronizationapis11_42.html

#include <Windows.h>
#include <stdio.h>

//Constantes
#define SHARED_SIZE 1024 // Define tamanho para a partilha de informa��o
#define labelMutex "labelMutex" //Esta constante � para ser utilizada como identificador nas opera��es das threads com mutex (mutual exclusive - vari�vel de sincroniza��o entre threads para partilha de um recurso ) 

//Declara��o de fun��es
DWORD WINAPI thread_function(LPVOID lpParam);

//Vari�veis globais
char userInputText[SHARED_SIZE]; // String para a partilha de informa��o
HANDLE sharedMutex; // Handle � uma vari�vel que permite armazenar um valor �nico de identifica��o. Neste caso � um apontador para a mem�ria kernel, especificamente aponta o sharedMutex na mem�ria.

int main()
{
    HANDLE a_thread; //vari�vel para armazenar a informa��o da thread
    DWORD a_threadId, // identificador da thread 'a_thread'
        waitForSingleObject_MainThread; // resultado da execu��o da fun��o WaitForSingleObject

    DWORD thread_result;

    // Cria um Mutex (vari�vel para a sincroniza��o das threads no acesso aos recursos)
    sharedMutex = CreateMutex(
        NULL, //Valor sobre os atributos do Mutex. Atualmente este par�metro n�o � utilizado e deve ser passado NULL.
        TRUE, //Booleano sobre o criador inicial. Se for TRUE, a thread criada tem associado o estatuto de criador do Mutex.
        labelMutex); //String para especificar o identificador do Mutex.

    //Avisa o utilizador do estado da cria��o do Mutex
    if (sharedMutex == NULL)
    {
        printf("CreateMutex() - Mutex initialization failed, error % u\n", GetLastError());
        exit(EXIT_FAILURE);
    }
    else
        printf("CreateMutex() is OK! A Mutex was created successfully!\n");

    // Cria uma nova thread.
    a_thread = CreateThread(
        NULL, // valor padr�o para os atributos de seguran�a
        0, // utiliza o tamanho padr�o da stack
        thread_function, // nome da fun��o da thread
        (void*)NULL, // par�metro para ser enviado na fun��o anterior da thread
        0, // utiliza na cria��o da thread flags por defeito
        &a_threadId); // retorna o identificador da thread

    //Avisa o utilizador do estado da cria��o da thread
    if (a_thread == NULL)
    {
        printf("Main Thread - CreateThread() - Thread creation failed, error % u", GetLastError());
        exit(EXIT_FAILURE);
    }
    else
        printf("Main Thread - CreateThread() is OK, thread ID is % u\n", a_threadId);

    printf("\n Main Thread - Input 'done' to finish or anything else to continue\n\n");

    // Enquanto o utilizador n�o introduzir a palavra 'done', continua no ciclo while
    while (strcmp("done", userInputText) != 0)
    {

        fgets(userInputText, SHARED_SIZE, stdin); // parecido ao scanf, mas aceita mais caracteres como o 'espa�o'
        userInputText[strcspn(userInputText, "\r\n")] = 0; // remove o caracter \r ou \n do final da mensagem
        if (ReleaseMutex(sharedMutex) != 0) //liberta o mutex, permitindo que outra thread possa continuar com a sua tarefa se estiver bloqueada
            printf("Main Thread - sharedMutex handle was released!\n");
        else
            printf("Main Thread - Failed to release the sharedMutex handle, error % u\n", GetLastError());


        /* A fun��o WaitForSingleObject espera pelo HANDLE (primeiro par�metro) esteja sinalizado (neste caso que o sharedMutex esteja dispon�vel) ou que o intervalo de tempo (2� par�metro) expire.
          Esta fun��o tamb�m pede para ficar com direito sobre o Mutex, assim podendo realizar a sua tarefa. Quando termina a tarefa, � libertado o direito sobre o Mutex, com o ReleaseMutex.
           
           Alguns resultados poss�veis desta fun��o s�o os seguintes :
             WAIT_OBJECT_0 - 0x00000000L -> O HANDLE foi sinalizado
             WAIT_TIMEOUT - 0x00000102L -> O tempo do intervalo especificado expirou
           Para saber de mais informa��o sobre este m�todo, consultar a documenta��o em https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject
         */

        waitForSingleObject_MainThread = WaitForSingleObject(sharedMutex, INFINITE); // Como o tempo est� declarado como infinito, significa que o c�digo s� avan�a quando o sharedMutex for sinalizado.
        printf("Main Thread - waitForSingleObject_MainThread returned value is 0X % .8X\n\n", waitForSingleObject_MainThread);

    }

    // Liberta o direito do Mutex, podendo assim outras threads que utilizam este Mutex continuar com a sua tarefa
    if (ReleaseMutex(sharedMutex) != 0)
        printf("Main Thread - sharedMutex handle was released!\n");
    else
        printf("Main Thread - Failed to release the sharedMutex handle, error % u\n", GetLastError());

    printf("Main Thread - Waiting for thread to finish...\n");


    if (waitForSingleObject_MainThread != WAIT_OBJECT_0) // se o resultado do waitForSingleObjeto n�o chegou aqui porque o HANDLE n�o foi sinalizado (WAIT_OBJECT_0), quer dizer que existiu algum erro
    {
        printf("Main Thread - waitForSingleObject_MainThread failed, thread join failed, error % u\n", GetLastError());
        exit(EXIT_FAILURE);
    }
    else
        printf("Main Thread - waitForSingleObject_MainThread 4 is OK!\n");


    // Retorna o c�digo que resulta da Thread.
    if (GetExitCodeThread(a_thread, &thread_result) != 0)
        printf("Main Thread - GetExitCodeThread() is OK!Thread joined, exit code % u\n", thread_result);
    else
        printf("Main Thread - GetExitCodeThread() failed, error % u\n", GetLastError());

    //Fecha o handle do Mutex
    if (CloseHandle(sharedMutex) != 0)
        printf("Main Thread - sharedMutex handle was closed successfully!\n");

    else
        printf("Main Thread - Failed to close sharedMutex handle, error % u\n", GetLastError());

    return 0;

}


DWORD WINAPI thread_function(void* arg) // Fun��o para ser executada quando for criada a thread. O par�metro de entrada � um apontador void, que permite passar qualquer tipo de dado. O par�metro de sa�da DWORD significa que retorna uma resposta de 32 bits integer e o WinAPI � uma conven��o utilizada para o compilador saber que a fun��o chamada fica respons�vel por limpar a stack associada
    {// Quando a fun��o retorna um valor, depois a thread desaparece

    DWORD   waitForSingleObject_CreatedThread; // resultado da execu��o da fun��o WaitForSingleObject


    // Abre uma vari�vel Mutex j� existente, para sincroniza��o do acesso aos recursos, por parte das threads. 
    // Handle � uma var�avel que permite armazenar um valor �nico de identifica��o. Neste caso � um apontador para a mem�ria kernel, especificamente aponta o sharedThread na mem�ria.
    HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, labelMutex);

    //Avisa o utilizador sobre o estado de abertura do Mutex existente (criado previamente na fun��o main)
    if (hMutex == NULL)
        printf("Created Thread- OpenMutex() failed, error % u\n", GetLastError());
    else
        printf("Created Thread- %s mutex handle successfully opened!\n", labelMutex);

    /* A fun��o WaitForSingleObject espera pelo HANDLE (primeiro par�metro) esteja sinalizado (neste caso que o sharedMutex esteja dispon�vel) ou que o intervalo de tempo (2� par�metro) expire.
          Esta fun��o tamb�m pede para ficar com direito sobre o Mutex, assim podendo realizar a sua tarefa. Quando termina a tarefa, � libertado o direito sobre o Mutex, com o ReleaseMutex.

           Alguns resultados poss�veis desta fun��o s�o os seguintes :
             WAIT_OBJECT_0 - 0x00000000L -> O HANDLE foi sinalizado
             WAIT_TIMEOUT - 0x00000102L -> O tempo do intervalo especificado expirou
           Para saber de mais informa��o sobre este m�todo, consultar a documenta��o em https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject
         */
    waitForSingleObject_CreatedThread = WaitForSingleObject(hMutex, INFINITE); 
    printf("Created Thread- waitForSingleObject_CreatedThread 1 was signalized!\n");

    // Enquanto o utilizador n�o introduzir a palavra 'done', continua no ciclo while
    userInputText[strcspn(userInputText, "\r\n")] = 0; // remove o caracter \r ou \n do final da mensagem
    while (strcmp("done", userInputText) != 0)
    {
        printf("Created Thread - You input % d characters...\n", strlen(userInputText) - 1);

        // Liberta o direito sobre o mutex 
        if (ReleaseMutex(hMutex) != 0)
            printf("Created Thread- hMutex handle was released!\n");
        else
            printf("Created Thread- Failed to release the hMutex handle, error % u\n", GetLastError());

        // antes de voltar a aceder � string do userInput, volta-se a esperar que o mutex esteja liberto. Neste caso � feito assim, pois o input do utilizador � processado pela MainThread. Ap�s a MainThread libertar o Mutex, � continuada a tarefa.
        waitForSingleObject_CreatedThread = WaitForSingleObject(hMutex, INFINITE);
        printf("Created Thread- waitForSingleObject_CreatedThread was signalized!\n");
    }

    // Liberta o direito sobre o mutex 
    if (ReleaseMutex(hMutex) != 0)
        printf("Created Thread- hMutex handle was released!\n");
    else
        printf("Created Thread- Failed to release the hMutex handle, error % u\n", GetLastError());
    
    // Fecha o handle do Mutex
    if (CloseHandle(hMutex) != 0)
        printf("Created Thread- hMutex handle was closed successfully!\n");
    else
        printf("Created Thread- Failed to close hMutex handle, error % u\n", GetLastError());

    //quando � retornado algum valor na thread, � quando a thread criada � eliminada
    return 0;

}
