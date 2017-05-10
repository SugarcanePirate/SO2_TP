#include "Main.h"
#include "Ligaçoes.h"
#define BufferSize 100
#define Buffers 10

TCHAR NomeMemoria[] = TEXT("Nome da Memória Partilhada");
TCHAR *NomeSemaforoPodeEscrever = { TEXT("Semáforo Pode Escrever") };
TCHAR *NomeSemaforoPodeLer = { TEXT("Semáforo Pode Ler") };
TCHAR(*PtrMemoria)[Buffers][BufferSize];
HANDLE PodeEscrever;
HANDLE PodeLer;
HANDLE hMemoria;


#include "..\..\SO2_TP\SO2_TP_DLL\DLL_TP.h"

int criaLigacoes(void) {
//	DWORD n;
//	HANDLE hThread;
	HANDLE hFile;
//	PROCESS_INFORMATION pi;
//	STARTUPINFO si;
	


#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	PodeEscrever = CreateSemaphore(NULL, Buffers, Buffers, NomeSemaforoPodeEscrever);
	PodeLer = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeLer);

	//memoria partilhada

	hFile = CreateFile(TEXT("mapa.txt"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	hMemoria = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, sizeof(TCHAR[Buffers][BufferSize]), NomeMemoria);
	//nome do mapfile qualquer
	if (PodeEscrever == NULL || PodeLer == NULL || hMemoria == NULL) {
		_tprintf(TEXT("[Erro]Criação de objectos do Windows(%d)\n"), GetLastError());
		Sleep(10000);
		exit(-1);
	}


	PtrMemoria = (TCHAR(*)[Buffers][BufferSize])MapViewOfFile(hMemoria, FILE_MAP_WRITE, 0, 0, sizeof(TCHAR[Buffers][BufferSize]));
	if (PtrMemoria == NULL) {
		_tprintf(TEXT("[Erro]Mapeamento da memória partilhada(%d)\n"), GetLastError());
		Sleep(10000);
		exit(-1);
	}

	for (int i = 0; i < 100; i++)
	{
		WaitForSingleObject(PodeEscrever, INFINITE);
		_tprintf(TEXT("Escrever para buffer %i\n"), i);
		_stprintf_s((*PtrMemoria)[i%Buffers], BufferSize, TEXT("Escritor-%i\n"), teste());
		Sleep(500);
		ReleaseSemaphore(PodeLer, 1, NULL);
	}

	////Thread para novos clientes
	//hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecebeCliente, NULL, 0, NULL);

	UnmapViewOfFile(PtrMemoria);
	CloseHandle(PodeEscrever);
	CloseHandle(PodeLer);
	CloseHandle(hMemoria);
	exit(0);
}
