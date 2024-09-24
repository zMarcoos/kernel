#include "kernel.h"
#include <stdio.h>

#define MAX_PROCESSES 10  // Número máximo de processos no pool

// Estrutura do pool de processos
process processPool[MAX_PROCESSES];  // Pool de processos, agora sem volatile
volatile unsigned int processCount = 0;       // Contador de processos no pool (com volatile)
volatile unsigned int systemTick = 0;         // Tick global do sistema (com volatile)

// Função para adicionar processos ao pool
char kernelAddProc(ptrFunc func, unsigned int deadline, unsigned int period) {
    if (processCount < MAX_PROCESSES) {
        processPool[processCount].func = func;
        processPool[processCount].deadline = systemTick + deadline;  // Configura o deadline baseado no tick atual
        processPool[processCount].period = period;
        processCount++;
        return SUCCESS;
    }
    return FAIL;  // Retorna falha se o pool estiver cheio
}

// Inicializa o kernel, resetando o pool de processos e o tick do sistema
char kernelInit(void) {
    processCount = 0;
    systemTick = 0;
    return SUCCESS;
}

// Simula a interrupção de temporizador, incrementando o tick global
void kernelTick(void) {
    systemTick++;  // Simula um evento de tick (tempo avançando)
}

// Função principal do kernel que gerencia a execução dos processos
void kernelLoop(void) {
    for (unsigned int i = 0; i < processCount; i++) {
        // Verifica se o processo está pronto para ser executado (deadline alcançado)
        if (processPool[i].deadline <= systemTick) {
            char result = processPool[i].func();  // Executa o processo

            if (result == SUCCESS || result == FAIL) {
                // Remove o processo se ele retornou SUCCESS ou FAIL
                for (unsigned int j = i; j < processCount - 1; j++) {
                    processPool[j] = processPool[j + 1];  // Shift dos processos para a esquerda
                }
                processCount--;
                i--;  // Ajusta o índice para o próximo processo
            } else if (result == REPEAT) {
                // Se o processo é repetitivo, reconfigura seu deadline baseado no período
                processPool[i].deadline = systemTick + processPool[i].period;
            }
        }
    }
}

// Função que retorna o número de processos ainda no pool
unsigned int getProcessCount(void) {
    return processCount;  // Retorna o número de processos ativos
}

