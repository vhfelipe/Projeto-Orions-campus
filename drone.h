#ifndef DRONE_H
#define DRONE_H

#define MAX_DRONES      200
#define MAX_ATIV_DRONE  60   /* capacidade de atividades vinculadas por drone */

/* Valor sentinela: campo vazio = recurso não alocado */
#define SEM_RECURSO ""

typedef enum {
    DRONE_DISPONIVEL  = 0,
    DRONE_EM_MISSAO   = 1,
    DRONE_MANUTENCAO  = 2,
    DRONE_RETORNANDO  = 3,
    DRONE_STATUS_COUNT = 4   /* sentinela para validação de range */
} StatusDrone;

typedef struct {
    char        id[16];
    char        modelo[64];
    int         autonomia;               /* minutos */
    int         nivelDisponibilidade;    /* 0-100 (%) */
    float       capacidade;             /* kg */
    StatusDrone status;
    char        setorVinc[32];
    char        atividadesVinculadas[MAX_ATIV_DRONE][16]; /* códigos de missão/evento */
    int         totalAtividades;
} Drone;

extern Drone drones[MAX_DRONES];
extern int   totalDrones;

/* Retorna string do status para exibição */
const char *strStatusDrone(StatusDrone s);

#endif /* DRONE_H */
