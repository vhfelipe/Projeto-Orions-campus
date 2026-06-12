#ifndef CONSULTA_H
#define CONSULTA_H

#include "drone.h"
#include "operador.h"
#include "missao.h"
#include "evento.h"

/* Busca linear O(n) no vetor de drones.
 * Retorna ponteiro para o drone ou NULL se não encontrado */
Drone *buscarDronePorId(const char *id);

/* Busca linear O(n) no vetor de operadores.
 * Retorna ponteiro para o operador ou NULL se não encontrado */
Operador *buscarOperadorPorMatricula(const char *mat);

/* Busca linear O(n) no vetor de missões.
 * Retorna ponteiro para a missão ou NULL se não encontrado */
Missao *buscarMissaoPorCodigo(const char *codigo);

/* Travessia da lista de eventos (cabeça a cauda).
 * Retorna ponteiro para o NoEvento ou NULL se não encontrado */
NoEvento *buscarEventoPorCodigo(const char *codigo);

/* Constrói e exibe fila unificada ordenada */
void listarMissoesPorPrioridade(void);

/* Percorre lista de sentinela a sentinela, filtra EV_ABERTO e EV_EM_ATENDIMENTO */
void listarOcorrenciasAbertas(void);

/* Menus interativos para buscas */
void menuBuscarDrone(void);
void menuBuscarOperador(void);
void menuBuscarMissao(void);

#endif /* CONSULTA_H */
