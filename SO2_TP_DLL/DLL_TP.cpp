#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <io.h> 
#include <fcntl.h> 
#include <stdio.h>

#include "DLL_TP.h"




//funções
int teste () {
	jogador jogador;
	jogador.numero = 2;

	return jogador.numero;
}
