#ifndef MISSAO_H
#define MISSAO_H

#define MAX_MISSOES 300

typedef enum {
    MISSAO_PENDENTE    = 0,
    MISSAO_EM_EXECUCAO = 1,
    MISSAO_CONCLUIDA   = 2,
    MISSAO_CANCELADA   = 3,
    MISSAO_REPLANEJADA = 4,
    MISSAO_INTERROMPIDA = 5,
    MISSAO_STATUS_COUNT = 6
} StatusMissao;

typedef enum {
    TIPO_RONDA          = 0,
    TIPO_VISTORIA       = 1,
    TIPO_INSPECAO       = 2,
    TIPO_ACOMPANHAMENTO = 3,
    TIPO_OUTRO          = 4,
    TIPO_MISSAO_COUNT   = 5
} TipoMissao;

typedef struct {
    char         codigo[16];
    char         horario[6];             /* HH:MM */
    int          prioridade;
    char         area[64];
    int          duracao;                /* minutos */
    TipoMissao   tipo;
    char         recursoPreferencial[16];
    StatusMissao status;
    char         droneAlocado[16];       /* "" = não alocado */
    char         operadorAlocado[16];    /* "" = não alocado */
} Missao;

extern Missao missoes[MAX_MISSOES];
extern int    totalMissoes;

const char *strStatusMissao(StatusMissao s);
const char *strTipoMissao(TipoMissao t);

#endif /* MISSAO_H */
