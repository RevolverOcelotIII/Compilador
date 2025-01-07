#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Analex.h"
#include "Funcaux.h"

#define TAM_LEXEMA 50
#define TAM_NUM 20

TOKEN AnaLex(FILE *fd) {
  int estado = 0;
  char lexema[TAM_LEXEMA] = "";
  int tamanho_lexema = 0;
  int tamanho_literais = 0;
  char digitos[TAM_NUM] = "";
  int tamanho_digito = 0;
  TOKEN token;
  token.processado = TRUE;

  while (1) {
    char c = fgetc(fd);
    // controle de linhas
    if (c == '\n') {
      incrementaLinha();
    } else zeraLinhasDuplas();

    switch (estado) {
    case 0:
      if (c == ' ' || c == '\t' || c == '\n')
        estado = 0;
      else if (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        estado = 1;
        lexema[tamanho_lexema] = c;
        lexema[++tamanho_lexema] = '\0';
      }

      else if (c >= '0' &&
               c <= '9') { // inicio de constante inteira - inicializa digitos
        estado = 10;
        digitos[tamanho_digito] = c;
        digitos[++tamanho_digito] = '\0';
      }

      else if (c == '+') {
        token.cat = SN;
        token.codigo = ADICAO;
        return token;
      }

      else if (c == '-') {
        token.cat = SN;
        token.codigo = SUBTRACAO;
        return token;
      }

      else if (c == '*') {
        token.cat = SN;
        token.codigo = MULTIPLICACAO;
        return token;
      }

      else if (c == '/') {
        estado = 13;
      }

      else if (c == '=') {
        estado = 5;
      }

      else if (c == '(') {
        token.cat = SN;
        token.codigo = A_PARENTESE;
        return token;
      }

      else if (c == ')') { // sinal de fecha parenteses - monta e devolve token
        token.cat = SN;
        token.codigo = F_PARENTESE;
        return token;
      }

      else if (c == '>') {
        estado = 6;
      }

      else if (c == '<') {
        estado = 7;
      }

      else if (c == '[') {
        token.cat = SN;
        token.codigo = A_COLCHETE;
        return token;
      }

      else if (c == ']') {
        token.cat = SN;
        token.codigo = F_COLCHETE;
        return token;
      }

      else if (c == '&') {
        estado = 8;
      }

      else if (c == '!') {
        estado = 9;
      }

      else if (c == '|') {
        estado = 12;
      }

      else if (c == '\'') {
        estado = 2;
      }

      else if (c == '"') {
        estado = 4;
      }

      else if (c == ',') {
        token.cat = SN;
        token.codigo = VIRGULA;
        return token;
      } else if (c == '{') {
        token.cat = SN;
        token.codigo = A_CHAVES;
        return token;
      }

      else if (c == '}') {
        token.cat = SN;
        token.codigo = F_CHAVES;
        return token;
      }

      else if (c == EOF) { // fim da expressao encontrado
        token.cat = FIM_ARQ;
        return token;
      }

      else
        erro("Caracter invalido na expressao!"); // sem transicao valida no AFD
      break;

    case 1:
      if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9') || (c == '_')) {
        estado = 1;
        lexema[tamanho_lexema] = c; // acumula caracteres lidos em lexema
        lexema[++tamanho_lexema] = '\0';
      }

      else { // transicao OUTRO* do estado 1 do AFD
        ungetc(c, fd);
        // Tratamento de palavras reservadas
        for (int i = 0; i < tamanho_pal_reservadas; i++) {
          if (strcmp(palavras_reservadas[i], lexema) == 0) {
            token.cat = PAL_R;
            token.codigo = i + 1;
            return token;
          }
        }
        // caso nao sejam encontradas palavras reservadas
        token.cat = ID;
        strcpy(token.lexema, lexema);
        return token;
      }
      break;
    case 2:
      if (isprint(c) && c != '\'' && c != '\\') {
        estado = 3;
        lexema[tamanho_lexema] = c;
        lexema[++tamanho_lexema] = '\0';
      }
      if (c == '\\') {
        estado = 15;
        lexema[tamanho_lexema] = c;
        lexema[++tamanho_lexema] = '\0';
      }
      if (!isprint(c))
        erro("caracter invalido na expressao!");
      break;

    case 3:
      if (c == '\'') {
        token.cat = CT_C;
        token.codigo = -1;
        token.caracter = lexema[tamanho_lexema - 1];
        return token;
      } else
        erro("Caractere invalido na expressao!");
      break;

    case 4:
      if (isprint(c) && c != '"' && c != '\n') {
        lexema[tamanho_lexema] = c;
        lexema[++tamanho_lexema] = '\0';
      } else if (c == '"') {
        token.cat = LT;
        strcpy(tabela_literais[tamanho_literais], lexema);
        token.codigo = tamanho_literais;
        return token;
      } else
        erro("caracter invalido na expressao!");
      break;
    case 5:
      if (c == '=') {
        token.cat = SN;
        token.codigo = COMPARACAO;
        return token;
      } else {
        ungetc(c, fd);
        token.cat = SN;
        token.codigo = ATRIBUICAO;
        return token;
      }
      break;

    case 6:
      if (c == '=') {
        token.cat = SN;
        token.codigo = MAIOR_IGUAL;
        return token;
      } else {
        ungetc(c, fd);
        token.cat = SN;
        token.codigo = MAIOR;
        return token;
      }
      break;

    case 7:
      if (c == '=') {
        token.cat = SN;
        token.codigo = MENOR_IGUAL;
        return token;
      } else {
        ungetc(c, fd);
        token.cat = SN;
        token.codigo = MENOR;
        return token;
      }
      break;

    case 8:
      if (c == '&') {
        token.cat = SN;
        token.codigo = AND;
        return token;
      } else {
        ungetc(c, fd);
        token.cat = SN;
        token.codigo = ENDERECO;
        return token;
      }
      break;

    case 9:
      if (c == '=') {
        token.cat = SN;
        token.codigo = NAO_IGUAL;
        return token;
      } else {
        ungetc(c, fd);
        token.cat = SN;
        token.codigo = NAO;
        return token;
      }
      break;

    case 10:
      if (c >= '0' && c <= '9') {
        estado = 10;
        digitos[tamanho_digito] =
            c; // acumula digitos lidos na variavel digitos
        digitos[++tamanho_digito] = '\0';
      } else if (c == '.') {
        estado = 11;
        digitos[tamanho_digito] = c;
        digitos[++tamanho_digito] = '\0';
      } else {
        ungetc(c, fd);
        token.cat = CT_I;
        token.valInt = atoi(digitos);
        return token;
      }
      break;

    case 11:
      if (c >= '0' && c <= '9') {
        digitos[tamanho_digito] = c;
        digitos[++tamanho_digito] = '\0';
      } else {
        ungetc(c, fd);
        token.cat = CT_R;
        token.valReal = atof(digitos);
        return token;
      }
      break;

    case 12:
      if (c == '|') {
        token.cat = SN;
        token.codigo = OU;
        return token;
      } else {
        erro("caracter invalido na expressao!");
      }
      break;
    // Tratamento de comentario
    case 13:
      if (c == '/')
        estado = 14;
      else {
        ungetc(c, fd);
        token.cat = SN;
        token.codigo = DIVISAO;
        return token;
      }
      break;

    case 14:
      if (c == '\n')
        estado = 0;
      if (c == EOF) { // fim da expressao encontrado
        token.cat = FIM_ARQ;
        return token;
      }
      break;

    case 15:
      if (c == 'n') {
        estado = 16;
      } else if (c == '0') {
        estado = 17;
      } else
        erro("Caractere invalido na expressao!");
      break;

    case 16:
      if (c == '\'') {
        token.cat = CT_C;
        token.codigo = '\n';
        return token;
      }
      break;

    case 17:
      if (c == '\'') {
        token.cat = CT_C;
        token.codigo = '\0';
        return token;
      }
      break;
    }
  }
}
