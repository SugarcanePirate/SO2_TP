#include <windows.h>
#include <tchar.h>
#include <io.h> 
#include <fcntl.h> 


struct jogadores
{
	int numero;

};
typedef struct jogadores jogador;
#ifdef DLL_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else 
#define DLL_IMP_API __declspec(dllimport) 
#endif


//funcoes servidor
DLL_IMP_API int teste();
