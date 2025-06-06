#include <IO.h>
#include <definitions.h>
#include <lib.h>
#include <memman.h>
#include <pipes.h>
#include <scheduler.h>

typedef struct pipe_t {
    uint16_t id;
    char buffer[pipe_buffer_size];
    uint16_t buffer_count;
    uint16_t start_position;
    uint16_t input_pid;
    uint16_t output_pid;
    uint16_t open;
} pipe_t;

typedef struct pipe_manager_cdt {
    pipe_t *pipes[max_pipes];
    uint16_t count;
    uint16_t next_pipe_id;
} pipe_manager_cdt;

#define buffer_position(pipe_t) (((pipe_t)->buffer_count + (pipe_t)->start_position) % pipe_buffer_size)

static pipe_t *get_pipe(uint16_t id);
static pipe_manager_adt get_pipe_manager();
static uint16_t get_next_pipe();
static void free_pipe(uint16_t id);

static pipe_manager_adt get_pipe_manager() {
    return (pipe_manager_adt)pipe_manager_address;
}

static uint16_t get_next_pipe() {
    pipe_manager_adt manager = get_pipe_manager();
    if (manager->count >= max_pipes) {
        printf("Maximo de pipes alcanzado\n");
        return -1;
    }
    while (manager->pipes[manager->next_pipe_id] != NULL) {
        manager->next_pipe_id = (manager->next_pipe_id + 1) % max_pipes;
    }
    return manager->next_pipe_id;
}

static pipe_t *get_pipe(uint16_t id) {
    pipe_manager_adt manager = get_pipe_manager();
    pipe_t *pipe = manager->pipes[id];
    if (pipe == NULL || id < BUILTIN_FDS) {
        printf("No existe esa pipe1\n");
        return NULL;
    }
    return pipe;
}

static void free_pipe(uint16_t id) {
    pipe_manager_adt manager = get_pipe_manager();
    pipe_t *pipe = manager->pipes[id];
    b_free(pipe);
    manager->pipes[id] = NULL;
    manager->count--;
}

pipe_manager_adt init_pipe_manager() {
    pipe_manager_adt manager = (pipe_manager_adt)pipe_manager_address;
    for (uint16_t i = BUILTIN_FDS; i < max_pipes; i++) {
        manager->pipes[i] = NULL;
    }

    manager->pipes[STDIN] = (pipe_t *)b_alloc(sizeof(pipe));
    manager->pipes[STDOUT] = (pipe_t *)b_alloc(sizeof(pipe));
    manager->pipes[STDERR] = (pipe_t *)b_alloc(sizeof(pipe));

    manager->next_pipe_id = BUILTIN_FDS;
    manager->count = BUILTIN_FDS;
    return manager;
}

uint16_t create_pipe() {
    pipe_manager_adt manager = get_pipe_manager();
    pipe_t *pipe = b_alloc(sizeof(struct pipe_t));
    pipe->id = get_next_pipe();
    pipe->buffer_count = 0;
    pipe->start_position = 0;
    manager->pipes[pipe->id] = pipe;
    pipe->open = closed;
    pipe->input_pid = -1;
    pipe->output_pid = -1;
    manager->count++;
    return pipe->id;
}

uint16_t open_pipe(uint16_t pid, uint16_t id, uint8_t mode) {
    pipe_t *pipe = get_pipe(id);
    if (pipe == NULL) {
        printf("Esa pipe no existe2\n");
        return -1;
    }
    pipe->open = opened;
    if (mode == read_mode && pipe->output_pid == -1) {
        pipe->output_pid = pid;
    }
    if (mode == write_mode && pipe->input_pid == -1) {
        pipe->input_pid = pid;
    }
    return 0;
}

uint16_t close_pipe_by_pid(uint16_t pid, uint16_t id) {
    pipe_t *pipe = get_pipe(id);
    if (pipe == NULL) {
        printf("Esa pipe no existe3\n");
        return -1;
    }

    pipe->open = closed;

    if (pipe->input_pid == pid) {
        char eof_string[1] = {EOF};
        write_pipe(pid, id, eof_string, 1);
    } else if (pipe->output_pid == pid) {
        free_pipe(id);
    } else {
        return -1;
    }
    return 0;
}

uint16_t close_pipe(uint16_t id) {
    return close_pipe_by_pid(get_current_pid(), id);
}

uint16_t write_pipe(uint16_t pid, uint16_t id, const char *data, uint16_t size) {
    pipe_t *pipe = get_pipe(id);
    if (size == 0 || data == NULL || pipe == NULL || pipe->input_pid != pid) {
        return -1;
    }
    uint16_t bytes_written = 0;
    while (bytes_written < size && (int)pipe->buffer[buffer_position(pipe)] != EOF) {
        if (pipe->buffer_count >= pipe_buffer_size) {
            pipe->open = closed;
            block_process(pipe->input_pid);
            yield();
        }
        while (pipe->buffer_count < pipe_buffer_size && bytes_written < size) {
            pipe->buffer[buffer_position(pipe)] = data[bytes_written];
            if ((int)data[bytes_written++] == EOF) {
                break;
            }
            pipe->buffer_count++;
        }
        if (pipe->open == closed) {
            unblock_process((uint16_t)pipe->output_pid);
            pipe->open = opened;
        }
    }
    return bytes_written;
}

uint16_t read_pipe(uint16_t pid, uint16_t id, char *data, uint16_t size) {
    pipe_t *pipe = get_pipe(id);
    if (pipe == NULL || pipe->output_pid != pid) {
        return -1;
    }
    uint8_t eof_flag = 0;
    uint64_t bytes_readed = 0;
    while (bytes_readed < size && !eof_flag) {
        if (pipe->buffer_count == 0 && (int)pipe->buffer[pipe->start_position] != EOF) {
            pipe->open = closed;
            block_process(pipe->output_pid);
            yield();
        }
        while ((pipe->buffer_count > 0 || (int)pipe->buffer[pipe->start_position] == EOF) && bytes_readed < size) {
            data[bytes_readed] = pipe->buffer[pipe->start_position];
            if ((int)data[bytes_readed++] == EOF) {
                eof_flag = 1;
                break;
            }
            pipe->buffer_count--;
            pipe->start_position = (pipe->start_position + 1) % pipe_buffer_size;
        }
        if (pipe->open == closed) {
            unblock_process(pipe->input_pid);
            pipe->open = opened;
        }
    }
    return bytes_readed;
}
