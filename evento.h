#ifndef EVENTO_H
#define EVENTO_H

typedef enum {
    EV_ABERTO        = 0,
    EV_EM_ATENDIMENTO = 1,
    EV_ENCERRADO     = 2,
    EV_STATUS_COUNT  = 3
} StatusEvento;

typedef enum {
    EV_INVASAO             = 0,
    EV_ALAGAMENTO          = 1,
    EV_RISCO_ESTRUTURAL    = 2,
    EV_PANE_ELETRICA       = 3,
    EV_INSPECAO_EMERGENCIAL = 4,
    EV_OUTRO               = 5,
    EV_TIPO_COUNT          = 6
} TipoEvento;

typedef struct {
    char         codigo[16];
    TipoEvento   tipo;
    char         horarioAbertura[6];  /* HH:MM */
    int          prioridade;          /* sempre 0 (urgente) */
    StatusEvento status;
    char         droneAlocado[16];    /* "" = não alocado */
    char         operadorAlocado[16]; /* "" = não alocado */
} EventoDinamico;

typedef struct NoEvento {
    EventoDinamico  evento;
    struct NoEvento *prox;
} NoEvento;

/*
 * Sentinelas estáticos: cabeça e cauda nunca removidos, sem dado real.
 * Inicialização obrigatória: cabecaEventos.prox = &caudaEventos
 *                            caudaEventos.prox  = NULL
 */
extern NoEvento *cabecaEventos;  /* ponteiro para sentinela início */
extern NoEvento *caudaEventos;   /* ponteiro para sentinela fim    */

/* Inicializa os sentinelas da lista — chamar antes de qualquer uso */
void inicializarListaEventos(void);

/* Remove nó com o código informado e libera memória.
 * Retorna 0 se removido, -1 se não encontrado. */
int removerEventoDaLista(const char *codigo);

const char *strStatusEvento(StatusEvento s);
const char *strTipoEvento(TipoEvento t);

#endif /* EVENTO_H */
