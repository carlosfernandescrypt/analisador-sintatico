#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token tokenAtual;

void erro(const char *mensagem) {
  printf("%d:%s [%s].\n", tokenAtual.linha, mensagem, tokenAtual.lexema);
  exit(1);
}

void CasaToken(const char *esperado) {
  if (strcmp(tokenAtual.nome, esperado) == 0) {
    tokenAtual = getToken(stdin);
  } else {
    printf("%d:token nao esperado [%s].", tokenAtual.linha, tokenAtual.nome);
    /*printf("Erro: Token esperado '%s', mas encontrado '%s' na linha %d\n",
           esperado, tokenAtual.nome, tokenAtual.linha, tokenAtual.coluna);*/
    exit(1);
  }
}

void Programa() {
  printf("Regra: programa -> 'program' ID ';' bloco '.'\n");
  CasaToken("program");
  CasaToken("ID");
  CasaToken("SMB_SEM");
  Bloco();
  CasaToken("SMB_DOT");
}

void Bloco() {
  printf("Regra: bloco -> declvar comando_composto\n");
  DeclVar();
  ComandoComposto();
}

void DeclVar() {
  if (strcmp(tokenAtual.nome, "var") == 0) {
    printf("Regra: declvar -> 'var' lista_decl_var\n");
    CasaToken("var");
    ListaDeclVar();
  } else {
    printf("Regra: declvar -> ε\n");
  }
}

void ListaDeclVar() {
  printf("Regra: lista_decl_var -> ID ':' tipo ';' lista_decl_var | ε\n");
  if (strcmp(tokenAtual.nome, "ID") == 0) {
    CasaToken("ID");
    CasaToken("OP_DP");
    Tipo();
    CasaToken("SMB_SEM");
    ListaDeclVar();
  } else {
    printf("Regra: lista_decl_var -> ε\n");
  }
}

void Tipo() {
  if (strcmp(tokenAtual.nome, "integer") == 0) {
    printf("Regra: tipo -> 'integer'\n");
    CasaToken("integer");
  } else if (strcmp(tokenAtual.nome, "real") == 0) {
    printf("Regra: tipo -> 'real'\n");
    CasaToken("real");
  } else {
    erro("tipo esperado");
  }
}

void ComandoComposto() {
  printf("Regra: comando_composto -> 'begin' lista_comandos 'end'\n");
  CasaToken("begin");
  ListaComandos();
  CasaToken("end");
}

void ListaComandos() {
  printf("Regra: lista_comandos -> comando ';' lista_comandos | ε\n");
  if (strcmp(tokenAtual.nome, "ID") == 0 ||
      strcmp(tokenAtual.nome, "begin") == 0) {
    Comando();
    CasaToken("SMB_SEM");
    ListaComandos();
  }
}

void Comando() {
  if (strcmp(tokenAtual.nome, "ID") == 0) {
    printf("Regra: comando -> ID ':=' expressao\n");
    CasaToken("ID");
    CasaToken("OP_ASS");
    Expressao();
  } else if (strcmp(tokenAtual.nome, "begin") == 0) {
    printf("Regra: comando -> comando_composto\n");
    ComandoComposto();
  } else {
    printf("Regra: comando -> ε\n");
  }
}

void Expressao() {
  printf("Regra: expressao -> termo\n");
  Termo();

  while (strcmp(tokenAtual.nome, "OP_AD") == 0 ||
         strcmp(tokenAtual.nome, "OP_SUB") == 0) {
    printf("Regra: expressao -> OP_AD/OP_SUB termo\n");
    CasaToken(tokenAtual.nome);
    Termo();
  }
}

void Termo() {
  printf("Regra: termo -> fator\n");
  Fator();

  while (strcmp(tokenAtual.nome, "OP_MUL") == 0 ||
         strcmp(tokenAtual.nome, "OP_DIV") == 0) {
    CasaToken(tokenAtual.nome);
    Fator();
  }
}

void Fator() {
  if (strcmp(tokenAtual.nome, "ID") == 0) {
    printf("Regra: fator -> ID\n");
    CasaToken("ID");
  } else if (strcmp(tokenAtual.nome, "NUM_INT") == 0) {
    printf("Regra: fator -> NUM_INT\n");
    CasaToken("NUM_INT");
  } else if (strcmp(tokenAtual.nome, "NUM_REAL") == 0) {
    printf("Regra: fator -> NUM_REAL\n");
    CasaToken("NUM_REAL");
  } else if (strcmp(tokenAtual.nome, "SMB_OPA") == 0) {
    printf("Regra: fator -> '(' expressao ')'\n");
    CasaToken("SMB_OPA");
    Expressao();
    CasaToken("SMB_CPA");
  } else {
    erro("fator esperado");
  }
}

int main() {
  printf("Iniciando o analisador sintático...\n");
  tokenAtual = getToken(stdin);
  Programa();
  if (strcmp(tokenAtual.nome, "EOF") == 0) {
    printf("Análise sintática concluída com sucesso.\n");
  } else {
    erro("fim de arquivo não esperado");
  }
  return 0;
}
