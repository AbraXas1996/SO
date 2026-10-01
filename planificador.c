#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>

#define max_str 100

// struct va fuera del main para q sea visible globalmente (1: prender_carbon: 500:)
    typedef struct 
    
    {
        char id_actividad[max_str]; // id guardado como texto
        char nombre_actividad[max_str];
        int tiempo_ms; // tiempo guardado como texto
        char dependencias[max_str];

        // INICIALIZAR EL BOOL EN FALSE
        bool estado_dependencias; // Esto nos servira para validar si una actividad puede ejecutarse o no, dependiendo de si sus dependencias han sido completadas
        /* data */
    } planificador;
    
    planificador Actividades[10000];
    int total_actividades = 0;
    // Parseo de los datos
int parseo_planificador_strtok(char *linea, planificador *p){
    char *token;

    // obtener el ID
    token = strtok(linea, ":"); // La función strtok() lee "cadena" como una serie de cero o más señales, y ':' como el conjunto de caracteres que sirven como delimitadores de las señales en cadena
    if(token==NULL) return 0;
    sscanf(token, " %s", p->id_actividad);

    // obtener el nombre de la actvidad
    token = strtok(NULL, ":");
    if(token==NULL) return 0;
    sscanf(token," %s", p->nombre_actividad);

    // lo mismo para el timepo (este es un int asiq le aplicamos atoi)
    token = strtok(NULL, ":\n");
    if(token==NULL) return 0;
    p->tiempo_ms = atoi(token); // convierte una cadena '500' a 500 

    // ahora para las dependencias
    token = strtok(NULL, ":\n"); // No hay ":" al final de cada línea
    if(token!=NULL) //ya que podrían no haber dependencias
    {
    strncpy(p->dependencias, token, sizeof(p->dependencias)-1);
    p->dependencias[sizeof(p->dependencias)-1]='\0';}; 
    
    
    return 1;
}

int validar_dependencias(planificador *p){

    p->estado_dependencias = false; // Inicializamos el estado de dependencias como falso

    strcpy(p->dependencias, p->dependencias); // Copiamos las dependencias a una variable temporal para no modificar el original

    if(strcmp(p->dependencias, "") == 0) {
        p->estado_dependencias = true; // No hay dependencias, se puede ejecutar
        return 1;
    }


    return 0;
}

int main (int argc, char* argv[]) {

bool error_entrada = false;

if (argc != 3) {
    printf("\nERROR: Se esperan 3 argumentos [./planificador, plan.txt, K] donde K es el límite de concurrencia\n");
    error_entrada = true;
}

if(strcmp(argv[0], "./planificador") != 0 && strcmp(argv[0], "planificador") != 0) // usando strcmp para comparar strings
    {
        printf("\nERROR: Ha ingresado incorrectamente el primer argumento\nSe espera: ./planificador\n");
        error_entrada = true;
    };


if (strcmp(argv[1], "plan.txt") != 0) {
    printf("\nERROR: Ha ingresado mal el nombre del archivo de texto\nSe espera: plan.txt\n");
    error_entrada = true;
}

if (atoi(argv[2]) <= 0) {
    printf("\nERROR: Valor K (límite de concurrencia) inválido, por favor ingrese un númeor entero mayor que 0\n");
    error_entrada = true;
}

if (error_entrada) return 1;

    // mejor abrir el archivo con argv[1] para que sea más flexible y no dependa del nombre del archivo
    FILE *fp = fopen(argv[1], "r");
    char buffer[max_str];

    if (fp == NULL) {
        printf("\nERROR: archivo %s no encontrado, asegúrese que se encuentre en el mismo directorio que planificador.c\n", argv[1]);
        return 1;
    }

    // Lee línea por línea hasta el final del archivo
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    buffer[strcspn(buffer, "\n")] = 0; // Se limpia el salto de línea al final de cada línea leída

    if(strlen(buffer) == 0) continue; // Omitir lineas vacías

    if(parseo_planificador_strtok(buffer, &Actividades[total_actividades])) {
        total_actividades++;
    } else {
        printf("\nERROR: Formato de línea inválido en el archivo de texto: %s\n", buffer);
        fclose(fp);
        return 1;
    }
   }
   fclose(fp);
   printf("\nSe han cargado %d actividades desde el archivo %s\n", total_actividades, argv[1]);

    return 0;

}