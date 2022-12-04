#include <stdio.h>
#include <time.h>
#include <unistd.h> 
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>

struct cidade {
	char uf[2];
	int codUF;
	int codigoMunicipio;
	char nome[50];
	int populacao;
	struct cidade* next;
};
typedef struct cidade Cidade;

struct hash {
	int qtd, TABLE_SIZE;
	struct cidade **itens;
};
typedef struct hash Hash;

Hash* criaHash(int TABLE_SIZE);

int hash_chaveDivisao(int chave, int codUF, int TABLE_SIZE);

void liberaHash(Hash* ha);
int insereHash(Hash* ha, struct cidade c);
int buscaHash(Hash* ha, int codigoMunicipio, struct cidade* c, int codigoUF);
int buscaMaiorAcesso(Hash* ha, struct cidade* cidadeLista, FILE* csv, int* maior, int* tamanho);
