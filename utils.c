#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void strcpy_safe(char *dst, const char *src, int tamanho) {
    strncpy(dst, src, tamanho - 1);
    dst[tamanho - 1] = '\0';
}

int compararHorario(const char *h1, const char *h2) {
    /* HH:MM — formato lexicográfico coincide com ordem cronológica */
    return strcmp(h1, h2);
}

int validarFormatoHorario(const char *horario) {
    int hh, mm;
    if (horario == NULL) return 0;
    if (strlen(horario) != 5) return 0;
    if (horario[2] != ':') return 0;
    if (!isdigit((unsigned char)horario[0])) return 0;
    if (!isdigit((unsigned char)horario[1])) return 0;
    if (!isdigit((unsigned char)horario[3])) return 0;
    if (!isdigit((unsigned char)horario[4])) return 0;
    hh = (horario[0] - '0') * 10 + (horario[1] - '0');
    mm = (horario[3] - '0') * 10 + (horario[4] - '0');
    if (hh < 0 || hh > 23) return 0;
    if (mm < 0 || mm > 59) return 0;
    return 1;
}

void lerString(char *dst, int tamanho) {
    int c;
    int i = 0;
    /* Lê até newline ou EOF, descarta excesso */
    while ((c = getchar()) != '\n' && c != EOF) {
        if (i < tamanho - 1) {
            dst[i++] = (char)c;
        }
    }
    dst[i] = '\0';
}

void removerNewline(char *str) {
    int len = (int)strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

int lerInteiro(const char *prompt, int min, int max) {
    int valor;
    char buffer[256];
    int tentativas = 0;

    while (tentativas < 3) {
        printf("%s", prompt);
        fflush(stdout);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("\n[INFO] Fim de entrada detectado.\n");
            return min; /* Retorna valor padrão ao EOF */
        }

        /* Remove newline */
        removerNewline(buffer);

        /* Verifica se é número */
        int i = 0;
        if (buffer[0] == '-') i = 1;
        for (; buffer[i] != '\0'; i++) {
            if (!isdigit((unsigned char)buffer[i])) {
                printf("[ERRO] Digite apenas numeros.\n");
                tentativas++;
                goto proxima_tentativa;
            }
        }

        if (sscanf(buffer, "%d", &valor) != 1) {
            printf("[ERRO] Valor invalido.\n");
            tentativas++;
            goto proxima_tentativa;
        }

        if (valor < min || valor > max) {
            printf("[ERRO] Valor deve estar entre %d e %d.\n", min, max);
            tentativas++;
            goto proxima_tentativa;
        }

        return valor;

        proxima_tentativa:
        continue;
    }

    printf("[ERRO] Numero maximo de tentativas excedido. Usando valor padrao %d.\n", min);
    return min;
}

float lerFloat(const char *prompt, float min, float max) {
    float valor;
    char buffer[256];
    int tentativas = 0;

    while (tentativas < 3) {
        printf("%s", prompt);
        fflush(stdout);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("\n[INFO] Fim de entrada detectado.\n");
            return min;
        }

        /* Remove newline */
        removerNewline(buffer);

        /* Verifica se é número */
        int i = 0, pontos = 0;
        if (buffer[0] == '-') i = 1;
        for (; buffer[i] != '\0'; i++) {
            if (buffer[i] == '.') {
                pontos++;
                if (pontos > 1) {
                    printf("[ERRO] Formato numerico invalido.\n");
                    tentativas++;
                    goto proxima_tentativa;
                }
            } else if (!isdigit((unsigned char)buffer[i])) {
                printf("[ERRO] Digite apenas numeros.\n");
                tentativas++;
                goto proxima_tentativa;
            }
        }

        if (sscanf(buffer, "%f", &valor) != 1) {
            printf("[ERRO] Valor invalido.\n");
            tentativas++;
            goto proxima_tentativa;
        }

        if (valor < min || valor > max) {
            printf("[ERRO] Valor deve estar entre %.2f e %.2f.\n", min, max);
            tentativas++;
            goto proxima_tentativa;
        }

        return valor;

        proxima_tentativa:
        continue;
    }

    printf("[ERRO] Numero maximo de tentativas excedido. Usando valor padrao %.2f.\n", min);
    return min;
}

int lerOpcao(const char *prompt, int min, int max) {
    int valor;
    char buffer[256];
    int tentativas = 0;

    while (tentativas < 3) {
        printf("%s", prompt);
        fflush(stdout); /* Garante que o prompt é exibido antes de ler */
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            /* EOF atingido: retorna 0 (ou min) para sair gracefully */
            printf("\n[INFO] Fim de entrada detectado.\n");
            return 0;
        }

        /* Remove newline */
        removerNewline(buffer);

        /* Verifica se é número */
        int i = 0;
        for (; buffer[i] != '\0'; i++) {
            if (!isdigit((unsigned char)buffer[i])) {
                printf("[ERRO] Digite apenas o numero da opcao.\n");
                tentativas++;
                goto proxima_tentativa;
            }
        }

        if (sscanf(buffer, "%d", &valor) != 1) {
            printf("[ERRO] Opcao invalida.\n");
            tentativas++;
            goto proxima_tentativa;
        }

        if (valor < min || valor > max) {
            printf("[ERRO] Opcao deve estar entre %d e %d.\n", min, max);
            tentativas++;
            goto proxima_tentativa;
        }

        return valor;

        proxima_tentativa:
        continue;
    }

    printf("[ERRO] Numero maximo de tentativas excedido. Usando opcao padrao %d.\n", min);
    return 0; /* Retorna 0 para sair do menu ao invés de min */
}
