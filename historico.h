#ifndef HISTORICO_H
#define HISTORICO_H

#define MAX_HISTORICO 500

typedef enum {
    ORIGEM_PLANEJADA = 0,
    ORIGEM_DINAMICA  = 1
} TipoOrigem;

typedef enum {
    HIST_CONCLUIDA    = 0,
    HIST_CANCELADA    = 1,
    HIST_INTERROMPIDA = 2,
    HIST_REPLANEJADA  = 3
} StatusFinalHist;

typedef struct {
    char            referenciaOrigem[16];
    TipoOrigem      tipoOrigem;
    StatusFinalHist statusFinal;
    char            horarioInicioReal[6];
    char            horarioEncerramento[6];
    char            droneUtilizado[16];
    char            operadorResponsavel[16];
    char            observacoes[128];
} HistoricoEntry;

typedef struct {
    HistoricoEntry dados[MAX_HISTORICO];
    int            topo;   /* -1 = pilha vazia */
} PilhaHistorico;

extern PilhaHistorico historico;

/* Push na pilha. Retorna 0 se ok, -1 se pilha cheia */
int empilhar(HistoricoEntry entry);

/* Peek sem remover. Retorna NULL se pilha vazia.
 * CONTRATO: não armazenar o ponteiro retornado além do escopo imediato */
HistoricoEntry *consultarTopo(void);

/* Percorre do topo à base (mais recente ao mais antigo) */
void listarHistorico(void);

/* Percorre pilha e exibe entradas do drone informado */
void filtrarPorDrone(const char *idDrone);

/* Percorre pilha e exibe entradas do operador informado */
void filtrarPorOperador(const char *mat);

const char *strTipoOrigem(TipoOrigem t);
const char *strStatusFinalHist(StatusFinalHist s);

#endif /* HISTORICO_H */
