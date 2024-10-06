#include "rtypes.h"
#include <pthread.h>
#ifndef RTEMPC_SLOT_COUNT
#define RTEMPC_SLOT_COUNT 20
#endif
#ifndef RTEMPC_SLOT_SIZE
#define RTEMPC_SLOT_SIZE 1024 * 64 * 128
#endif

bool _rtempc_initialized = 0;
pthread_mutex_t _rtempc_thread_lock;
bool rtempc_use_mutex = true;
byte _current_rtempc_slot = 1;
char _rtempc_buffer[RTEMPC_SLOT_COUNT][RTEMPC_SLOT_SIZE];
char *rtempc(char *data) {

    if (rtempc_use_mutex) {
        if (!_rtempc_initialized) {
            _rtempc_initialized = true;
            pthread_mutex_init(&_rtempc_thread_lock, NULL);
        }

        pthread_mutex_lock(&_rtempc_thread_lock);
    }

    uint current_rtempc_slot = _current_rtempc_slot;
    _rtempc_buffer[current_rtempc_slot][0] = 0;
    strcpy(_rtempc_buffer[current_rtempc_slot], data);
    _current_rtempc_slot++;
    if (_current_rtempc_slot == RTEMPC_SLOT_COUNT) {
        _current_rtempc_slot = 0;
    }
    if (rtempc_use_mutex)
        pthread_mutex_unlock(&_rtempc_thread_lock);
    return _rtempc_buffer[current_rtempc_slot];
}

#define sstring(_pname, _psize)                                                \
    static char _##_pname[_psize];                                             \
    _##_pname[0] = 0;                                                          \
    char *_pname = _##_pname;

#define string(_pname, _psize)                                                 \
    char _##_pname[_psize];                                                    \
    _##_pname[0] = 0;                                                          \
    char *_pname = _##_pname;

#define sreset(_pname, _psize) _pname = _##_pname;

#define sbuf(val) rtempc(val)
