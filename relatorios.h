#ifndef RELATORIOS_H
#define RELATORIOS_H

/* REL01 — Status geral de todos os drones */
void relStatusDrones(void);

/* REL02 — Status geral de todos os operadores */
void relStatusOperadores(void);

/* REL03 — Missões por status (filtro opcional) */
void relMissoesPorStatus(void);

/* REL03B — Agenda operacional do dia em ordem de horário */
void relAgendaDia(void);

/* REL04 — Fila de missões pendentes ordenada por prioridade */
void relMissoesPendentes(void);

/* REL05 — Ocorrências abertas e em atendimento */
void relOcorrenciasAbertas(void);

/* REL06 — Histórico completo (mais recente ao mais antigo) */
void relHistorico(void);

/* REL07 — Atividades por drone (fonte canônica: histórico) */
void relAtividadesPorDrone(void);

/* REL08 — Atividades por operador */
void relAtividadesPorOperador(void);

/* Menu de relatórios */
void menuRelatorios(void);

#endif /* RELATORIOS_H */
