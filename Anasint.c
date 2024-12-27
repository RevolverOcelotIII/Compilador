#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Anasint.h"
#include "Funcaux.h"
#include "TabSimb.h"

TabIdef tabela_idef;
TabSimb ultimoSimbolo;
int simboloBuscado = -1;

void debugToken() {
  switch (t.cat) {
  case ID:
    printf("<ID, %s> ", t.lexema);
    break;

  case SN:
    switch (t.codigo) {
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
    printf("<CT_I, %d> ", t.valInt);
    break;
  case CT_R:
    printf("<CT_R, %f> ", t.valReal);
    break;
  case CT_C:
    // printf("%d", t.codigo);
    if (t.codigo < 0)
      printf("<CT_C, %c> ", t.caracter);
    else
      printf("<CT_C, %d> ", t.codigo);
    break;
  case LT:
    printf("<LT, %s> ", tabela_literais[t.codigo]);
    break;
  case PAL_R:
    printf("<PAL_R, %s> ", palavras_reservadas[t.codigo - 1]);
    break;
  case FIM_ARQ:
    printf("\nFim da expressao encontrado.\n");
  }
}

int VerificaConst() {
  return t.cat == CT_I || t.cat == CT_R || t.cat == CT_C || t.cat == LT;
}

void AtribuirValorInt(int valor) { ultimoSimbolo.val_int = valor; }
void AtribuirValorReal(float valor) { ultimoSimbolo.val_real = valor; }
void AtribuirValorChar(char valor) { ultimoSimbolo.val_char = valor; }
void AtribuirValorBool(int valor) { ultimoSimbolo.val_bool = valor; }

void FlushSimbolo() {
  ultimoSimbolo.lexema[0] = '\0';
  ultimoSimbolo.array = 0;
  ultimoSimbolo.categoria = 0;
  ultimoSimbolo.dim1 = 0;
  ultimoSimbolo.dim2 = 0;
  ultimoSimbolo.is_const = 0;
  ultimoSimbolo.tipo = 0;
  ultimoSimbolo.endereco = 0;
  ultimoSimbolo.escopo = GLOBAL;
  ultimoSimbolo.passagem = 0;
  ultimoSimbolo.rotulo = 0;
  ultimoSimbolo.zombie = 0;
}

void DeclVar() {
  PrintNodo("<DeclVar>", AVANCA);
  t = AnaLex(fd);
  if (t.cat == ID) {
    PrintNodo(t.lexema, MANTEM);
    strcpy(ultimoSimbolo.lexema, t.lexema);
    t = AnaLex(fd);
    t.processado = FALSE;
  } else
    erro("identificador esperado");
  int simbolo_inserido = FALSE;
  if (t.cat == SN && t.codigo == A_COLCHETE) {
    t.processado = TRUE;
    int tamanho_colchete = 0;
    while (t.codigo == A_COLCHETE) {
      tamanho_colchete++;
      PrintNodo("[", MANTEM);
      t = AnaLex(fd);
      if (t.cat == CT_I) {
        PrintNodoInt(t.valInt, MANTEM);
        if (tamanho_colchete == 1) {
          ultimoSimbolo.dim1 = t.valInt;
        } else
          ultimoSimbolo.dim2 = t.valInt;
      } else if (t.cat == ID) {
        PrintNodo(t.lexema, MANTEM);
      } else
        erro("Identificador ou constante esperado");

      t = AnaLex(fd);
      if (t.cat == SN && t.codigo == F_COLCHETE)
        PrintNodo("]", MANTEM);
      else
        erro("Fecha colchete esperado");
      t = AnaLex(fd);
    }
    if (tamanho_colchete > 2)
      erro("Tamanho máximo de colchetes excedido");
    ultimoSimbolo.array = tamanho_colchete;
    if (ultimoSimbolo.escopo == GLOBAL)
      ultimoSimbolo.categoria = V_GLOBAL;
    else
      ultimoSimbolo.categoria = V_LOCAL;
    InserirTabela(ultimoSimbolo);
    simbolo_inserido = TRUE;
    if (t.cat == SN && t.codigo == ATRIBUICAO) {
      PrintNodo("=", MANTEM);
      t = AnaLex(fd);
      if (t.cat == SN && t.codigo == A_CHAVES) {
        PrintNodo("{", MANTEM);
        while (t.codigo != F_CHAVES) {
          t = AnaLex(fd);
          if (!VerificaConst())
            erro("constante esperada");
          if (t.cat == CT_I) {
            PrintNodoInt(t.valInt, MANTEM);
          }
          if (t.cat == CT_R) {
            PrintNodoFloat(t.valInt, MANTEM);
          }
          if (t.cat == CT_R) {
            PrintNodoChar(t.valInt, MANTEM);
          }
          if (t.cat == LT) {
            PrintNodo(tabela_literais[t.codigo], MANTEM);
          }
          t = AnaLex(fd);
          if (t.codigo == VIRGULA)
            PrintNodo(",", MANTEM);
          else if (t.codigo != F_CHAVES)
            erro("Virgula ou fecha chaves esperado");
        }
        PrintNodo("}", MANTEM);
      } else
        erro("chaves esperada");
    }
  }
  if (t.codigo == ATRIBUICAO) {
    PrintNodo("=", MANTEM);
    t.processado = TRUE;
    t = AnaLex(fd);
    if (!VerificaConst())
      erro("constante esperada");
    if (t.cat == CT_I) {
      PrintNodoInt(t.valInt, MANTEM);
      AtribuirValorInt(t.valInt);
    }
    if (t.cat == CT_R) {
      PrintNodoFloat(t.valReal, MANTEM);
      AtribuirValorReal(t.valReal);
    }
    if (t.cat == CT_C) {
      PrintNodoChar(t.caracter, MANTEM);
      AtribuirValorChar(t.caracter);
    }
    if (ultimoSimbolo.escopo == GLOBAL)
      ultimoSimbolo.categoria = V_GLOBAL;
    else
      ultimoSimbolo.categoria = V_LOCAL;
    InserirTabela(ultimoSimbolo);
    simbolo_inserido = TRUE;
  }
  if (!simbolo_inserido)
    InserirTabela(ultimoSimbolo);
  PrintNodo("", RETROCEDE);
}

void Tipo() {
  PrintNodo("<Tipo>", AVANCA);
  if (t.cat == PAL_R) {
    switch (t.codigo) {
    case CHAR:
      PrintNodo("char", MANTEM);
      ultimoSimbolo.tipo = T_CHAR;
      break;
    case INT:
      PrintNodo("int", MANTEM);
      ultimoSimbolo.tipo = T_INT;
      break;
    case REAL:
      PrintNodo("real", MANTEM);
      ultimoSimbolo.tipo = T_REAL;
      break;
    case BOOL:
      PrintNodo("bool", MANTEM);
      ultimoSimbolo.tipo = T_BOOL;
      break;
    default:
      erro("Tipo esperado");
      break;
    }
  } else
    erro("Palavra reservada esperada");
  PrintNodo("", RETROCEDE);
}

int VerificaTipo() {
  if (t.cat == PAL_R) {
    switch (t.codigo) {
    case CHAR:
    case INT:
    case REAL:
    case BOOL:
      return TRUE;
      break;
    default:
      return FALSE;
      break;
    }
  }
  return FALSE;
}

void DeclListVar() {
  while ((t.cat == PAL_R && t.codigo == CONST) || VerificaTipo()) {
    PrintNodo("<DeclListVar>", AVANCA);
    if (t.cat == PAL_R && t.codigo == CONST) {
      PrintNodo("const", MANTEM);
      ultimoSimbolo.is_const = TRUE;
      t = AnaLex(fd);
    }
    Tipo();
    DeclVar();
    if (t.processado)
      t = AnaLex(fd);
    while (t.cat == SN && t.codigo == VIRGULA) {
      PrintNodo(",", MANTEM);
      DeclVar();
      t = AnaLex(fd);
    }
    PrintNodo("", RETROCEDE);
  }
}

void DeclDefProc() {
  while (t.cat == PR && (t.codigo == PROT || t.codigo == DEF)) {
    PrintNodo("<DeclDefProc>", AVANCA);
    if (t.codigo == PROT) {
      PrintNodo("prot", MANTEM);
      t = AnaLex(fd);
      if (t.cat == ID) {
        PrintNodo(t.lexema, MANTEM);

        strcpy(ultimoSimbolo.lexema, t.lexema);
        ultimoSimbolo.categoria = PROTOTIPO;
        ultimoSimbolo.escopo = GLOBAL;
        InserirTabela(ultimoSimbolo);
        FlushSimbolo();
        t = AnaLex(fd);
        if (t.cat == SN && t.codigo == A_PARENTESE) {
          PrintNodo("(", MANTEM);
          t = AnaLex(fd);
          if (t.cat == SN && t.codigo == ENDERECO) {
            PrintNodo("&", MANTEM);
            ultimoSimbolo.passagem = REFERENCIA;
            t = AnaLex(fd);
          }
          if (VerificaTipo()) {
            Tipo();
          } else
            erro("Tipo esperado");
          t = AnaLex(fd);
          int tamanho_colchete = 0;
          while (t.cat == SN && t.codigo == A_COLCHETE) {
            tamanho_colchete++;
            PrintNodo("[", MANTEM);
            t = AnaLex(fd);
            if (t.cat == SN && t.codigo == F_COLCHETE) {
              PrintNodo("]", MANTEM);
              t = AnaLex(fd);
              if (tamanho_colchete > 2)
                erro("Tamanho máximo de colchetes excedido");
              ultimoSimbolo.array = tamanho_colchete;
            } else
              erro("fecha colchete esperado");
          }

          ultimoSimbolo.categoria = PARAMETRO;
          InserirTabela(ultimoSimbolo);
          FlushSimbolo();
          while (t.cat == SN && t.codigo == VIRGULA) {
            PrintNodo(",", MANTEM);
            t = AnaLex(fd);
            if (t.cat == SN && t.codigo == ENDERECO) {
              PrintNodo("&", MANTEM);
              ultimoSimbolo.passagem = REFERENCIA;
              t = AnaLex(fd);
            }
            if (VerificaTipo()) {
              Tipo();
            } else
              erro("Tipo esperado");
            t = AnaLex(fd);
            int tamanho_colchete = 0;
            while (t.cat == SN && t.codigo == A_COLCHETE) {
              tamanho_colchete++;
              PrintNodo("[", MANTEM);
              t = AnaLex(fd);
              if (t.cat == SN && t.codigo == F_COLCHETE) {
                PrintNodo("]", MANTEM);
                if (tamanho_colchete > 2)
                  erro("Tamanho máximo de colchetes excedido");
                ultimoSimbolo.array = tamanho_colchete;
                t = AnaLex(fd);
              } else
                erro("fecha colchete esperado");
            }

            ultimoSimbolo.categoria = PARAMETRO;
            InserirTabela(ultimoSimbolo);
            FlushSimbolo();
          }
          if (t.cat == SN && t.codigo == F_PARENTESE) {
            PrintNodo(")", MANTEM);
            t = AnaLex(fd);
          } else
            erro("Fecha parenteses esperado");
        } else
          erro("Abre parenteses esperado");
      } else
        erro("Identificador esperado");
    } else if (t.codigo == DEF) {
      PrintNodo("def", MANTEM);
      t = AnaLex(fd);
      if (t.cat == PAL_R && t.codigo == INIT) {
        PrintNodo("init", MANTEM);
        t = AnaLex(fd);
        ultimoSimbolo.escopo = LOCAL;
        DeclListVar();
        Cmd();
        if (t.cat == PAL_R && t.codigo == ENDP) {
          PrintNodo("endp", MANTEM);
          RemoverSimbolosProc();
          ultimoSimbolo.escopo = GLOBAL;
          t = AnaLex(fd);
        } else
          erro("endp esperado");
      } else if (t.cat == ID) {
        PrintNodo(t.lexema, MANTEM);
        strcpy(ultimoSimbolo.lexema, t.lexema);
        int posicaoTabela = BuscarTabela(ultimoSimbolo.lexema);
        t = AnaLex(fd);
        int is_Prototype =
            posicaoTabela >= 0 &&
            tabela_idef.tabela_simb[posicaoTabela].categoria == PROTOTIPO;
        ultimoSimbolo.categoria = PROCEDIMENTO;
        if (posicaoTabela == -1) {
          strcpy(ultimoSimbolo.lexema, t.lexema);
          ultimoSimbolo.escopo = GLOBAL;
          InserirTabela(ultimoSimbolo);
        } else if (is_Prototype) {
          EditarLinhaTabela(posicaoTabela, ultimoSimbolo);
        } else {
          char mensagem_erro[50];
          snprintf(mensagem_erro, sizeof(mensagem_erro),
                   "Identificador '%s' já declarado no escopo atual.",
                   ultimoSimbolo.lexema);
          erro(mensagem_erro);
        }
        FlushSimbolo();
        if (t.cat == SN && t.codigo == A_PARENTESE) {
          PrintNodo("(", MANTEM);
          t = AnaLex(fd);
          posicaoTabela++;
          if (t.cat == SN && t.codigo == ENDERECO) {
            PrintNodo("&", MANTEM);
            t = AnaLex(fd);
            ultimoSimbolo.passagem = REFERENCIA;
            if (is_Prototype &&
                tabela_idef.tabela_simb[posicaoTabela].passagem != REFERENCIA)
              erro("Parametro declarado no prototipo nao possuia referencia");
          }
          if (VerificaTipo()) {
            Tipo();
          } else
            erro("Tipo esperado");
          if (is_Prototype &&
              tabela_idef.tabela_simb[posicaoTabela].tipo != ultimoSimbolo.tipo)
            erro("Tipo nao condizente com o prototipo");
          t = AnaLex(fd);
          if (t.cat == ID) {
            PrintNodo(t.lexema, MANTEM);
            strcpy(ultimoSimbolo.lexema, t.lexema);
            t = AnaLex(fd);
          } else
            erro("Identificador esperado");
          int tamanho_colchete = 0;
          while (t.cat == SN && t.codigo == A_COLCHETE) {
            tamanho_colchete++;
            PrintNodo("[", MANTEM);
            t = AnaLex(fd);
            if (t.cat == SN && t.codigo == F_COLCHETE) {
              PrintNodo("]", MANTEM);
              t = AnaLex(fd);
              if (tamanho_colchete > 2)
                erro("Tamanho máximo de colchetes excedido");
              ultimoSimbolo.array = tamanho_colchete;
            } else
              erro("fecha colchete esperado");
          }
          if (is_Prototype && tabela_idef.tabela_simb[posicaoTabela].array !=
                                  ultimoSimbolo.array)
            erro("Estrutura de array invalidada pela declaracao do prototipo");
          if (is_Prototype)
            EditarLinhaTabela(posicaoTabela, ultimoSimbolo);
          else
            InserirTabela(ultimoSimbolo);
          FlushSimbolo();
          while (t.cat == SN && t.codigo == VIRGULA) {
            PrintNodo(",", MANTEM);
            if (is_Prototype)
              posicaoTabela++;
            t = AnaLex(fd);
            if (t.cat == SN && t.codigo == ENDERECO) {
              PrintNodo("&", MANTEM);
              t = AnaLex(fd);
              ultimoSimbolo.passagem = REFERENCIA;
              if (is_Prototype &&
                  tabela_idef.tabela_simb[posicaoTabela].passagem != REFERENCIA)
                erro("Parametro declarado no prototipo nao possuia referencia");
            }
            if (VerificaTipo()) {
              Tipo();
            } else
              erro("Tipo esperado");
            if (is_Prototype && tabela_idef.tabela_simb[posicaoTabela].tipo !=
                                    ultimoSimbolo.tipo)
              erro("Tipo nao condizente com o prototipo");
            t = AnaLex(fd);
            if (t.cat == ID) {
              PrintNodo(t.lexema, MANTEM);
              strcpy(ultimoSimbolo.lexema, t.lexema);
              t = AnaLex(fd);
            } else
              erro("Identificador esperado");
            int tamanho_colchete = 0;
            while (t.cat == SN && t.codigo == A_COLCHETE) {
              tamanho_colchete++;
              PrintNodo("[", MANTEM);
              t = AnaLex(fd);
              if (t.cat == SN && t.codigo == F_COLCHETE) {
                PrintNodo("]", MANTEM);
                t = AnaLex(fd);
                if (tamanho_colchete > 2)
                  erro("Tamanho máximo de colchetes excedido");
                ultimoSimbolo.array = tamanho_colchete;
              } else
                erro("fecha colchete esperado");
            }
            if (is_Prototype && tabela_idef.tabela_simb[posicaoTabela].array !=
                                    ultimoSimbolo.array)
              erro(
                  "Estrutura de array invalidada pela declaracao do prototipo");
            if (is_Prototype)
              EditarLinhaTabela(posicaoTabela, ultimoSimbolo);
            else
              InserirTabela(ultimoSimbolo);
            FlushSimbolo();
          }
          if (t.cat == SN && t.codigo == F_PARENTESE) {
            PrintNodo(")", MANTEM);
            t = AnaLex(fd);
          } else
            erro("Fecha parenteses esperado");
        } else
          erro("Abre parenteses esperado");
        ultimoSimbolo.escopo = LOCAL;
        DeclListVar();
        Cmd();
        if (t.cat == PAL_R && t.codigo == ENDP) {
          PrintNodo("endp", MANTEM);
          ultimoSimbolo.escopo = GLOBAL;
          RemoverSimbolosProc();
          t = AnaLex(fd);
        } else
          erro("endp esperado");
      }
    }
  }
}

void DeclProcProt() {
  PrintNodo("<DeclProcProt>", AVANCA);
  PrintNodo(t.lexema, MANTEM);
  t = AnaLex(fd);
  if (t.cat == SN && t.codigo == A_PARENTESE) {
    PrintNodo("(", MANTEM);
    t = AnaLex(fd);
    if (t.cat == SN && t.codigo == ENDERECO) {
      PrintNodo("&", MANTEM);
      t = AnaLex(fd);
    }
    if (VerificaTipo()) {
      Tipo();
    }
    t = AnaLex(fd);
    while (t.cat == SN && t.codigo == A_COLCHETE) {
      PrintNodo("[", MANTEM);
      t = AnaLex(fd);
      if (t.cat == SN && t.codigo == F_COLCHETE) {
        PrintNodo("]", MANTEM);
        t = AnaLex(fd);
      } else
        erro("Fecha colchete esperado");
    }
    while (t.cat == SN && t.codigo == VIRGULA) {
      PrintNodo(",", MANTEM);
      t = AnaLex(fd);
      if (t.cat == SN && t.codigo == ENDERECO) {
        PrintNodo("&", MANTEM);
        t = AnaLex(fd);
      }
      Tipo();
      t = AnaLex(fd);
      if (t.cat == SN && t.codigo == A_COLCHETE) {
        while (t.codigo == A_COLCHETE) {
          PrintNodo("[", MANTEM);
          t = AnaLex(fd);
          if (t.cat == SN && t.codigo == F_COLCHETE) {
            PrintNodo("]", MANTEM);
            t = AnaLex(fd);
          } else
            erro("Fecha colchete esperado");
        }
      }
    }
    if (t.cat == SN && t.codigo == F_PARENTESE) {
      PrintNodo(")", MANTEM);
      t = AnaLex(fd);
    } else
      erro("Fecha parenteses esperado");
  } else
    erro("Abre parenteses esperado");
  PrintNodo("", RETROCEDE);
}

int Fator() {
  PrintNodo("<Fator>", AVANCA);
  switch (t.cat) {
  case ID:
    PrintNodo(t.lexema, MANTEM);
    t = AnaLex(fd);
    int posicaoTabela = BuscarTabela(t.lexema);
    while (t.cat == SN && t.codigo == A_COLCHETE) {
      PrintNodo("[", MANTEM);
      t = AnaLex(fd);
      int tipoExpr = Expr();
      if (tipoExpr != T_INT) erro("Tipo incompativel");
      if (t.cat == SN && t.codigo == F_COLCHETE) {
        PrintNodo("]", MANTEM);
        t = AnaLex(fd);
      } else
        erro("Fecha colchete esperado");
    }
    return tabela_idef.tabela_simb[posicaoTabela].tipo;
    break;
  case CT_I:
    PrintNodoInt(t.valInt, MANTEM);
    t = AnaLex(fd);
    return T_INT;
    break;
  case CT_R:
    PrintNodoFloat(t.valReal, MANTEM);
    t = AnaLex(fd);
    return T_REAL;
    break;
  case CT_C:
    PrintNodoChar(t.caracter, MANTEM);
    t = AnaLex(fd);
    return T_CHAR;
    break;
  case SN:
    if (t.codigo == A_PARENTESE) {
      PrintNodo("(", MANTEM);
      Expr();
      if (t.cat == SN && t.codigo == F_PARENTESE) {
        PrintNodo(")", MANTEM);
        t = AnaLex(fd);
      } else
        erro("Fecha parenteses esperado");
    }
    if (t.codigo == NAO) {
      PrintNodo("!", MANTEM);
      t = AnaLex(fd);
      Fator();
    }
    break;
  default:
    erro("Fator esperado");
    break;
  }
  PrintNodo("", RETROCEDE);
}

int VerificaTermo() {
  switch (t.codigo) {
  case MULTIPLICACAO:
  case DIVISAO:
  case AND:
    return TRUE;
    break;
  default:
    return FALSE;
    break;
  }
}

int Termo() {
  PrintNodo("<Termo>", AVANCA);
  Fator();
  if (t.cat == SN) {
    while (t.cat == SN && VerificaTermo()) {
      switch (t.codigo) {
      case MULTIPLICACAO:
        PrintNodo("*", MANTEM);
        break;
      case DIVISAO:
        PrintNodo("/", MANTEM);
        break;
      case AND:
        PrintNodo("&&", MANTEM);
        break;
      }
      t = AnaLex(fd);
      Fator();
    }
  }
  PrintNodo("", RETROCEDE);
}

int ExprSimp() {
  PrintNodo("<ExprSimp>", AVANCA);
  if (t.cat == SN && t.codigo == ADICAO) {
    PrintNodo("+", MANTEM);
    t = AnaLex(fd);
  } else if (t.cat == SN && t.codigo == SUBTRACAO) {
    PrintNodo("-", MANTEM);
    t = AnaLex(fd);
  }
  Termo();
  while (t.cat == SN &&
         (t.codigo == ADICAO || t.codigo == SUBTRACAO || t.codigo == OU)) {
    if (t.cat == SN && t.codigo == ADICAO) {
      PrintNodo("+", MANTEM);
      t = AnaLex(fd);
    } else if (t.cat == SN && t.codigo == SUBTRACAO) {
      PrintNodo("-", MANTEM);
      t = AnaLex(fd);
    } else if (t.cat == SN && t.codigo == OU) {
      PrintNodo("||", MANTEM);
      t = AnaLex(fd);
    }
    Termo();
  }
  PrintNodo("", RETROCEDE);
}

int VerificaOpRel() {
  if (t.cat == SN) {
    switch (t.codigo) {
    case COMPARACAO:
    case NAO_IGUAL:
    case MENOR_IGUAL:
    case MENOR:
    case MAIOR:
    case MAIOR_IGUAL:
      return TRUE;
      break;
    default:
      return FALSE;
      break;
    }
  }
  return FALSE;
}

void OpRel() {
  PrintNodo("<OpRel>", AVANCA);
  if (t.cat == SN) {
    switch (t.codigo) {
    case COMPARACAO:
      PrintNodo("==", MANTEM);
      break;
    case NAO_IGUAL:
      PrintNodo("!=", MANTEM);
      break;
    case MENOR_IGUAL:
      PrintNodo("<=", MANTEM);
      break;
    case MENOR:
      PrintNodo("<", MANTEM);
      break;
    case MAIOR:
      PrintNodo(">", MANTEM);
      break;
    case MAIOR_IGUAL:
      PrintNodo(">=", MANTEM);
      break;
    }
  }
  t = AnaLex(fd);
  PrintNodo("", RETROCEDE);
}

void Atrib() {
  PrintNodo("<Atrib>", AVANCA);
  PrintNodo(t.lexema, MANTEM);
  simboloBuscado = BuscarTabela(t.lexema);
  if (simboloBuscado == -1)
    erro("Identificador nao declarado");
  t = AnaLex(fd);
  int contadorColchetes = 0;
  while (t.cat == SN && t.codigo == A_COLCHETE) {
    Expr();
    if (t.cat == SN && t.codigo == F_COLCHETE) {
      PrintNodo("]", MANTEM);
      contadorColchetes++;
      t = AnaLex(fd);
    } else
      erro("Fecha colchete esperado");
  }
  if (contadorColchetes != tabela_idef.tabela_simb[simboloBuscado].array)
    erro("Estrutura de array incompativel com declaracao");
  if (t.cat == SN && t.codigo == ATRIBUICAO) {
    PrintNodo("=", MANTEM);
    Expr();
  } else
    erro("Atribuicao esperada");
}

int Expr() {
  PrintNodo("<Expr>", AVANCA);
  t = AnaLex(fd);
  ExprSimp();
  if (VerificaOpRel()) {
    OpRel();
    ExprSimp();
  }
}

int VerificaCmd() {
  if (t.cat == PAL_R) {
    switch (t.codigo) {
    case DO:
    case WHILE:
    case VAR:
    case IF:
    case GETOUT:
    case GETINT:
    case GETREAL:
    case GETCHAR:
    case PUTINT:
    case PUTREAL:
    case PUTCHAR:
    case PUTSTR:
      return TRUE;
      break;
    default:
      return FALSE;
      break;
    }
  }
  if (t.cat == ID)
    return TRUE;
  return FALSE;
}

void Cmd() {
  while (VerificaCmd()) {
    PrintNodo("<Cmd>", AVANCA);
    if (t.cat == PR) {
      switch (t.codigo) {
      case DO:
        PrintNodo("do", MANTEM);
        t = AnaLex(fd);
        if (t.cat == ID) {
          PrintNodo(t.lexema, MANTEM);
          t = AnaLex(fd);
          if (t.cat == SN && t.codigo == A_PARENTESE) {
            PrintNodo("(", MANTEM);
            t = AnaLex(fd);
            if (t.cat == ID) {
              PrintNodo(t.lexema, MANTEM);
              t = AnaLex(fd);
              while (t.cat == SN && t.codigo == VIRGULA) {
                PrintNodo(",", MANTEM);
                t = AnaLex(fd);
                if (t.cat == ID) {
                  PrintNodo(t.lexema, MANTEM);
                  t = AnaLex(fd);
                } else
                  erro("identificador esperado");
              }
              if (t.cat == SN && t.codigo == F_PARENTESE) {
                PrintNodo(")", MANTEM);
              } else
                erro("Fecha parenteses esperado");
            }
          } else
            erro("Abre parenteses esperado");
        } else
          erro("identificador esperado");
        break;

      case WHILE:
        PrintNodo("while", MANTEM);
        t = AnaLex(fd);
        if (t.cat == SN && t.codigo == A_PARENTESE) {
          PrintNodo("(", MANTEM);
          Expr();
          // t = AnaLex(fd);
          if (t.cat == SN && t.codigo == F_PARENTESE) {
            PrintNodo(")", MANTEM);
            t = AnaLex(fd);
            Cmd();
            if (t.cat == PAL_R && t.codigo == ENDW) {
              PrintNodo("endw", MANTEM);
            } else
              erro("endw esperado");
          } else
            erro("Fecha parenteses esperado");
        } else
          erro("Fecha parenteses esperado");
        break;

      case VAR:
        PrintNodo("var", MANTEM);
        t = AnaLex(fd);
        if (t.cat == ID) {
          PrintNodo(t.lexema, MANTEM);
          t = AnaLex(fd);
          if (t.cat == PAL_R && t.codigo == FROM) {
            PrintNodo("from", MANTEM);
            Expr();
            if (t.codigo != TO && t.codigo != DT)
              erro("do ou dt esperado");
            if (t.cat == PAL_R && t.codigo == TO) {
              PrintNodo("to", MANTEM);
            } else if (t.cat == PAL_R && t.codigo == DT) {
              PrintNodo("dt", MANTEM);
            }
            Expr();
            if (t.cat == PAL_R && t.codigo == BY) {
              PrintNodo("by", MANTEM);
              t = AnaLex(fd);
              if (t.cat != CT_I && t.cat != ID)
                erro("constante inteira ou identificador esperados");
              if (t.cat == CT_I) {
                PrintNodoInt(t.valInt, MANTEM);
              } else if (t.cat == ID) {
                PrintNodo(t.lexema, MANTEM);
              }
              t = AnaLex(fd);
            }
            Cmd();
            if (t.cat == PAL_R && t.codigo == ENDV) {
              PrintNodo("endv", MANTEM);
            } else
              erro("endv esperado");
          } else
            erro("from esperado");
        } else
          erro("identificador esperado");
        break;

      case IF:
        PrintNodo("if", MANTEM);
        t = AnaLex(fd);
        if (t.cat == SN && t.codigo == A_PARENTESE) {
          PrintNodo("(", MANTEM);
          Expr();
          if (t.cat == SN && t.codigo == F_PARENTESE) {
            PrintNodo(")", MANTEM);
            t = AnaLex(fd);
            Cmd();
            while (t.cat == PAL_R && t.codigo == ELIF) {
              PrintNodo("elif", MANTEM);
              t = AnaLex(fd);
              if (t.cat == SN && t.codigo == A_PARENTESE) {
                PrintNodo("(", MANTEM);
                Expr();
                if (t.cat == SN && t.codigo == F_PARENTESE) {
                  PrintNodo(")", MANTEM);
                  t = AnaLex(fd);
                  Cmd();
                } else
                  erro("Fecha parenteses esperado");
              }
            }
            if (t.cat == PAL_R && t.codigo == ELSE) {
              PrintNodo("else", MANTEM);
              t = AnaLex(fd);
              Cmd();
            }
            if (t.cat == PAL_R && t.codigo == ENDI) {
              PrintNodo("endi", MANTEM);
            } else
              erro("endi esperado");
          } else
            erro("Fecha parenteses esperado");
        } else
          erro("Abre parenteses esperado");
        break;

      case GETOUT:
        PrintNodo("getout", MANTEM);
        break;

      case GETINT:
        PrintNodo("getint", MANTEM);
        t = AnaLex(fd);
        if (t.cat == ID) {
          PrintNodo(t.lexema, MANTEM);
        } else
          erro("identificador esperado");
        break;

      case GETREAL:
        PrintNodo("getreal", MANTEM);
        t = AnaLex(fd);
        if (t.cat == ID) {
          PrintNodo(t.lexema, MANTEM);
        } else
          erro("identificador esperado");
        break;

      case GETCHAR:
        PrintNodo("getchar", MANTEM);
        t = AnaLex(fd);
        if (t.cat == ID) {
          PrintNodo(t.lexema, MANTEM);
        } else
          erro("identificador esperado");
        break;

      case PUTINT:
        PrintNodo("putint", MANTEM);
        t = AnaLex(fd);
        if (t.cat != ID && t.cat != CT_I)
          erro("identificador ou constante inteira esperada");
        if (t.cat == ID) {
          PrintNodo(t.lexema, MANTEM);
        }
        if (t.cat == CT_I) {
          PrintNodoInt(t.valInt, MANTEM);
        }
        break;

      case PUTREAL:
        PrintNodo("putreal", MANTEM);
        t = AnaLex(fd);
        if (t.cat != ID && t.cat != CT_R)
          erro("identificador ou constante real esperada");
        if (t.cat == ID) {
          PrintNodo(t.lexema, MANTEM);
        }
        if (t.cat == CT_R) {
          PrintNodoFloat(t.valReal, MANTEM);
        }
        break;

      case PUTCHAR:
        PrintNodo("putchar", MANTEM);
        t = AnaLex(fd);
        if (t.cat != ID && t.cat != CT_C)
          erro("identificador ou caracter esperada");
        if (t.cat == ID) {
          PrintNodo(t.lexema, MANTEM);
        }
        if (t.cat == CT_C) {
          PrintNodoChar(t.caracter, MANTEM);
        }
        break;

      case PUTSTR:
        PrintNodo("putstr", MANTEM);
        t = AnaLex(fd);
        if (t.cat != ID && t.cat != LT)
          erro("identificador ou literal esperados");
        if (t.cat == ID) {
          PrintNodo(t.lexema, MANTEM);
        }
        if (t.cat == LT) {
          PrintNodo(tabela_literais[t.codigo], MANTEM);
        }
        break;
      }
    } else if (t.cat == ID) {
      Atrib();
      t.processado = FALSE;
    }
    if (t.processado)
      t = AnaLex(fd);
    t.processado = TRUE;
    PrintNodo("", RETROCEDE);
  }
}

void ProcInit() {
  PrintNodo("<ProcInit>", AVANCA);
  PrintNodo("init", MANTEM);
  if (t.cat == PR && t.codigo == INIT) {
    t = AnaLex(fd);
    DeclListVar();
    Cmd();
    if (t.cat == PR && t.codigo == ENDP) {
      PrintNodo("endp", MANTEM);
    } else
      erro("endp esperado");

  } else
    erro("Init esperado");
  PrintNodo("", RETROCEDE);
}

void Prog() {
  PrintNodo("<Prog>", AVANCA);
  FlushSimbolo();
  t = AnaLex(fd);
  while (t.cat != FIM_ARQ) {
    DeclListVar();
    DeclDefProc();
    /*
    while (t.cat == PAL_R || t.codigo == PR) {
      PrintNodo("pr", MANTEM);
      t = AnaLex(fd);
      if (t.cat == ID) {
        DeclProcProt();
        if (t.codigo != PR)
          erro("pr esperado");
      } else if (t.cat == PAL_R && t.codigo == INIT) {
        ProcInit();
        break;
      }
    }
    */
  }
  PrintNodo("", RETROCEDE);
}
