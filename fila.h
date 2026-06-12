#ifndef FILA_H
#define FILA_H

#include "drone.h"
#include "missao.h"
#include "evento.h"

/* Tamanho conservador: todas as missões + capacidade extra para eventos */
#define MAX_FILA_UNIFICADA (MAX_MISSOES + MAX_DRONES)

/* Discriminador de tipo — resolve o cast cego de void* (R02/Skill1, A03/Skill2) */
typedef enum {
    ITEM_MISSAO  = 0,
    ITEM_EVENTO  = 1
} TipoItem;

typedef struct {
    void    *ptr;   /* aponta para Missao* ou NoEvento* original (sem cópia) */
    TipoItem tipo;
    int      prioridade;    /* 0=urgente(evento), >=1 para missões */
    char     horario[6];    /* HH:MM — para desempate por horário (RN05) */
    int      ordemCadastro; /* posição no vetor/lista — desempate secundário */
} ItemFila;

typedef struct {
    ItemFila itens[MAX_FILA_UNIFICADA];
    int      total;
} FilaUnificada;

/* Monta vetor de ponteiros com missões PENDENTE + eventos ABERTO.
 * Retorna ponteiro para FilaUnificada estática interna. Caller pode
 * chamar liberarFila(), mas a operação é no-op para manter o sistema
 * sem malloc exceto para nós de evento dinâmico. */
FilaUnificada *construirFilaUnificada(void);

/* Ordena a fila por prioridade, horário e ordem de cadastro.
 * Para filas pequenas usa insertion sort estável.
 * Para filas maiores que 200 itens usa merge sort estável.
 * Convenção: menor valor numérico = maior prioridade.
 * Eventos dinâmicos: prioridade 0 (urgente, sempre no topo).
 * Missões planejadas: prioridade >= 1. */
void ordenarFila(FilaUnificada *f);

/* Retorna ponteiro para ItemFila de maior prioridade sem remover.
 * Retorna NULL se fila vazia. */
ItemFila *proximaDaFila(FilaUnificada *f);

/* Libera o vetor da FilaUnificada (não os dados originais) */
void liberarFila(FilaUnificada *f);

#endif /* FILA_H */
