#ifndef LEXER_H
#define LEXER_H

#include <stdio.h> 


typedef struct {
    char nome[20];
    char lexema[100];
    int linha;
    int coluna;
} Token;

typedef struct {
    char lexema[100];
    char tipo[20]; 
} Symbol;

extern Symbol tabelaSimbolos[100];
extern int simboloCount;
extern Token tokens[1000];
extern int tokenCount;

extern char palavrasReservadas[][20];
extern char operadores[][10];
extern char simbolos[][10];

// lexer
void inicializarTabelaSimbolos();
int buscarTabelaSimbolos(char *lexema);
void inserirTabelaSimbolos(char *lexema, char *tipo);
Token getToken(FILE *fonte);
void exibirTabelaSimbolos();
void registrarToken(char *nome, char *lexema, int linha, int coluna);
int ehPalavraReservada(char *lexema);
int ehOperador(char *lexema);
int ehSimbolo(char *lexema);

// parser 
void Programa();
void Bloco();
void DeclVar();
void ListaDeclVar();
void Tipo();
void ComandoComposto();
void ListaComandos();
void Comando();
void Expressao();
void Termo();
void Fator();

#endif
