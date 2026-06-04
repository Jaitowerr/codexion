#ifndef DONGLE_H
# define DONGLE_H

typedef struct s_dongle
{
    int             id; //identificador mismo que coder
    bool            taken;  //si está siendo usado o no
    int             coder_id;   // el id del coder al que pertenece
    struct timeval  available_at;   //fecha/hora en el qiue estará disponible  dongle_cooldown
    pthread_mutex_t mutex;  //sirve para proteger acceso porque varios programadores pueden intentar cogerlo a la vez
    pthread_cond_t  cond;   //sirve para avisar a los hilos que esperana que esté libre
    // struct s_dongle *next;
    // struct s_dongle *prev;
} t_dongle;

void init_dongles(t_dongle *dongles, int count);

#endif