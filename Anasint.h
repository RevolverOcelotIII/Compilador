#ifndef _ANASINT_
#define _ANASINT_

#include "Analex.h"
#include <stdio.h>

/* Variaveis globais */
extern TOKEN t;
extern TOKEN lookahead;
extern FILE *fd;
extern FILE *codigo_mp;

extern char TABS[500];

extern int contador_linhas;

/* Assinaturas das funcoes */
void Prog();
void ObjDef();
void DataSec();
void MethSec();
void DeclListVar();
void Decl();
void DeclVar();
void Tipo();
void TiposParam();
void Func();
void FuncProt();
void Escopo();
void Cmd();
void Atrib();
int Expr();
int ExprSimp();
int Termo();
int Fator();

enum BOOL verificaOpRel();
enum BOOL verificaTipo();

#endif // ANASINT