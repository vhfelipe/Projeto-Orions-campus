#!/bin/bash
echo "Compilando Orion Campus..."
gcc -Wall -Wextra -std=c99 -o orion utils.c drone.c operador.c missao.c evento.c historico.c fila.c cadastro.c consulta.c operacao.c relatorios.c main.c
if [ $? -eq 0 ]; then
    echo "✓ Compilacao concluida!"
else
    echo "✗ Erro na compilacao!"
fi
