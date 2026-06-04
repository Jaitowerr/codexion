#ifndef CODER_H
# define CODER_H

typedef struct s_coder
{
    int         id; //id de programador
    int         compile_count;  //contador de compilaciones totales
    struct timeval last_compile_start;  //momento en que empieza a compilar para calcular tiempo
    t_dongle    *left_dongle;   //puntero izquierdo a su dongle
    t_dongle    *right_dongle;  //puntero derecho al dongle del vecino
    t_config    *config;    /acceso a a todos lso tiempos y aprámetros globales
    // struct s_coder *next;
    // struct s_coder *prev;
} t_coder;

void init_coders(t_coder *coders, t_dongle *dongles, int count, t_config *config);

#endif