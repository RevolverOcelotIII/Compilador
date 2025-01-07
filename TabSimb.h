#ifndef TABSIMB
#define TABSIMB

#include <stdio.h>
#include <stdlib.h>
#include "Analex.h"
#include <stdbool.h>

#define TAM_MAX_TAB 100

// Definição da estrutura TabSimb
typedef struct
{
    char lexema[TAM_MAX_LEXEMA];
    int tipo;
    int escopo;
    int categoria;
    int passagem;
    int array;
    int dim1;
    int dim2;
    int is_const;
    bool zombie;
    int endereco;
    int rotulo;
    union {      
        int val_int;
        float val_real;
        int val_bool;              
        char val_char;
    };
} TabSimb;

// Definição da estrutura TabIdef
typedef struct
{
    int tamTab;
    TabSimb tabela_simb[TAM_MAX_TAB];
} TabIdef;

enum ESCOPO { GLOBAL = 1, LOCAL };
enum TIPO { T_INT = 1, T_REAL, T_CHAR, T_BOOL };
enum CATEGORIA { PROCEDIMENTO = 1, PARAMETRO, V_GLOBAL, V_LOCAL, PROTOTIPO };
enum PASSAGEM { REFERENCIA = 1, VALOR };
enum ARRAY { VETOR = 1, MATRIZ };

// Declaração de variáveis externas
extern TabIdef tabela_idef;
extern TOKEN tk;
extern int escopoAtual;

// Declaração de funções
void IniciarTabela();
void ImprimirTabela();
void EditarLinhaTabela(int i, TabSimb simbolo);
int BuscarTabela(char lexema[]);
int BuscarEscopo(char lexema[], int escopo);
int InserirTabela(TabSimb ultimoSimbolo);
int Remover_ultimo();
void RemoverSimbolosProc();
int VerificarDistanciaParametro(int indiceParametro);
int VerificarContagemParametros(int indiceFuncao);

#endif