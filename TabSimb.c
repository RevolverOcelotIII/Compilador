#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "TabSimb.h"
#include "Funcaux.h"

// Inicializa a tabela de símbolos
void IniciarTabela() { tabela_idef.tamTab = 0; }

char *print_escopo[] = {"N/A", "local", "global"};
char *print_tipo[] = {"N/A", "int", "real", "char", "bool"};
char *print_categoria[] = {"N/A",     "procedimento", "parametro",
                           "vGlobal", "vLocal",       "prototipo"};
char *print_passagem[] = {"N/A", "referencia", "valor"};
char *print_array[] = {"N/A", "vetor", "matriz"};

char *ImprimirValConst(TabSimb ultimoSimbolo) {
  char valConst[50];
  switch (ultimoSimbolo.tipo) {
  case T_INT:
    snprintf(valConst, sizeof(valConst), "%d", ultimoSimbolo.val_int);
  case T_CHAR:
    snprintf(valConst, sizeof(valConst), "%c", ultimoSimbolo.val_char);
  case T_REAL:
    snprintf(valConst, sizeof(valConst), "%f", ultimoSimbolo.val_real);
  default:
    return strcpy(valConst, "");
  }
  printf("%s", valConst);
  return valConst;
}

// Imprime a tabela de símbolos
void ImprimirTabela() {
  printf("\nTabela de Símbolos:\n");
  printf("---------------------------------------------------------------------"
         "---------------------------------------------------------------\n");
  printf("| %-2s | %-10s | %-6s | %-8s | %-6s | %-8s | %-8s | %-10s | %-5s | "
         "%-5s | %-8s | %-8s | %-6s | %-8s |\n",
         "Índice", "Lexema", "Tipo", "Escopo", "Categoria", "Passagem", "Array",
         "Dim1", "Dim2", "Const", "Val_Const", "Zombie", "Endereço", "Rótulo");
  printf("---------------------------------------------------------------------"
         "---------------------------------------------------------------\n");
  for (int i = 0; i <= tabela_idef.tamTab; i++) {
    printf(
        "| %-2d | %-10s | %-6s | %-8s | %-6s | %-8s | %-8s | %-10d | %-5d | "
        "%-5s | %-8s | %-8s | %-6d | %-8d |\n",
        i, tabela_idef.tabela_simb[i].lexema,
        print_tipo[tabela_idef.tabela_simb[i].tipo], // Tipo (int, real, char,
                                                     // bool)
        print_escopo[tabela_idef.tabela_simb[i].escopo], // Escopo (local,
                                                         // global)
        print_categoria[tabela_idef.tabela_simb[i]
                            .categoria], // Categoria (procedimento, parametro,
                                         // etc.)
        print_passagem[tabela_idef.tabela_simb[i]
                           .passagem], // Passagem (referência, valor)
        print_array[tabela_idef.tabela_simb[i].array], // Array (vetor, matriz)
        tabela_idef.tabela_simb[i].dim1,               // Dimensão 1
        tabela_idef.tabela_simb[i].dim2,               // Dimensão 2
        tabela_idef.tabela_simb[i].is_const ? "Sim"
                                            : "Não",  // Constante (Sim/Não)
        ImprimirValConst(tabela_idef.tabela_simb[i]), // Valor da constante
        tabela_idef.tabela_simb[i].zombie ? "Sim" : "Não", // Zombie (Sim/Não)
        tabela_idef.tabela_simb[i].endereco,               // Endereço
        tabela_idef.tabela_simb[i].rotulo);                // Rótulo
  }
  printf("---------------------------------------------------------------------"
         "---------------------------------------------------------------------"
         "-------------------------\n");
}

void ImprimirSimbolo(TabSimb simbolo) {
  printf("---------------------------------------------------------------------"
         "---------------------------------------------------------------\n");
  printf("| %-2s | %-10s | %-6s | %-8s | %-6s | %-8s | %-8s | %-10s | %-5s | "
         "%-5s | %-8s | %-8s | %-6s | %-8s |\n",
         "Índice", "Lexema", "Tipo", "Escopo", "Categoria", "Passagem", "Array",
         "Dim1", "Dim2", "Const", "Val_Const", "Zombie", "Endereço", "Rótulo");
  printf("---------------------------------------------------------------------"
         "---------------------------------------------------------------\n");
  printf("| %-10s | %-6s | %-8s | %-6s | %-8s | %-8s | %-10d | %-5d | %-5s | "
         "%-8s | %-8s | %-6d | %-8d |\n",
         simbolo.lexema,
         print_tipo[simbolo.tipo],           // Tipo (int, real, char, bool)
         print_escopo[simbolo.escopo],       // Escopo (local, global)
         print_categoria[simbolo.categoria], // Categoria (procedimento,
                                             // parametro, etc.)
         print_passagem[simbolo.passagem],   // Passagem (referência, valor)
         print_array[simbolo.array],         // Array (vetor, matriz)
         simbolo.dim1,                       // Dimensão 1
         simbolo.dim2,                       // Dimensão 2
         simbolo.is_const ? "Sim" : "Não",   // Constante (Sim/Não)
         ImprimirValConst(simbolo),          // Valor da constante
         simbolo.zombie ? "Sim" : "Não",     // Zombie (Sim/Não)
         simbolo.endereco,                   // Endereço
         simbolo.rotulo);                    // Rótulo
}

// Busca um lexema na tabela de símbolos
int BuscarTabela(char lexema[]) {
  for (int i = tabela_idef.tamTab -1; i >= 0; i--) {
    if (strcmp(tabela_idef.tabela_simb[i].lexema, lexema) == 0) {
      return i; // Retorna o índice se encontrar o lexema
    }
  }
  return -1; // Retorna -1 se não encontrar o lexema
}

// Busca um lexema em um escopo específico
int BuscarEscopo(char lexema[], int escopo) {
  for (int i = 0; i < tabela_idef.tamTab; i++) {
    if (strcmp(tabela_idef.tabela_simb[i].lexema, lexema) == 0 &&
        tabela_idef.tabela_simb[i].escopo == escopo) {
      return i;
    }
  }
  return -1;
}

int VerificarDistanciaParametro(int indiceParametro) {
  int distancia = 0;
  for (int i = indiceParametro; i >=0; i--) {
    if (tabela_idef.tabela_simb[i].categoria != PROCEDIMENTO) {
      distancia++;
    } else return distancia;
  }
  return -1;
}

int VerificarContagemParametros(int indiceFuncao) {
  int cont_parametros = 0;
  for (int i = indiceFuncao + 1; i < tabela_idef.tamTab; i++) {
    if (tabela_idef.tabela_simb[i].categoria == PARAMETRO) {
      cont_parametros++;
    } else return cont_parametros;
  }
  return -1;
}

int VerificarSimboloRepetido(TabSimb ultimoSimbolo) {
  if (ultimoSimbolo.lexema[0] == '\0')
    return 0;
  for (int i = 0; i < tabela_idef.tamTab; i++) {
    if (strcmp(tabela_idef.tabela_simb[i].lexema, ultimoSimbolo.lexema) == 0) {
      if (tabela_idef.tabela_simb[i].escopo == ultimoSimbolo.escopo &&
          !tabela_idef.tabela_simb[i].zombie)
        return 1;
    }
  }
  return 0;
}

// Insere um novo símbolo na tabela de símbolos e testa se já foi declarado
int InserirTabela(TabSimb ultimoSimbolo) {
  if (tabela_idef.tamTab >= TAM_MAX_TAB) {
    printf("[ERRO] Tabela de símbolos cheia.\n");
    return -1;
  }

  if (VerificarSimboloRepetido(ultimoSimbolo)) {
    char mensagem_erro[60];
    snprintf(mensagem_erro, sizeof(mensagem_erro),
             "Erro: Identificador '%s' já declarado no escopo atual.",
             ultimoSimbolo.lexema);
    erro(mensagem_erro);
  }
  // ImprimirSimbolo(ultimoSimbolo);

  ultimoSimbolo.endereco = tabela_idef.tamTab;
  // Insere o novo símbolo na tabela e incrementa o contador
  tabela_idef.tabela_simb[tabela_idef.tamTab++] = ultimoSimbolo;

  return 0; // Sucesso
}

void EditarLinhaTabela(int i, TabSimb simbolo) {
  if (sizeof(simbolo.lexema) > 0)
    strcpy(tabela_idef.tabela_simb[i].lexema, simbolo.lexema);
  tabela_idef.tabela_simb[i].zombie = simbolo.zombie;
  if (simbolo.categoria)
    tabela_idef.tabela_simb[i].categoria = simbolo.categoria;
}

// Remove o último símbolo inserido na tabela de símbolos
int Remover_ultimo() {
  if (tabela_idef.tamTab > 0) {
    tabela_idef.tamTab--;
    return 0;
  } else {
    printf("[ERRO] Tabela de símbolos vazia.\n");
    return -1;
  }
}

void RemoverSimbolosProc() {
  for (int i = tabela_idef.tamTab; i >= 0; i--) {
    if (tabela_idef.tabela_simb[i].categoria == V_LOCAL) {
      if (tabela_idef.tamTab > 0)
        tabela_idef.tamTab--;
    }
    if (tabela_idef.tabela_simb[i].categoria == PARAMETRO) {
      tabela_idef.tabela_simb[i].zombie = TRUE;
    }
    if (tabela_idef.tabela_simb[i].categoria == PROCEDIMENTO)
      return;
  }
}