#include "fila.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static FilaUnificada filaBuffer;
static ItemFila mergeBuffer[MAX_FILA_UNIFICADA];

FilaUnificada *construirFilaUnificada(void) {
    FilaUnificada *fila = &filaBuffer;
    NoEvento *atual;
    int i;
    int ordemEvento = 0;

    fila->total = 0;

    /* Adicionar missões PENDENTE — apenas este status entra na fila ativa */
    for (i = 0; i < totalMissoes; i++) {
        if (missoes[i].status == MISSAO_PENDENTE) {
            if (fila->total >= MAX_FILA_UNIFICADA) {
                printf("[AVISO] Fila unificada cheia. Alguns itens omitidos.\n");
                break;
            }
            fila->itens[fila->total].ptr          = &missoes[i];
            fila->itens[fila->total].tipo         = ITEM_MISSAO;
            fila->itens[fila->total].prioridade   = missoes[i].prioridade;
            strcpy_safe(fila->itens[fila->total].horario,
                        missoes[i].horario, sizeof(fila->itens[0].horario));
            fila->itens[fila->total].ordemCadastro = i; /* índice no vetor = ordem de cadastro */
            fila->total++;
        }
    }

    /* Adicionar eventos ABERTO — sempre prioridade 0 (urgente) */
    atual = cabecaEventos->prox;
    while (atual != caudaEventos) {
        if (atual->evento.status == EV_ABERTO) {
            if (fila->total >= MAX_FILA_UNIFICADA) {
                printf("[AVISO] Fila unificada cheia. Alguns eventos omitidos.\n");
                break;
            }
            fila->itens[fila->total].ptr          = atual;
            fila->itens[fila->total].tipo         = ITEM_EVENTO;
            fila->itens[fila->total].prioridade   = 0; /* urgente */
            strcpy_safe(fila->itens[fila->total].horario,
                        atual->evento.horarioAbertura, sizeof(fila->itens[0].horario));
            fila->itens[fila->total].ordemCadastro = ordemEvento++;
            fila->total++;
        }
        atual = atual->prox;
    }

    return fila;
}

static int compararItemFila(const ItemFila *a, const ItemFila *b) {
    int cmpPrio = a->prioridade - b->prioridade;
    if (cmpPrio != 0) {
        return cmpPrio;
    }

    int cmpHora = compararHorario(a->horario, b->horario);
    if (cmpHora != 0) {
        return cmpHora;
    }

    return a->ordemCadastro - b->ordemCadastro;
}

static void mergeSegmento(ItemFila *orig, ItemFila *temp, int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        if (compararItemFila(&orig[i], &orig[j]) <= 0) {
            temp[k++] = orig[i++];
        } else {
            temp[k++] = orig[j++];
        }
    }

    while (i <= mid) {
        temp[k++] = orig[i++];
    }
    while (j <= right) {
        temp[k++] = orig[j++];
    }
    for (i = left; i <= right; i++) {
        orig[i] = temp[i];
    }
}

static void mergeSortFila(ItemFila *orig, ItemFila *temp, int left, int right) {
    if (left >= right) {
        return;
    }
    int mid = left + (right - left) / 2;
    mergeSortFila(orig, temp, left, mid);
    mergeSortFila(orig, temp, mid + 1, right);
    mergeSegmento(orig, temp, left, mid, right);
}

void ordenarFila(FilaUnificada *f) {
    if (f == NULL || f->total <= 1) {
        return;
    }

    if (f->total > 200) {
        mergeSortFila(f->itens, mergeBuffer, 0, f->total - 1);
        return;
    }

    /* Insertion sort estável para filas pequenas. */
    int i, j;
    ItemFila chave;

    for (i = 1; i < f->total; i++) {
        chave = f->itens[i];
        j = i - 1;

        while (j >= 0) {
            if (compararItemFila(&f->itens[j], &chave) > 0) {
                f->itens[j + 1] = f->itens[j];
                j--;
            } else {
                break;
            }
        }
        f->itens[j + 1] = chave;
    }
}

ItemFila *proximaDaFila(FilaUnificada *f) {
    if (f == NULL || f->total == 0) {
        return NULL;
    }
    return &f->itens[0];
}

void liberarFila(FilaUnificada *f) {
    /* Não libera nada: a fila usa buffer estático interno. */
    (void)f;
}
