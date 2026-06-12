#ifndef UTILS_H
#define UTILS_H

/* Copia string com segurança: sempre null-termina dst */
void strcpy_safe(char *dst, const char *src, int tamanho);

/* Compara horários no formato HH:MM.
 * Retorna <0 se h1 < h2, 0 se iguais, >0 se h1 > h2 */
int compararHorario(const char *h1, const char *h2);

/* Valida se string está no formato HH:MM.
 * Retorna 1 se válido, 0 se inválido */
int validarFormatoHorario(const char *horario);

/* Lê string do stdin com limite de tamanho; descarta excesso */
void lerString(char *dst, int tamanho);

/* Remove '\n' do final da string se presente */
void removerNewline(char *str);

/* Lê e valida entrada inteira com limites */
int lerInteiro(const char *prompt, int min, int max);

/* Lê e valida entrada float com limites */
float lerFloat(const char *prompt, float min, float max);

/* Lê e valida seleção de opção */
int lerOpcao(const char *prompt, int min, int max);

#endif /* UTILS_H */
