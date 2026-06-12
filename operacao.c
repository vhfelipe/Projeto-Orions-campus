#include "operacao.h"
#include "cadastro.h"
#include "consulta.h"
#include "historico.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

/* ─────────────────────────────────────────────
   HELPER: liberar recursos de drone e operador
   ───────────────────────────────────────────── */
static void liberarDrone(Drone *d) {
    if (d == NULL) return;
    d->status = DRONE_RETORNANDO;
}

static void liberarOperador(Operador *op) {
    if (op == NULL) return;
    op->situacao = OP_ATIVO;
}

/* ─────────────────────────────────────────────
   HELPER: montar e empilhar entrada no histórico
   com tratamento de falha de empilhamento (L06)
   ───────────────────────────────────────────── */
static int empilharComVerificacao(HistoricoEntry *entry,
                                   const char *operacao) {
    int ret = empilhar(*entry);
    if (ret != 0) {
        printf("[AVISO] Historico cheio. Entrada de '%s' nao registrada no historico.\n",
               operacao);
        /* Retorna 0 pois a operação principal prossegue; apenas o histórico falhou */
        return 0;
    }
    return 0;
}

/* ─────────────────────────────────────────────
   ALOCAR RECURSOS — MISSÃO PLANEJADA
   BLOCO 1: todas as validações antes de qualquer atribuição
   BLOCO 2: todas as atribuições após validação completa
   ───────────────────────────────────────────── */
int alocarRecursos(const char *codMissao, const char *idDrone, const char *matOp) {
    Missao   *m;
    Drone    *d;
    Operador *op;
    int i;

    /* BLOCO 1 — VALIDAÇÕES APENAS */
    m = buscarMissaoPorCodigo(codMissao);
    if (m == NULL) {
        printf("[ERRO] Missao '%s' nao encontrada.\n", codMissao);
        return -1;
    }
    if (m->status != MISSAO_PENDENTE) {
        printf("[ERRO] Missao '%s' nao esta PENDENTE (status atual: %s).\n",
               codMissao, strStatusMissao(m->status));
        return -1;
    }

    d = buscarDronePorId(idDrone);
    if (d == NULL) {
        printf("[ERRO] Drone '%s' nao encontrado.\n", idDrone);
        return -1;
    }
    if (d->status != DRONE_DISPONIVEL) {
        printf("[ERRO] Drone '%s' nao esta DISPONIVEL (status atual: %s). RN01/RN03.\n",
               idDrone, strStatusDrone(d->status));
        return -1;
    }

    op = buscarOperadorPorMatricula(matOp);
    if (op == NULL) {
        printf("[ERRO] Operador '%s' nao encontrado.\n", matOp);
        return -1;
    }
    if (op->situacao != OP_ATIVO) {
        printf("[ERRO] Operador '%s' nao esta ATIVO (situacao atual: %s).\n",
               matOp, strSituacaoOperador(op->situacao));
        return -1;
    }

    /* RN02: verificar turno do operador compatível com horário da missão */
    {
        int hh = (m->horario[0] - '0') * 10 + (m->horario[1] - '0');
        Turno turnoRequerido;
        if (hh >= 6 && hh < 12)       turnoRequerido = TURNO_MANHA;
        else if (hh >= 12 && hh < 18) turnoRequerido = TURNO_TARDE;
        else                           turnoRequerido = TURNO_NOITE;

        if (op->turno != turnoRequerido) {
            printf("[ERRO] Operador '%s' (turno %s) incompativel com horario %s da missao (turno %s). RN02.\n",
                   matOp, strTurno(op->turno), m->horario, strTurno(turnoRequerido));
            return -1;
        }
    }

    /* RN09: verificar se drone já está alocado em outra missão */
    for (i = 0; i < totalMissoes; i++) {
        if (missoes[i].status == MISSAO_PENDENTE ||
            missoes[i].status == MISSAO_EM_EXECUCAO) {
            if (strcmp(missoes[i].droneAlocado, idDrone) == 0 &&
                strcmp(missoes[i].codigo, codMissao) != 0) {
                printf("[ERRO] Drone '%s' ja esta alocado na missao '%s'. RN09.\n",
                       idDrone, missoes[i].codigo);
                return -1;
            }
            if (strcmp(missoes[i].operadorAlocado, matOp) == 0 &&
                strcmp(missoes[i].codigo, codMissao) != 0) {
                printf("[ERRO] Operador '%s' ja esta alocado na missao '%s'. RN09.\n",
                       matOp, missoes[i].codigo);
                return -1;
            }
        }
    }

    /* RN09: verificar se drone/operador estão alocados em eventos ativos */
    {
        NoEvento *ev = cabecaEventos->prox;
        while (ev != caudaEventos) {
            if (ev->evento.status == EV_ABERTO ||
                ev->evento.status == EV_EM_ATENDIMENTO) {
                if (strlen(ev->evento.droneAlocado) > 0 &&
                    strcmp(ev->evento.droneAlocado, idDrone) == 0) {
                    printf("[ERRO] Drone '%s' ja esta alocado no evento '%s'. RN09.\n",
                           idDrone, ev->evento.codigo);
                    return -1;
                }
                if (strlen(ev->evento.operadorAlocado) > 0 &&
                    strcmp(ev->evento.operadorAlocado, matOp) == 0) {
                    printf("[ERRO] Operador '%s' ja esta alocado no evento '%s'. RN09.\n",
                           matOp, ev->evento.codigo);
                    return -1;
                }
            }
            ev = ev->prox;
        }
    }

    /* BLOCO 2 — ATRIBUIÇÕES APENAS — zero validações aqui */
    strcpy_safe(m->droneAlocado,    idDrone, sizeof(m->droneAlocado));
    strcpy_safe(m->operadorAlocado, matOp,   sizeof(m->operadorAlocado));
    /* Status da missão permanece PENDENTE após alocação */

    printf("[OK] Recursos alocados para missao '%s': drone '%s', operador '%s'.\n",
           codMissao, idDrone, matOp);
    return 0;
}

/* ─────────────────────────────────────────────
   INICIAR MISSÃO
   ───────────────────────────────────────────── */
int iniciarMissao(const char *codMissao) {
    Missao   *m;
    Drone    *d;
    Operador *op;

    m = buscarMissaoPorCodigo(codMissao);
    if (m == NULL) {
        printf("[ERRO] Missao '%s' nao encontrada.\n", codMissao);
        return -1;
    }
    if (m->status != MISSAO_PENDENTE) {
        printf("[ERRO] Missao '%s' nao esta PENDENTE (status atual: %s).\n",
               codMissao, strStatusMissao(m->status));
        return -1;
    }
    if (strlen(m->droneAlocado) == 0) {
        printf("[ERRO] Missao '%s' nao possui drone alocado.\n", codMissao);
        return -1;
    }
    if (strlen(m->operadorAlocado) == 0) {
        printf("[ERRO] Missao '%s' nao possui operador alocado.\n", codMissao);
        return -1;
    }

    d  = buscarDronePorId(m->droneAlocado);
    op = buscarOperadorPorMatricula(m->operadorAlocado);

    /* Aplicar transições de estado */
    m->status   = MISSAO_EM_EXECUCAO;
    if (d  != NULL) d->status    = DRONE_EM_MISSAO;
    if (op != NULL) op->situacao = OP_EM_MISSAO;

    /* Registrar atividade vinculada ao drone */
    if (d != NULL && d->totalAtividades < MAX_ATIV_DRONE) {
        strcpy_safe(d->atividadesVinculadas[d->totalAtividades],
                    codMissao, 16);
        d->totalAtividades++;
    }

    printf("[OK] Missao '%s' iniciada. Drone '%s' e operador '%s' em missao.\n",
           codMissao, m->droneAlocado, m->operadorAlocado);
    return 0;
}

/* ─────────────────────────────────────────────
   ATUALIZAR STATUS (uso interno ao módulo)
   Verifica estado atual antes de aceitar novo (A06/Skill2)
   ───────────────────────────────────────────── */
int atualizarStatus(const char *codMissao, StatusMissao novoStatus) {
    Missao *m;
    Drone    *d;
    Operador *op;

    m = buscarMissaoPorCodigo(codMissao);
    if (m == NULL) {
        printf("[ERRO] Missao '%s' nao encontrada.\n", codMissao);
        return -1;
    }

    /* Validar range do enum */
    if (novoStatus < 0 || novoStatus >= MISSAO_STATUS_COUNT) {
        printf("[ERRO] Status invalido.\n");
        return -1;
    }

    /* Máquina de estados — switch no estado ATUAL */
    switch (m->status) {
        case MISSAO_PENDENTE:
            if (novoStatus == MISSAO_EM_EXECUCAO ||
                novoStatus == MISSAO_CANCELADA) {
                break; /* transição válida */
            }
            printf("[ERRO] Transicao invalida: PENDENTE -> %s.\n",
                   strStatusMissao(novoStatus));
            return -1;

        case MISSAO_EM_EXECUCAO:
            if (novoStatus == MISSAO_CONCLUIDA   ||
                novoStatus == MISSAO_CANCELADA   ||
                novoStatus == MISSAO_INTERROMPIDA ||
                novoStatus == MISSAO_REPLANEJADA) {
                break; /* transição válida */
            }
            printf("[ERRO] Transicao invalida: EM_EXECUCAO -> %s.\n",
                   strStatusMissao(novoStatus));
            return -1;

        case MISSAO_CONCLUIDA:
        case MISSAO_CANCELADA:
        case MISSAO_REPLANEJADA:
        case MISSAO_INTERROMPIDA:
            printf("[ERRO] Missao '%s' ja encerrada (status: %s). Sem transicao possivel.\n",
                   codMissao, strStatusMissao(m->status));
            return -1;

        default:
            printf("[ERRO] Estado atual desconhecido para missao '%s'.\n", codMissao);
            return -1;
    }

    /* Liberar recursos ao encerrar */
    if (novoStatus == MISSAO_CONCLUIDA   ||
        novoStatus == MISSAO_CANCELADA   ||
        novoStatus == MISSAO_INTERROMPIDA ||
        novoStatus == MISSAO_REPLANEJADA) {

        d  = buscarDronePorId(m->droneAlocado);
        op = buscarOperadorPorMatricula(m->operadorAlocado);
        if (d  != NULL) liberarDrone(d);
        if (op != NULL) liberarOperador(op);
    }

    m->status = novoStatus;
    return 0;
}

/* ─────────────────────────────────────────────
   CANCELAR MISSÃO
   ───────────────────────────────────────────── */
int cancelarMissao(const char *codMissao, const char *motivo) {
    Missao       *m;
    Drone        *d;
    Operador     *op;
    HistoricoEntry entry;

    if (motivo == NULL || strlen(motivo) == 0) {
        printf("[ERRO] Motivo do cancelamento nao pode ser vazio.\n");
        return -1;
    }

    m = buscarMissaoPorCodigo(codMissao);
    if (m == NULL) {
        printf("[ERRO] Missao '%s' nao encontrada.\n", codMissao);
        return -1;
    }
    if (m->status == MISSAO_CONCLUIDA || m->status == MISSAO_CANCELADA) {
        printf("[ERRO] Missao '%s' ja esta %s. Nao pode ser cancelada.\n",
               codMissao, strStatusMissao(m->status));
        return -1;
    }

    /* Montar entrada do histórico ANTES de alterar qualquer estado */
    memset(&entry, 0, sizeof(HistoricoEntry));
    strcpy_safe(entry.referenciaOrigem,   codMissao,         sizeof(entry.referenciaOrigem));
    entry.tipoOrigem  = ORIGEM_PLANEJADA;
    entry.statusFinal = HIST_CANCELADA;
    strcpy_safe(entry.horarioInicioReal,  m->horario,        sizeof(entry.horarioInicioReal));
    strcpy_safe(entry.horarioEncerramento, "--:--",           sizeof(entry.horarioEncerramento));
    strcpy_safe(entry.droneUtilizado,     m->droneAlocado,   sizeof(entry.droneUtilizado));
    strcpy_safe(entry.operadorResponsavel, m->operadorAlocado, sizeof(entry.operadorResponsavel));
    strcpy_safe(entry.observacoes,        motivo,            sizeof(entry.observacoes));

    /* Liberar recursos — verificar campos antes de buscar (REC03/Skill4) */
    if (strlen(m->droneAlocado) > 0) {
        d = buscarDronePorId(m->droneAlocado);
        if (d != NULL) liberarDrone(d);
    }
    if (strlen(m->operadorAlocado) > 0) {
        op = buscarOperadorPorMatricula(m->operadorAlocado);
        if (op != NULL) liberarOperador(op);
    }

    m->status = MISSAO_CANCELADA;

    /* Empilhar no histórico — tratamento de falha (L06/Skill5) */
    empilharComVerificacao(&entry, codMissao);

    printf("[OK] Missao '%s' cancelada. Motivo: %s\n", codMissao, motivo);
    return 0;
}

/* ─────────────────────────────────────────────
   REPLANEJAR MISSÃO
   ───────────────────────────────────────────── */
int replanejarMissao(const char *codMissao, Missao novaMissao) {
    Missao        *original;
    Drone         *d;
    Operador      *op;
    HistoricoEntry entry;

    /* VERIFICAR LIMITE ANTES de qualquer alteração (R05/Skill1, E10/Skill4) */
    if (totalMissoes >= MAX_MISSOES) {
        printf("[ERRO] Limite de missoes atingido. Replanejamento impossivel sem perda de rastreabilidade.\n");
        return -1;
    }

    original = buscarMissaoPorCodigo(codMissao);
    if (original == NULL) {
        printf("[ERRO] Missao original '%s' nao encontrada.\n", codMissao);
        return -1;
    }
    if (original->status != MISSAO_EM_EXECUCAO) {
        printf("[ERRO] Somente missoes EM_EXECUCAO podem ser replanejadas.\n");
        return -1;
    }

    /* Validar nova missão com as mesmas regras de cadastro */
    if (strlen(novaMissao.codigo) == 0) {
        printf("[ERRO] Codigo da nova missao nao pode ser vazio.\n");
        return -1;
    }
    if (buscarMissaoPorCodigo(novaMissao.codigo) != NULL) {
        printf("[ERRO] Codigo '%s' ja existe no sistema.\n", novaMissao.codigo);
        return -1;
    }
    if (!validarFormatoHorario(novaMissao.horario)) {
        printf("[ERRO] Horario invalido para nova missao.\n");
        return -1;
    }
    if (novaMissao.prioridade < 1) {
        printf("[ERRO] Prioridade da nova missao deve ser >= 1.\n");
        return -1;
    }
    if (novaMissao.duracao <= 0) {
        printf("[ERRO] Duracao da nova missao deve ser > 0.\n");
        return -1;
    }

    /* Montar histórico da original */
    memset(&entry, 0, sizeof(HistoricoEntry));
    strcpy_safe(entry.referenciaOrigem,    codMissao,              sizeof(entry.referenciaOrigem));
    entry.tipoOrigem  = ORIGEM_PLANEJADA;
    entry.statusFinal = HIST_REPLANEJADA;
    strcpy_safe(entry.horarioInicioReal,   original->horario,      sizeof(entry.horarioInicioReal));
    strcpy_safe(entry.horarioEncerramento, "--:--",                 sizeof(entry.horarioEncerramento));
    strcpy_safe(entry.droneUtilizado,      original->droneAlocado, sizeof(entry.droneUtilizado));
    strcpy_safe(entry.operadorResponsavel, original->operadorAlocado, sizeof(entry.operadorResponsavel));
    strcpy_safe(entry.observacoes,         "Replanejada",           sizeof(entry.observacoes));

    /* Liberar recursos da missão original */
    if (strlen(original->droneAlocado) > 0) {
        d = buscarDronePorId(original->droneAlocado);
        if (d != NULL) liberarDrone(d);
    }
    if (strlen(original->operadorAlocado) > 0) {
        op = buscarOperadorPorMatricula(original->operadorAlocado);
        if (op != NULL) liberarOperador(op);
    }

    /* Marcar original como REPLANEJADA */
    original->status = MISSAO_REPLANEJADA;

    /* Empilhar original no histórico */
    empilharComVerificacao(&entry, codMissao);

    /* Inserir nova missão — campos de alocação zerados */
    novaMissao.status = MISSAO_PENDENTE;
    novaMissao.droneAlocado[0]    = '\0';
    novaMissao.operadorAlocado[0] = '\0';
    missoes[totalMissoes] = novaMissao;
    missoes[totalMissoes].codigo[15]  = '\0';
    missoes[totalMissoes].horario[5]  = '\0';
    missoes[totalMissoes].area[63]    = '\0';
    totalMissoes++;

    printf("[OK] Missao '%s' replanejada. Nova missao '%s' criada como PENDENTE.\n",
           codMissao, novaMissao.codigo);
    return 0;
}

/* ─────────────────────────────────────────────
   ALOCAR RECURSOS — EVENTO DINÂMICO
   ───────────────────────────────────────────── */
int alocarRecursosEvento(const char *codEvento, const char *idDrone, const char *matOp) {
    NoEvento *no;
    Drone    *d;
    Operador *op;
    int i;

    no = buscarEventoPorCodigo(codEvento);
    if (no == NULL) {
        printf("[ERRO] Evento '%s' nao encontrado.\n", codEvento);
        return -1;
    }
    if (no->evento.status != EV_ABERTO) {
        printf("[ERRO] Evento '%s' nao esta ABERTO.\n", codEvento);
        return -1;
    }

    d = buscarDronePorId(idDrone);
    if (d == NULL) {
        printf("[ERRO] Drone '%s' nao encontrado.\n", idDrone);
        return -1;
    }
    if (d->status != DRONE_DISPONIVEL) {
        printf("[ERRO] Drone '%s' nao esta DISPONIVEL (status: %s).\n",
               idDrone, strStatusDrone(d->status));
        return -1;
    }

    op = buscarOperadorPorMatricula(matOp);
    if (op == NULL) {
        printf("[ERRO] Operador '%s' nao encontrado.\n", matOp);
        return -1;
    }
    if (op->situacao != OP_ATIVO) {
        printf("[ERRO] Operador '%s' nao esta ATIVO.\n", matOp);
        return -1;
    }

    /* RN09: verificar alocação dupla em missões */
    for (i = 0; i < totalMissoes; i++) {
        if (missoes[i].status == MISSAO_PENDENTE ||
            missoes[i].status == MISSAO_EM_EXECUCAO) {
            if (strcmp(missoes[i].droneAlocado, idDrone) == 0) {
                printf("[ERRO] Drone '%s' ja alocado em missao '%s'. RN09.\n",
                       idDrone, missoes[i].codigo);
                return -1;
            }
            if (strcmp(missoes[i].operadorAlocado, matOp) == 0) {
                printf("[ERRO] Operador '%s' ja alocado em missao '%s'. RN09.\n",
                       matOp, missoes[i].codigo);
                return -1;
            }
        }
    }

    /* RN09: verificar alocação dupla em outros eventos ativos */
    {
        NoEvento *ev = cabecaEventos->prox;
        while (ev != caudaEventos) {
            if ((ev->evento.status == EV_ABERTO ||
                 ev->evento.status == EV_EM_ATENDIMENTO) &&
                strcmp(ev->evento.codigo, codEvento) != 0) {
                if (strlen(ev->evento.droneAlocado) > 0 &&
                    strcmp(ev->evento.droneAlocado, idDrone) == 0) {
                    printf("[ERRO] Drone '%s' ja alocado no evento '%s'. RN09.\n",
                           idDrone, ev->evento.codigo);
                    return -1;
                }
                if (strlen(ev->evento.operadorAlocado) > 0 &&
                    strcmp(ev->evento.operadorAlocado, matOp) == 0) {
                    printf("[ERRO] Operador '%s' ja alocado no evento '%s'. RN09.\n",
                           matOp, ev->evento.codigo);
                    return -1;
                }
            }
            ev = ev->prox;
        }
    }

    /* Atribuições */
    strcpy_safe(no->evento.droneAlocado,    idDrone, sizeof(no->evento.droneAlocado));
    strcpy_safe(no->evento.operadorAlocado, matOp,   sizeof(no->evento.operadorAlocado));

    printf("[OK] Recursos alocados para evento '%s': drone '%s', operador '%s'.\n",
           codEvento, idDrone, matOp);
    return 0;
}

/* ─────────────────────────────────────────────
   INICIAR ATENDIMENTO DE EVENTO (ABERTO -> EM_ATENDIMENTO) — L04/Skill5
   ───────────────────────────────────────────── */
int iniciarAtendimentoEvento(const char *codEvento) {
    NoEvento *no;
    Drone    *d;
    Operador *op;

    no = buscarEventoPorCodigo(codEvento);
    if (no == NULL) {
        printf("[ERRO] Evento '%s' nao encontrado.\n", codEvento);
        return -1;
    }
    if (no->evento.status != EV_ABERTO) {
        printf("[ERRO] Evento '%s' nao esta ABERTO (status: %s).\n",
               codEvento, strStatusEvento(no->evento.status));
        return -1;
    }
    if (strlen(no->evento.droneAlocado) == 0) {
        printf("[ERRO] Evento '%s' nao possui drone alocado.\n", codEvento);
        return -1;
    }
    if (strlen(no->evento.operadorAlocado) == 0) {
        printf("[ERRO] Evento '%s' nao possui operador alocado.\n", codEvento);
        return -1;
    }

    no->evento.status = EV_EM_ATENDIMENTO;

    d  = buscarDronePorId(no->evento.droneAlocado);
    op = buscarOperadorPorMatricula(no->evento.operadorAlocado);
    if (d  != NULL) d->status    = DRONE_EM_MISSAO;
    if (op != NULL) op->situacao = OP_EM_MISSAO;

    /* Registrar atividade vinculada ao drone */
    if (d != NULL && d->totalAtividades < MAX_ATIV_DRONE) {
        strcpy_safe(d->atividadesVinculadas[d->totalAtividades],
                    codEvento, 16);
        d->totalAtividades++;
    }

    printf("[OK] Atendimento do evento '%s' iniciado.\n", codEvento);
    return 0;
}

/* ─────────────────────────────────────────────
   ENCERRAR EVENTO (EM_ATENDIMENTO -> ENCERRADO -> histórico)
   ───────────────────────────────────────────── */
int encerrarEvento(const char *codEvento) {
    NoEvento      *no;
    Drone         *d;
    Operador      *op;
    HistoricoEntry entry;
    /* Copiar dados antes de remover o nó */
    EventoDinamico evCopia;

    no = buscarEventoPorCodigo(codEvento);
    if (no == NULL) {
        printf("[ERRO] Evento '%s' nao encontrado.\n", codEvento);
        return -1;
    }
    if (no->evento.status != EV_EM_ATENDIMENTO) {
        printf("[ERRO] Evento '%s' nao esta EM_ATENDIMENTO (status: %s). Encerramento requer atendimento ativo.\n",
               codEvento, strStatusEvento(no->evento.status));
        return -1;
    }

    /* Copiar dados antes de remover o nó (free libera memória) */
    evCopia = no->evento;

    /* Liberar recursos */
    if (strlen(evCopia.droneAlocado) > 0) {
        d = buscarDronePorId(evCopia.droneAlocado);
        if (d != NULL) liberarDrone(d);
    }
    if (strlen(evCopia.operadorAlocado) > 0) {
        op = buscarOperadorPorMatricula(evCopia.operadorAlocado);
        if (op != NULL) liberarOperador(op);
    }

    /* Remover da lista com free — encapsulado em evento.c (R04/Skill1) */
    removerEventoDaLista(codEvento);

    /* Montar e empilhar histórico com dados da cópia */
    memset(&entry, 0, sizeof(HistoricoEntry));
    strcpy_safe(entry.referenciaOrigem,    evCopia.codigo,            sizeof(entry.referenciaOrigem));
    entry.tipoOrigem  = ORIGEM_DINAMICA;
    entry.statusFinal = HIST_CONCLUIDA;
    strcpy_safe(entry.horarioInicioReal,   evCopia.horarioAbertura,   sizeof(entry.horarioInicioReal));
    strcpy_safe(entry.horarioEncerramento, "--:--",                    sizeof(entry.horarioEncerramento));
    strcpy_safe(entry.droneUtilizado,      evCopia.droneAlocado,      sizeof(entry.droneUtilizado));
    strcpy_safe(entry.operadorResponsavel, evCopia.operadorAlocado,   sizeof(entry.operadorResponsavel));
    strcpy_safe(entry.observacoes,         "Encerrado normalmente",    sizeof(entry.observacoes));

    empilharComVerificacao(&entry, codEvento);

    printf("[OK] Evento '%s' encerrado e registrado no historico.\n", codEvento);
    return 0;
}

/* ─────────────────────────────────────────────
   CANCELAR EVENTO (ABERTO -> ENCERRADO) — L05/Skill5
   ───────────────────────────────────────────── */
int cancelarEvento(const char *codEvento, const char *motivo) {
    NoEvento      *no;
    Drone         *d;
    Operador      *op;
    HistoricoEntry entry;
    EventoDinamico evCopia;

    if (motivo == NULL || strlen(motivo) == 0) {
        printf("[ERRO] Motivo do cancelamento nao pode ser vazio.\n");
        return -1;
    }

    no = buscarEventoPorCodigo(codEvento);
    if (no == NULL) {
        printf("[ERRO] Evento '%s' nao encontrado.\n", codEvento);
        return -1;
    }
    if (no->evento.status != EV_ABERTO) {
        printf("[ERRO] Apenas eventos ABERTOS podem ser cancelados diretamente.\n");
        return -1;
    }

    evCopia = no->evento;

    if (strlen(evCopia.droneAlocado) > 0) {
        d = buscarDronePorId(evCopia.droneAlocado);
        if (d != NULL) liberarDrone(d);
    }
    if (strlen(evCopia.operadorAlocado) > 0) {
        op = buscarOperadorPorMatricula(evCopia.operadorAlocado);
        if (op != NULL) liberarOperador(op);
    }

    removerEventoDaLista(codEvento);

    memset(&entry, 0, sizeof(HistoricoEntry));
    strcpy_safe(entry.referenciaOrigem,    evCopia.codigo,          sizeof(entry.referenciaOrigem));
    entry.tipoOrigem  = ORIGEM_DINAMICA;
    entry.statusFinal = HIST_CANCELADA;
    strcpy_safe(entry.horarioInicioReal,   evCopia.horarioAbertura, sizeof(entry.horarioInicioReal));
    strcpy_safe(entry.horarioEncerramento, "--:--",                  sizeof(entry.horarioEncerramento));
    strcpy_safe(entry.droneUtilizado,      evCopia.droneAlocado,    sizeof(entry.droneUtilizado));
    strcpy_safe(entry.operadorResponsavel, evCopia.operadorAlocado,  sizeof(entry.operadorResponsavel));
    strcpy_safe(entry.observacoes,         motivo,                   sizeof(entry.observacoes));

    empilharComVerificacao(&entry, codEvento);

    printf("[OK] Evento '%s' cancelado. Motivo: %s\n", codEvento, motivo);
    return 0;
}

/* ─────────────────────────────────────────────
   GESTÃO DE STATUS DO DRONE — L01/Skill5
   ───────────────────────────────────────────── */
int colocarDroneEmManutencao(const char *idDrone) {
    Drone *d = buscarDronePorId(idDrone);
    if (d == NULL) {
        printf("[ERRO] Drone '%s' nao encontrado.\n", idDrone);
        return -1;
    }
    if (d->status != DRONE_DISPONIVEL && d->status != DRONE_EM_MISSAO) {
        printf("[ERRO] Drone '%s' esta %s. Somente DISPONIVEL ou EM_MISSAO podem ir para MANUTENCAO.\n",
               idDrone, strStatusDrone(d->status));
        return -1;
    }
    d->status = DRONE_MANUTENCAO;
    printf("[OK] Drone '%s' colocado em MANUTENCAO.\n", idDrone);
    return 0;
}

int confirmarRetornoManutencao(const char *idDrone) {
    Drone *d = buscarDronePorId(idDrone);
    if (d == NULL) {
        printf("[ERRO] Drone '%s' nao encontrado.\n", idDrone);
        return -1;
    }
    if (d->status != DRONE_MANUTENCAO) {
        printf("[ERRO] Drone '%s' nao esta em MANUTENCAO.\n", idDrone);
        return -1;
    }
    d->status = DRONE_DISPONIVEL;
    printf("[OK] Drone '%s' retornou da manutencao. Status: DISPONIVEL.\n", idDrone);
    return 0;
}

int confirmarRetornoDrone(const char *idDrone) {
    Drone *d = buscarDronePorId(idDrone);
    if (d == NULL) {
        printf("[ERRO] Drone '%s' nao encontrado.\n", idDrone);
        return -1;
    }
    if (d->status != DRONE_RETORNANDO) {
        printf("[ERRO] Drone '%s' nao esta RETORNANDO.\n", idDrone);
        return -1;
    }
    d->status = DRONE_DISPONIVEL;
    printf("[OK] Drone '%s' retornou. Status: DISPONIVEL.\n", idDrone);
    return 0;
}

/* ─────────────────────────────────────────────
   GESTÃO DE STATUS DO OPERADOR — L02/Skill5
   ───────────────────────────────────────────── */
int comunicarAusenciaOperador(const char *mat) {
    Operador *op = buscarOperadorPorMatricula(mat);
    if (op == NULL) {
        printf("[ERRO] Operador '%s' nao encontrado.\n", mat);
        return -1;
    }
    if (op->situacao != OP_ATIVO) {
        printf("[ERRO] Operador '%s' nao esta ATIVO (situacao: %s).\n",
               mat, strSituacaoOperador(op->situacao));
        return -1;
    }
    op->situacao = OP_AUSENTE;
    printf("[OK] Ausencia do operador '%s' registrada.\n", mat);
    return 0;
}

int retornarOperadorAoServico(const char *mat) {
    Operador *op = buscarOperadorPorMatricula(mat);
    if (op == NULL) {
        printf("[ERRO] Operador '%s' nao encontrado.\n", mat);
        return -1;
    }
    if (op->situacao != OP_AUSENTE) {
        printf("[ERRO] Operador '%s' nao esta AUSENTE.\n", mat);
        return -1;
    }
    op->situacao = OP_ATIVO;
    printf("[OK] Operador '%s' retornou ao servico. Status: ATIVO.\n", mat);
    return 0;
}

/* ─────────────────────────────────────────────
   CONCLUIR MISSÃO (helper para o menu)
   ───────────────────────────────────────────── */
static int concluirMissao(const char *codMissao, const char *horarioFim) {
    Missao        *m;
    Drone         *d;
    Operador      *op;
    HistoricoEntry entry;

    m = buscarMissaoPorCodigo(codMissao);
    if (m == NULL) {
        printf("[ERRO] Missao '%s' nao encontrada.\n", codMissao);
        return -1;
    }
    if (m->status != MISSAO_EM_EXECUCAO) {
        printf("[ERRO] Missao '%s' nao esta EM_EXECUCAO.\n", codMissao);
        return -1;
    }

    memset(&entry, 0, sizeof(HistoricoEntry));
    strcpy_safe(entry.referenciaOrigem,    codMissao,           sizeof(entry.referenciaOrigem));
    entry.tipoOrigem  = ORIGEM_PLANEJADA;
    entry.statusFinal = HIST_CONCLUIDA;
    strcpy_safe(entry.horarioInicioReal,   m->horario,          sizeof(entry.horarioInicioReal));
    strcpy_safe(entry.horarioEncerramento, horarioFim,          sizeof(entry.horarioEncerramento));
    strcpy_safe(entry.droneUtilizado,      m->droneAlocado,     sizeof(entry.droneUtilizado));
    strcpy_safe(entry.operadorResponsavel, m->operadorAlocado,  sizeof(entry.operadorResponsavel));
    strcpy_safe(entry.observacoes,         "Concluida normalmente", sizeof(entry.observacoes));

    if (strlen(m->droneAlocado) > 0) {
        d = buscarDronePorId(m->droneAlocado);
        if (d != NULL) liberarDrone(d);
    }
    if (strlen(m->operadorAlocado) > 0) {
        op = buscarOperadorPorMatricula(m->operadorAlocado);
        if (op != NULL) liberarOperador(op);
    }

    m->status = MISSAO_CONCLUIDA;
    empilharComVerificacao(&entry, codMissao);

    printf("[OK] Missao '%s' concluida.\n", codMissao);
    return 0;
}

/* ─────────────────────────────────────────────
   MENUS INTERATIVOS
   ───────────────────────────────────────────── */
void menuAlocarRecursos(void) {
    char cod[16], drone[16], mat[16];
    printf("\n--- ALOCAR RECURSOS (MISSAO) ---\n");
    printf("Codigo da missao: "); lerString(cod, sizeof(cod));
    printf("ID do drone: ");      lerString(drone, sizeof(drone));
    printf("Matricula do operador: "); lerString(mat, sizeof(mat));
    alocarRecursos(cod, drone, mat);
}

void menuIniciarMissao(void) {
    char cod[16];
    printf("\n--- INICIAR MISSAO ---\n");
    printf("Codigo da missao: "); lerString(cod, sizeof(cod));
    iniciarMissao(cod);
}

void menuConcluirMissao(void) {
    char cod[16], horario[6];
    printf("\n--- CONCLUIR MISSAO ---\n");
    printf("Codigo da missao: "); lerString(cod, sizeof(cod));
    printf("Horario de encerramento (HH:MM): "); lerString(horario, sizeof(horario));
    if (!validarFormatoHorario(horario)) {
        strcpy_safe(horario, "--:--", sizeof(horario));
    }
    concluirMissao(cod, horario);
}

void menuCancelarMissao(void) {
    char cod[16], motivo[128];
    printf("\n--- CANCELAR MISSAO ---\n");
    printf("Codigo da missao: "); lerString(cod, sizeof(cod));
    printf("Motivo: ");           lerString(motivo, sizeof(motivo));
    cancelarMissao(cod, motivo);
}

void menuReplanejarMissao(void) {
    Missao nova;
    char codOriginal[16];
    int tipo;
    char horario[7];

    printf("\n--- REPLANEJAR MISSAO ---\n");
    printf("Codigo da missao original: "); lerString(codOriginal, sizeof(codOriginal));

    memset(&nova, 0, sizeof(Missao));
    printf("--- DADOS DA NOVA MISSAO ---\n");
    printf("Novo codigo: ");    lerString(nova.codigo, sizeof(nova.codigo));
    printf("Horario (HH:MM): ");    lerString(horario, sizeof(horario));
    if (!validarFormatoHorario(horario)) {
        printf("[ERRO] Horario invalido. Usando 08:00.\n");
        strcpy_safe(horario, "08:00", sizeof(horario));
    }
    strcpy_safe(nova.horario, horario, sizeof(nova.horario));
    nova.prioridade = lerInteiro("Prioridade (>= 1): ", 1, 100);
    printf("Area: ");              lerString(nova.area, sizeof(nova.area));
    nova.duracao = lerInteiro("Duracao (minutos): ", 1, 1440);
    printf("Tipo:\n  0-RONDA  1-VISTORIA  2-INSPECAO  3-ACOMPANHAMENTO  4-OUTRO\n");
    tipo = lerOpcao("Escolha: ", 0, 4);
    nova.tipo = (TipoMissao)tipo;
    printf("Recurso preferencial (opcional): ");
    lerString(nova.recursoPreferencial, sizeof(nova.recursoPreferencial));

    replanejarMissao(codOriginal, nova);
}

void menuEncerrarEvento(void) {
    char cod[16];
    printf("\n--- ENCERRAR EVENTO ---\n");
    printf("Codigo do evento: "); lerString(cod, sizeof(cod));
    encerrarEvento(cod);
}

void menuAlocarRecursosEvento(void) {
    char cod[16], drone[16], mat[16];
    printf("\n--- ALOCAR RECURSOS (EVENTO) ---\n");
    printf("Codigo do evento: ");         lerString(cod, sizeof(cod));
    printf("ID do drone: ");              lerString(drone, sizeof(drone));
    printf("Matricula do operador: ");    lerString(mat, sizeof(mat));
    alocarRecursosEvento(cod, drone, mat);
}

void menuIniciarAtendimentoEvento(void) {
    char cod[16];
    printf("\n--- INICIAR ATENDIMENTO DE EVENTO ---\n");
    printf("Codigo do evento: "); lerString(cod, sizeof(cod));
    iniciarAtendimentoEvento(cod);
}

void menuGestaoStatusDrone(void) {
    char id[16];
    int opcao;
    printf("\n--- GESTAO DE STATUS DO DRONE ---\n");
    printf("  1 - Colocar em MANUTENCAO\n");
    printf("  2 - Confirmar retorno da MANUTENCAO\n");
    printf("  3 - Confirmar RETORNO de missao\n");
    opcao = lerOpcao("Opcao: ", 1, 3);
    printf("ID do drone: "); lerString(id, sizeof(id));
    switch (opcao) {
        case 1: colocarDroneEmManutencao(id);     break;
        case 2: confirmarRetornoManutencao(id);   break;
        case 3: confirmarRetornoDrone(id);         break;
        default: printf("[ERRO] Opcao invalida.\n");
    }
}

void menuGestaoStatusOperador(void) {
    char mat[16];
    int opcao;
    printf("\n--- GESTAO DE STATUS DO OPERADOR ---\n");
    printf("  1 - Comunicar AUSENCIA\n");
    printf("  2 - Retornar ao SERVICO\n");
    opcao = lerOpcao("Opcao: ", 1, 2);
    printf("Matricula: "); lerString(mat, sizeof(mat));
    switch (opcao) {
        case 1: comunicarAusenciaOperador(mat);  break;
        case 2: retornarOperadorAoServico(mat);  break;
        default: printf("[ERRO] Opcao invalida.\n");
    }
}

void menuCancelarEvento(void) {
    char cod[16], motivo[128];
    printf("\n--- CANCELAR EVENTO ---\n");
    printf("Codigo do evento: "); lerString(cod, sizeof(cod));
    printf("Motivo: ");           lerString(motivo, sizeof(motivo));
    cancelarEvento(cod, motivo);
}
