#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Anasint.h"
#include "Funcaux.h"
#include "TabSimb.h"

TabIdef tabela_idef;
TabSimb ultimoSimbolo;
int simboloBuscado = -1;
int ultimoRotulo = 2; // Rotulo 1 reservado ao proc init
int contadorEnderecoGlobal = 0;
int contadorEnderecoLocal = 0;
int contador_linha_mp = 1;
int contador_init = 0;

void debugToken() {
  switch (t.cat) {
  case ID:
    fprintf(codigo_mp, "<ID, %s> ", t.lexema);
    break;

  case SN:
    switch (t.codigo) {
    case ADICAO:
      fprintf(codigo_mp, "<SN, ADICAO> ");
      break;
    case SUBTRACAO:
      fprintf(codigo_mp, "<SN, SUBTRACAO> ");
      break;
    case MULTIPLICACAO:
      fprintf(codigo_mp, "<SN, MULTIPLICACAO> ");
      break;
    case DIVISAO:
      fprintf(codigo_mp, "<SN, DIVISAO> ");
      break;
    case MAIOR:
      fprintf(codigo_mp, "<SN, MAIOR> ");
      break;
    case MAIOR_IGUAL:
      fprintf(codigo_mp, "<SN, MAIOR IGUAL> ");
      break;
    case MENOR:
      fprintf(codigo_mp, "<SN, MENOR> ");
      break;
    case MENOR_IGUAL:
      fprintf(codigo_mp, "<SN, MENOR IGUAL> ");
      break;
    case COMPARACAO:
      fprintf(codigo_mp, "<SN, COMPARACAO> ");
      break;
    case ATRIBUICAO:
      fprintf(codigo_mp, "<SN, ATRIBUICAO> ");
      break;
    case A_PARENTESE:
      fprintf(codigo_mp, "<SN, A PARENTESE> ");
      break;
    case F_PARENTESE:
      fprintf(codigo_mp, "<SN, F PARENTESE> ");
      break;
    case MODIFICADOR:
      fprintf(codigo_mp, "<SN, MODIFICADOR> ");
      break;
    case A_COLCHETE:
      fprintf(codigo_mp, "<SN, A COLCHETE> ");
      break;
    case F_COLCHETE:
      fprintf(codigo_mp, "<SN, F COLCHETE> ");
      break;
    case OU:
      fprintf(codigo_mp, "<SN, OU> ");
      break;
    case IDENTIF_PONTEIRO:
      fprintf(codigo_mp, "<SN, IDENTIF PONTEIRO> ");
      break;
    case AND:
      fprintf(codigo_mp, "<SN, AND> ");
      break;
    case NAO:
      fprintf(codigo_mp, "<SN, NAO> ");
      break;
    case NAO_IGUAL:
      fprintf(codigo_mp, "<SN, NAO IGUAL> ");
      break;
    case APOSTOFO:
      fprintf(codigo_mp, "<SN, APOSTOFO> ");
      break;
    case ASPAS_D:
      fprintf(codigo_mp, "<SN, ASPAS D> ");
      break;
    case VIRGULA:
      fprintf(codigo_mp, "<SN, VIRGULA> ");
      break;
    case ENDERECO:
      fprintf(codigo_mp, "<SN, ENDERECO> ");
      break;
    default:
      fprintf(codigo_mp, "<SN, DESCONHECIDO> ");
    }
    break;

  case CT_I:
    fprintf(codigo_mp, "<CT_I, %d> ", t.valInt);
    break;
  case CT_R:
    fprintf(codigo_mp, "<CT_R, %f> ", t.valReal);
    break;
  case CT_C:
    if (t.codigo < 0)
      fprintf(codigo_mp, "<CT_C, %c> ", t.caracter);
    else
      fprintf(codigo_mp, "<CT_C, %d> ", t.codigo);
    break;
  case LT:
    fprintf(codigo_mp, "<LT, %s> ", tabela_literais[t.codigo]);
    break;
  case PAL_R:
    fprintf(codigo_mp, "<PAL_R, %s> ", palavras_reservadas[t.codigo - 1]);
    break;
  case FIM_ARQ:
    fprintf(codigo_mp, "\nFim da expressao encontrado.\n");
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
  ultimoSimbolo.passagem = 0;
  ultimoSimbolo.rotulo = 0;
  ultimoSimbolo.zombie = 0;
  ultimoSimbolo.val_bool = 0;
  ultimoSimbolo.val_int = 0;
  ultimoSimbolo.val_char = '\0';
  ultimoSimbolo.val_real = 0.0;
}

void comparaTipos(int primeiro_tipo, int segundo_tipo) {
  if (primeiro_tipo == segundo_tipo)
    return;
  if ((primeiro_tipo == T_INT && segundo_tipo == T_BOOL) ||
      (primeiro_tipo == T_BOOL && segundo_tipo == T_INT))
    return;
  if ((primeiro_tipo == T_INT && segundo_tipo == T_CHAR) ||
      (primeiro_tipo == T_CHAR && segundo_tipo == T_INT))
    return;
  erro("Incompatibidade de tipos");
}

void DeclVar() {
  PrintNodo("<DeclVar>", AVANCA);
  t = AnaLex(fd);
  if (t.cat == ID) {
    PrintNodo(t.lexema, MANTEM);
    strcpy(ultimoSimbolo.lexema, t.lexema);
    if (ultimoSimbolo.escopo == LOCAL) {
      ultimoSimbolo.endereco = contadorEnderecoLocal;
      contadorEnderecoLocal++;
    } else {
      ultimoSimbolo.endereco = contadorEnderecoGlobal;
      contadorEnderecoGlobal++;
    }
    t = AnaLex(fd);
    t.processado = FALSE;
  } else
    erro("identificador esperado");
  int simbolo_inserido = FALSE;
  int is_array = FALSE;
  if (t.cat == SN && t.codigo == A_COLCHETE) {
    is_array = TRUE;
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
        int posicaoTabela = BuscarTabela(t.lexema);
        if (tabela_idef.tabela_simb[posicaoTabela].tipo != T_INT)
          erro("Identificador do tipo nao inteiro identificado em colchetes");
      } else
        erro("Identificador ou constante inteira esperados");

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
      fprintf(codigo_mp, "AMEM %d\n", ultimoSimbolo.dim1);
      PrintNodo("=", MANTEM);
      t = AnaLex(fd);
      if (t.cat == SN && t.codigo == A_CHAVES) {
        PrintNodo("{", MANTEM);
        int contador_valores = 0;
        while (t.codigo != F_CHAVES) {
          contador_valores++;
          if (contador_valores > ultimoSimbolo.dim1)
            erro("Capacidade de array excedida");
          t = AnaLex(fd);
          if (!VerificaConst())
            erro("constante esperada");
          if (t.cat == CT_I) {
            fprintf(codigo_mp, "PUSH %d\n", t.valInt);
            if (ultimoSimbolo.escopo == LOCAL) {
              fprintf(codigo_mp, "STOR %d,%d\n", ultimoSimbolo.escopo - 1,
                     contadorEnderecoLocal);
              contadorEnderecoLocal++;
            } else {
              fprintf(codigo_mp, "STOR %d,%d\n", ultimoSimbolo.escopo - 1,
                     contadorEnderecoGlobal);
              contadorEnderecoGlobal++;
            }
            PrintNodoInt(t.valInt, MANTEM);
          }
          if (t.cat == CT_R) {
            fprintf(codigo_mp, "PUSH %f\n", t.valReal);
            if (ultimoSimbolo.escopo == LOCAL) {
              fprintf(codigo_mp, "STOR %d,%d\n", ultimoSimbolo.escopo - 1,
                     contadorEnderecoLocal);
              contadorEnderecoLocal++;
            } else {
              fprintf(codigo_mp, "STOR %d,%d\n", ultimoSimbolo.escopo - 1,
                     contadorEnderecoGlobal);
              contadorEnderecoGlobal++;
            }
            PrintNodoFloat(t.valInt, MANTEM);
          }
          if (t.cat == LT) {
            int i = 0;
            while (tabela_literais[t.codigo][i] != '\0') {
              fprintf(codigo_mp, "PUSH %d\n", tabela_literais[t.codigo][i]);
              if (ultimoSimbolo.escopo == LOCAL) {
                fprintf(codigo_mp, "STOR %d,%d\n", ultimoSimbolo.escopo - 1,
                       contadorEnderecoLocal);
                contadorEnderecoLocal++;
              } else {
                fprintf(codigo_mp, "STOR %d,%d\n", ultimoSimbolo.escopo - 1,
                       contadorEnderecoGlobal);
                contadorEnderecoGlobal++;
              }
            }
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

    } else {
      if (ultimoSimbolo.is_const)
        erro("Identificador do tipo constante deve ser inicializado");
      t.processado = FALSE;
    }
    ultimoSimbolo.array = 0;
    ultimoSimbolo.dim1 = 0;
    ultimoSimbolo.dim2 = 0;
  }
  if (t.cat == SN && t.codigo == ATRIBUICAO) {
    if (is_array)
      erro("Array nao pode ser declarado duas vezes");
    PrintNodo("=", MANTEM);
    t.processado = TRUE;
    t = AnaLex(fd);
    if (!VerificaConst())
      erro("constante esperada");
    if (t.cat == CT_I) {
      PrintNodoInt(t.valInt, MANTEM);
      AtribuirValorInt(t.valInt);
      fprintf(codigo_mp, "PUSH %d\n", t.valInt);
    }
    if (t.cat == CT_R) {
      PrintNodoFloat(t.valReal, MANTEM);
      AtribuirValorReal(t.valReal);
      fprintf(codigo_mp, "PUSH %f\n", t.valReal);
    }
    if (t.cat == CT_C) {
      PrintNodoChar(t.caracter, MANTEM);
      AtribuirValorChar(t.caracter);
      fprintf(codigo_mp, "PUSH %d\n", t.caracter);
    }
    if (ultimoSimbolo.escopo == GLOBAL)
      ultimoSimbolo.categoria = V_GLOBAL;
    else
      ultimoSimbolo.categoria = V_LOCAL;
    InserirTabela(ultimoSimbolo);
    simbolo_inserido = TRUE;
    fprintf(codigo_mp, "STOR %d,%d\n", ultimoSimbolo.escopo - 1, ultimoSimbolo.endereco);
  } else {
    if (ultimoSimbolo.is_const && !is_array)
      erro("Identificador do tipo constante deve ser inicializado");
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
    fprintf(codigo_mp, "AMEM 1\n");
    Tipo();
    DeclVar();
    if (t.processado)
      t = AnaLex(fd);
    while (t.cat == SN && t.codigo == VIRGULA) {
      PrintNodo(",", MANTEM);
      fprintf(codigo_mp, "AMEM 1\n");
      DeclVar();
      if (t.processado)
        t = AnaLex(fd);
    }
    FlushSimbolo();
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
        ultimoSimbolo.rotulo = ultimoRotulo;
        ultimoRotulo++;
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
        contador_init++;
        if (contador_init >= 2) erro("Nao pode existir mais de um init");
        PrintNodo("init", MANTEM);
        fprintf(codigo_mp, "LABEL L1\n");
        t = AnaLex(fd);
        ultimoSimbolo.escopo = LOCAL;
        DeclListVar();
        Cmd();
        if (t.cat == PAL_R && t.codigo == ENDP) {
          PrintNodo("endp", MANTEM);
          RemoverSimbolosProc();
          ultimoSimbolo.escopo = GLOBAL;
          fprintf(codigo_mp, "DMEM %d\n", contadorEnderecoGlobal);
          fprintf(codigo_mp, "DMEM %d\n", contadorEnderecoLocal);
          fprintf(codigo_mp, "HALT\n");
          contadorEnderecoLocal = 0;
          t = AnaLex(fd);
        } else
          erro("endp esperado");
      } else if (t.cat == ID) {
        PrintNodo(t.lexema, MANTEM);
        strcpy(ultimoSimbolo.lexema, t.lexema);
        int posicaoTabela = BuscarTabela(ultimoSimbolo.lexema);
        int is_Prototype =
            posicaoTabela >= 0 &&
            tabela_idef.tabela_simb[posicaoTabela].categoria == PROTOTIPO;
        int contagem_parametros_base = 0;
        int contagem_parametros = 0;
        if (is_Prototype)
          contagem_parametros_base = VerificarContagemParametros(posicaoTabela);
        ultimoSimbolo.categoria = PROCEDIMENTO;
        if (posicaoTabela == -1) {
          strcpy(ultimoSimbolo.lexema, t.lexema);
          ultimoSimbolo.escopo = GLOBAL;
          ultimoSimbolo.rotulo = ultimoRotulo;
          ultimoRotulo++;
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
        if (is_Prototype)
          fprintf(codigo_mp, "LABEL L%d\n", tabela_idef.tabela_simb[posicaoTabela].rotulo);
        else
          fprintf(codigo_mp, "LABEL L%d\n", ultimoSimbolo.rotulo);
        fprintf(codigo_mp, "INIPR 1\n");
        FlushSimbolo();
        t = AnaLex(fd);
        if (t.cat == SN && t.codigo == A_PARENTESE) {
          PrintNodo("(", MANTEM);
          t = AnaLex(fd);
          if ((t.cat == SN && t.codigo == ENDERECO) || VerificaTipo()) {
            posicaoTabela++;
            contagem_parametros++;
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
              int parametro_posicao = BuscarTabela(t.lexema);
              if (parametro_posicao != -1) {
                if (tabela_idef.tabela_simb[parametro_posicao].categoria ==
                        PARAMETRO &&
                    !tabela_idef.tabela_simb[parametro_posicao].zombie)
                  erro("Parametro de mesmo nome ja declarado nesta funcao");
              }
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
            while (t.cat == SN && t.codigo == VIRGULA) {
              PrintNodo(",", MANTEM);
              if (is_Prototype)
                posicaoTabela++;
              contagem_parametros++;
              t = AnaLex(fd);
              if (t.cat == SN && t.codigo == ENDERECO) {
                PrintNodo("&", MANTEM);
                t = AnaLex(fd);
                ultimoSimbolo.passagem = REFERENCIA;
                if (is_Prototype &&
                    tabela_idef.tabela_simb[posicaoTabela].passagem !=
                        REFERENCIA)
                  erro("Parametro declarado no prototipo nao possuia "
                       "referencia");
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
                int parametro_posicao = BuscarTabela(t.lexema);
                if (parametro_posicao != -1) {
                  if (tabela_idef.tabela_simb[parametro_posicao].categoria ==
                          PARAMETRO &&
                      !tabela_idef.tabela_simb[parametro_posicao].zombie)
                    erro("Parametro de mesmo nome ja declarado nesta funcao");
                }
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
              if (is_Prototype &&
                  tabela_idef.tabela_simb[posicaoTabela].array !=
                      ultimoSimbolo.array)
                erro("Estrutura de array invalidada pela declaracao do "
                     "prototipo");
              if (is_Prototype)
                EditarLinhaTabela(posicaoTabela, ultimoSimbolo);
              else
                InserirTabela(ultimoSimbolo);
              FlushSimbolo();
            }
          }
          if (t.cat == SN && t.codigo == F_PARENTESE) {
            PrintNodo(")", MANTEM);
            if (is_Prototype)
              if (contagem_parametros != contagem_parametros_base)
                erro("Contagem de parametros nao condizente com prototipo");
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
          fprintf(codigo_mp, "RET 1,%d\n", contagem_parametros);
          fprintf(codigo_mp, "DMEM %d\n", contadorEnderecoLocal);
          contadorEnderecoLocal = 0;
          RemoverSimbolosProc();
          t = AnaLex(fd);
        } else
          erro("endp esperado");
      }
    }
  }
  if (t.cat != FIM_ARQ) erro("Fim de arquivo, prototipo ou procedimento esperado");
  if (t.cat == FIM_ARQ && contador_init == 0) erro("Nao foi declarado nenhum procedimento init");
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
    int posicaoTabela = BuscarTabela(t.lexema);
    t = AnaLex(fd);
    if (posicaoTabela == -1)
      erro("Identificador nao declarado");
    if (tabela_idef.tabela_simb[posicaoTabela].categoria == PARAMETRO) {
      int distancia_parametro = VerificarDistanciaParametro(posicaoTabela);
      fprintf(codigo_mp, "LOAD %d,-%d\n", tabela_idef.tabela_simb[posicaoTabela].escopo - 1,
             distancia_parametro + 2);
    } else
      fprintf(codigo_mp, "LOAD %d,%d\n", tabela_idef.tabela_simb[posicaoTabela].escopo - 1,
             tabela_idef.tabela_simb[posicaoTabela].endereco);
    while (t.cat == SN && t.codigo == A_COLCHETE) {
      PrintNodo("[", MANTEM);
      t = AnaLex(fd);
      int tipoExpr = Expr();
      if (tipoExpr != T_INT)
        erro("Tipo incompativel");
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
    fprintf(codigo_mp, "PUSH %d\n", t.valInt);
    t = AnaLex(fd);
    return T_INT;
    break;
  case CT_R:
    PrintNodoFloat(t.valReal, MANTEM);
    fprintf(codigo_mp, "PUSH %f\n", t.valReal);
    t = AnaLex(fd);
    return T_REAL;
    break;
  case CT_C:
    PrintNodoChar(t.caracter, MANTEM);
    fprintf(codigo_mp, "PUSH %d\n", t.caracter);
    t = AnaLex(fd);
    return T_CHAR;
    break;
  case SN:
    if (t.codigo == A_PARENTESE) {
      PrintNodo("(", MANTEM);
      int tipo_expressao = Expr();
      if (t.cat == SN && t.codigo == F_PARENTESE) {
        PrintNodo(")", MANTEM);
        t = AnaLex(fd);
        return tipo_expressao;
      } else
        erro("Fecha parenteses esperado");
    }
    if (t.codigo == NAO) {
      PrintNodo("!", MANTEM);
      t = AnaLex(fd);
      Fator();
      int primeiraLabel = ++ultimoRotulo;
      int segundaLabel = ++ultimoRotulo;
      fprintf(codigo_mp, "GOFALSE L%d\n", primeiraLabel);
      fprintf(codigo_mp, "PUSH 0\n");
      fprintf(codigo_mp, "GOTO L%d\n", segundaLabel);
      fprintf(codigo_mp, "LABEL L%d\n", primeiraLabel);
      fprintf(codigo_mp, "PUSH 1\n");
      fprintf(codigo_mp, "LABEL L%d\n", segundaLabel);
      return T_BOOL;
    }
    break;
  default:
    erro("Fator esperado");
    break;
  }
  PrintNodo("", RETROCEDE);
  return -1;
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
  int primeiro_tipo = Fator();
  int retorno_tipo = primeiro_tipo;
  int is_and = 0;
  if (t.cat == SN) {
    int primeiro_rotulo = 0, segundo_rotulo = 0;
    while (t.cat == SN && VerificaTermo()) {
      char operacao[4];
      switch (t.codigo) {
      case MULTIPLICACAO:
        strcpy(operacao, "MUL");
        PrintNodo("*", MANTEM);
        break;
      case DIVISAO:
        strcpy(operacao, "DIV");
        PrintNodo("/", MANTEM);
        break;
      case AND:
        PrintNodo("&&", MANTEM);
        is_and = 1;
        primeiro_rotulo = ++ultimoRotulo;
        segundo_rotulo = ++ultimoRotulo;
        fprintf(codigo_mp, "COPY\n");
        fprintf(codigo_mp, "GOFALSE L%d\n", primeiro_rotulo);
        retorno_tipo = T_BOOL;
        break;
      }
      t = AnaLex(fd);
      int segundo_tipo = Fator();
      comparaTipos(primeiro_tipo, segundo_tipo);
      if(is_and) {
        fprintf(codigo_mp, "LABEL L%d\n", primeiro_rotulo);
        fprintf(codigo_mp, "PUSH 0\n");
        fprintf(codigo_mp, "LABEL L%d\n", segundo_rotulo);
        fprintf(codigo_mp, "MUL\n");
      } else fprintf(codigo_mp, "%s\n", operacao);
    }
  }
  PrintNodo("", RETROCEDE);
  return retorno_tipo;
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
  int primeiro_rotulo = 0, segundo_rotulo = 0;
  int is_ou = 0;
  int primeiro_tipo = Termo();
  while (t.cat == SN &&
         (t.codigo == ADICAO || t.codigo == SUBTRACAO || t.codigo == OU)) {
    char operacao[4];
    if (t.cat == SN && t.codigo == ADICAO) {
      PrintNodo("+", MANTEM);
      strcpy(operacao, "ADD");
      t = AnaLex(fd);
    } else if (t.cat == SN && t.codigo == SUBTRACAO) {
      PrintNodo("-", MANTEM);
      strcpy(operacao, "SUB");
      t = AnaLex(fd);
    } else if (t.cat == SN && t.codigo == OU) {
      PrintNodo("||", MANTEM);
      is_ou = 1;
      primeiro_tipo = T_BOOL;
      primeiro_rotulo = ++ultimoRotulo;
      segundo_rotulo = ++ultimoRotulo;
      fprintf(codigo_mp, "GOTRUE L%d\n", primeiro_rotulo);
      t = AnaLex(fd);
    }
    
    int segundo_tipo = Termo();
    comparaTipos(primeiro_tipo, segundo_tipo);
    if(is_ou) {
      fprintf(codigo_mp, "LABEL L%d\n", primeiro_rotulo);
      fprintf(codigo_mp, "PUSH 1\n");
      fprintf(codigo_mp, "LABEL L%d\n", segundo_rotulo);
      fprintf(codigo_mp, "ADD\n");
    } else fprintf(codigo_mp, "%s\n", operacao);
  }
  PrintNodo("", RETROCEDE);
  return primeiro_tipo;
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
  if (tabela_idef.tabela_simb[simboloBuscado].is_const)
    erro("Identificador constante nao pode ser atribuido");
  t = AnaLex(fd);
  int contadorColchetes = 0;
  while (t.cat == SN && t.codigo == A_COLCHETE) {
    int tipo_colchete = Expr();
    if (tipo_colchete != T_INT)
      erro("Tipo inteiro esperado");
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
    int tipo_expressao = Expr();
    comparaTipos(tabela_idef.tabela_simb[simboloBuscado].tipo, tipo_expressao);
    if (tabela_idef.tabela_simb[simboloBuscado].categoria == PARAMETRO) {
      int distancia_parametro = VerificarDistanciaParametro(simboloBuscado);
      fprintf(codigo_mp, "STOR %d,-%d\n",             tabela_idef.tabela_simb[simboloBuscado].escopo - 1,
             distancia_parametro + 2);
    } else
      fprintf(codigo_mp, "STOR %d,%d\n", tabela_idef.tabela_simb[simboloBuscado].escopo - 1,             tabela_idef.tabela_simb[simboloBuscado].endereco);
  } else
    erro("Atribuicao esperada");
}

int Expr() {
  PrintNodo("<Expr>", AVANCA);
  if (t.processado)
    t = AnaLex(fd);
  int primeiro_tipo = ExprSimp();
  if (VerificaOpRel()) {
    // OpRel();
    int tipoOpRel = t.codigo;
    t = AnaLex(fd);
    int segundo_tipo = ExprSimp();
    int primeiraLabel = 0;
    int segundaLabel = 0;
    comparaTipos(primeiro_tipo, segundo_tipo);
    switch(tipoOpRel) {
      case COMPARACAO:
        fprintf(codigo_mp, "SUB\n");
        primeiraLabel = ++ultimoRotulo;
        segundaLabel = ++ultimoRotulo;
        fprintf(codigo_mp, "GOFALSE L%d\n", primeiraLabel);
        fprintf(codigo_mp, "PUSH 0\n");
        fprintf(codigo_mp, "GOTO L%d\n", segundaLabel);
        fprintf(codigo_mp, "LABEL L%d\n", primeiraLabel);
        fprintf(codigo_mp, "PUSH 1\n");
        fprintf(codigo_mp, "LABEL L%d\n", segundaLabel);
        break;
        
      case NAO_IGUAL:
        fprintf(codigo_mp, "SUB\n");
        primeiraLabel = ++ultimoRotulo;
        segundaLabel = ++ultimoRotulo;
        fprintf(codigo_mp, "GOTRUE L%d\n", primeiraLabel);
        fprintf(codigo_mp, "PUSH 0\n");
        fprintf(codigo_mp, "GOTO L%d\n", segundaLabel);
        fprintf(codigo_mp, "LABEL L%d\n", primeiraLabel);
        fprintf(codigo_mp, "PUSH 1\n");
        fprintf(codigo_mp, "LABEL L%d\n", segundaLabel);
        break;

      case MENOR_IGUAL:
        fprintf(codigo_mp, "menor igual");
        fprintf(codigo_mp, "SUB\n");
        primeiraLabel = ++ultimoRotulo;
        segundaLabel = ++ultimoRotulo;
        fprintf(codigo_mp, "GOTRUE L%d\n", primeiraLabel);
        fprintf(codigo_mp, "PUSH 1\n");
        fprintf(codigo_mp, "GOTO L%d\n", segundaLabel);
        fprintf(codigo_mp, "LABEL L%d\n", primeiraLabel);
        fprintf(codigo_mp, "PUSH 0\n");
        fprintf(codigo_mp, "LABEL L%d\n", segundaLabel);
        break;

      case MENOR:
        fprintf(codigo_mp, "SUB\n");
        primeiraLabel = ++ultimoRotulo;
        segundaLabel = ++ultimoRotulo;
        fprintf(codigo_mp, "GOFALSE L%d\n", primeiraLabel);
        fprintf(codigo_mp, "PUSH 0\n");
        fprintf(codigo_mp, "GOTO L%d\n", segundaLabel);
        fprintf(codigo_mp, "LABEL L%d\n", primeiraLabel);
        fprintf(codigo_mp, "PUSH 1\n");
        fprintf(codigo_mp, "LABEL L%d\n", segundaLabel);
        break;

      case MAIOR:
        fprintf(codigo_mp, "SUB\n");
        primeiraLabel = ++ultimoRotulo;
        segundaLabel = ++ultimoRotulo;
        fprintf(codigo_mp, "GOTRUE L%d\n", primeiraLabel);
        fprintf(codigo_mp, "PUSH 0\n");
        fprintf(codigo_mp, "GOTO L%d\n", segundaLabel);
        fprintf(codigo_mp, "LABEL L%d\n", primeiraLabel);
        fprintf(codigo_mp, "PUSH 1\n");
        fprintf(codigo_mp, "LABEL L%d\n", segundaLabel);
        break;

      case MAIOR_IGUAL:
        fprintf(codigo_mp, "SUB\n");
        primeiraLabel = ++ultimoRotulo;
        segundaLabel = ++ultimoRotulo;
        fprintf(codigo_mp, "GOFALSE L%d\n", primeiraLabel);
        fprintf(codigo_mp, "PUSH 1\n");
        fprintf(codigo_mp, "GOTO L%d\n", segundaLabel);
        fprintf(codigo_mp, "LABEL L%d\n", primeiraLabel);
        fprintf(codigo_mp, "PUSH 0\n");
        fprintf(codigo_mp, "LABEL L%d\n", segundaLabel);
        break;
    }
    // OpRel();
    return T_BOOL;
  }
  return primeiro_tipo;
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
          int procPosicao = BuscarTabela(t.lexema);
          if (procPosicao == -1)
            erro("Procedimento nao declarado");
          int rotuloProc = tabela_idef.tabela_simb[procPosicao].rotulo;
          t = AnaLex(fd);
          if (t.cat == SN && t.codigo == A_PARENTESE) {
            PrintNodo("(", MANTEM);
            t = AnaLex(fd);
            if (!(t.cat == SN && t.codigo == F_PARENTESE)) {
              procPosicao++;
              t.processado = FALSE;
              int primeiro_tipo_expressao = Expr();
              if (tabela_idef.tabela_simb[procPosicao].categoria == PARAMETRO) {
                comparaTipos(primeiro_tipo_expressao,
                             tabela_idef.tabela_simb[procPosicao].tipo);
                if (tabela_idef.tabela_simb[procPosicao].passagem ==
                        REFERENCIA &&
                    ultimoSimbolo.is_const)
                  erro("Parametro constante nao pode ser passado por "
                       "referencia");
              } else
                erro("Incompatibilidade de parametros");
              fprintf(codigo_mp, "PUT\n");
              while (t.cat == SN && t.codigo == VIRGULA) {
                PrintNodo(",", MANTEM);
                procPosicao++;
                int tipo_expressao = Expr();
                if (tabela_idef.tabela_simb[procPosicao].categoria == PARAMETRO)
                  comparaTipos(tipo_expressao,
                               tabela_idef.tabela_simb[procPosicao].tipo);
                else
                  erro("Incompatibilidade de parametros");
                fprintf(codigo_mp, "PUT\n");
              }
            }
            /*
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
            */
            if (t.cat == SN && t.codigo == F_PARENTESE) {
              if (tabela_idef.tabela_simb[procPosicao + 1].categoria ==
                  PARAMETRO)
                erro("Alguns parametros nao foram declarados");
              PrintNodo(")", MANTEM);
              fprintf(codigo_mp, "CALL L%d\n", rotuloProc);
            } else
              erro("Fecha parenteses esperado");
          } else
            erro("Abre parenteses esperado");
        } else
          erro("identificador esperado");
        break;

      case WHILE:
        PrintNodo("while", MANTEM);
        t = AnaLex(fd);
        int rotulo_while = ultimoRotulo;
        int rotulo_end_while = ++ultimoRotulo;
        fprintf(codigo_mp, "LABEL L%d\n", rotulo_while);
        if (t.cat == SN && t.codigo == A_PARENTESE) {
          PrintNodo("(", MANTEM);
          Expr();
          // t = AnaLex(fd);
          if (t.cat == SN && t.codigo == F_PARENTESE) {
            PrintNodo(")", MANTEM);
            fprintf(codigo_mp, "GOFALSE L%d\n", rotulo_end_while);
            t = AnaLex(fd);
            Cmd();
            if (t.cat == PAL_R && t.codigo == ENDW) {
              PrintNodo("endw", MANTEM);
              fprintf(codigo_mp, "GOTO L%d\n", rotulo_while);
              fprintf(codigo_mp, "LABEL L%d\n", rotulo_end_while);
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
            fprintf(codigo_mp, "GOFALSE L%d\n", ultimoRotulo);
            Cmd();
            int label_exit = ++ultimoRotulo;
            fprintf(codigo_mp, "GOTO L%d\n", label_exit);
            fprintf(codigo_mp, "LABEL L%d\n", ultimoRotulo);
            ultimoRotulo++;
            while (t.cat == PAL_R && t.codigo == ELIF) {
              PrintNodo("elif", MANTEM);
              t = AnaLex(fd);
              if (t.cat == SN && t.codigo == A_PARENTESE) {
                PrintNodo("(", MANTEM);
                Expr();
                if (t.cat == SN && t.codigo == F_PARENTESE) {
                  PrintNodo(")", MANTEM);
                  t = AnaLex(fd);
                  fprintf(codigo_mp, "GOFALSE L%d\n", ultimoRotulo);
                  Cmd();
                  fprintf(codigo_mp, "GOTO L%d\n", label_exit);
                  fprintf(codigo_mp, "LABEL L%d\n", ultimoRotulo);
                  ultimoRotulo++;
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
              fprintf(codigo_mp, "LABEL L%d\n", label_exit);
            } else
              erro("endi esperado");
          } else
            erro("Fecha parenteses esperado");
        } else
          erro("Abre parenteses esperado");
        break;

      case GETOUT:
        PrintNodo("getout", MANTEM);
        fprintf(codigo_mp, "GETOUT\n");
        break;

      case GETINT:
        PrintNodo("getint", MANTEM);
        t = AnaLex(fd);
        if (t.cat == ID) {
          int posicaoTabela = BuscarTabela(t.lexema);
          if (posicaoTabela == -1)
            erro("Identificador nao declarado");
          if (tabela_idef.tabela_simb[posicaoTabela].tipo != T_INT)
            erro("Tipo inteiro esperado");
          fprintf(codigo_mp, "GETINT %d,%d\n",
                 tabela_idef.tabela_simb[posicaoTabela].escopo - 1,
                 tabela_idef.tabela_simb[posicaoTabela].endereco);
          PrintNodo(t.lexema, MANTEM);
        } else
          erro("identificador esperado");
        break;

      case GETREAL:
        PrintNodo("getreal", MANTEM);
        t = AnaLex(fd);
        if (t.cat == ID) {
          PrintNodo(t.lexema, MANTEM);
          int posicaoTabela = BuscarTabela(t.lexema);
          if (posicaoTabela == -1)
            erro("Identificador nao declarado");
          if (tabela_idef.tabela_simb[posicaoTabela].tipo != T_REAL)
            erro("Tipo real esperado");
          fprintf(codigo_mp, "GETREAL %d,%d\n",
                 tabela_idef.tabela_simb[posicaoTabela].escopo - 1,
                 tabela_idef.tabela_simb[posicaoTabela].endereco);
        } else
          erro("identificador esperado");
        break;

      case GETCHAR:
        PrintNodo("getchar", MANTEM);
        t = AnaLex(fd);
        if (t.cat == ID) {
          PrintNodo(t.lexema, MANTEM);
          int posicaoTabela = BuscarTabela(t.lexema);
          if (posicaoTabela == -1)
            erro("Identificador nao declarado");
          if (tabela_idef.tabela_simb[posicaoTabela].tipo != T_CHAR)
            erro("Tipo char esperado");
          fprintf(codigo_mp, "GETCHAR %d,%d\n",
                 tabela_idef.tabela_simb[posicaoTabela].escopo - 1,
                 tabela_idef.tabela_simb[posicaoTabela].endereco);
        } else
          erro("identificador esperado");
        break;

      case PUTINT:
        PrintNodo("putint", MANTEM);
        t = AnaLex(fd);
        if (t.cat != ID)
          erro("identificador esperado");
        if (t.cat == ID) {
          int posicaoTabela = BuscarTabela(t.lexema);
          if (posicaoTabela == -1)
            erro("Identificador nao declarado");
          if (tabela_idef.tabela_simb[posicaoTabela].tipo != T_INT)
            erro("Tipo inteiro esperado");
          fprintf(codigo_mp, "PUTINT %d,%d\n",
                 tabela_idef.tabela_simb[posicaoTabela].escopo - 1,
                 tabela_idef.tabela_simb[posicaoTabela].endereco);
          PrintNodo(t.lexema, MANTEM);
        }
        break;

      case PUTREAL:
        PrintNodo("putreal", MANTEM);
        t = AnaLex(fd);
        if (t.cat != ID)
          erro("identificador esperado");
        if (t.cat == ID) {
          PrintNodo(t.lexema, MANTEM);
          int posicaoTabela = BuscarTabela(t.lexema);
          if (posicaoTabela == -1)
            erro("Identificador nao declarado");
          if (tabela_idef.tabela_simb[posicaoTabela].tipo != T_REAL)
            erro("Tipo real esperado");
          fprintf(codigo_mp, "PUREAL %d,%d\n",
                 tabela_idef.tabela_simb[posicaoTabela].escopo - 1,
                 tabela_idef.tabela_simb[posicaoTabela].endereco);
        }
        break;

      case PUTCHAR:
        PrintNodo("putchar", MANTEM);
        t = AnaLex(fd);
        if (t.cat != ID && t.cat != CT_C)
          erro("identificador ou caracter esperada");
        if (t.cat == ID) {
          PrintNodo(t.lexema, MANTEM);
          int posicaoTabela = BuscarTabela(t.lexema);
          if (posicaoTabela == -1)
            erro("Identificador nao declarado");
          if (tabela_idef.tabela_simb[posicaoTabela].tipo != T_CHAR)
            erro("Tipo char esperado");
          fprintf(codigo_mp, "PUTCHAR %d,%d\n",
                 tabela_idef.tabela_simb[posicaoTabela].escopo - 1,
                 tabela_idef.tabela_simb[posicaoTabela].endereco);
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
  fprintf(codigo_mp, "INIP\n");
  PrintNodo("<Prog>", AVANCA);
  FlushSimbolo();
  ultimoSimbolo.escopo = GLOBAL;
  t = AnaLex(fd);
  while (t.cat != FIM_ARQ) {
    DeclListVar();
    fprintf(codigo_mp, "GOTO L1\n");
    DeclDefProc();
  }
  PrintNodo("", RETROCEDE);
}
