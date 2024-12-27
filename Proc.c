#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Analex.h"
#include "Funcaux.h"
#include "TabSimb.h"

/* Variaveis globais */

TOKEN t;
TOKEN lookahead;
FILE *fd;
enum BOOL mostraArvore = TRUE;
char TABS[500] = "";
char tabela_literais [TAM_TABELA_LITERAIS][TAM_MAX_LEXEMA];


char *palavras_reservadas[] = {
"const", "pr", "init", "endp", "def", "prot", "char", "int", "real", 
"bool", "do", "while", "endw", "var", "from", "to",
"dt", "by", "if", "endv", "elif", "else", "endi",
"getout", "getint", "getchar", "getreal", "putint", "putchar", "putreal", "putstr"};

int tamanho_pal_reservadas = 31;

void TestaAnalex() {
  FILE *fd;
  TOKEN token;
  if ((fd = fopen("expressao.dat", "r")) == NULL)
    erro("Arquivo de entrada da expressao nao encontrado!");

  while (1) {
    token = AnaLex(fd);
    switch (token.cat) {
    case ID:
      printf("<ID, %s> ", token.lexema);
      break;

    case SN:
      switch (token.codigo) {
      case ADICAO:
        printf("<SN, ADICAO> ");
        break;
      case SUBTRACAO:
        printf("<SN, SUBTRACAO> ");
        break;
      case MULTIPLICACAO:
        printf("<SN, MULTIPLICACAO> ");
        break;
      case DIVISAO:
        printf("<SN, DIVISAO> ");
        break;
      case MAIOR:
        printf("<SN, MAIOR> ");
        break;
      case MAIOR_IGUAL:
        printf("<SN, MAIOR IGUAL> ");
        break;
      case MENOR:
        printf("<SN, MENOR> ");
        break;
      case MENOR_IGUAL:
        printf("<SN, MENOR IGUAL> ");
        break;
      case COMPARACAO:
        printf("<SN, COMPARACAO> ");
        break;
      case ATRIBUICAO:
        printf("<SN, ATRIBUICAO> ");
        break;
      case A_PARENTESE:
        printf("<SN, A PARENTESE> ");
        break;
      case F_PARENTESE:
        printf("<SN, F PARENTESE> ");
        break;
      case MODIFICADOR:
        printf("<SN, MODIFICADOR> ");
        break;
      case A_COLCHETE:
        printf("<SN, A COLCHETE> ");
        break;
      case F_COLCHETE:
        printf("<SN, F COLCHETE> ");
        break;
      case OU:
        printf("<SN, OU> ");
        break;
      case IDENTIF_PONTEIRO:
        printf("<SN, IDENTIF PONTEIRO> ");
        break;
      case AND:
        printf("<SN, AND> ");
        break;
      case NAO:
        printf("<SN, NAO> ");
        break;
      case NAO_IGUAL:
        printf("<SN, NAO IGUAL> ");
        break;
      case APOSTOFO:
        printf("<SN, APOSTOFO> ");
        break;
      case ASPAS_D:
        printf("<SN, ASPAS D> ");
        break;
      case VIRGULA:
        printf("<SN, VIRGULA> ");
        break;
      case ENDERECO:
        printf("<SN, ENDERECO> ");
        break;
      default:
        printf("<SN, DESCONHECIDO> ");
      }
      break;

    case CT_I:
      printf("<CT_I, %d> ", token.valInt);
      break;
    case CT_R:
      printf("<CT_R, %f> ", token.valReal);
      break;
    case CT_C:
      // printf("%d", token.codigo);
      if (token.codigo < 0)
        printf("<CT_C, %c> ", token.caracter);
      else
        printf("<CT_C, %d> ", token.codigo);
      break;
    case LT:
      printf("<LT, %s> ", tabela_literais[token.codigo]);
      break;
    case PAL_R:
      printf("<PAL_R, %s> ", palavras_reservadas[token.codigo-1]);
      break;
    case FIM_ARQ:
      printf("\nFim da expressao encontrado.\n");
    }

    if (token.cat == FIM_ARQ)
      break;
  }
  fclose(fd);
}

void TestaAnasint() {
  if ((fd = fopen("expressao.dat", "r")) == NULL)
    erro("Arquivo de entrada da expressao nao encontrado!");

  Prog();
  
  if (t.cat == FIM_ARQ){
    printf("Resultado> Expressao OK!\n");
  }
  fclose(fd);
}

int main() {
  // printf("\n[Analise Lexica -------------------]\n");
  // TestaAnalex();
  printf("\n[Analise Sintatica ----------------]\n");
  TestaAnasint();
  printf("\n[Tabela de Simbolos ----------------]\n");
  ImprimirTabela();
}