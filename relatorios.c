#include "relatorios.h"
#include "drone.h"
#include "operador.h"
#include "missao.h"
#include "evento.h"
#include "historico.h"
#include "fila.h"
#include "consulta.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

/* ─────────────────────────────────────────────
   HELPERS DE FORMATAÇÃO
   ───────────────────────────────────────────── */
static void imprimirSeparador(int largura) {
    int i;
    for (i = 0; i < largura; i++) printf("-");
    printf("\n");
}

static void imprimirCabecalho(const char *titulo) {
    printf("\n");
    imprimirSeparador(70);
    printf("  ORION CAMPUS — %s\n", titulo);
    imprimirSeparador(70);
}

/* ─────────────────────────────────────────────
   REL01 — STATUS GERAL DOS DRONES
   ───────────────────────────────────────────── */
void relStatusDrones(void) {
    int i;
    int contadores[DRONE_STATUS_COUNT];
    int k;

    imprimirCabecalho("RELATORIO DE STATUS DOS DRONES");

    if (totalDrones == 0) {
        printf("  Nenhum drone cadastrado.\n");
        imprimirSeparador(70);
        return;
    }

    for (k = 0; k < DRONE_STATUS_COUNT; k++) contadores[k] = 0;

    printf("  %-8s | %-20s | %-11s | %-5s | %-8s | %-10s\n",
           "ID", "MODELO", "STATUS", "BAT%", "CAP(kg)", "SETOR");
    imprimirSeparador(70);

    for (i = 0; i < totalDrones; i++) {
        Drone *d = &drones[i];
        printf("  %-8s | %-20s | %-11s | %-5d | %-8.2f | %-10s\n",
               d->id,
               d->modelo,
               strStatusDrone(d->status),
               d->nivelDisponibilidade,
               d->capacidade,
               d->setorVinc);
        if (d->status >= 0 && d->status < DRONE_STATUS_COUNT) {
            contadores[d->status]++;
        }
    }

    imprimirSeparador(70);
    printf("  Total cadastrados : %d\n", totalDrones);
    printf("  Disponiveis       : %d\n", contadores[DRONE_DISPONIVEL]);
    printf("  Em missao         : %d\n", contadores[DRONE_EM_MISSAO]);
    printf("  Manutencao        : %d\n", contadores[DRONE_MANUTENCAO]);
    printf("  Retornando        : %d\n", contadores[DRONE_RETORNANDO]);
    imprimirSeparador(70);
}

/* ─────────────────────────────────────────────
   REL02 — STATUS GERAL DOS OPERADORES
   ───────────────────────────────────────────── */
void relStatusOperadores(void) {
    int i;
    int contAtivo = 0, contAusente = 0, contEmMissao = 0;

    imprimirCabecalho("RELATORIO DE STATUS DOS OPERADORES");

    if (totalOperadores == 0) {
        printf("  Nenhum operador cadastrado.\n");
        imprimirSeparador(70);
        return;
    }

    printf("  %-8s | %-20s | %-20s | %-6s | %-10s\n",
           "MAT", "NOME", "CERTIFICACAO", "TURNO", "SITUACAO");
    imprimirSeparador(70);

    for (i = 0; i < totalOperadores; i++) {
        Operador *op = &operadores[i];
        printf("  %-8s | %-20s | %-20s | %-6s | %-10s\n",
               op->mat,
               op->nome,
               op->certificacao,
               strTurno(op->turno),
               strSituacaoOperador(op->situacao));
        switch (op->situacao) {
            case OP_ATIVO:     contAtivo++;    break;
            case OP_AUSENTE:   contAusente++;  break;
            case OP_EM_MISSAO: contEmMissao++; break;
            default: break;
        }
    }

    imprimirSeparador(70);
    printf("  Total cadastrados : %d\n", totalOperadores);
    printf("  Ativos            : %d\n", contAtivo);
    printf("  Em missao         : %d\n", contEmMissao);
    printf("  Ausentes          : %d\n", contAusente);
    imprimirSeparador(70);
}

/* ─────────────────────────────────────────────
   REL03 — MISSÕES POR STATUS
   ───────────────────────────────────────────── */
void relMissoesPorStatus(void) {
    int i, filtro, encontrou;
    int opcao;

    imprimirCabecalho("RELATORIO DE MISSOES POR STATUS");

    printf("  Filtrar por status?\n");
    printf("  0 - Todos\n");
    printf("  1 - PENDENTE\n");
    printf("  2 - EM_EXECUCAO\n");
    printf("  3 - CONCLUIDA\n");
    printf("  4 - CANCELADA\n");
    printf("  5 - REPLANEJADA\n");
    printf("  6 - INTERROMPIDA\n");
    opcao = lerOpcao("  Escolha: ", 0, 6);

    /* 0 = todos os status */
    filtro = (opcao >= 1 && opcao <= 6) ? (opcao - 1) : -1;

    if (totalMissoes == 0) {
        printf("  Nenhuma missao cadastrada.\n");
        imprimirSeparador(70);
        return;
    }

    printf("\n  %-8s | %-6s | %-5s | %-20s | %-13s | %-8s | %-8s\n",
           "CODIGO", "HORA", "PRIO", "AREA", "STATUS", "DRONE", "OPERADOR");
    imprimirSeparador(70);

    encontrou = 0;
    for (i = 0; i < totalMissoes; i++) {
        Missao *m = &missoes[i];
        if (filtro >= 0 && (int)m->status != filtro) continue;
        printf("  %-8s | %-6s | %-5d | %-20s | %-13s | %-8s | %-8s\n",
               m->codigo,
               m->horario,
               m->prioridade,
               m->area,
               strStatusMissao(m->status),
               strlen(m->droneAlocado) > 0 ? m->droneAlocado : "-",
               strlen(m->operadorAlocado) > 0 ? m->operadorAlocado : "-");
        encontrou = 1;
    }

    if (!encontrou) {
        printf("  Nenhuma missao encontrada para o filtro selecionado.\n");
    }
    imprimirSeparador(70);
}

/* ─────────────────────────────────────────────
   REL04 — FILA DE MISSÕES PENDENTES (ordenada)
   ───────────────────────────────────────────── */
static int compararHorarioMissao(const Missao *a, const Missao *b) {
    int cmp = compararHorario(a->horario, b->horario);
    if (cmp != 0) return cmp;
    return a->prioridade - b->prioridade;
}

void relAgendaDia(void) {
    Missao *lista[MAX_MISSOES];
    int total = 0;
    int i, j;

    imprimirCabecalho("AGENDA OPERACIONAL DO DIA");

    for (i = 0; i < totalMissoes; i++) {
        if (missoes[i].status != MISSAO_CANCELADA) {
            lista[total++] = &missoes[i];
        }
    }

    if (total == 0) {
        printf("  Nenhuma missao planejada cadastrada.\n");
        imprimirSeparador(70);
        return;
    }

    /* Ordenação estável simples por horário, usando insertion sort */
    for (i = 1; i < total; i++) {
        Missao *chave = lista[i];
        j = i - 1;
        while (j >= 0 && compararHorarioMissao(lista[j], chave) > 0) {
            lista[j + 1] = lista[j];
            j--;
        }
        lista[j + 1] = chave;
    }

    printf("  %-6s | %-8s | %-20s | %-10s | %-6s | %-10s\n",
           "HORARIO", "CODIGO", "AREA", "TIPO", "PRIO", "STATUS");
    imprimirSeparador(70);

    for (i = 0; i < total; i++) {
        Missao *m = lista[i];
        printf("  %-6s | %-8s | %-20s | %-10s | %-6d | %-10s\n",
               m->horario,
               m->codigo,
               m->area,
               strTipoMissao(m->tipo),
               m->prioridade,
               strStatusMissao(m->status));
    }

    imprimirSeparador(70);
}

void relMissoesPendentes(void) {
    FilaUnificada *fila;
    int i;

    imprimirCabecalho("FILA DE MISSOES E OCORRENCIAS PENDENTES (PRIORIDADE)");

    fila = construirFilaUnificada();
    if (fila == NULL) {
        printf("  Erro ao construir fila.\n");
        imprimirSeparador(70);
        return;
    }

    ordenarFila(fila);

    if (fila->total == 0) {
        printf("  Nenhuma missao ou ocorrencia pendente no momento.\n");
        liberarFila(fila);
        imprimirSeparador(70);
        return;
    }

    printf("  Convencao: prioridade 0 = URGENTE (eventos dinamicos)\n");
    printf("             prioridade >= 1 = missoes planejadas (menor = mais urgente)\n\n");

    printf("  %-3s | %-8s | %-9s | %-10s | %-6s | %-22s\n",
           "POS", "CODIGO", "ORIGEM", "PRIORIDADE", "HORARIO", "DESCRICAO");
    imprimirSeparador(70);

    for (i = 0; i < fila->total; i++) {
        ItemFila *item = &fila->itens[i];
        if (item->tipo == ITEM_MISSAO) {
            Missao *m = (Missao *)item->ptr;
            printf("  %-3d | %-8s | %-9s | %-10d | %-6s | %-22s\n",
                   i + 1,
                   m->codigo,
                   "PLANEJADA",
                   m->prioridade,
                   m->horario,
                   m->area);
        } else {
            NoEvento *n = (NoEvento *)item->ptr;
            printf("  %-3d | %-8s | %-9s | %-10s | %-6s | %-22s\n",
                   i + 1,
                   n->evento.codigo,
                   "DINAMICA",
                   "URGENTE",
                   n->evento.horarioAbertura,
                   strTipoEvento(n->evento.tipo));
        }
    }

    imprimirSeparador(70);
    printf("  Total na fila: %d\n", fila->total);
    imprimirSeparador(70);

    liberarFila(fila);
}

/* ─────────────────────────────────────────────
   REL05 — OCORRÊNCIAS ABERTAS E EM ATENDIMENTO
   ───────────────────────────────────────────── */
void relOcorrenciasAbertas(void) {
    NoEvento *atual;
    int encontrou = 0;
    int contAberto = 0, contAtendimento = 0;

    imprimirCabecalho("RELATORIO DE OCORRENCIAS ABERTAS / EM ATENDIMENTO");

    printf("  %-8s | %-20s | %-8s | %-14s | %-8s | %-10s\n",
           "CODIGO", "TIPO", "ABERTURA", "STATUS",
           "DRONE", "OPERADOR");
    imprimirSeparador(70);

    atual = cabecaEventos->prox;
    while (atual != caudaEventos) {
        if (atual->evento.status == EV_ABERTO ||
            atual->evento.status == EV_EM_ATENDIMENTO) {

            printf("  %-8s | %-20s | %-8s | %-14s | %-8s | %-10s\n",
                   atual->evento.codigo,
                   strTipoEvento(atual->evento.tipo),
                   atual->evento.horarioAbertura,
                   strStatusEvento(atual->evento.status),
                   strlen(atual->evento.droneAlocado) > 0
                       ? atual->evento.droneAlocado : "-",
                   strlen(atual->evento.operadorAlocado) > 0
                       ? atual->evento.operadorAlocado : "-");
            encontrou = 1;

            if (atual->evento.status == EV_ABERTO)
                contAberto++;
            else
                contAtendimento++;
        }
        atual = atual->prox;
    }

    if (!encontrou) {
        printf("  Nenhuma ocorrencia ativa no momento.\n");
    }

    imprimirSeparador(70);
    printf("  Abertos          : %d\n", contAberto);
    printf("  Em atendimento   : %d\n", contAtendimento);
    imprimirSeparador(70);
}

/* ─────────────────────────────────────────────
   REL06 — HISTÓRICO COMPLETO
   ───────────────────────────────────────────── */
void relHistorico(void) {
    imprimirCabecalho("HISTORICO DE ATIVIDADES (MAIS RECENTE AO MAIS ANTIGO)");

    if (historico.topo < 0) {
        printf("  Historico vazio.\n");
    } else {
        /* listarHistorico() já imprime cabeçalho e dados — não duplicar aqui */
        listarHistorico();
        imprimirSeparador(70);
        printf("  Total de registros: %d\n", historico.topo + 1);
    }

    imprimirSeparador(70);
}

/* ─────────────────────────────────────────────
   REL07 — ATIVIDADES POR DRONE
   Fonte canônica: histórico (R07/Skill1)
   ───────────────────────────────────────────── */
void relAtividadesPorDrone(void) {
    char idDrone[16];

    imprimirCabecalho("RELATORIO DE ATIVIDADES POR DRONE");

    if (totalDrones == 0) {
        printf("  Nenhum drone cadastrado.\n");
        imprimirSeparador(70);
        return;
    }

    printf("  ID do drone (ou ENTER para todos): ");
    lerString(idDrone, sizeof(idDrone));

    if (strlen(idDrone) == 0) {
        /* Exibir histórico de todos os drones */
        int i;
        for (i = 0; i < totalDrones; i++) {
            printf("\n  [ Drone: %-8s | Modelo: %-20s | Status: %-11s ]\n",
                   drones[i].id,
                   drones[i].modelo,
                   strStatusDrone(drones[i].status));
            filtrarPorDrone(drones[i].id);
        }
    } else {
        Drone *d = buscarDronePorId(idDrone);
        if (d == NULL) {
            printf("  [INFO] Drone '%s' nao encontrado.\n", idDrone);
            imprimirSeparador(70);
            return;
        }
        printf("\n  [ Drone: %-8s | Modelo: %-20s | Status: %-11s ]\n",
               d->id, d->modelo, strStatusDrone(d->status));
        filtrarPorDrone(idDrone);
    }

    imprimirSeparador(70);
}

/* ─────────────────────────────────────────────
   REL08 — ATIVIDADES POR OPERADOR
   Fonte canônica: histórico
   ───────────────────────────────────────────── */
void relAtividadesPorOperador(void) {
    char mat[16];

    imprimirCabecalho("RELATORIO DE ATIVIDADES POR OPERADOR");

    if (totalOperadores == 0) {
        printf("  Nenhum operador cadastrado.\n");
        imprimirSeparador(70);
        return;
    }

    printf("  Matricula (ou ENTER para todos): ");
    lerString(mat, sizeof(mat));

    if (strlen(mat) == 0) {
        int i;
        for (i = 0; i < totalOperadores; i++) {
            printf("\n  [ Operador: %-8s | Nome: %-20s | Turno: %-5s ]\n",
                   operadores[i].mat,
                   operadores[i].nome,
                   strTurno(operadores[i].turno));
            filtrarPorOperador(operadores[i].mat);
        }
    } else {
        Operador *op = buscarOperadorPorMatricula(mat);
        if (op == NULL) {
            printf("  [INFO] Operador '%s' nao encontrado.\n", mat);
            imprimirSeparador(70);
            return;
        }
        printf("\n  [ Operador: %-8s | Nome: %-20s | Turno: %-5s ]\n",
               op->mat, op->nome, strTurno(op->turno));
        filtrarPorOperador(mat);
    }

    imprimirSeparador(70);
}

/* ─────────────────────────────────────────────
   MENU DE RELATÓRIOS
   ───────────────────────────────────────────── */
void menuRelatorios(void) {
    int opcao;

    do {
        printf("\n==============================\n");
        printf("  RELATORIOS — ORION CAMPUS\n");
        printf("==============================\n");
        printf("  1  - Status dos Drones\n");
        printf("  2  - Status dos Operadores\n");
        printf("  3  - Missoes por Status\n");
        printf("  4  - Agenda Operacional do Dia\n");
        printf("  5  - Fila de Prioridade (Pendentes)\n");
        printf("  6  - Ocorrencias Abertas / Em Atendimento\n");
        printf("  7  - Historico Completo\n");
        printf("  8  - Atividades por Drone\n");
        printf("  9  - Atividades por Operador\n");
        printf("  0  - Voltar ao Menu Principal\n");
        printf("------------------------------\n");
        opcao = lerOpcao("  Opcao: ", 0, 9);

        switch (opcao) {
            case 1: relStatusDrones();       break;
            case 2: relStatusOperadores();   break;
            case 3: relMissoesPorStatus();   break;
            case 4: relAgendaDia();          break;
            case 5: relMissoesPendentes();   break;
            case 6: relOcorrenciasAbertas(); break;
            case 7: relHistorico();          break;
            case 8: relAtividadesPorDrone(); break;
            case 9: relAtividadesPorOperador(); break;
            case 0: break;
            default:
                printf("  [ERRO] Opcao invalida.\n");
        }
    } while (opcao != 0);
}
