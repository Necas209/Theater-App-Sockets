//O objetivo deste código é ensinar o acesso controlado das threads a recursos. Para isso é utilizado Mutexes, que permita controlar o acesso das threads de qualquer recurso (string, int, ficheiro...).
//Neste código é controlado o acesso de uma thread e um processo, a uma variável que armazena o que o utilizador escreve no terminal.
//Enquanto que o processo fica responsável por registar o texto na variável, a thread conta o número de caracteres.

//O código seguinte foi adaptado do tutorial https://www.installsetupconfig.com/win32programming/threadprocesssynchronizationapis11_42.html

#include <Windows.h>
#include <stdio.h>

//Constantes
#define SHARED_SIZE 1024 // Define tamanho para a partilha de informação
#define labelMutex "labelMutex" //Esta constante é para ser utilizada como identificador nas operações das threads com mutex (mutual exclusive - variável de sincronização entre threads para partilha de um recurso ) 

//Declaração de funções
DWORD WINAPI thread_function(LPVOID lpParam);

//Variáveis globais
char userInputText[SHARED_SIZE]; // String para a partilha de informação
HANDLE sharedMutex; // Handle é uma variável que permite armazenar um valor único de identificação. Neste caso é um apontador para a memória kernel, especificamente aponta o sharedMutex na memória.

int main()
{
	HANDLE a_thread; //variável para armazenar a informação da thread
	DWORD a_threadId, // identificador da thread 'a_thread'
		waitForSingleObject_MainThread; // resultado da execução da função WaitForSingleObject
	DWORD thread_result;

	// Cria um Mutex (variável para a sincronização das threads no acesso aos recursos)
	sharedMutex = CreateMutex(
		NULL, //Valor sobre os atributos do Mutex. Atualmente este parâmetro não é utilizado e deve ser passado NULL.
		TRUE, //Booleano sobre o criador inicial. Se for TRUE, a thread criada tem associado o estatuto de criador do Mutex.
		labelMutex); //String para especificar o identificador do Mutex.

	//Avisa o utilizador do estado da criação do Mutex
	if (sharedMutex == NULL)
	{
		printf("CreateMutex() - Mutex initialization failed, error % u\n", GetLastError());
		exit(EXIT_FAILURE);
	}
	else
		printf("CreateMutex() is OK! A Mutex was created successfully!\n");

	// Cria uma nova thread.
	a_thread = CreateThread(
		NULL, // valor padrão para os atributos de segurança
		0, // utiliza o tamanho padrão da stack
		thread_function, // nome da função da thread
		NULL, // parâmetro para ser enviado na função anterior da thread
		0, // utiliza na criação da thread flags por defeito
		&a_threadId); // retorna o identificador da thread

	//Avisa o utilizador do estado da criação da thread
	if (a_thread == NULL)
	{
		printf("Main Thread - CreateThread() - Thread creation failed, error % u", GetLastError());
		exit(EXIT_FAILURE);
	}
	else
		printf("Main Thread - CreateThread() is OK, thread ID is % u\n", a_threadId);

	printf("\n Main Thread - Input 'done' to finish or anything else to continue\n\n");

	// Enquanto o utilizador não introduzir a palavra 'done', continua no ciclo while
	while (strcmp("done", userInputText) != 0)
	{
		fgets(userInputText, SHARED_SIZE, stdin); // parecido ao scanf, mas aceita mais caracteres como o 'espaço'
		userInputText[strcspn(userInputText, "\r\n")] = '\0'; // remove o caracter \r ou \n do final da mensagem
		if (ReleaseMutex(sharedMutex) != 0) //liberta o mutex, permitindo que outra thread possa continuar com a sua tarefa se estiver bloqueada
			printf("Main Thread - sharedMutex handle was released!\n");
		else
			printf("Main Thread - Failed to release the sharedMutex handle, error % u\n", GetLastError());

		/* A função WaitForSingleObject espera pelo HANDLE (primeiro parâmetro) esteja sinalizado (neste caso que o sharedMutex esteja disponível) ou que o intervalo de tempo (2.º parâmetro) expire.
		  Esta função também pede para ficar com direito sobre o Mutex, assim podendo realizar a sua tarefa. Quando termina a tarefa, é libertado o direito sobre o Mutex, com o ReleaseMutex.

		   Alguns resultados possíveis desta função são os seguintes :
			 WAIT_OBJECT_0 - 0x00000000L -> O HANDLE foi sinalizado
			 WAIT_TIMEOUT - 0x00000102L -> O tempo do intervalo especificado expirou
		   Para saber de mais informação sobre este método, consultar a documentação em https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject
		 */

		waitForSingleObject_MainThread = WaitForSingleObject(sharedMutex, INFINITE); // Como o tempo está declarado como infinito, significa que o código só avanç   a quando o sharedMutex for sinalizado.
		printf("Main Thread - waitForSingleObject_MainThread returned value is 0X % .8X\n\n", waitForSingleObject_MainThread);
	}

	// Liberta o direito do Mutex, podendo assim outras threads que utilizam este Mutex continuar com a sua tarefa
	if (ReleaseMutex(sharedMutex) != 0)
		printf("Main Thread - sharedMutex handle was released!\n");
	else
		printf("Main Thread - Failed to release the sharedMutex handle, error % u\n", GetLastError());

	printf("Main Thread - Waiting for thread to finish...\n");


	if (waitForSingleObject_MainThread != WAIT_OBJECT_0) // se o resultado do waitForSingleObjeto não chegou aqui porque o HANDLE não foi sinalizado (WAIT_OBJECT_0), quer dizer que existiu algum erro
	{
		printf("Main Thread - waitForSingleObject_MainThread failed, thread join failed, error % u\n", GetLastError());
		exit(EXIT_FAILURE);
	}
	else
		printf("Main Thread - waitForSingleObject_MainThread 4 is OK!\n");


	// Retorna o código que resulta da Thread.
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


DWORD WINAPI thread_function(LPVOID lpParam) // função para ser executada quando for criada a thread. O parâmetro de entrada é um apontador void, que permite passar qualquer tipo de dado. O parâmetro de saída DWORD significa que retorna uma resposta de 32 bits integer e o WinAPI é uma convenção utilizada para o compilador saber que a função chamada fica responsável por limpar a stack associada
{// Quando a função retorna um valor, depois a thread desaparece

	DWORD waitForSingleObject_CreatedThread; // resultado da execução da função WaitForSingleObject

	// Abre uma variável Mutex já existente, para sincronização do acesso aos recursos, por parte das threads. 
	// Handle é uma variável que permite armazenar um valor único de identificação. Neste caso é um apontador para a memória kernel, especificamente aponta o sharedThread na memória.
	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, labelMutex);

	//Avisa o utilizador sobre o estado de abertura do Mutex existente (criado previamente na função main)
	if (hMutex == NULL)
		printf("Created Thread- OpenMutex() failed, error % u\n", GetLastError());
	else
		printf("Created Thread- %s mutex handle successfully opened!\n", labelMutex);

	/* A função WaitForSingleObject espera pelo HANDLE (primeiro parâmetro) esteja sinalizado (neste caso que o sharedMutex esteja disponível) ou que o intervalo de tempo (2.º parâmetro) expire.
		  Esta função também pede para ficar com direito sobre o Mutex, assim podendo realizar a sua tarefa. Quando termina a tarefa, é libertado o direito sobre o Mutex, com o ReleaseMutex.

		   Alguns resultados possíveis desta função são os seguintes :
			 WAIT_OBJECT_0 - 0x00000000L -> O HANDLE foi sinalizado
			 WAIT_TIMEOUT - 0x00000102L -> O tempo do intervalo especificado expirou
		   Para saber de mais informação sobre este método, consultar a documentação em https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject
		 */
	waitForSingleObject_CreatedThread = WaitForSingleObject(hMutex, INFINITE);
	printf("Created Thread- waitForSingleObject_CreatedThread 1 was signalized!\n");

	// Enquanto o utilizador não introduzir a palavra 'done', continua no ciclo while
	userInputText[strcspn(userInputText, "\r\n")] = 0; // remove o caracter \r ou \n do final da mensagem
	while (strcmp("done", userInputText) != 0)
	{
		printf("Created Thread - You input %d characters...\n", strlen(userInputText) - 1);

		// Liberta o direito sobre o mutex 
		if (ReleaseMutex(hMutex) != 0)
			printf("Created Thread- hMutex handle was released!\n");
		else
			printf("Created Thread- Failed to release the hMutex handle, error % u\n", GetLastError());

		// antes de voltar a aceder é string do userInput, volta-se a esperar que o mutex esteja liberto. Neste caso é feito assim, pois o input do utilizador é processado pela MainThread. Após a MainThread libertar o Mutex, é continuada a tarefa.
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

	//quando é retornado algum valor na thread, é quando a thread criada é eliminada
	return 0;

}
