#ifndef queue_pid_h
#define queue_pid_h

#include <stdbool.h>
#include <stdint.h>

typedef struct queue_pid_cdt *queue_pid_adt;

// crea una nueva cola vacía
queue_pid_adt create_queue(void);

// agrega un pid al final de la cola
void enqueue(queue_pid_adt queue, int64_t pid);

// elimina y retorna el pid al frente de la cola
// retorna -1 si la cola está vacía
int64_t dequeue(queue_pid_adt queue);

// retorna el pid al frente sin eliminarlo
// retorna -1 si la cola está vacía
int64_t peek(queue_pid_adt queue);

// retorna true si la cola está vacía, false en caso contrario
bool is_empty(queue_pid_adt queue);

// retorna la cantidad actual de elementos en la cola
uint64_t get_queue_size(queue_pid_adt queue);

// elimina todos los elementos de la cola
void clear_queue(queue_pid_adt queue);

// verifica si un pid específico existe en la cola
bool contains_pid(queue_pid_adt queue, int64_t pid);

// libera toda la memoria asociada a la cola
void destroy_queue(queue_pid_adt queue);

void dequeue_pid(queue_pid_adt queue, int64_t pid);

#endif // queue_pid_h