# ============================================================
#  Orion Campus — Makefile
#  Uso:
#    make          -> compila o projeto (gera ./orion)
#    make run      -> compila e executa
#    make clean    -> remove binário e objetos
#    make rebuild  -> limpa e recompila do zero
# ============================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99
TARGET  = orion

SRCS = utils.c \
       drone.c \
       operador.c \
       missao.c \
       evento.c \
       historico.c \
       fila.c \
       cadastro.c \
       consulta.c \
       operacao.c \
       relatorios.c \
       main.c

OBJS = $(SRCS:.c=.o)

# ── Alvo padrão ──────────────────────────────────────────────
all: $(TARGET)
	@echo ""
	@echo "  Compilacao concluida: ./$(TARGET)"
	@echo "  Para executar: make run"
	@echo ""

# ── Link final ───────────────────────────────────────────────
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# ── Compilação de cada .c em .o ──────────────────────────────
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ── Executar ─────────────────────────────────────────────────
run: all
	./$(TARGET)

# ── Limpar binário e objetos ─────────────────────────────────
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "  Projeto limpo."

# ── Recompilar do zero ───────────────────────────────────────
rebuild: clean all

.PHONY: all run clean rebuild
