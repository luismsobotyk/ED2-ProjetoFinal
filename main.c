#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "header.h"

#define N 8191


main(){
	setlocale(LC_ALL, "Portuguese");
	Hash* ha= criaHash(N);
	FILE *csv;
	int close=1;
	do{
		int op=0;
		printf("\nMENU DE OPÇÕES:\n\t0- Sair\n\t1- Carregar Dados\n\t2- Testar acessos\n\t3- Inserir novo registro\n\t4- Buscar Registro\n");
		scanf("%d", &op);
		switch(op){
			case 0:;
				//0- Sair
				close=0;
			break;
			
			case 1:;
				//1- Carregar Dados					
				csv=fopen("data2.csv", "r");
				int i;
				while(!feof(csv)){
					char row[10000];
					char uf[2];
					int codUF;
					int codigoMunicipio;
					char nome[50];
					int populacao;
			
					fgets(row, 10000, csv);
					
					char *token;
					token= strtok(row, ";");
					i=0;
					while(token != NULL){
						if (i==0){
							strcpy(uf, token);
						}else if(i==1){
							sscanf(token, "%d", &codUF);
						}else if(i==2){
							sscanf(token, "%d", &codigoMunicipio);
						}else if(i==3){
							strcpy(nome, token);
						}else if(i==4){
							sscanf(token, "%d", &populacao);
						}
			            i++;
						token = strtok(NULL, ";");
					}
					struct cidade cidade;
					cidade.codigoMunicipio=codigoMunicipio;
					cidade.codUF= codUF;
					cidade.populacao= populacao;
					strcpy(cidade.uf, uf);
					strcpy(cidade.nome, nome);
					insereHash(ha, cidade);
				}
				fclose(csv);
			break;	
				
			case 2:;
				//2- Testar acessos
				int maior= -1;
				int tamanho= 0;
				struct cidade city[N];
				int resultado= buscaMaiorAcesso(ha, &city, csv, &maior, &tamanho);
				printf("O maior número de acessos foi %d em %d casos. Quantos você deseja mostrar?\n", maior, tamanho);
				int opcao=-1;
				do{
					scanf("%d", &opcao);
					if(opcao<0 || opcao>tamanho){
						printf("Valor incompatível. Por favor, escolha uma valor entre 0 e %d\n", tamanho);
					}
				}while(opcao<0 || opcao>tamanho);		
				for(i=0; i<opcao; i++){
					printf("\n\tUF: %s\n\tCódigo UF: %d\n\tCódigo Município: %d\n\tNome: %s\n\tPopulação: %d\n", city[i].uf, city[i].codUF, city[i].codigoMunicipio, city[i].nome, city[i].populacao);
				}
								
			break;
			
			case 3:;
				//3- Inserir novo registro
				struct cidade cid;
				printf("Informe o código do município:\n");
				scanf("%d", &cid.codigoMunicipio);
				printf("Informe o nome do município:\n");
				scanf("%s", cid.nome);
				printf("Informe a UF município:\n");
				scanf("%s", cid.uf);
				printf("Informe o código da UF do município (RS: 43):\n");
				scanf("%d", &cid.codUF);
				printf("Informe a população do município:\n");
				scanf("%d", &cid.populacao);
				if(insereHash(ha, cid)){
					int access= buscaHash(ha, cid.codigoMunicipio, &cid, cid.codUF);
					int posi= hash_chaveDivisao(cid.codigoMunicipio, cid.codUF, ha->TABLE_SIZE);
					printf("Município inserido na posição %d\nNumero de acessos necessários: %d", posi, access);
				};
			break;
			
			case 4:;
				int cod, uf;
				printf("Digite o código do municipio:\n");
				scanf("%d", &cod);
				printf("Digite o código da UF:\n");
				scanf("%d", &uf);
				struct cidade c;
				int result= buscaHash(ha, cod, &c, uf);
				if(result==0){
					printf("Município não encontrado.\n");
					break;
				}
				printf("\n\nMunicípio encontrado em %d acessos:\n\tUF: %s\n\tCódigo UF: %d\n\tCódigo Município: %d\n\tNome: %s\n\tPopulação: %d\n\n\n", result, c.uf, c.codUF, c.codigoMunicipio, c.nome, c.populacao);
				system("pause");
			break;
			
			default:;
				printf("Opção inválida!\n");
		}
	}while(close!=0);
	
	liberaHash(ha);
	return 0;
}

// NUMERO PRIMO EVITA COLISAO E POTENCIA DE DOIS É MAIS RAPIDO
Hash* criaHash(int TABLE_SIZE){
	Hash* ha = (Hash*) malloc(sizeof(Hash));
	if(ha != NULL){
		int i;
		ha->TABLE_SIZE= TABLE_SIZE;
		ha->itens = (struct cidade**) malloc(TABLE_SIZE * sizeof(struct cidade*));
		if(ha->itens == NULL){
			free(ha);
			return NULL;
		}
		ha->qtd= 0;
		for(i=0; i<ha->TABLE_SIZE-1; i++){
			ha->itens[i]= NULL;
		}
	}
	return ha;
}


int hash_chaveDivisao(int chave, int codUF, int TABLE_SIZE){
	//int hk;
	//int c1=3, c2=5;
	//hk = ((codUF + c1*chave + c2*chave*chave) & 0x7FFFFFFF) % TABLE_SIZE;
	//return hk;
		
	return ((chave & 0x7FFFFFFF)*codUF/11)%TABLE_SIZE; // Elimina o bit de sinal garantindo sempre um valor positivo, evitando assim overflow.
}

void liberaHash(Hash* ha){
	if(ha!=NULL){
		int i;
		for(i=0; i<ha->TABLE_SIZE; i++){
			if(ha->itens[i]!=NULL){
				free(ha->itens[i]);
			}
			free(ha->itens);
			free(ha);
		}
	}
}

int insereHash(Hash* ha, struct cidade c){
	//if(ha==NULL || ha->qtd==ha->TABLE_SIZE){
	if(ha==NULL){
		return 0;
	}
	int chave= c.codigoMunicipio;
	int pos= hash_chaveDivisao(chave, c.codUF, ha->TABLE_SIZE);
	if(ha->itens[pos]==NULL){
		struct cidade* novo;
		novo= (struct cidade*) malloc(sizeof(struct cidade));
		if(novo==NULL){
			return 0;
		}
		*novo= c;
		novo->next= NULL;
		ha->itens[pos]= novo;
		ha->qtd++;
		printf("Codigo %d -> %d [INSERIDO]\n", c.codigoMunicipio, pos);
		return 1;
	}
	struct cidade* atual;
	atual= ha->itens[pos];
	while(1){
		if(atual->next==NULL){
			struct cidade* novo;
			novo= (struct cidade*) malloc(sizeof(struct cidade));
			if(novo==NULL){
				return 0;
			}
			*novo= c;
			novo->next= NULL;
			atual->next= novo;
			ha->qtd++;
			printf("Codigo %d -> %d [INSERIDO]\n", c.codigoMunicipio, pos);
			return 1;
		}
		atual= atual->next;
	}

}

int buscaHash(Hash* ha, int codigoMunicipio, struct cidade* c, int codigoUF){
	if(ha==NULL){
		return 0;
	}
	int i, pos, acessos=1;
	int chave= codigoMunicipio;
	pos= hash_chaveDivisao(chave, codigoUF, ha->TABLE_SIZE);
	struct cidade* atual;
	while(1){
		atual= ha->itens[pos];
		if(atual->codigoMunicipio==codigoMunicipio && atual->codUF==codigoUF){
			*c= *atual;
			return acessos;
		}
		if(atual->next==NULL){
			return 0;
		}
		*atual= *atual->next;
		acessos++;
	}
	return 0;
}

int buscaMaiorAcesso(Hash* ha, struct cidade* cidadeLista, FILE* csv, int* maior, int* tamanho){
	if(ha==NULL){
		printf("\nA lista está vazia.");
	}
	FILE *csv2;
	csv2=fopen("data2.csv", "r");
	while(!feof(csv2)){	
		char row[10000];
		fgets(row, 10000, csv2);
		char *token;
		token= strtok(row, ";");
		int i=0;
		int codigoUF;
		int codCidade;
		while(token != NULL){
			if(i==1){
				sscanf(token, "%d", &codigoUF);
			}
			if(i==2){
				sscanf(token, "%d", &codCidade);
			}
			i++;
			token = strtok(NULL, ";");
		}
		struct cidade c;
		int acessos= buscaHash(ha, codCidade, &c, codigoUF);
		if(acessos==*maior){
			cidadeLista[*tamanho]= c;
			(*tamanho)++;
		}
		if(acessos>*maior){
			cidadeLista[0]=c;
			*tamanho= 1;
			*maior= acessos;
		}	
		printf("Testado: %d(%d)\n", codCidade, codigoUF);	
	}
	if(*tamanho>0){
		return 1;
	}
	return 0;
}
