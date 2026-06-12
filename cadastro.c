#include "cadastro.h"
#include "utils.h"
#include "consulta.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ─────────────────────────────────────────────
   CADASTRAR DRONE
   ───────────────────────────────────────────── */
int cadastrarDrone(Drone d) {
    int i;

    /* Validação: ID não vazio */
    if (strlen(d.id) == 0) {
        printf("[ERRO] ID do drone nao pode ser vazio.\n");
        return -1;
    }
    /* Validação: ID único */
    if (buscarDronePorId(d.id) != NULL) {
        printf("[ERRO] Drone com ID '%s' ja existe.\n", d.id);
        return -1;
    }
    /* Validação: limite de drones */
    if (totalDrones >= MAX_DRONES) {
        printf("[ERRO] Limite de drones atingido.\n");
        return -1;
    }
    /* Validação: autonomia > 0 */
    if (d.autonomia <= 0) {
        printf("[ERRO] Autonomia deve ser maior que zero.\n");
        return -1;
    }
    /* Validação: nivelDisponibilidade entre 0-100 */
    if (d.nivelDisponibilidade < 0 || d.nivelDisponibilidade > 100) {
        printf("[ERRO] Nivel de disponibilidade deve estar entre 0 e 100.\n");
        return -1;
    }
    /* Validação: capacidade > 0 */
    if (d.capacidade <= 0.0f) {
        printf("[ERRO] Capacidade deve ser maior que zero.\n");
        return -1;
    }

    /* Bloco de atribuição — apenas após todas as validações */
    drones[totalDrones] = d;
    /* Garantir null-termination e inicialização de campos internos */
    drones[totalDrones].id[15]       = '\0';
    drones[totalDrones].modelo[63]   = '\0';
    drones[totalDrones].setorVinc[31]= '\0';
    drones[totalDrones].totalAtividades = 0;
    for (i = 0; i < MAX_ATIV_DRONE; i++) {
        drones[totalDrones].atividadesVinculadas[i][0] = '\0';
    }
    drones[totalDrones].status = DRONE_DISPONIVEL;

    totalDrones++;
    printf("[OK] Drone '%s' cadastrado com sucesso.\n", d.id);
    return 0;
}

/* ─────────────────────────────────────────────
   CADASTRAR OPERADOR
   ───────────────────────────────────────────── */
int cadastrarOperador(Operador op) {
    /* Validação: matrícula não vazia */
    if (strlen(op.mat) == 0) {
        printf("[ERRO] Matricula do operador nao pode ser vazia.\n");
        return -1;
    }
    /* Validação: matrícula única */
    if (buscarOperadorPorMatricula(op.mat) != NULL) {
        printf("[ERRO] Operador com matricula '%s' ja existe.\n", op.mat);
        return -1;
    }
    /* Validação: limite de operadores */
    if (totalOperadores >= MAX_OPERADORES) {
        printf("[ERRO] Limite de operadores atingido.\n");
        return -1;
    }
    /* Validação: turno válido */
    if (op.turno < TURNO_MANHA || op.turno >= TURNO_COUNT) {
        printf("[ERRO] Turno invalido.\n");
        return -1;
    }

    operadores[totalOperadores] = op;
    operadores[totalOperadores].mat[15]    = '\0';
    operadores[totalOperadores].nome[63]   = '\0';
    operadores[totalOperadores].certificacao[31] = '\0';
    operadores[totalOperadores].situacao   = OP_ATIVO;

    totalOperadores++;
    printf("[OK] Operador '%s' cadastrado com sucesso.\n", op.mat);
    return 0;
}

/* ─────────────────────────────────────────────
   CADASTRAR MISSÃO
   ───────────────────────────────────────────── */
int cadastrarMissao(Missao m) {
    /* Validação: código não vazio */
    if (strlen(m.codigo) == 0) {
        printf("[ERRO] Codigo da missao nao pode ser vazio.\n");
        return -1;
    }
    /* Validação: código único */
    if (buscarMissaoPorCodigo(m.codigo) != NULL) {
        printf("[ERRO] Missao com codigo '%s' ja existe.\n", m.codigo);
        return -1;
    }
    /* Validação: limite de missoes */
    if (totalMissoes >= MAX_MISSOES) {
        printf("[ERRO] Limite de missoes atingido.\n");
        return -1;
    }
    /* Validação: horário no formato HH:MM */
    if (!validarFormatoHorario(m.horario)) {
        printf("[ERRO] Horario invalido. Use o formato HH:MM.\n");
        return -1;
    }
    /* Validação: prioridade >= 1 (0 é reservada para eventos urgentes) */
    if (m.prioridade < 1) {
        printf("[ERRO] Prioridade da missao deve ser >= 1 (0 e reservada para eventos urgentes).\n");
        return -1;
    }
    /* Validação: duração > 0 */
    if (m.duracao <= 0) {
        printf("[ERRO] Duracao deve ser maior que zero.\n");
        return -1;
    }
    /* Validação: tipo válido */
    if (m.tipo < TIPO_RONDA || m.tipo >= TIPO_MISSAO_COUNT) {
        printf("[ERRO] Tipo de missao invalido.\n");
        return -1;
    }

    missoes[totalMissoes] = m;
    missoes[totalMissoes].codigo[15]   = '\0';
    missoes[totalMissoes].horario[5]   = '\0';
    missoes[totalMissoes].area[63]     = '\0';
    missoes[totalMissoes].status       = MISSAO_PENDENTE;
    /* Campos de alocação inicializados como vazios (sentinela "não alocado") */
    missoes[totalMissoes].droneAlocado[0]    = '\0';
    missoes[totalMissoes].operadorAlocado[0] = '\0';

    totalMissoes++;
    printf("[OK] Missao '%s' cadastrada com sucesso.\n", m.codigo);
    return 0;
}

/* ─────────────────────────────────────────────
   REGISTRAR EVENTO
   ───────────────────────────────────────────── */
int registrarEvento(EventoDinamico ev) {
    NoEvento *novoNo;

    /* Validação: tipo de evento válido */
    if (ev.tipo < EV_INVASAO || ev.tipo >= EV_TIPO_COUNT) {
        printf("[ERRO] Tipo de evento invalido.\n");
        return -1;
    }
    /* Validação: horário no formato HH:MM */
    if (!validarFormatoHorario(ev.horarioAbertura)) {
        printf("[ERRO] Horario invalido. Use o formato HH:MM.\n");
        return -1;
    }
    /* Validação: código não vazio */
    if (strlen(ev.codigo) == 0) {
        printf("[ERRO] Codigo do evento nao pode ser vazio.\n");
        return -1;
    }
    /* Validação: código único na lista */
    if (buscarEventoPorCodigo(ev.codigo) != NULL) {
        printf("[ERRO] Evento com codigo '%s' ja existe.\n", ev.codigo);
        return -1;
    }

    /* Alocar nó — verificar NULL antes de qualquer operação */
    novoNo = (NoEvento *)malloc(sizeof(NoEvento));
    if (novoNo == NULL) {
        printf("[ERRO] Falha de alocacao de memoria para o evento.\n");
        return -1;
    }

    /* Preencher dados */
    novoNo->evento          = ev;
    novoNo->evento.codigo[15]           = '\0';
    novoNo->evento.horarioAbertura[5]   = '\0';
    novoNo->evento.prioridade           = 0; /* sempre urgente */
    novoNo->evento.status               = EV_ABERTO;
    novoNo->evento.droneAlocado[0]      = '\0';
    novoNo->evento.operadorAlocado[0]   = '\0';

    /* Inserção no início da lista (após cabeça) — O(1)
     * Sequência obrigatória: primeiro redirecionar, depois atualizar cabeça */
    novoNo->prox        = cabecaEventos->prox;
    cabecaEventos->prox = novoNo;

    printf("[OK] Evento '%s' registrado com sucesso (prioridade URGENTE).\n", ev.codigo);
    return 0;
}

/* ─────────────────────────────────────────────
   MENUS INTERATIVOS
   ───────────────────────────────────────────── */
void menuCadastrarDrone(void) {
    Drone d;

    memset(&d, 0, sizeof(Drone));

    printf("\n--- CADASTRAR DRONE ---\n");
    printf("ID: ");
    lerString(d.id, sizeof(d.id));

    printf("Modelo: ");
    lerString(d.modelo, sizeof(d.modelo));

    d.autonomia = lerInteiro("Autonomia (minutos): ", 1, 1440); /* 1 min a 24h */

    d.nivelDisponibilidade = lerInteiro("Nivel de disponibilidade (0-100): ", 0, 100);

    d.capacidade = lerFloat("Capacidade (kg): ", 0.1f, 1000.0f);

    printf("Setor vinculado: ");
    lerString(d.setorVinc, sizeof(d.setorVinc));

    /* Status inicial sempre DISPONIVEL — máquina de estados proíbe outros estados ao cadastrar */
    d.status = DRONE_DISPONIVEL;

    cadastrarDrone(d);
}

void menuCadastrarOperador(void) {
    Operador op;
    int turno;

    memset(&op, 0, sizeof(Operador));

    printf("\n--- CADASTRAR OPERADOR ---\n");
    printf("Matricula: ");
    lerString(op.mat, sizeof(op.mat));

    printf("Nome: ");
    lerString(op.nome, sizeof(op.nome));

    printf("Certificacao: ");
    lerString(op.certificacao, sizeof(op.certificacao));

    printf("Turno:\n  0 - MANHA\n  1 - TARDE\n  2 - NOITE\n");
    turno = lerOpcao("Escolha: ", 0, 2);
    op.turno   = (Turno)turno;
    op.situacao = OP_ATIVO;

    cadastrarOperador(op);
}

void menuCadastrarMissao(void) {
    Missao m;
    int tipo;
    char horario[7];

    memset(&m, 0, sizeof(Missao));

    printf("\n--- CADASTRAR MISSAO ---\n");
    printf("Codigo: ");
    lerString(m.codigo, sizeof(m.codigo));

    printf("Horario (HH:MM): ");
    lerString(horario, sizeof(horario));
    if (!validarFormatoHorario(horario)) {
        printf("[ERRO] Horario invalido. Usando 08:00.\n");
        strcpy_safe(horario, "08:00", sizeof(horario));
    }
    strcpy_safe(m.horario, horario, sizeof(m.horario));

    m.prioridade = lerInteiro("Prioridade (>= 1): ", 1, 100);

    printf("Area: ");
    lerString(m.area, sizeof(m.area));

    m.duracao = lerInteiro("Duracao (minutos): ", 1, 1440); /* 1 min a 24h */

    printf("Tipo:\n  0-RONDA  1-VISTORIA  2-INSPECAO  3-ACOMPANHAMENTO  4-OUTRO\n");
    tipo = lerOpcao("Escolha: ", 0, 4);
    m.tipo = (TipoMissao)tipo;

    printf("Recurso preferencial (opcional): ");
    lerString(m.recursoPreferencial, sizeof(m.recursoPreferencial));

    cadastrarMissao(m);
}

void menuRegistrarEvento(void) {
    EventoDinamico ev;
    int tipo;
    char horario[7];

    memset(&ev, 0, sizeof(EventoDinamico));

    printf("\n--- REGISTRAR EVENTO DINAMICO ---\n");
    printf("Codigo: ");
    lerString(ev.codigo, sizeof(ev.codigo));

    printf("Tipo:\n  0-INVASAO  1-ALAGAMENTO  2-RISCO_ESTRUTURAL\n");
    printf("  3-PANE_ELETRICA  4-INSPECAO_EMERGENCIAL  5-OUTRO\n");
    tipo = lerOpcao("Escolha: ", 0, 5);
    ev.tipo = (TipoEvento)tipo;

    printf("Horario de abertura (HH:MM): ");
    lerString(horario, sizeof(horario));
    if (!validarFormatoHorario(horario)) {
        printf("[ERRO] Horario invalido. Usando hora atual.\n");
        strcpy_safe(horario, "12:00", sizeof(horario));
    }
    strcpy_safe(ev.horarioAbertura, horario, sizeof(ev.horarioAbertura));

    registrarEvento(ev);
}
