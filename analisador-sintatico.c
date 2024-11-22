#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEXEMA_SIZE 100
#define MAX_TOKENS 1000
#define MAX_SYMBOLS 100

/* estrutura do token */
typedef struct {
  char nome[20];
  char lexema[MAX_LEXEMA_SIZE];
  int linha;
  int coluna;
} Token;

/* estrutura tabela de simbolos */
typedef struct {
  char lexema[MAX_LEXEMA_SIZE];
  char tipo[20]; // pode ser identificador ou palavra reservada
} Symbol;

/* variaveis globais */
Symbol tabelaSimbolos[MAX_SYMBOLS];
int simboloCount = 0;

Token tokens[MAX_TOKENS];
int tokenCount = 0;

char palavrasReservadas[][20] = {"program", "var",   "integer", "real",
                                 "begin",   "end",   "if",      "then",
                                 "else",    "while", "do"};

char operadores[][10] = {":=", "=", ">=", "<=", "<>", ">",
                         "<",  "+", "-",  "*",  "/"};

char simbolos[][10] = {"{", "}", ",", ";", "(", ")"};

/* funcoes */
void inicializarTabelaSimbolos();
int buscarTabelaSimbolos(char *lexema);
void inserirTabelaSimbolos(char *lexema, char *tipo);
Token getToken(FILE *fonte);
void exibirTabelaSimbolos();
void registrarToken(char *nome, char *lexema, int linha, int coluna);
int ehPalavraReservada(char *lexema);
int ehOperador(char *lexema);
int ehSimbolo(char *lexema);

int tokenAtual = 0; //  token atual

void Programa();
void Bloco();
void Variaveis();
void Declaracoes();
void Tipo();
void Comandos();
void Comando();
void Atribuicao();
void Condicional();
void Expressao();
void ExpressaoCont();
void Termo();
void TermoCont();
void Fator();

int main(int argc, char *argv[]) {
  FILE *fonte;
  FILE *saida;
  char nomeArquivoFonte[100];
  char nomeArquivoSaida[100];
  Token token;
  int linha = 1, coluna = 0;

  if (argc < 2) {
    printf("Uso: %s <arquivo_fonte>\n", argv[0]);
    return 1;
  }

  strcpy(nomeArquivoFonte, argv[1]);
  strcpy(nomeArquivoSaida, "saida.lex");

  fonte = fopen(nomeArquivoFonte, "r");
  if (fonte == NULL) {
    printf("Erro ao abrir o arquivo fonte.\n");
    return 1;
  }

  saida = fopen(nomeArquivoSaida, "w");
  if (saida == NULL) {
    printf("Erro ao criar o arquivo de saida.\n");
    return 1;
  }

  inicializarTabelaSimbolos();

  while (!feof(fonte)) {
    token = getToken(fonte);
    if (strcmp(token.nome, "ERRO") == 0) {
      printf("Erro lexico na linha %d, coluna %d: Caractere invalido '%s'\n",
             token.linha, token.coluna, token.lexema);
      break;
    } else if (strcmp(token.nome, "EOF") != 0) {
      registrarToken(token.nome, token.lexema, token.linha, token.coluna);
    } else {
      registrarToken(token.nome, token.lexema, token.linha, token.coluna);
      break;
    }
  }

  // chamar o  símbolo inicial da gramática
  Programa();

  fclose(fonte);
  fclose(saida);

  printf("Analise lexica concluida. Tokens registrados em '%s'.\n",
         nomeArquivoSaida);
  printf("\nTabela de Simbolos:\n");
  exibirTabelaSimbolos();

  return 0;
}

// função pra casar o token atual com o token esperado
void CasaToken(char *tokenEsperado) {
  if (strcmp(tokens[tokenAtual].nome, tokenEsperado) == 0) {
    tokenAtual++;
  } else {
    printf("%d:token nao esperado [%s].\n", tokens[tokenAtual].linha,
           tokens[tokenAtual].lexema);
    exit(1);
  }
}

void Programa() {
  printf("programa -> program ID ; bloco .\n");
  CasaToken("program");
  CasaToken("ID");
  CasaToken("SMB_SEM");
  Bloco();
  CasaToken("SMB_PON");
}

void Bloco() {
  printf("bloco -> variaveis begin comandos end\n");
  Variaveis();
  CasaToken("begin");
  Comandos();
  CasaToken("end");
}

void Variaveis() {
  if (strcmp(tokens[tokenAtual].nome, "var") == 0) {
    printf("variaveis -> var declaracoes\n");
    CasaToken("var");
    Declaracoes();
  } else {
    printf("variaveis -> ε\n");
  }
}

void Declaracoes() {
  if (strcmp(tokens[tokenAtual].nome, "ID") == 0) {
    printf("declaracoes -> ID : tipo ; declaracoes\n");
    CasaToken("ID");
    CasaToken("OP_DP");
    Tipo();
    CasaToken("SMB_SEM");
    Declaracoes();
  } else {
    printf("declaracoes -> ε\n");
  }
}

void Tipo() {
  if (strcmp(tokens[tokenAtual].nome, "integer") == 0) {
    printf("tipo -> integer\n");
    CasaToken("integer");
  } else if (strcmp(tokens[tokenAtual].nome, "real") == 0) {
    printf("tipo -> real\n");
    CasaToken("real");
  } else {
    printf("%d:token nao esperado [%s]. Esperado 'integer' ou 'real'\n",
           tokens[tokenAtual].linha, tokens[tokenAtual].lexema);
    exit(1);
  }
}

void Comandos() {
  if (strcmp(tokens[tokenAtual].nome, "ID") == 0 ||
      strcmp(tokens[tokenAtual].nome, "if") == 0) {
    printf("comandos -> comando ; comandos\n");
    Comando();
    CasaToken("SMB_SEM");
    Comandos();
  } else {
    printf("comandos -> ε\n");
  }
}

void Comando() {
  if (strcmp(tokens[tokenAtual].nome, "ID") == 0) {
    printf("comando -> atribuicao\n");
    Atribuicao();
  } else if (strcmp(tokens[tokenAtual].nome, "if") == 0) {
    printf("comando -> condicional\n");
    Condicional();
  } else {
    printf("%d:token nao esperado [%s]. Esperado 'ID' ou 'if'\n",
           tokens[tokenAtual].linha, tokens[tokenAtual].lexema);
    exit(1);
  }
}

void Atribuicao() {
  printf("atribuicao -> ID := expressao\n");
  CasaToken("ID");
  CasaToken("OP_ASS");
  Expressao();
}

void Condicional() {
  printf("condicional -> if expressao then comando\n");
  CasaToken("if");
  Expressao();
  CasaToken("then");
  Comando();
}

void Expressao() {
  printf("expressao -> termo expressao_cont\n");
  Termo();
  ExpressaoCont();
}

void ExpressaoCont() {
  if (strcmp(tokens[tokenAtual].nome, "OP_AD") == 0) {
    printf("expressao_cont -> + termo expressao_cont\n");
    CasaToken("OP_AD");
    Termo();
    ExpressaoCont();
  } else if (strcmp(tokens[tokenAtual].nome, "OP_MIN") == 0) {
    printf("expressao_cont -> - termo expressao_cont\n");
    CasaToken("OP_MIN");
    Termo();
    ExpressaoCont();
  } else {
    printf("expressao_cont -> ε\n");
  }
}

void Termo() {
  printf("termo -> fator termo_cont\n");
  Fator();
  TermoCont();
}

void TermoCont() {
  if (strcmp(tokens[tokenAtual].nome, "OP_MUL") == 0) {
    printf("termo_cont -> * fator termo_cont\n");
    CasaToken("OP_MUL");
    Fator();
    TermoCont();
  } else if (strcmp(tokens[tokenAtual].nome, "OP_DIV") == 0) {
    printf("termo_cont -> / fator termo_cont\n");
    CasaToken("OP_DIV");
    Fator();
    TermoCont();
  } else {
    printf("termo_cont -> ε\n");
  }
}

void Fator() {
  if (strcmp(tokens[tokenAtual].nome, "ID") == 0) {
    printf("fator -> ID\n");
    CasaToken("ID");
  } else if (strcmp(tokens[tokenAtual].nome, "NUM_INT") == 0) {
    printf("fator -> NUM_INT\n");
    CasaToken("NUM_INT");
  } else if (strcmp(tokens[tokenAtual].nome, "NUM_REAL") == 0) {
    printf("fator -> NUM_REAL\n");
    CasaToken("NUM_REAL");
  } else if (strcmp(tokens[tokenAtual].nome, "SMB_OPA") == 0) {
    printf("fator -> ( expressao )\n");
    CasaToken("SMB_OPA");
    Expressao();
    CasaToken("SMB_CPA");
  } else {
    printf("%d:token nao esperado [%s]. Esperado 'ID', 'NUM_INT', 'NUM_REAL' "
           "ou '('\n",
           tokens[tokenAtual].linha, tokens[tokenAtual].lexema);
    exit(1);
  }
}

/* inicia a tabela de simbolos com paalvras reservadas */
void inicializarTabelaSimbolos() {
  int i;
  for (i = 0; i < sizeof(palavrasReservadas) / sizeof(palavrasReservadas[0]);
       i++) {
    inserirTabelaSimbolos(palavrasReservadas[i], "RESERVED");
  }
}

/* procura a tabela de simbolos */
int buscarTabelaSimbolos(char *lexema) {
  int i;
  for (i = 0; i < simboloCount; i++) {
    if (strcmp(tabelaSimbolos[i].lexema, lexema) == 0) {
      return i;
    }
  }
  return -1;
}

/* insere na tabela */
void inserirTabelaSimbolos(char *lexema, char *tipo) {
  if (buscarTabelaSimbolos(lexema) == -1 && simboloCount < MAX_SYMBOLS) {
    strcpy(tabelaSimbolos[simboloCount].lexema, lexema);
    strcpy(tabelaSimbolos[simboloCount].tipo, tipo);
    simboloCount++;
  }
}

/* mostra a tabela */
void exibirTabelaSimbolos() {
  int i;
  printf("Indice\tLexema\t\tTipo\n");
  for (i = 0; i < simboloCount; i++) {
    printf("%d\t%s\t\t%s\n", i, tabelaSimbolos[i].lexema,
           tabelaSimbolos[i].tipo);
  }
}

/* registra o token */
void registrarToken(char *nome, char *lexema, int linha, int coluna) {
  if (tokenCount < MAX_TOKENS) {
    strcpy(tokens[tokenCount].nome, nome);
    strcpy(tokens[tokenCount].lexema, lexema);
    tokens[tokenCount].linha = linha;
    tokens[tokenCount].coluna = coluna;
    tokenCount++;
  }
}

/* verifica se o lexema é uma palavra reservada */
int ehPalavraReservada(char *lexema) {
  int i;
  for (i = 0; i < sizeof(palavrasReservadas) / sizeof(palavrasReservadas[0]);
       i++) {
    if (strcmp(palavrasReservadas[i], lexema) == 0) {
      return 1;
    }
  }
  return 0;
}

/* ve se o lexema é um operador */
int ehOperador(char *lexema) {
  int i;
  for (i = 0; i < sizeof(operadores) / sizeof(operadores[0]); i++) {
    if (strcmp(operadores[i], lexema) == 0) {
      return 1;
    }
  }
  return 0;
}

/* ve se o lexema é um simbolo */
int ehSimbolo(char *lexema) {
  int i;
  for (i = 0; i < sizeof(simbolos) / sizeof(simbolos[0]); i++) {
    if (strcmp(simbolos[i], lexema) == 0) {
      return 1;
    }
  }
  return 0;
}

/* implementação do automato */
Token getToken(FILE *fonte) {
  Token token;
  int estado = 0;
  int c;
  int i = 0;
  static int linha = 1, coluna = 0;
  int inicioColuna = coluna + 1;
  char lexema[MAX_LEXEMA_SIZE];

  while ((c = fgetc(fonte)) != EOF) {
    coluna++;

    /* pula espaços e linhas vazias */
    if (c == ' ' || c == '\t') {
      continue;
    } else if (c == '\n') {
      linha++;
      coluna = 0;
      continue;
    }

    /* processamento de tokens */
    lexema[i++] = c;
    lexema[i] = '\0';

    /* estados do automato */
    switch (estado) {
    case 0:
      if (isalpha(c)) {
        estado = 1; // identificar paalvra reservada
      } else if (isdigit(c)) {
        estado = 2; // numero
      } else if (c == ':') {
        estado = 3; // pode ser ":" ou pode ser ":="
      } else if (c == '<') {
        estado = 4; // pode ser '<', '<=', ou '<>'
      } else if (c == '>') {
        estado = 5; // pode ser '>' ou '>='
      } else if (c == '+') {
        strcpy(token.nome, "OP_AD");
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, "+");
        return token;
      } else if (c == '-') {
        strcpy(token.nome, "OP_MIN");
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, "-");
        return token;
      } else if (c == '*') {
        strcpy(token.nome, "OP_MUL");
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, "*");
        return token;
      } else if (c == '/') {
        strcpy(token.nome, "OP_DIV");
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, "/");
        return token;
      } else if (c == '=') {
        strcpy(token.nome, "OP_EQ");
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, "=");
        return token;
      } else if (ehSimbolo(lexema)) {
        if (strcmp(lexema, ";") == 0) {
          strcpy(token.nome, "SMB_SEM");
        } else if (strcmp(lexema, ",") == 0) {
          strcpy(token.nome, "SMB_COM");
        } else if (strcmp(lexema, "(") == 0) {
          strcpy(token.nome, "SMB_OPA");
        } else if (strcmp(lexema, ")") == 0) {
          strcpy(token.nome, "SMB_CPA");
        } else if (strcmp(lexema, "{") == 0) {
          strcpy(token.nome, "SMB_OBC");
        } else if (strcmp(lexema, "}") == 0) {
          strcpy(token.nome, "SMB_CBC");
        }
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, lexema);
        return token;
      } else {
        /* caractere não reconhecido */
        strcpy(token.nome, "ERRO");
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, lexema);
        return token;
      }
      break;

    case 1:
      if (isalnum(c)) {
        estado = 1;
      } else {
        lexema[--i] = '\0'; // remove ultimo caractere
        ungetc(c, fonte);
        coluna--;
        if (ehPalavraReservada(lexema)) {
          strcpy(token.nome, lexema);
          inserirTabelaSimbolos(lexema, "RESERVED");
        } else {
          strcpy(token.nome, "ID");
          inserirTabelaSimbolos(lexema, "ID");
        }
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, lexema);
        return token;
      }
      break;

    case 2:
      if (isdigit(c)) {
        estado = 2;
      } else if (c == '.') {
        estado = 6; // número real
      } else {
        lexema[--i] = '\0';
        ungetc(c, fonte);
        coluna--;
        strcpy(token.nome, "NUM_INT");
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, lexema);
        return token;
      }
      break;

    case 3:
      if (c == '=') {
        strcpy(token.nome, "OP_ASS");
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, ":=");
        return token;
      } else {
        lexema[--i] = '\0';
        ungetc(c, fonte);
        coluna--;
        strcpy(token.nome, "OP_DP");
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, ":");
        return token;
      }
      break;

    case 4:
      if (c == '=') {
        strcpy(token.nome, "OP_LE");
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, "<=");
        return token;
      } else if (c == '>') {
        strcpy(token.nome, "OP_NE");
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, "<>");
        return token;
      } else {
        lexema[--i] = '\0';
        ungetc(c, fonte);
        coluna--;
        strcpy(token.nome, "OP_LT");
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, "<");
        return token;
      }
      break;

    case 5:
      if (c == '=') {
        strcpy(token.nome, "OP_GE");
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, ">=");
        return token;
      } else {
        lexema[--i] = '\0';
        ungetc(c, fonte);
        coluna--;
        strcpy(token.nome, "OP_GT");
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, ">");
        return token;
      }
      break;

    case 6:
      if (isdigit(c)) {
        estado = 6;
      } else {
        lexema[--i] = '\0';
        ungetc(c, fonte);
        coluna--;
        strcpy(token.nome, "NUM_REAL");
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, lexema);
        return token;
      }
      break;

    default:
      strcpy(token.nome, "ERRO");
      token.linha = linha;
      token.coluna = inicioColuna;
      strcpy(token.lexema, lexema);
      return token;
    }
  }

  strcpy(token.nome, "EOF");
  token.linha = linha;
  token.coluna = coluna;
  strcpy(token.lexema, "EOF");
  return token;
}
