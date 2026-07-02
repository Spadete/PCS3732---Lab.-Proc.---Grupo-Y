#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define NUM_MEASUREMENTS 1000

int soma(int a, int b) { return a + b; }
int subtracao(int a, int b) { return a - b; }
int multiplicacao(int a, int b) { return a * b; }

float divisao(int a, int b) {
    return (float)a / (float)b;
}

unsigned long long fatorial(int n) {
    if (n < 0) return 0;
    if (n == 0 || n == 1) return 1;
    unsigned long long result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

void run_benchmark() {
    struct timespec start, end;
    double times[NUM_MEASUREMENTS];
    double sum_time = 0.0, mean_time = 0.0, variance = 0.0, std_dev = 0.0;
    int a = 15, b = 7; 

    printf("\n--- Iniciando Benchmark (%d iteracoes) ---\n", NUM_MEASUREMENTS);

    for (int i = 0; i < NUM_MEASUREMENTS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        
        volatile int r1 = soma(a, b);
        volatile int r2 = subtracao(a, b);
        volatile int r3 = multiplicacao(a, b);
        volatile float r4 = divisao(a, b);
        volatile unsigned long long r5 = fatorial(12);

        clock_gettime(CLOCK_MONOTONIC, &end);
        
        double elapsed = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec); 
        times[i] = elapsed;
        sum_time += elapsed;
    }

    mean_time = sum_time / NUM_MEASUREMENTS;

    for (int i = 0; i < NUM_MEASUREMENTS; i++) {
        variance += pow(times[i] - mean_time, 2);
    }
    variance /= NUM_MEASUREMENTS;
    std_dev = sqrt(variance);

    printf("Tempo medio de execucao: %.2f ns\n", mean_time);
    printf("Desvio padrao: %.2f ns\n", std_dev);
}

int main() {
    int op, a, b;
    struct timespec start, end;
    double elapsed;
    
    printf("Calculadora Binaria - Raspberry Pi 3\n");
    printf("1: Soma\n2: Subtracao\n3: Multiplicacao\n4: Divisao\n5: Fatorial\n6: Executar Benchmark\n");
    printf("Escolha a operacao: ");
    if (scanf("%d", &op) != 1) return 1;

    if (op >= 1 && op <= 4) {
        printf("Digite os dois valores: ");
        scanf("%d %d", &a, &b);
        a &= 0x0F; b &= 0x0F; 
        printf("Valores truncados para 4 bits: A=%d, B=%d\n", a, b);
        
        if (op == 1) {
            clock_gettime(CLOCK_MONOTONIC, &start);
            int res = soma(a, b);
            clock_gettime(CLOCK_MONOTONIC, &end);
            printf("Resultado: %d\n", res);
        } else if (op == 2) {
            clock_gettime(CLOCK_MONOTONIC, &start);
            int res = subtracao(a, b);
            clock_gettime(CLOCK_MONOTONIC, &end);
            printf("Resultado: %d\n", res);
        } else if (op == 3) {
            clock_gettime(CLOCK_MONOTONIC, &start);
            int res = multiplicacao(a, b);
            clock_gettime(CLOCK_MONOTONIC, &end);
            printf("Resultado: %d\n", res);
        } else if (op == 4) {
            clock_gettime(CLOCK_MONOTONIC, &start);
            float res = divisao(a, b);
            clock_gettime(CLOCK_MONOTONIC, &end);
            printf("Resultado: %.2f\n", res);
        }
        
        elapsed = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
        printf("Tempo de execucao: %.2f ns\n", elapsed);

    } else if (op == 5) {
        printf("Digite o valor para fatorial (limitado a 4 bits): ");
        scanf("%d", &a);
        a &= 0x0F;
        
        clock_gettime(CLOCK_MONOTONIC, &start);
        unsigned long long res = fatorial(a);
        clock_gettime(CLOCK_MONOTONIC, &end);
        
        printf("Fatorial de %d: %llu\n", a, res);
        
        elapsed = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
        printf("Tempo de execucao: %.2f ns\n", elapsed);

    } else if (op == 6) {
        run_benchmark();
    } else {
        printf("Operacao invalida.\n");
    }

    return 0;
}