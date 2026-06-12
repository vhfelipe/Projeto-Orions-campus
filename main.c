#include <stdio.h>
#include <string.h>

#include "drone.h"
#include "operador.h"
#include "missao.h"
#include "evento.h"
#include "historico.h"
#include "cadastro.h"
#include "consulta.h"
#include "operacao.h"
#include "relatorios.h"
#include "utils.h"

/* ─────────────────────────────────────────────
   INICIALIZAÇÃO CENTRALIZADA DO SISTEMA (R03/Skill1)
   Garante que todas as estruturas globais são
   inicializadas antes de qualquer operação.
   ───────────────────────────────────────────── */
static void inicializarSistema(void) {
    int i, j;

    /* Vetores estáticos -- inicialização explícita */
    totalDrones     = 0;
    totalOperadores = 0;
    totalMissoes    = 0;

    for (i = 0; i < MAX_DRONES; i++) {
        memset(&drones[i], 0, sizeof(Drone));
    }
    for (i = 0; i < MAX_OPERADORES; i++) {
        memset(&operadores[i], 0, sizeof(Operador));
    }
    for (i = 0; i < MAX_MISSOES; i++) {
        memset(&missoes[i], 0, sizeof(Missao));
    }

    /* Suprimir warning de variável não usada em loops internos */
    (void)i; (void)j;

    /* Lista encadeada de eventos -- sentinelas */
    inicializarListaEventos();

    /* Pilha de histórico */
    historico.topo = -1;

    printf("  Sistema Orion Campus inicializado.\n");
}

/* ─────────────────────────────────────────────
   MENU DE CADASTROS
   ───────────────────────────────────────────── */
static void menuCadastros(void) {
    int opcao;
    do {
        printf("\n==============================\n");
        printf("  CADASTROS - ORION CAMPUS\n");
        printf("==============================\n");
        printf("  1 - Cadastrar Drone\n");
        printf("  2 - Cadastrar Operador\n");
        printf("  3 - Cadastrar Missao Planejada\n");
        printf("  4 - Registrar Evento Dinamico\n");
        printf("  0 - Voltar\n");
        printf("------------------------------\n");
        opcao = lerOpcao("  Opcao: ", 0, 4);

        switch (opcao) {
            case 1: menuCadastrarDrone();    break;
            case 2: menuCadastrarOperador(); break;
            case 3: menuCadastrarMissao();   break;
            case 4: menuRegistrarEvento();   break;
            case 0: break;
            default: printf("  [ERRO] Opcao invalida.\n");
        }
    } while (opcao != 0);
}

/* ─────────────────────────────────────────────
   MENU DE OPERAÇÕES
   ───────────────────────────────────────────── */
static void menuOperacoes(void) {
    int opcao;
    do {
        printf("\n==============================\n");
        printf("  OPERACOES - ORION CAMPUS\n");
        printf("==============================\n");
        printf("  --- MISSOES PLANEJADAS ---\n");
        printf("  1  - Alocar Recursos para Missao\n");
        printf("  2  - Iniciar Missao\n");
        printf("  3  - Concluir Missao\n");
        printf("  4  - Cancelar Missao\n");
        printf("  5  - Replanejar Missao\n");
        printf("  --- EVENTOS DINAMICOS ---\n");
        printf("  6  - Alocar Recursos para Evento\n");
        printf("  7  - Iniciar Atendimento de Evento\n");
        printf("  8  - Encerrar Evento\n");
        printf("  9  - Cancelar Evento\n");
        printf("  --- GESTAO DE RECURSOS ---\n");
        printf("  10 - Gestao de Status do Drone\n");
        printf("  11 - Gestao de Status do Operador\n");
        printf("  0  - Voltar\n");
        printf("------------------------------\n");
        opcao = lerOpcao("  Opcao: ", 0, 11);

        switch (opcao) {
            case 1:  menuAlocarRecursos();            break;
            case 2:  menuIniciarMissao();             break;
            case 3:  menuConcluirMissao();            break;
            case 4:  menuCancelarMissao();            break;
            case 5:  menuReplanejarMissao();          break;
            case 6:  menuAlocarRecursosEvento();      break;
            case 7:  menuIniciarAtendimentoEvento();  break;
            case 8:  menuEncerrarEvento();            break;
            case 9:  menuCancelarEvento();            break;
            case 10: menuGestaoStatusDrone();         break;
            case 11: menuGestaoStatusOperador();      break;
            case 0:  break;
            default: printf("  [ERRO] Opcao invalida.\n");
        }
    } while (opcao != 0);
}

/* ─────────────────────────────────────────────
   MENU DE CONSULTAS
   ───────────────────────────────────────────── */
static void menuConsultas(void) {
    int opcao;
    do {
        printf("\n==============================\n");
        printf("  CONSULTAS - ORION CAMPUS\n");
        printf("==============================\n");
        printf("  1 - Buscar Drone por ID\n");
        printf("  2 - Buscar Operador por Matricula\n");
        printf("  3 - Buscar Missao por Codigo\n");
        printf("  4 - Listar Fila de Prioridade\n");
        printf("  5 - Listar Ocorrencias Abertas\n");
        printf("  6 - Historico por Drone\n");
        printf("  7 - Historico por Operador\n");
        printf("  0 - Voltar\n");
        printf("------------------------------\n");
        opcao = lerOpcao("  Opcao: ", 0, 7);

        switch (opcao) {
            case 1: menuBuscarDrone();           break;
            case 2: menuBuscarOperador();        break;
            case 3: menuBuscarMissao();          break;
            case 4: listarMissoesPorPrioridade(); break;
            case 5: listarOcorrenciasAbertas();   break;
            case 6: {
                char id[16];
                printf("ID do drone: ");
                lerString(id, sizeof(id));
                filtrarPorDrone(id);
                break;
            }
            case 7: {
                char mat[16];
                printf("Matricula: ");
                lerString(mat, sizeof(mat));
                filtrarPorOperador(mat);
                break;
            }
            case 0: break;
            default: printf("  [ERRO] Opcao invalida.\n");
        }
    } while (opcao != 0);
}

/* ─────────────────────────────────────────────
   MENU PRINCIPAL
   ───────────────────────────────────────────── */
static void menuPrincipal(void) {
    int opcao;

    do {
        printf("\n");
        printf("=============================================\n");
        printf("    ORION CAMPUS - SISTEMA OPERACIONAL       \n");
        printf("    Controle de Drones | Seguranca Campus    \n");
        printf("==============================================\n");
        printf("  1 - Cadastros\n");
        printf("  2 - Operacoes\n");
        printf("  3 - Consultas\n"); 
        printf("  4 - Relatorios\n");
        printf("  0 - Encerrar Sistema\n");
        printf("----------------------------------------------\n");
        printf("  Drones: %d  |  Operadores: %d  |  Missoes: %d\n",
               totalDrones,
               totalOperadores,
               totalMissoes);
        printf("----------------------------------------------\n");
        opcao = lerOpcao("  Opcao: ", 0, 4);

        switch (opcao) {
            case 1: menuCadastros();  break;
            case 2: menuOperacoes();  break;
            case 3: menuConsultas();  break;
            case 4: menuRelatorios(); break;
            case 0:
                printf("\n  Encerrando Orion Campus. Ate logo.\n\n");
                break;
            default:
                printf("  [ERRO] Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);
}

/* ─────────────────────────────────────────────
   PONTO DE ENTRADA
   ───────────────────────────────────────────── */
int main(void) {
    printf("\n");
    printf("==============================================\n");
    printf("  Inicializando ORION CAMPUS...\n");
    printf("==============================================\n");

    inicializarSistema();

    menuPrincipal();

    return 0;
}
