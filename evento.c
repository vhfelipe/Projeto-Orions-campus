#include "evento.h"
#include <stdlib.h>
#include <string.h>

/* Sentinelas estáticos — nunca alocados com malloc */
static NoEvento sentinelaHead;
static NoEvento sentinelasTail;

/* Ponteiros globais para os sentinelas */
NoEvento *cabecaEventos = &sentinelaHead;
NoEvento *caudaEventos  = &sentinelasTail;

void inicializarListaEventos(void) {
    cabecaEventos->prox = caudaEventos;
    caudaEventos->prox  = NULL;
}

int removerEventoDaLista(const char *codigo) {
    NoEvento *anterior = cabecaEventos;
    NoEvento *atual    = cabecaEventos->prox;

    /* Percorre da cabeça até a cauda buscando o nó pelo código */
    while (atual != caudaEventos) {
        if (strcmp(atual->evento.codigo, codigo) == 0) {
            /* Sequência obrigatória: redirecionar ANTES de liberar */
            anterior->prox = atual->prox;
            atual->prox    = NULL;  /* isolar nó removido (defensive) */
            free(atual);
            return 0;
        }
        anterior = atual;
        atual    = atual->prox;
    }
    return -1; /* não encontrado */
}

const char *strStatusEvento(StatusEvento s) {
    switch (s) {
        case EV_ABERTO:         return "ABERTO";
        case EV_EM_ATENDIMENTO: return "EM_ATENDIMENTO";
        case EV_ENCERRADO:      return "ENCERRADO";
        default:                return "DESCONHECIDO";
    }
}

const char *strTipoEvento(TipoEvento t) {
    switch (t) {
        case EV_INVASAO:              return "INVASAO";
        case EV_ALAGAMENTO:           return "ALAGAMENTO";
        case EV_RISCO_ESTRUTURAL:     return "RISCO_ESTRUTURAL";
        case EV_PANE_ELETRICA:        return "PANE_ELETRICA";
        case EV_INSPECAO_EMERGENCIAL: return "INSPECAO_EMERGENCIAL";
        case EV_OUTRO:                return "OUTRO";
        default:                      return "DESCONHECIDO";
    }
}
