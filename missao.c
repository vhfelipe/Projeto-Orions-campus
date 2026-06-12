#include "missao.h"

Missao missoes[MAX_MISSOES];
int    totalMissoes = 0;

const char *strStatusMissao(StatusMissao s) {
    switch (s) {
        case MISSAO_PENDENTE:     return "PENDENTE";
        case MISSAO_EM_EXECUCAO:  return "EM_EXECUCAO";
        case MISSAO_CONCLUIDA:    return "CONCLUIDA";
        case MISSAO_CANCELADA:    return "CANCELADA";
        case MISSAO_REPLANEJADA:  return "REPLANEJADA";
        case MISSAO_INTERROMPIDA: return "INTERROMPIDA";
        default:                  return "DESCONHECIDO";
    }
}

const char *strTipoMissao(TipoMissao t) {
    switch (t) {
        case TIPO_RONDA:          return "RONDA";
        case TIPO_VISTORIA:       return "VISTORIA";
        case TIPO_INSPECAO:       return "INSPECAO";
        case TIPO_ACOMPANHAMENTO: return "ACOMPANHAMENTO";
        case TIPO_OUTRO:          return "OUTRO";
        default:                  return "DESCONHECIDO";
    }
}
