#include "historico.h"
#include <stdio.h>
#include <string.h>

PilhaHistorico historico;

int empilhar(HistoricoEntry entry) {
    if (historico.topo >= MAX_HISTORICO - 1) {
        printf("[ERRO] Historico cheio. Nao foi possivel registrar a entrada.\n");
        return -1;
    }
    historico.dados[++(historico.topo)] = entry;
    return 0;
}

HistoricoEntry *consultarTopo(void) {
    if (historico.topo < 0) return NULL;
    return &historico.dados[historico.topo];
}

void listarHistorico(void) {
    int i;
    if (historico.topo < 0) {
        printf(" Historico vazio.\n");
        return;
    }
    printf(" %-8s | %-9s | %-13s | %-6s | %-6s | %-8s | %-10s\n",
           "CODIGO", "ORIGEM", "STATUS FINAL", "INICIO", "FIM", "DRONE", "OPERADOR");
    printf("---------+-----------+---------------+--------+-------+---------+-----------\n");
    for (i = historico.topo; i >= 0; i--) {
        HistoricoEntry *e = &historico.dados[i];
        printf(" %-8s | %-9s | %-13s | %-6s | %-6s | %-8s | %-10s\n",
               e->referenciaOrigem,
               strTipoOrigem(e->tipoOrigem),
               strStatusFinalHist(e->statusFinal),
               e->horarioInicioReal,
               e->horarioEncerramento,
               e->droneUtilizado,
               e->operadorResponsavel);
    }
}

void filtrarPorDrone(const char *idDrone) {
    int i;
    int encontrou = 0;
    for (i = historico.topo; i >= 0; i--) {
        if (strcmp(historico.dados[i].droneUtilizado, idDrone) == 0) {
            if (!encontrou) {
                printf(" %-8s | %-9s | %-13s | %-6s | %-6s\n",
                       "CODIGO", "ORIGEM", "STATUS FINAL", "INICIO", "FIM");
                printf("---------+-----------+---------------+--------+-------\n");
                encontrou = 1;
            }
            HistoricoEntry *e = &historico.dados[i];
            printf(" %-8s | %-9s | %-13s | %-6s | %-6s\n",
                   e->referenciaOrigem,
                   strTipoOrigem(e->tipoOrigem),
                   strStatusFinalHist(e->statusFinal),
                   e->horarioInicioReal,
                   e->horarioEncerramento);
        }
    }
    if (!encontrou) {
        printf(" Nenhuma entrada no historico para o drone '%s'.\n", idDrone);
    }
}

void filtrarPorOperador(const char *mat) {
    int i;
    int encontrou = 0;
    for (i = historico.topo; i >= 0; i--) {
        if (strcmp(historico.dados[i].operadorResponsavel, mat) == 0) {
            if (!encontrou) {
                printf(" %-8s | %-9s | %-13s | %-6s | %-6s\n",
                       "CODIGO", "ORIGEM", "STATUS FINAL", "INICIO", "FIM");
                printf("---------+-----------+---------------+--------+-------\n");
                encontrou = 1;
            }
            HistoricoEntry *e = &historico.dados[i];
            printf(" %-8s | %-9s | %-13s | %-6s | %-6s\n",
                   e->referenciaOrigem,
                   strTipoOrigem(e->tipoOrigem),
                   strStatusFinalHist(e->statusFinal),
                   e->horarioInicioReal,
                   e->horarioEncerramento);
        }
    }
    if (!encontrou) {
        printf(" Nenhuma entrada no historico para o operador '%s'.\n", mat);
    }
}

const char *strTipoOrigem(TipoOrigem t) {
    switch (t) {
        case ORIGEM_PLANEJADA: return "PLANEJADA";
        case ORIGEM_DINAMICA:  return "DINAMICA";
        default:               return "DESCONHECIDO";
    }
}

const char *strStatusFinalHist(StatusFinalHist s) {
    switch (s) {
        case HIST_CONCLUIDA:    return "CONCLUIDA";
        case HIST_CANCELADA:    return "CANCELADA";
        case HIST_INTERROMPIDA: return "INTERROMPIDA";
        case HIST_REPLANEJADA:  return "REPLANEJADA";
        default:                return "DESCONHECIDO";
    }
}
