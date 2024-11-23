#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>


Symbol tabelaSimbolos[100];
int simboloCount = 0;
Token tokens[1000];
int tokenCount = 0;

char palavrasReservadas[][20] = {"program", "var",   "integer", "real",
                                 "begin",   "end",   "if",      "then",
                                 "else",    "while", "do"};
char operadores[][10] = {":=", "=", ">=", "<=", "<>", ">",
                         "<",  "+", "-",  "*",  "/"};
char simbolos[][10] = {"{", "}", ",", ";", "(", ")"};

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

int ehSimbolo(char *lexema) {
  int i;
  for (i = 0; i < sizeof(simbolos) / sizeof(simbolos[0]); i++) {
    if (strcmp(simbolos[i], lexema) == 0) {
      return 1;
    }
  }
  return 0; 
}


void inicializarTabelaSimbolos() {
  int i;
  for (i = 0; i < sizeof(palavrasReservadas) / sizeof(palavrasReservadas[0]);
       i++) {
    inserirTabelaSimbolos(palavrasReservadas[i], "RESERVED");
  }
}

int buscarTabelaSimbolos(char *lexema) {
  int i;
  for (i = 0; i < simboloCount; i++) {
    if (strcmp(tabelaSimbolos[i].lexema, lexema) == 0) {
      return i;
    }
  }
  return -1;
}

void inserirTabelaSimbolos(char *lexema, char *tipo) {
  if (buscarTabelaSimbolos(lexema) == -1 && simboloCount < 100) {
    strcpy(tabelaSimbolos[simboloCount].lexema, lexema);
    strcpy(tabelaSimbolos[simboloCount].tipo, tipo);
    simboloCount++;
  }
}

Token getToken(FILE *fonte) {
  Token token;
  int estado = 0;
  int c;
  int i = 0;
  static int linha = 1, coluna = 0;
  int inicioColuna = coluna + 1;
  char lexema[100];

  while ((c = fgetc(fonte)) != EOF) {
    /*printf("DEBUG: Lendo caractere '%c' (linha: %d, coluna: %d)\n", c, linha,
           coluna);*/
    coluna++;

    if (c == ' ' || c == '\t' || c == '\n') {
      if (i > 0) { 
        lexema[i] = '\0';
        if (ehPalavraReservada(lexema)) {
          strcpy(token.nome, lexema); 
        } else {
          strcpy(token.nome, "ID");
        }
        strcpy(token.lexema, lexema);
        token.linha = linha;
        token.coluna = inicioColuna;
        /*printf("DEBUG: Gerando token -> Nome: %s, Lexema: %s, Linha: %d, "
               "Coluna: %d\n",
               token.nome, token.lexema, token.linha, token.coluna);*/
        return token;
      }
      if (c == '\n') { 
        linha++;
        coluna = 0;
      }
      continue; 
    }

    lexema[i++] = c;
    lexema[i] = '\0';


    switch (estado) {
    case 0: 
      if (isalpha(c)) {
        estado = 1; 
      } else if (isdigit(c)) {
        estado = 2;          
      } else if (c == ':') { 
        c = fgetc(fonte);
        if (c == '=') {
          strcpy(token.nome, "OP_ASS"); 
          token.linha = linha;
          token.coluna = inicioColuna;
          strcpy(token.lexema, ":=");
          /*printf("DEBUG: Gerando token -> Nome: %s, Lexema: %s, Linha: %d, "
                 "Coluna: %d\n",
                 token.nome, token.lexema, token.linha, token.coluna);*/
          return token;
        } else {
          ungetc(c, fonte);
          strcpy(token.nome, "OP_DP"); 
          token.linha = linha;
          token.coluna = inicioColuna;
          strcpy(token.lexema, ":");
          /*printf("DEBUG: Gerando token -> Nome: %s, Lexema: %s, Linha: %d, "
                 "Coluna: %d\n",
                 token.nome, token.lexema, token.linha, token.coluna);*/
          return token;
        }
      } else if (ehSimbolo(lexema)) {
        if (strcmp(lexema, ";") == 0) {
          strcpy(token.nome, "SMB_SEM"); // ponto e vírgula
        } else if (strcmp(lexema, ",") == 0) {
          strcpy(token.nome, "SMB_COM"); // vírgula
        } else if (strcmp(lexema, "(") == 0) {
          strcpy(token.nome, "SMB_OPA"); // parêntese esquerdo
        } else if (strcmp(lexema, ")") == 0) {
          strcpy(token.nome, "SMB_CPA"); // parêntese direito
        }
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, lexema);
        /*printf("DEBUG: Gerando token -> Nome: %s, Lexema: %s, Linha: %d, "
               "Coluna: %d\n",
               token.nome, token.lexema, token.linha, token.coluna);*/
        return token;
      } else {
        strcpy(token.nome, "ERRO");
        token.linha = linha;
        token.coluna = inicioColuna;
        strcpy(token.lexema, lexema);
        return token;
      }
      break;

    case 1:              
      if (!isalnum(c)) { 
        lexema[--i] = '\0';
        ungetc(c, fonte); 
        coluna--;

        if (ehPalavraReservada(lexema)) {
          strcpy(token.nome, lexema); 
        } else {
          strcpy(token.nome, "ID");
        }

        strcpy(token.lexema, lexema);
        token.linha = linha;
        token.coluna = inicioColuna;
        /*printf("DEBUG: Gerando token -> Nome: %s, Lexema: %s, Linha: %d, "
               "Coluna: %d\n",
               token.nome, token.lexema, token.linha, token.coluna);*/
        return token;
      }
      break;

    case 2: 
      if (isdigit(c)) {
        estado = 2; 
      } else {
        if (i > 0) {
          lexema[--i] = '\0'; 
          ungetc(c, fonte);  
          coluna--;
          strcpy(token.nome, "NUM_INT");
          strcpy(token.lexema, lexema);
          token.linha = linha;
          token.coluna = inicioColuna;
          /*printf("DEBUG: Gerando token -> Nome: NUM_INT, Lexema: %s, Linha: "
                 "%d, Coluna: %d\n",
                 token.lexema, token.linha, token.coluna);*/
          return token;
        }
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

  if (i > 0) {
    lexema[i] = '\0';
    if (ehPalavraReservada(lexema)) {
      strcpy(token.nome, lexema); 
    } else {
      strcpy(token.nome, "ID");
    }
    strcpy(token.lexema, lexema);
    token.linha = linha;
    token.coluna = inicioColuna;
    /*printf(
        "DEBUG: Gerando token -> Nome: %s, Lexema: %s, Linha: %d, Coluna: %d\n",
        token.nome, token.lexema, token.linha, token.coluna);*/
    return token;
  }

  strcpy(token.nome, "EOF");
  token.linha = linha;
  token.coluna = coluna;
  strcpy(token.lexema, "EOF");
  /*printf(
      "DEBUG: Gerando token -> Nome: %s, Lexema: %s, Linha: %d, Coluna: %d\n",
      token.nome, token.lexema, token.linha, token.coluna);*/
  return token;
}
