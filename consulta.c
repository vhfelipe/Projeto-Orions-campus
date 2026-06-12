#include "consulta.h"
#include "fila.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

Drone *buscarDronePorId(const char *id) {
    int i;
    if (id == NULL || strlen(id) == 0) return NULL;
    for (i = 0; i < totalDrones; i++) {
        if (strcmp(drones[i].id, id) == 0) {
            return &drones[i];
        }
    }
    return NULL;
}

Operador *buscarOperadorPorMatricula(const char *mat) {
    int i;
    if (mat == NULL || strlen(mat) == 0) return NULL;
    for (i = 0; i < totalOperadores; i++) {
        if (strcmp(operadores[i].mat, mat) == 0) {
            return &operadores[i];
        }
    }
    return NULL;
}

Missao *buscarMissaoPorCodigo(const char *codigo) {
    int i;
    if (codigo == NULL || strlen(codigo) == 0) return NULL;
    for (i = 0; i < totalMissoes; i++) {
        if (strcmp(missoes[i].codigo, codigo) == 0) {
            return &missoes[i];
        }
    }
    return NULL;
}

NoEvento *buscarEventoPorCodigo(const char *codigo) {
    NoEvento *atual;
    if (codigo == NULL || strlen(codigo) == 0) return NULL;
    /* Percorre da cabeça (exclusive) até a cauda (exclusive) */
    atual = cabecaEventos->prox;
    while (atual != caudaEventos) {
        if (strcmp(atual->evento.codigo, codigo) == 0) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

void listarMissoesPorPrioridade(void) {
    FilaUnificada *fila = construirFilaUnificada();
    int i;

    if (fila == NULL) {
        printf(" Erro ao construir fila de prioridade.\n");
        return;
    }

    ordenarFila(fila);

    if (fila->total == 0) {
        printf(" Nenhuma missao ou ocorrencia pendente.\n");
        liberarFila(fila);
        return;
    }

    printf(" %-3s | %-8s | %-9s | %-10s | %-6s | %-20s\n",
           "POS", "CODIGO", "ORIGEM", "PRIORIDADE", "HORARIO", "AREA");
    printf("-----+---------+-----------+------------+---------+---------------------\n");

    for (i = 0; i < fila->total; i++) {
        ItemFila *item = &fila->itens[i];
        if (item->tipo == ITEM_MISSAO) {
            Missao *m = (Missao *)item->ptr;
            printf(" %-3d | %-8s | %-9s | %-10d | %-6s | %-20s\n",
                   i + 1, m->codigo, "PLANEJADA", m->prioridade,
                   m->horario, m->area);
        } else {
            NoEvento *n = (NoEvento *)item->ptr;
            printf(" %-3d | %-8s | %-9s | %-10s | %-6s | %-20s\n",
                   i + 1, n->evento.codigo, "DINAMICA", "URGENTE",
                   n->evento.horarioAbertura, strTipoEvento(n->evento.tipo));
        }
    }

    liberarFila(fila);
}

void listarOcorrenciasAbertas(void) {
    NoEvento *atual = cabecaEventos->prox;
    int encontrou = 0;

    while (atual != caudaEventos) {
        if (atual->evento.status == EV_ABERTO ||
            atual->evento.status == EV_EM_ATENDIMENTO) {
            if (!encontrou) {
                printf(" %-8s | %-20s | %-8s | %-14s | %-8s | %-10s\n",
                       "CODIGO", "TIPO", "ABERTURA", "STATUS",
                       "DRONE", "OPERADOR");
                printf("---------+---------------------+----------+----------------+----------+-----------\n");
                encontrou = 1;
            }
            printf(" %-8s | %-20s | %-8s | %-14s | %-8s | %-10s\n",
                   atual->evento.codigo,
                   strTipoEvento(atual->evento.tipo),
                   atual->evento.horarioAbertura,
                   strStatusEvento(atual->evento.status),
                   strlen(atual->evento.droneAlocado) > 0 ? atual->evento.droneAlocado : "-",
                   strlen(atual->evento.operadorAlocado) > 0 ? atual->evento.operadorAlocado : "-");
        }
        atual = atual->prox;
    }

    if (!encontrou) {
        printf(" Nenhuma ocorrencia aberta ou em atendimento.\n");
    }
}

void menuBuscarDrone(void) {
    char id[16];
    Drone *d;
    printf("\n--- BUSCAR DRONE ---\n");
    printf("ID do drone: ");
    lerString(id, sizeof(id));
    d = buscarDronePorId(id);
    if (d == NULL) {
        printf("[INFO] Drone '%s' nao encontrado.\n", id);
        return;
    }
    printf("\n  ID       : %s\n", d->id);
    printf("  Modelo   : %s\n", d->modelo);
    printf("  Status   : %s\n", strStatusDrone(d->status));
    printf("  Autonomia: %d min\n", d->autonomia);
    printf("  Bateria  : %d%%\n", d->nivelDisponibilidade);
    printf("  Capacid. : %.2f kg\n", d->capacidade);
    printf("  Setor    : %s\n", d->setorVinc);
    printf("  Atividades vinculadas: %d\n", d->totalAtividades);
}

void menuBuscarOperador(void) {
    char mat[16];
    Operador *op;
    printf("\n--- BUSCAR OPERADOR ---\n");
    printf("Matricula: ");
    lerString(mat, sizeof(mat));
    op = buscarOperadorPorMatricula(mat);
    if (op == NULL) {
        printf("[INFO] Operador '%s' nao encontrado.\n", mat);
        return;
    }
    printf("\n  Matricula    : %s\n", op->mat);
    printf("  Nome         : %s\n", op->nome);
    printf("  Certificacao : %s\n", op->certificacao);
    printf("  Turno        : %s\n", strTurno(op->turno));
    printf("  Situacao     : %s\n", strSituacaoOperador(op->situacao));
}

void menuBuscarMissao(void) {
    char cod[16];
    Missao *m;
    printf("\n--- BUSCAR MISSAO ---\n");
    printf("Codigo: ");
    lerString(cod, sizeof(cod));
    m = buscarMissaoPorCodigo(cod);
    if (m == NULL) {
        printf("[INFO] Missao '%s' nao encontrada.\n", cod);
        return;
    }
    printf("\n  Codigo    : %s\n", m->codigo);
    printf("  Horario   : %s\n", m->horario);
    printf("  Prioridade: %d\n", m->prioridade);
    printf("  Area      : %s\n", m->area);
    printf("  Tipo      : %s\n", strTipoMissao(m->tipo));
    printf("  Duracao   : %d min\n", m->duracao);
    printf("  Status    : %s\n", strStatusMissao(m->status));
    printf("  Drone     : %s\n", strlen(m->droneAlocado) > 0 ? m->droneAlocado : "(nao alocado)");
    printf("  Operador  : %s\n", strlen(m->operadorAlocado) > 0 ? m->operadorAlocado : "(nao alocado)");
}
