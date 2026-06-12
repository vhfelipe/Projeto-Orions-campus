#include "drone.h"
#include <string.h>

/* Definição das variáveis globais */
Drone drones[MAX_DRONES];
int   totalDrones = 0;

const char *strStatusDrone(StatusDrone s) {
    switch (s) {
        case DRONE_DISPONIVEL:  return "DISPONIVEL";
        case DRONE_EM_MISSAO:   return "EM_MISSAO";
        case DRONE_MANUTENCAO:  return "MANUTENCAO";
        case DRONE_RETORNANDO:  return "RETORNANDO";
        default:                return "DESCONHECIDO";
    }
}
