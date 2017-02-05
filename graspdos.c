#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
//#include <sys/types.h>
//#include <sys/times.h>
//#include <time.h>
//#include <utime.h>

/**********************************************************************************
  PROGRAMA: APLICACAO DO ALGORITMO GRASP AO PQA USANDO A DEFINICAO DA VIZINHANCA                 
            USADA NA BUSCA LOCAL DADA POR [RESENDE].

  ELABORADO POR: MARIA CRISTINA RANGEL
                 MARIA CLAUDIA SILVA BOERES

  ULTIMA ATUALIZACAO: 03/02/99

  CALCULA A MEDIA DAS SOLUCOES INICIAIS

  CALCULA A MEDIA DAS SOLUCOES OTIMAS LOCAIS

  LINHA DE COMANDO: <executavel> <arq.dados> <num.iter> <alpha> <beta> <otimo>
  obs: o otimo e' a solucao do PQA dividida por 2.
   
  FLAG PARA TERMINAR: NUMERO MAXIMO DE ITERACOES OU ALCANCOU OTIMO
 
**********************************************************************************/

/*================================================================================
                         Definicao dos tipos de dados usados
  ================================================================================*/
struct par_ord  { int valor; int posicao;                 };
struct auxiliar { int pos_f; int pos_d;                   };
struct trip     { int val;   int pos_fluxo; int pos_dist; };
    
typedef struct par_ord  par;
typedef struct auxiliar auxi;
typedef struct trip     tripla;

typedef struct solucao_viavel
{
	int   iteracao;
	int   valor;
	int   fi;
	float tempo;

	//solucao_viavel *prox;

} TpSolucao;

/*
typedef struct conjunto_solucoes
{
	int        quantidade
	TpSolucao *solucoes;
} TpConjSolucao;
*/

/*================================================================================
                             Declaracao das variaveis           
  ================================================================================*/
int n, N, otimo, mc, ci, CustoBest, Num_Repeticoes, rep;
   
int **Mat_Dist, **Mat_Fluxo, *Dist, *Fluxo, *parf, *pard, *Fi, *FiBest, *FiMelhor, *FiMelhorAnterior;
   
float alfa, beta, media, media1, limite, Lim1, Lim2;

par *F_ord, *D_ord;
tripla *Produto;

long seed, *Lst_Sementes, iseed;

float comeco, fim;

//struct tms buffer;

char melhor[512];
TpSolucao *solucoes;
/*
TpConjSolucao LstSolucoes;
TpSolucao     *prox_solucao;
TpSolucao     *solucao_atual;
*/
/**********************************************************************************
                                Declaracao das funcoes 
**********************************************************************************/

int main();
void LeGrasp(),
     Ordenap(),
     Ordenat(),
     semente();

int* Inversa_Psi();

int  CustoFD(); 

int*    Alocai();
par*    Alocap();
tripla* Alocat();
auxi*   Aloca_aux();

void Grava_Vetor(char nm_saida[256], int repeticao, int iteracao, char vetor[1000], int custo_final);

void Ordena_Menos(int *vet, int tam, int ini, int fim);
 int Consulta( int *novo, int tam, int item );
 int Ver_Troca ( int *troca, int tam, int posicao );
void Ordena_Matrizes( int ident );

/**********************************************************************************
                                Inicio do programa principal 
**********************************************************************************/

int main(argc, argv)
int argc;
char *argv[];
{
	int i, j, k, r, x,y, max, maxiter, inversoes_fluxo=0, inversoes_dist=0;
	float tempo_ordenacao=0.0;
	char *instancia, *alfa1, *beta1, *otimo1;
	char nm_saida[256], comando[256];
	FILE *arq_saida=NULL;
	int media_iteracao=0, media_valor=0, media_fi=0,
	    min_iteracao=0,   min_valor=0,   min_fi=0,
	    max_iteracao=0,   max_valor=0,   max_fi=0;
	float media_tempo=0.0, min_tempo=0.0, max_tempo=0.0;

	/* Inicializacao das variaveis e leitura de dados */	
	instancia = argv[1];
	LeGrasp(instancia);

    /* Leitura do numero maximo de iteracoes grasp */
	maxiter = atoi(argv[2]);

	alfa1 = argv[3];
	beta1 = argv[4];
	otimo1 = argv[5];
	sscanf(alfa1,"%f",&alfa);
	sscanf(beta1,"%f",&beta);
	sscanf(otimo1,"%d",&otimo);
	
	printf("Inicio de Tudo: %s [%i] %f %f %i\n\n",instancia, maxiter, alfa, beta, otimo);

	/* Inicializacao da semente */
	//semente(); /* seed = 0; */

	/* Alocacao de memoria */
	FiMelhor = Alocai(n + 1);
	FiMelhorAnterior = Alocai(n + 1);
 	
	/* Loop com maxiter iteracoes GRASP */	
	FiBest = Alocai(n + 1);

/********************************************
	LstSolucoes.quantidade = 0;
	LstSolucoes.solucoes   = NULL;
	prox_solucao = LstSolucoes.solucoes;
*********************************************/
	Num_Repeticoes = 1; // Pra pegar a m้dia - RMS.
	solucoes = (TpSolucao *)malloc(Num_Repeticoes * sizeof(TpSolucao));
	Lst_Sementes = (long *)malloc(Num_Repeticoes * sizeof(long));
	for( y=0; y<Num_Repeticoes; y++ ) 
	{
		seed = 0;
		semente(); 
		Lst_Sementes[y] = seed + (rand() % (Num_Repeticoes*5)); 
		for(x=0; x<10000; x++) { r=x; i=seed; j=i%10; k=j*r; }
		//printf("%d ", Lst_Sementes[y]);
	}
	//printf("\n");

	for(y=0; y<2; y++)
	{	
		sprintf(nm_saida, "%s-%04i-%i.gsp", instancia, maxiter,y);
		arq_saida=fopen(nm_saida, "w");
		fclose(arq_saida);

		arq_saida=fopen(nm_saida, "a");
		//fprintf(arq_saida, "Matrizes:\n");
		inversoes_fluxo = 0;
		inversoes_dist  = 0;
		
		if( y == 1)
		{
			/*
            times(&buffer);
			comeco = buffer.tms_utime;
			*/

			printf("Ordena as matrizes\n\n");
			Ordena_Matrizes(0);   
			Ordena_Matrizes(1);   

			/*
            times(&buffer);
			fim = buffer.tms_utime;

			tempo_ordenacao = (fim-comeco)/60;   			
			*/
		}
		
		/* Le matriz de fluxos */
		//fprintf(arq_saida, "Matriz de Fluxo\n");
		for(i = 1; i <= n; i++)
		{
			for(j = 1; j <= n; j++)
			{
				x = Mat_Fluxo[i-1][j-1];
				//fprintf(arq_saida, "%i ", x);
				if (i < j)
				{
					r = Psi(i,j);
					Fluxo[r] = x;
					F_ord[r].valor = Fluxo[r];
					F_ord[r].posicao = r;
				}
			}
		}
		
		//fprintf(arq_saida, "\nMatriz de Distancia\n");
		/* Le a matriz de distancias */
		for(i = 1; i <= n; i++)
		{
			for(j = 1; j <= n; j++)
			{
				x = Mat_Dist[i-1][j-1];
				//fprintf(arq_saida, "%i ", x);
				if (i < j) 
				{
					r = Psi(i,j);
					Dist[r] = x;
					D_ord[r].valor = -Dist[r];
					D_ord[r].posicao = r;
				}
			}
		}
		//fprintf(arq_saida, "\n");
		
		/*
		if (inversoes_fluxo == 0)
		{
 	        inversoes_fluxo = Conta_Inversao_Menos(Fluxo, N);
 	        inversoes_dist = Conta_Inversao_Mais(Dist, N);
 	    }
		*/

		printf("/* Calculo do vetor de custos fij x dkl */\n\n");
		max = CustoFD();
		fclose(arq_saida);
		for(rep = 0;rep < Num_Repeticoes; rep++) /* executar o grasp repetidas vezes */
		{			
			arq_saida=fopen(nm_saida, "a");

			iseed = Lst_Sementes[rep];

			/* Impressao dos parametros */
			fprintf(arq_saida, "\n%i-%02i: semente = %d ",y,rep,iseed);

			//times(&buffer);
			//comeco = buffer.tms_utime;
		
	 		for(i=1; i<=N; i++)
			{
				j = N - (i-1);
				CustoBest = CustoBest + (F_ord[j].valor * -D_ord[i].valor);
			}
	
			media = 0; media1 = 0;
		
			/*Flag de saida: maxiter e encontrou solucao otima conhecida */
			i = 1; 
			printf("vai come็ar, %i - %i!!!\n\n", maxiter, CustoBest );
			while(i <= maxiter  &&  CustoBest != otimo)
			{
		 
				/* Construcao da permutacao inicial */
				printf("/* Construcao da permutacao inicial */\n\n");
				Construcao(max); ci = custo(Fi);		
				media1 = media1 + ci;
		
				/* Busca da melhor solucao existente na vizinhanca da solucao inicial */		
				printf("Busca_Local(nm_saida,rep, i, maxiter);\n\n");
				Busca_Local(nm_saida,rep, i, maxiter);		
				media = media + mc; 
				
				/* Guarda a melhor solucao ate o momento */
	 			Atualiza_Solucao(&i, arq_saida);
				i++;		 				
			}
			
			fprintf(arq_saida, melhor);
			media = media/(i-1);  media1 = media1/(i-1);			
		
			//times(&buffer);
			//fim = buffer.tms_utime;
	 
			/* Imprime a melhor solucao gerada pelo GRASP */
			//ImprimeDadosVetor(FiBest,n, arq_saida); 
			fprintf(arq_saida,"จจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจจ\n");
			fclose(arq_saida);
			
		} /* fim das repeticoes */

		arq_saida=fopen(nm_saida, "a");
		media_valor    = 0;
		media_iteracao = 0;
		media_fi       = 0;
		media_tempo    = 0.0;
		min_tempo      = solucoes[0].tempo;
		min_valor      = solucoes[0].valor;
		//min_fi         = solucoes[0].fi;
		max_tempo      = solucoes[0].tempo;
		max_valor      = solucoes[0].valor;
		//max_fi         = solucoes[0].fi;
		for(x=0; x<Num_Repeticoes; x++)
		{
			if ( min_tempo > solucoes[x].tempo ) min_tempo = solucoes[x].tempo;
			if ( min_valor > solucoes[x].valor ) min_valor = solucoes[x].valor;
			//if ( min_fi    > solucoes[x].fi    ) min_fi = solucoes[x].fi;
			if ( max_tempo < solucoes[x].tempo ) max_tempo = solucoes[x].tempo;
			if ( max_valor < solucoes[x].valor ) max_valor = solucoes[x].valor;
			//if ( max_fi    < solucoes[x].fi    ) max_fi = solucoes[x].fi;
			media_valor    += solucoes[x].valor;
			media_iteracao += solucoes[x].iteracao;
			//media_fi       += solucoes[x].fi;
			media_tempo    += solucoes[x].tempo;
		}
		
		media_valor    = media_valor/Num_Repeticoes;
		media_iteracao = media_iteracao/Num_Repeticoes;
		//media_fi       = media_fi/Num_Repeticoes;
		media_tempo    = media_tempo/Num_Repeticoes;
		
		/*
        fprintf(arq_saida, "------------------------------------------------------------------------------------\n");
		fprintf(arq_saida, "%s (n=%i) <> Iteracao: %i <> CustoBest: [%d|%d|%d] \n", 
                            instancia, n, media_iteracao, max_valor, media_valor, min_valor );
		fprintf(arq_saida, "************************************************************************************\n");
		fprintf(arq_saida, "************************************************************************************\n");
		*/
  		
		/***
		for(x=0; x<256; x++) comando[x] = 0;		
		sprintf(comando, "mkdir %s-gsp\n", instancia);
		system(comando);
		sprintf(comando, "mv *.gsp %s-gsp\n", instancia);
		system(comando);
		sprintf(comando, "tar -cf %s.tar %s-gsp\n", instancia, instancia);
		system(comando);
		***/
		
		fclose(arq_saida);		

	}


	free(solucoes);

	/* Liberacao de memoria referente a todos os vetores definidos no programa */
	free(Fi);
	free(FiBest);
	free(FiMelhor);
	free(FiMelhorAnterior);
  	free(Dist);
  	free(Fluxo);
  	free(D_ord);
  	free(F_ord);
	free(Produto);

	return 0;

} /*-----------------------fim do programa principal ----------------------------*/


/**********************************************************************************
                              Funcoes auxiliares 
**********************************************************************************/


/*************Leitura dos dados de um arquivo de entrada**************************/
void LeGrasp(char *instancia)
{
  FILE* pqa;
  int i,j,r,lixo; 

  pqa = fopen(instancia, "r");   
 
  /* Le numero de nos do problema */
  fscanf(pqa, "%d\n", &n);

  /* Aloca as Matrizes: */
  Mat_Dist  = (int **) malloc (n * sizeof(int *));
  Mat_Fluxo = (int **) malloc (n * sizeof(int *));
  for(i=0; i<n; i++)
  {
    Mat_Dist[i]  = (int *) malloc (n * sizeof(int));
    Mat_Fluxo[i] = (int *) malloc (n * sizeof(int));
  }
  //printf("@ Matrizes alocadas\n");
  /* Calcula numero de elementos da diagonal superior das matrizes simetricas
     de  fluxo e distancia */
  N = n * (n - 1) / 2;

  /* Aloca espaco de memoria para os vetores Fi, Dist, Fluxo, D_ord e F_ord */ 
  Fi = Alocai(n + 1);
  Dist  = Alocai(N + 1);
  Fluxo = Alocai(N + 1);
  D_ord = Alocap(N + 1);
  F_ord = Alocap(N + 1);

  /* Le matriz de fluxos */
  for(i = 1; i <= n; i++)
  {
	  for(j = 1; j <= n; j++)
	  {
		//if (i < j)
		{
			//r = Psi(i,j);
			fscanf(pqa, "%d ", &Mat_Fluxo[i-1][j-1]);
			//printf("%2d ", Mat_Fluxo[i-1][j-1]);
			//F_ord[r].valor = Fluxo[r];
			//F_ord[r].posicao = r;
		}
		//else fscanf(pqa, "%d ", &lixo);
	  }
	  fscanf(pqa, "\n");
	  //printf("\n");
  }

  /* Le a matriz de distancias */
  for(i = 1; i <= n; i++)
  {
	  for(j = 1; j <= n; j++)
	  {
		//if (i < j) 
		{
			//r = Psi(i,j);
			fscanf(pqa, "%d ", &Mat_Dist[i-1][j-1]);
			//printf("%2d ", Mat_Dist[i-1][j-1]);
			//D_ord[r].valor = -Dist[r];
			//D_ord[r].posicao = r;
		}
		//else fscanf(pqa, "%d ", &lixo);
	  }
	  fscanf(pqa, "\n");
	  //printf("\n");
  }

  fclose(pqa); 
 
}

/*********************Alocacao dinamica de memoria do tipo inteiro*****************/
int* Alocai(int dimensao)
{
    int *vetor; 

	if( (vetor = (int *) malloc( dimensao * sizeof(int) )) == NULL )
	{
		printf("Nao ha espaco de memoria para aresta\n");
		exit(0);
	}
	return(vetor);
}


/*********************Alocacao dinamica de memoria do tipo par**********************/
par* Alocap(int dimensao)
{
	par *vetor; 

	if( (vetor = (par *) malloc( dimensao * sizeof(par) )) == NULL )
	{
		printf("Nao ha espaco de memoria para aresta\n");
		exit(0);
	}
	return(vetor);
}

/*********************Alocacao dinamica de memoria do tipo auxiliar**********************/
auxi* Aloca_aux(int dimensao)
{
	auxi *vetor; 

	if( (vetor = (auxi *) malloc( dimensao * sizeof(auxi) )) == NULL )
	{
		printf("Nao ha espaco de memoria para aresta\n");
		exit(0);
	}
	return(vetor);
}

/*********************Alocacao dinamica de memoria do tipo tripla*******************/
tripla* Alocat(int dimensao)
{
	tripla *vetor; 

	if( (vetor = (tripla *) malloc( dimensao * sizeof(tripla) )) == NULL )
	{
		printf("Nao ha espaco de memoria para aresta\n");
		exit(0);
	}
	return(vetor);
}

/**********************************************************************************
                       Funcoes para calculo do vetor Produto 
**********************************************************************************/

/**********************Calculo dos elementos do vetor FD***************************/
CustoFD()
{
	int i, max; 

	printf("\tCustoFD() - Ordenap(F_ord, N, 0);\n\n"); Ordenap(F_ord, N, 0); 
	printf("\tCustoFD() - Ordenap(D_ord, N, 0);\n\n"); Ordenap(D_ord, N, 0);


   /* Calculo do numero de elementos a serem considerados em Produto (RCL) */	
	max = (int) (beta * N);

   /* Alocacao de espaco para Produto */
	Produto = Alocat(max + 1);

    printf("\tCustoFD() - /* Determinacao dos valores de Produto */\n\n");
	for(i = 1; i <= max; i++)
	{
		Produto[i].val = F_ord[i].valor * (-1) * D_ord[i].valor;
		Produto[i].pos_fluxo = F_ord[i].posicao;
		Produto[i].pos_dist = D_ord[i].posicao; 
	}

	/* Ordena o vetor Produto */
	printf("\tCustoFD() - Ordenat(Produto,max);\n\n"); Ordenat(Produto,max);

	return(max);
	
}

/***********************Ordena um vetor de pares pelo metodo de bolha************* /
void Ordenap(par* vet, int fim, int control)
{
	int inicio, ult_troca,guarda,indice,houve_troca;
	par aux_troca;    

	/* inicializacao * /	
	inicio = 1;  
	
	/* Considerar todo o vetor para ordenacao * /
	ult_troca = fim;
	houve_troca = 1;

	/* Enquanto o vetor nao estiver ordenado * /
	while (houve_troca == 1) 
	{
		houve_troca = 0;
		printf("\t\tOrdenap - /* Enquanto o vetor nao estiver ordenado * /\n\n");
		/* Deslocar maior elemento para a posicao onde ocorreu a ultima troca * /
		for (indice = 1 ; indice < ult_troca ; indice++)
		{
			if (vet[indice].valor > vet[indice+1].valor)
			{
				aux_troca = vet[indice];
				vet[indice] = vet[indice + 1];
				vet[indice + 1] = aux_troca;

				/* guardar indice da troca * /
				guarda = indice;
				houve_troca = 1;
			}
		}
		
		/* atualizar ult_troca * /
		ult_troca = guarda;

		/* Controle da quantidade de elementos ordenados * /
		if (control == 1 && ult_troca <= (N-3))
		{
			houve_troca = 0;
		}
	}	

}  
*/

int separap (par* vet, int p, int r) {

  int c = vet[p].valor; /*Piv๔*/
  int i = p+1;
  int j = r;
  par t;

  while (i <= j) {
    if (vet[i].valor <= c) ++i;
    else if (c < vet[j].valor) --j; 
    else {
      t = vet[i]; 
      vet[i] = vet[j]; 
      vet[j] = t;
      ++i; --j;
      }
  }

  t = vet[p]; 
  vet[p] = vet[j]; 
  vet[j] = t;
  //mostra_vetor(v);
  return j; 
}

void Ordenap (par* vet, int p, int r) {
  int j;
  if (p < r) {
    j = separap (vet, p, r);
    Ordenap (vet, p, j-1); 
    Ordenap (vet, j+1, r);
  }
}


/***********************Ordena um vetor de triplas pelo metodo de bolha************* /
void Ordenat(tripla* vet, int fim)
{
	int inicio, ult_troca,guarda,indice,houve_troca;
	tripla aux_troca;    

	/* inicializacao * /	
	inicio = 1;
	
	/* considerar todo o vetor para ordenacao * /
	ult_troca = fim;
	houve_troca = 1;

	/* Enquanto o vetor nao estiver ordenado * /
	while (houve_troca == 1) 
	{
		houve_troca = 0;

		/* Deslocar maior elemento para a posicao oonde ocorreu a ultima troca * /

		for (indice = 1 ; indice < ult_troca ; indice++)
		{
			if (vet[indice].val > vet[indice+1].val)
			{
				aux_troca = vet[indice];
				vet[indice] = vet[indice + 1];
				vet[indice + 1] = aux_troca;

				/* guardar indice da troca * /
				guarda = indice;
				houve_troca = 1;
			}
		}
		
		/* atualizar ult_troca * /
		ult_troca = guarda;
	}	
		
}  
*/

int separat (tripla* vet, int p, int r) {

  int c = vet[p].val; /*Piv๔*/
  int i = p+1;
  int j = r;
  tripla t;

  while (i <= j) {
    if (vet[i].val <= c) ++i;
    else if (c < vet[j].val) --j; 
    else {
      t = vet[i]; 
      vet[i] = vet[j]; 
      vet[j] = t;
      ++i; --j;
      }
  }

  t = vet[p]; 
  vet[p] = vet[j]; 
  vet[j] = t;
  //mostra_vetor(v);
  return j; 
}

void Ordenat (tripla* vet, int p, int r) {
  int j;
  if (p < r) {
    j = separat (vet, p, r);
    Ordenat (vet, p, j-1); 
    Ordenat (vet, j+1, r);
  }
}

/**********************************************************************************
                       Fase de Construcao do GRASP  
**********************************************************************************/
Construcao(int dimensao)
{
	Fase1(dimensao);
	Fase2();
}

/***********************Etapa 1 da fase de construcao*****************************/
Fase1(int dimensao)
{
	int  p, maximo;
	float max;
	
	
	max = alfa * dimensao;
	
	if (alfa != 1.0)
		maximo = (int) max + 1;
	else maximo = (int) max;
 
	
	p = (randomico()%maximo) + 1;
	parf = Inversa_Psi(Produto[p].pos_fluxo);
	pard = Inversa_Psi(Produto[p].pos_dist);
 
}

/****Determinacao da par (l,s) relativo a posicao respectiva nos vetores D e F****/
int *Inversa_Psi(int k)
{
	int i,j;
	int  *aresta; 
	printf("\tInversa_Psi(%i)\n\n", k);
	aresta = Alocai(3);
	i = 0;
	do {
		i = i + 1;
		k = k - (n - i);
	   }
	while (k > 0);
	j = n + k;

	aresta[1] = i;
	aresta[2] = j;	 

	return(aresta);       
}

/***********************Determinacao de semente aleatoria*************************/
void semente()
{
//	seed = (rand()%500) + 1; //(long)(time(&seg))%100000; 

	int seg;
	seed = (int)(time(&seg))%100; 
}

/***********************Geracao de numero aleatorio*******************************/
int randomico()
{
	int seg;

	iseed = (1103515245L * iseed + 12345) & 0x7FFFFFFF;
	return((int) (iseed & 077777));
}

/***********************Etapa 2 da fase de construcao*****************************/
Fase2()
{
	int i,k,lim,
	    ind1,ind2,
	    indice,
	    m,l,
	    s,
	    c_aux_ik,
	    continuar, 
	    dimensao;
	
	float dim;
	auxi *aux;
	tripla *custo_aux;
	
	printf("\tFase2()\n\n");

	/* inicializacoes */
	aux = Aloca_aux(n + 1);
	custo_aux = Alocat(n * n + 1);

	for(i=1;i <= n; i++)
	{
		Fi[i] = 0;
		aux[i].pos_f = 0; 
		aux[i].pos_d = 0; 
	}
	 
	/* Fi recebe os dois primeiros pares de associacao de F e D */
	Fi[parf[1]] = pard[1];
	Fi[parf[2]] = pard[2];
	
	/*  identificacao em fi dos pares construidos na Fase1 e armazenados em aux[1] e aux[2] */
	ind1 = 1;
	for(ind2 = 1; ind2 < 3; ind2++)
	{
		printf("\t\tidentificacao em fi dos pares construidos na Fase1 - %i\n\n", ind2);
        while ((Fi[ind1] == 0) && (ind1 <= n))
			ind1++;
		aux[ind2].pos_f = ind1; 
		aux[ind2].pos_d = Fi[ind1]; 
		ind1++;
	}

	lim = 2;

 	for(indice=3; indice<=n; indice++)
	{   
		m = 0;
        printf("\t\t/* Verifica se (i,k) nao pertence a permutacao */ - %i\n\n", indice);
        
		for(i=1; i<=n; i++)
		{
			for(k=1; k<=n; k++)
			{
				/* Verifica se (i,k) nao pertence a permutacao */
				continuar = 1;
				ind1 = 1;
				while (((i != aux[ind1].pos_f) || (k != aux[ind1].pos_d)) && (ind1 <= lim))
				{
					ind1++;
				}
				
				if (ind1 <= lim)
				{
					continuar = 0;
				}

				if (continuar == 1)      /* (i,k) ainda nao pertence a Fi */
				{
					c_aux_ik = 0;
					l = 1;
					while (l <= lim)
					{
						if	((i != aux[l].pos_f) && (k != aux[l].pos_d))
						{
							ind1 = Psi(i,aux[l].pos_f);
							ind2 = Psi(k,aux[l].pos_d);
							c_aux_ik = c_aux_ik + (Fluxo[ind1] * Dist[ind2]);
							l = l + 1;
						}
						else 
						{
							continuar = 0;
							l = lim + 1;
						}
					}

					if (continuar == 1)
					{
						m = m + 1;
						custo_aux[m].val = c_aux_ik;
						custo_aux[m].pos_fluxo = i;
						custo_aux[m].pos_dist = k;		
					}
				}	 
				
			}    
	    }
	
		Ordenat(custo_aux,0,m);

		/*  geracao de um numero s entre 1 e alpha*m   */		
		dim = alfa * m;

		if (alfa != 1.0)
			dimensao = (int) dim + 1;
		else dimensao = (int) dim;

		s = (randomico()%dimensao) + 1;

		/* atualizacao da permutacao */	
		Fi[custo_aux[s].pos_fluxo] = custo_aux[s].pos_dist;  
 
		/* atualizacao do vetor auxiliar */
		aux[l].pos_f = custo_aux[s].pos_fluxo;              
		aux[l].pos_d = custo_aux[s].pos_dist; 

		lim = lim + 1;

	} /**********fim do for para construir a permutacao inicial*********/
	free(aux);
	free(custo_aux);
	
}


/**********************************************************************************
                                Busca Local 
**********************************************************************************/
Busca_Local(char nm_saida[256] , int repeticao, int iteracao, int maxiter)
{
	int  i=0, melhora=0, c=0, custo_final=0, cont=0,
	     ind1=0, ind2=0, aux=0, 
		 *FiAux=NULL, *Fi1=NULL;
	FILE *arq_busca_local=NULL;
	char numx[10], vetor[1000];
	int controle=0;

    memset(numx,  0,  10);	
	memset(vetor, 0,1000);
	
	Fi1 = Alocai(n + 1);
    FiAux = Alocai(n + 1);

    /* inicializacao */
    for(i = 1;i <= n;i++) 
    {
		FiMelhor[i] = Fi[i];
		FiMelhorAnterior[i] = Fi[i];
    }
 
    melhora = 1; 

    /* construcao da arvore de solucoes vizinhas */
    while(melhora == 1)
    { 	 
		controle++;
		printf("\tBusca_Local (%i/%i) - %i\n\n", iteracao, maxiter, controle);
		
        /* inicializacao para buscar a melhor solucao da vizinhanca */
        for(i = 1;i <= n;i++) 
		{
			FiMelhor[i] = FiMelhorAnterior[i];
			Fi1[i] = FiMelhor[i];
		} 
  
		mc = custo(FiMelhor); 
	
		for(ind1 = 1;ind1 <= n - 1;ind1++)
		{
			printf("\t\tBusca_Local (%i/%i) - %i / %i\n\n",iteracao, maxiter, controle,ind1);
			aux = FiMelhor[ind1];
			for(ind2 = ind1 + 1;ind2 <= n;ind2++)                     
			{                                                         
			    //printf("\t\t\tBusca_Local - %i / %i / %i\n\n",controle, ind1, ind2);
				Fi1[ind1] = FiMelhor[ind2];
				Fi1[ind2] = aux;
	
				c = custo(Fi1);
				if (c < mc)     /* comparacao custo */
				{					
					mc = c;
					for(i = 1;i <= n;i++) 
						FiAux[i] = Fi1[i];
				}
				/* restauracao da Fi1 */
				for(i = 1;i <= n;i++)  
					Fi1[i] = FiMelhor[i];  
			}                
		}
		if (FiAux[1] != 0)
			for(i = 1;i <= n;i++)  
				FiMelhor[i] = FiAux[i]; 
	

		custo_final = custo(FiMelhor);
		if(custo(FiMelhorAnterior) > custo_final)
		{
		  /**/
		}
		else  
		{
			melhora = 0; 
		}
        {  		
			/*
			printf("%3i-%3i: %s] = %5i\n", repeticao, iteracao, vetor, custo_final);
			if (iteracao<10)
			{				
				Grava_Vetor(nm_saida, repeticao, iteracao, vetor, custo_final);
			}
			*/
			//*
			
			strcpy(vetor,"[");
			for(i=1;i<=n;i++)
			{
				FiMelhorAnterior[i] = FiMelhor[i];
				sprintf(numx, "%3i,", FiMelhor[i]);
				strcat(vetor,numx);
			}
   			
			if (maxiter<10)
			{				
				Grava_Vetor(nm_saida, repeticao, iteracao, vetor, custo_final);
			}
			else if (maxiter<100)
			{
				if ((iteracao%10) == 0)
				{
					Grava_Vetor(nm_saida, repeticao, iteracao, vetor, custo_final);
				}
			}
			else if (maxiter<1000)
			{
				if ((iteracao%100) == 0)
				{
					Grava_Vetor(nm_saida, repeticao, iteracao, vetor, custo_final);
				}
			}
			else
			{
				if ((iteracao%1000) == 0)
				{
					Grava_Vetor(nm_saida, repeticao, iteracao, vetor, custo_final);
				}
			}
			//*/
		}
		//else  
		{
			//melhora = 0; 
		}		
	} 


   /* Liberacao da memoria dos vetores da Busca_Local */
   free(Fi1); 
   free(FiAux);
}


void Grava_Vetor(char nm_saida[256], int repeticao, int iteracao, char vetor[1000], int custo_final)
{	
	char nm_arq[256];
	FILE *arq_busca_local=NULL;

	memset(nm_arq,0, 256);
	sprintf(nm_arq, "%s-%03i-%03i.gsp", nm_saida, repeticao, iteracao);
	arq_busca_local=fopen(nm_arq, "a");		
	fprintf(arq_busca_local, "%s] = %5i\n", vetor, custo_final);
	fclose(arq_busca_local);
}


/*************Conversao de uma posicao de um vetor em N em um par em K_F ou K_D****/
int Psi(int i, int j)
{
	int Psi;

	if (i > j)  Psi = ((j - 1)* n) - (j * (j + 1) / 2) + i;
    else        Psi = ((i - 1)* n) - (i * (i + 1) / 2) + j;

	return(Psi);
}


Atualiza_Solucao(int* i, FILE *arq )
{
	int ind, c;
	/*clock_t*/ float tempo_otimo=0.0;

	
	c = custo(FiMelhor);
	if (CustoBest > c)
	{
		for(ind = 1;ind <= n;ind++)  
			FiBest[ind] = FiMelhor[ind];
		CustoBest = c;
		//times(&buffer);
		//tempo_otimo = buffer.tms_utime;

		/*
		solucao_atual = (TpSolucao *)malloc(sizeof(TpSolucao));
		solucao_atual->iteracao = *i;
		solucao_atual->valor    = CustoBest;
		solucao_atual->fi       = ci;
		solucao_atual->tempo    = (tempo_otimo-comeco)/60;
		solucao_viavel *prox    = NULL;

		*prox_solucao = &solucao_atual;
		 prox_solucao = solucao_atual->prox;
		LstSolucoes.Quantidade++;
		*/

		solucoes[rep].valor    = CustoBest;
		solucoes[rep].iteracao = *i;
		solucoes[rep].fi       = ci;
		//solucoes[rep].tempo    = (tempo_otimo-comeco)/60;

 		//fprintf(arq, "[%07.2f]Iteracao %d com CustoBest = %d e Fi inicial = %d\n", (tempo_otimo-comeco)/60,*i,CustoBest,ci);
		sprintf(melhor, "[%07.2f]Iteracao %d com CustoBest = %d e Fi inicial = %d\n", (tempo_otimo-comeco)/60,*i,CustoBest,ci);
		//sprintf(melhor, "[00]Iteracao %d com CustoBest = %d e Fi inicial = %d\n", *i,CustoBest,ci);
	}
		
}


/*******Calcula o numero de inversoes da permutacao relaxada RoGamaB***********/
int num_inv(int* RoGamaB)
{
	int num_inv,i,j;

	/* inicializacao */
	num_inv = 0;
 
	for(i = 1;i <= N - 1; i++)
	{
		for( j = i + 1;j <= N; j++)
		{
			if (RoGamaB[i] > RoGamaB[j])
			{
				num_inv++;
			}

		}
	}
	return(num_inv);
}

/*********************Calculo do custo de uma permutacao***********************/
int custo(int* Per)
{
	int custo,i,j,ii,jj;

	custo = 0;
	i = 1;
	for(ii = 1; ii <= n; ii++)
		for(jj = (ii + 1); jj <= n; jj++)
		{
			j = Psi(Per[ii],Per[jj]);
			custo = custo + (Fluxo[i] * Dist[j]);
			i++;
		}
	 
	return(custo);
}	
 
 
/*********************Impressao dos dados de um vetor**************************/
ImprimeDadosVetor(int* Vet, int dim, FILE *arq)
{
	int i;
	
	for(i = 1;i <= dim;i++)
		fprintf(arq, "%d ",Vet[i]);

	fprintf(arq, "\n\n");
}


void Ordena_Menos(int *vet, int tam, int ini, int fim)
{
    int i=0,j=0,k=0,e=0,d=0, *esq=NULL, *dir=NULL;
	
	if (tam==1) return;
	if (tam==2)
	{
		if( vet[ini] < vet[fim] )
		{			
			k = vet[ini];
			vet[ini] = vet[fim];
			vet[fim] = k;			
		}
		return;
	}

	i = tam/2;
	j = tam-i;

	esq = (int *)malloc(i*sizeof(int));
	dir = (int *)malloc(j*sizeof(int));

	for(k=0; k<i; k++) esq[k] = vet[k];
	for(k=0; k<j; k++) dir[k] = vet[k+i];

	Ordena_Menos(esq, i, 0, i-1);
	Ordena_Menos(dir, j, 0, j-1);

	e = i;
	d = j;

	i = j = k = 0;
	while (( i<e ) && ( j<d ))
	{
		if( esq[i] > dir[j] )
			vet[k] = esq[i++];
		else
			vet[k] = dir[j++];
		k++;
	}

	while( i<e ) vet[k++] = esq[i++];
	while( j<d ) vet[k++] = dir[j++];
	
	free(esq);
	free(dir);
}

void Ordena_Mais(int *vet, int tam, int ini, int fim)
{
    int i=0,j=0,k=0,e=0,d=0, *esq=NULL, *dir=NULL;
	
	if (tam==1) return;
	if (tam==2)
	{
		if( vet[ini] > vet[fim] )
		{			
			k = vet[ini];
			vet[ini] = vet[fim];
			vet[fim] = k;			
		}
		return;
	}

	i = tam/2;
	j = tam-i;

	esq = (int *)malloc(i*sizeof(int));
	dir = (int *)malloc(j*sizeof(int));

	for(k=0; k<i; k++) esq[k] = vet[k];
	for(k=0; k<j; k++) dir[k] = vet[k+i];

	Ordena_Mais(esq, i, 0, i-1);
	Ordena_Mais(dir, j, 0, j-1);

	e = i;
	d = j;

	i = j = k = 0;
	while (( i<e ) && ( j<d ))
	{
		if( esq[i] < dir[j] )
			vet[k] = esq[i++];
		else
			vet[k] = dir[j++];
		k++;
	}

	while( i<e ) vet[k++] = esq[i++];
	while( j<d ) vet[k++] = dir[j++];
	
	free(esq);
	free(dir);
}


int Consulta( int *novo, int tam, int item )
{
	// Declaracao de Variaveis:
	int i=0, k=0;

	// Inicio do Programa:
	k = -1;
	while (( i < tam ) && ( k == -1 ))
	{
		if (novo[i] == item)
		{
			novo[i] = -1;
			k = i;
		}
		i++;
	}
	
	return k;
}

int Ver_Troca ( int *troca, int tam, int posicao )
{
    // Declaracao de Variaveis:
    int i=0, k=0;
    
    // Inicio do Programa:
    k = -1;
    while (( i<tam ) && ( k == -1 ))
    {
		if ( troca[i] == posicao )
        {
			k = troca[i];
        }
        i++;
    }
    
	return k;
}

void Ordena_Matrizes( int ident )
{
    int i=0, j=0, k=0, x=0, y=0, dN=0;
	int *troca=NULL,
		*vetx=NULL,
		*ordx=NULL,
		*vet_ident=NULL;
	int **matriz_troca_lin = NULL,
        **matriz_troca_col = NULL;
	
    matriz_troca_lin = (int **) malloc( n * sizeof(int *)); 
    matriz_troca_col = (int **) malloc( n * sizeof(int *));
    
    for ( i=0; i<n; i++ )
    {
        matriz_troca_lin[i] = (int *) malloc( n * sizeof(int *));
        matriz_troca_col[i] = (int *) malloc( n * sizeof(int *));        
        for ( j=0; j<n; j++ )
        {
            matriz_troca_lin[i][j] = 0;
            matriz_troca_col[i][j] = 0;
        }
    }
	
	dN = n-1;
    troca = (int *) malloc( dN * sizeof(int));
	for(x=0; x<n-2; x++)
	{
		vetx = (int *) malloc( dN * sizeof(int));
		ordx = (int *) malloc( dN * sizeof(int));
		
		y = x+1;
		if ( ident == 0 )
		{
		    vet_ident = Mat_Fluxo[x];
		    for( i=0; i<dN; i++ )
			{			
				k = vet_ident[i+y];
				vetx[i] = ordx[i] = k; 
			}
			Ordena_Menos( ordx, dN, 0, dN-1 );
	        
	        for ( i=0; i<dN; i++ )
	        {
				k = Consulta(vetx,dN,ordx[i]);
				troca[i] = k;
	        }

	        for ( i=0; i<n; i++ )
	        {            
				for ( j=0; j<n; j++ )
	            {            
	                matriz_troca_col[i][j] = Mat_Fluxo[i][j];
					matriz_troca_lin[i][j] = Mat_Fluxo[i][j];
	            }			
				for ( j=0; j<dN; j++ )
	            {            
					k = troca[j]+y;
	                matriz_troca_col[i][j+y] = Mat_Fluxo[i][k];
					matriz_troca_lin[i][j+y] = Mat_Fluxo[i][k];
	            }			
	        }

	        for ( i=0; i<dN; i++ )
	        {
				k = troca[i]+y;
	            for ( j=0; j<n; j++ )
	            {            
	                matriz_troca_lin[i+y][j] = matriz_troca_col[k][j];
	            }
	        }

	        for ( i=0; i<n; i++ )
	        {
	            for ( j=i; j<n; j++ )
	            {            
					Mat_Fluxo[i][j] = matriz_troca_lin[i][j];
					Mat_Fluxo[j][i] = matriz_troca_lin[j][i];
	            }
	        }
		}
		else
		{
		    vet_ident = Mat_Dist[x];
		    for( i=0; i<dN; i++ )
			{			
				k = vet_ident[i+y];
				vetx[i] = ordx[i] = k; 
			}
			Ordena_Mais( ordx, dN, 0, dN-1 );

	        for ( i=0; i<dN; i++ )
	        {
				k = Consulta(vetx,dN,ordx[i]);
				troca[i] = k;
	        }

	        for ( i=0; i<n; i++ )
	        {            
				for ( j=0; j<n; j++ )
	            {            
					matriz_troca_col[i][j] = Mat_Dist[i][j];
					matriz_troca_lin[i][j] = Mat_Dist[i][j];
	            }			
				for ( j=0; j<dN; j++ )
	            {            
					k = troca[j]+y;
					matriz_troca_col[i][j+y] = Mat_Dist[i][k];
					matriz_troca_lin[i][j+y] = Mat_Dist[i][k];
	            }			
	        }

	        for ( i=0; i<dN; i++ )
	        {
				k = troca[i]+y;
	            for ( j=0; j<n; j++ )
	            {            
	                matriz_troca_lin[i+y][j] = matriz_troca_col[k][j];
	            }
	        }

	        for ( i=0; i<n; i++ )
	        {
	            for ( j=i; j<n; j++ )
	            {            
					Mat_Dist[i][j] = matriz_troca_lin[i][j];
					Mat_Dist[j][i] = matriz_troca_lin[j][i];
	            }
	        }
		}

        dN = dN-1;
        
    }

    free(vetx);
    free(ordx);
    free(troca);
    free(matriz_troca_col);
    free(matriz_troca_lin);    

}


