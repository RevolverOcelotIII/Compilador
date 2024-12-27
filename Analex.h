#ifndef ANALEX

#define ANALEX
#define TAM_MAX_LEXEMA 2000
#define TAM_TABELA_LITERAIS 2000

enum TOKEN_CAT { ID = 1, PAL_R, SN, CT_I, CT_R, CT_C, LT, FIM_ARQ };

/* Onde: ID: Identificador, PR: Palavra Reservada; SN: Sinal; CT_*: Constante
 * numerica ou caracter, LT: cadeia de caracter */

enum PAL_RESERV {
  CONST = 1,
  PR,
  INIT,
  ENDP,
  DEF,
  PROT,
  CHAR,
  INT,
  REAL,
  BOOL,
  DO,
  WHILE,
  ENDW,
  VAR,
  FROM,
  TO,
  DT,
  BY,
  IF,
  ENDV,
  ELIF,
  ELSE,
  ENDI,
  GETOUT,
  GETINT,
  GETCHAR,
  GETREAL,
  PUTINT,
  PUTCHAR,
  PUTREAL,
  PUTSTR
};

void incrementaLinha();

extern char *palavras_reservadas[];
extern int tamanho_pal_reservadas;

enum BOOL { FALSE, TRUE };

enum SINAIS {
  ADICAO,
  SUBTRACAO,
  MULTIPLICACAO,
  DIVISAO,
  MAIOR,
  MAIOR_IGUAL,
  MENOR,
  MENOR_IGUAL,
  COMPARACAO,
  ATRIBUICAO,
  A_PARENTESE,
  F_PARENTESE,
  MODIFICADOR,
  A_COLCHETE,
  F_COLCHETE,
  OU,
  IDENTIF_PONTEIRO,
  AND,
  NAO,
  NAO_IGUAL,
  APOSTOFO,
  ASPAS_D,
  VIRGULA,
  ENDERECO,
  A_CHAVES,
  F_CHAVES
};

extern char tabela_literais [TAM_TABELA_LITERAIS][TAM_MAX_LEXEMA];

void contaLinha();

typedef struct {
  enum TOKEN_CAT cat;
  enum BOOL processado;
  union {       // parte variavel do registro
    int codigo; // para tokens das categorias PR e SN
    int indice; // indica o indice da tabela onde o Analex armazenou um literal
    char lexema[TAM_MAX_LEXEMA]; // cadeia de caractares que corresponde o nome
                                 // do token da cat. ID
    int valInt;    // valor da constante ineira em tokens da cat. CT_I
    float valReal; // valor da constante ineira em tokens da cat. CT_R
    char caracter; // caracter do token da cat. CT_C
  };
} TOKEN; // Tipo TOKEN

extern TOKEN t;
extern FILE *fd;
TOKEN AnaLex(FILE *);

#endif