#ifndef KERNEL_H
#define KERNEL_H

// Códigos de retorno para os processos
#define SUCCESS 0
#define FAIL 1
#define REPEAT 2

typedef char (*ptrFunc) (void);

// Estrutura para representar um processo com informações de tempo real
struct process {
    ptrFunc func;         // Função do processo
    unsigned int deadline; // Deadline em ticks
    unsigned int period;   // Periodicidade para processos repetitivos

    // Construtor padrão
    process() : func(nullptr), deadline(0), period(0) {}

    // Operador de atribuição para não-volatile
    process& operator=(const process& other) {
        func = other.func;
        deadline = other.deadline;
        period = other.period;
        return *this;
    }
};

// Funções do kernel
char kernelAddProc(ptrFunc func, unsigned int deadline, unsigned int period);
char kernelInit(void);
void kernelLoop(void);
void kernelTick(void);  // Função para simular interrupções de timer
unsigned int getProcessCount(void);  // Função para obter o número de processos ativos

#endif

