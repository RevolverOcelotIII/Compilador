#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Funcaux.h"

int contador_linhas = 1;
int contador_linhas_duplas = 0;

void PrintNodo(char info[], int movim) {
  return;
  if (movim == AVANCA) {
    printf("%s%s\n", TABS, info);
    strcat(TABS, "\t");
  } else if (movim == MANTEM) {
    printf("%s%s\n", TABS, info);
  } else if (movim == RETROCEDE) {
    TABS[strlen(TABS) - 1] = '\0';
  }
}

void PrintNodoInt(int val, int movim) {
  return;
  if (movim == AVANCA) {
    printf("%s%d\n", TABS, val);
    strcat(TABS, "\t");
  } else if (movim == MANTEM) {
    printf("%s%d\n", TABS, val);
  } else if (movim == RETROCEDE) {
    TABS[strlen(TABS) - 1] = '\0';
  }
}
void PrintNodoFloat(float val, int movim) {
  return;
  if (movim == AVANCA) {
    printf("%s%f\n", TABS, val);
    strcat(TABS, "\t");
  } else if (movim == MANTEM) {
    printf("%s%f\n", TABS, val);
  } else if (movim == RETROCEDE) {
    TABS[strlen(TABS) - 1] = '\0';
  }
}
void PrintNodoChar(char val, int movim) {
  return;
  if (movim == AVANCA) {
    printf("%s%c\n", TABS, val);
    strcat(TABS, "\t");
  } else if (movim == MANTEM) {
    printf("%s%c\n", TABS, val);
  } else if (movim == RETROCEDE) {
    TABS[strlen(TABS) - 1] = '\0';
  }
}

void incrementaLinha () {
  contador_linhas_duplas++;
  if(contador_linhas_duplas < 2) {
    contador_linhas ++;
  } else contador_linhas_duplas = 0;
}

void zeraLinhasDuplas () {
  contador_linhas_duplas = 0;
}

void zeraLinhas() {
  contador_linhas = 0;
}

void erro (char val[]) {
  printf("Erro na linha %d: %s\n", contador_linhas, val);
  exit(0);
}
