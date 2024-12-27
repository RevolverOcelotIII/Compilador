#ifndef _FUNC_AUX_
#define _FUNC_AUX_

enum MOVIM { AVANCA, MANTEM, RETROCEDE };

extern char TABS[500];

void erro(char msg[]);
void PrintNodo(char info[], int movim);
void PrintNodoInt(int val, int movim);
void PrintNodoFloat(float val, int movim);
void PrintNodoChar(char val, int movim);
void incrementaLinha();
void zeraLinhas();

#endif