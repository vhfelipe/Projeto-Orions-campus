#ifndef OPERACAO_H
#define OPERACAO_H

#include "missao.h"
#include "evento.h"
#include "drone.h"
#include "operador.h"

/* Valida disponibilidade e turno; atualiza status do drone e operador.
 * Retorna 0 se ok, -1 se erro */
int alocarRecursos(const char *codMissao, const char *idDrone, const char *matOp);

/* Muda status para EM_EXECUCAO; verifica alocação prévia.
 * Retorna 0 se ok, -1 se erro */
int iniciarMissao(const char *codMissao);

/* Aplica transição de estado conforme máquina de estados.
 * USO INTERNO ao módulo — não expor diretamente no menu.
 * Retorna 0 se ok, -1 se transição inválida */
int atualizarStatus(const char *codMissao, StatusMissao novoStatus);

/* Marca original como REPLANEJADA, empilha no histórico, insere nova.
 * Retorna 0 se ok, -1 se erro */
int replanejarMissao(const char *codMissao, Missao novaMissao);

/* Libera drone e operador; empilha no histórico com CANCELADA.
 * Retorna 0 se ok, -1 se erro */
int cancelarMissao(const char *codMissao, const char *motivo);

/* Remove evento da lista; empilha no histórico com CONCLUIDA.
 * Retorna 0 se ok, -1 se erro */
int encerrarEvento(const char *codEvento);

/* Aloca recursos para um evento dinâmico.
 * Retorna 0 se ok, -1 se erro */
int alocarRecursosEvento(const char *codEvento, const char *idDrone, const char *matOp);

/* Inicia atendimento de evento (ABERTO -> EM_ATENDIMENTO).
 * Retorna 0 se ok, -1 se erro */
int iniciarAtendimentoEvento(const char *codEvento);

/* Coloca drone em manutenção (DISPONIVEL ou EM_MISSAO -> MANUTENCAO).
 * Se em missão, interrompe a missão associada.
 * Retorna 0 se ok, -1 se erro */
int colocarDroneEmManutencao(const char *idDrone);

/* Confirma retorno do drone da manutenção (MANUTENCAO -> DISPONIVEL).
 * Retorna 0 se ok, -1 se erro */
int confirmarRetornoManutencao(const char *idDrone);

/* Confirma retorno do drone de missão (RETORNANDO -> DISPONIVEL).
 * Retorna 0 se ok, -1 se erro */
int confirmarRetornoDrone(const char *idDrone);

/* Comunica ausência do operador (ATIVO -> AUSENTE).
 * Retorna 0 se ok, -1 se erro */
int comunicarAusenciaOperador(const char *mat);

/* Retorna operador ao serviço (AUSENTE -> ATIVO).
 * Retorna 0 se ok, -1 se erro */
int retornarOperadorAoServico(const char *mat);

/* Cancela evento aberto (ABERTO -> ENCERRADO).
 * Retorna 0 se ok, -1 se erro */
int cancelarEvento(const char *codEvento, const char *motivo);

/* Menus interativos */
void menuAlocarRecursos(void);
void menuIniciarMissao(void);
void menuConcluirMissao(void);
void menuCancelarMissao(void);
void menuReplanejarMissao(void);
void menuEncerrarEvento(void);
void menuAlocarRecursosEvento(void);
void menuIniciarAtendimentoEvento(void);
void menuGestaoStatusDrone(void);
void menuGestaoStatusOperador(void);
void menuCancelarEvento(void);

#endif /* OPERACAO_H */
