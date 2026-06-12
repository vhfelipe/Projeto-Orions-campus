#ifndef OPERADOR_H
#define OPERADOR_H

#define MAX_OPERADORES 100

typedef enum {
    TURNO_MANHA = 0,
    TURNO_TARDE = 1,
    TURNO_NOITE = 2,
    TURNO_COUNT  = 3
} Turno;

typedef enum {
    OP_ATIVO    = 0,
    OP_AUSENTE  = 1,
    OP_EM_MISSAO = 2,
    OP_SITUACAO_COUNT = 3
} SituacaoOperador;

typedef struct {
    char             mat[16];
    char             nome[64];
    char             certificacao[32];
    Turno            turno;
    SituacaoOperador situacao;
} Operador;

extern Operador operadores[MAX_OPERADORES];
extern int      totalOperadores;

const char *strTurno(Turno t);
const char *strSituacaoOperador(SituacaoOperador s);

#endif /* OPERADOR_H */
