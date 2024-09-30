#include "rtypes.h"

#ifndef RTEMPC_SLOT_COUNT
#define RTEMPC_SLOT_COUNT 20
#endif
#ifndef RTEMPC_SLOT_SIZE
#define RTEMPC_SLOT_SIZE 1024 * 64
#endif
byte _current_rtempc_slot = 0;
char _rtempc_buffer[RTEMPC_SLOT_COUNT][RTEMPC_SLOT_SIZE];
char *rtempc(char *data) {

    uint current_rtempc_slot = _current_rtempc_slot;
    _rtempc_buffer[current_rtempc_slot][0] = 0;
    strcpy(_rtempc_buffer[current_rtempc_slot], data);
    _current_rtempc_slot++;
    if (_current_rtempc_slot == RTEMPC_SLOT_COUNT) {
        _current_rtempc_slot = 0;
    }
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
