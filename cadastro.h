#ifndef CADASTRO_H
#define CADASTRO_H

#include "drone.h"
#include "operador.h"
#include "missao.h"
#include "evento.h"

/* Valida e insere drone no vetor global.
 * Retorna 0 se ok, -1 se erro */
int cadastrarDrone(Drone d);

/* Valida e insere operador no vetor global.
 * Retorna 0 se ok, -1 se erro */
int cadastrarOperador(Operador op);

/* Valida e insere missão no vetor global.
 * Retorna 0 se ok, -1 se erro */
int cadastrarMissao(Missao m);

/* Cria nó e insere após a sentinela cabeça em O(1).
 * Retorna 0 se ok, -1 se erro */
int registrarEvento(EventoDinamico ev);

/* Funções de entrada interativa pelo terminal */
void menuCadastrarDrone(void);
void menuCadastrarOperador(void);
void menuCadastrarMissao(void);
void menuRegistrarEvento(void);

#endif /* CADASTRO_H */
