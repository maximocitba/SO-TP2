#include "../include/semaphore.h"
#include "../include/scheduler.h"
#include "../include/linked_list.h"
#include "../include/queue_pid.h"
// #include <syscalls.h>
#include "../include/IO.h"
#include "../include/memman.h"

static sem_t *semaphores[max_semaphores] = {NULL};

static int find_sem(int64_t id){
    for(int i = 0; i<max_semaphores;i++){
        if(semaphores[i] !=NULL && semaphores[i]->id == id){
            return i;
        }
    }
    return -1;
}

static void lock_semaphore(sem_t *sem){
    acquire(&sem->mutex);
}

static void unlock_semaphore(sem_t *sem){
    release(&sem->mutex);
}

int32_t sem_open(int64_t id, int64_t value){
    int find = find_sem(id);
    if(find != -1){
        return find;
    }
    if(semaphores[id] == NULL){
        semaphores[id]= (sem_t *) b_alloc(sizeof(sem_t));
        semaphores[id]->id = id;
        semaphores[id]->value = value;
        semaphores[id]->mutex = 1;
        semaphores[id]->list = create_queue();
        return 0;
    }
    return -1;
}

int32_t sem_create(int64_t value){
    for(int i = 0; i< max_semaphores; i++){
        if(semaphores[i] == NULL){
            int64_t id = 1;
            while(find_sem(id) != -1){
                id++;
            }
            if(sem_open(id,value)==0){
                return id;
            }
            return -1;
        }
    }
    return -1;
}

void sem_wait(int64_t id){
    int idx = find_sem(id);
    if(idx == -1){
        return;
    }
    sem_t *sem = semaphores[idx];
    lock_semaphore(sem);
    while(sem->value == 0){
        int64_t current_pid = get_current_pid();
        enqueue(sem->list,current_pid);
        unlock_semaphore(sem);
        block_process(current_pid);
        yield();
        lock_semaphore(sem);
    }
    sem->value--;
    unlock_semaphore(sem);
}

void sem_post(int64_t id){
    int idx = find_sem(id);
    if(idx == -1){
        return;
    }
    sem_t *sem = semaphores[idx];
    lock_semaphore(sem);
    sem->value++;
    if(sem->value != 0){
        int64_t waiting_id = peek(sem->list);
        if(waiting_id != -1){
            dequeue(sem->list);
            unblock_process(waiting_id);
        }
    }
    unlock_semaphore(sem);
}

void sem_close(int64_t id){
    int idx = find_sem(id);
    if(idx == -1){
        return;
    }
    destroy_queue(semaphores[idx]->list);
    b_free(semaphores[idx]);
    semaphores[idx]= NULL;
}

void sem_cleanup_process(int64_t pid){
    for(int i = 0; i< max_semaphores; i++){
        if(semaphores[i] != NULL){
            queue_pid_adt waiting_list = create_queue();
            while(!is_empty(semaphores[i]->list)){
                int64_t current_id = dequeue(semaphores[i]->list);
                if(current_id != pid){
                    enqueue(waiting_list,current_id);
                }
            }
            destroy_queue(semaphores[i]->list);
            semaphores[i]->list = waiting_list;
        }
    }
}

void remove_from_all_semaphores(uint32_t pid){
    for(int i = 0; i < max_semaphores; i++){
        if(semaphores[i] != NULL){
            dequeue_pid(semaphores[i]->list, pid);
        }
    }
}

