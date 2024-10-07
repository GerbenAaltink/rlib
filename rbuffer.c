#include "rbuffer.h"

int main(){
    unsigned char * content = (unsigned char *)"[   {\n\t \"\r1\t3\n4truefalsetrue \" }, ]";
    char * ignore = "\r| |\n|\t|\f|\v";
    rbuffer_t * buffer = rbuffer_new(content,ustrlen(content));
    assert(*rbuffer_consume(buffer,"{|[|,|\"|\\d|\\b",ignore) == '[');
    assert(*rbuffer_consume(buffer,"{|[|,|\"|\\d|\\b",ignore) == '{');
    rbuffer_reset(buffer);
    assert(*rbuffer_consume(buffer,"{|[|,|\"|\\d|\\b",ignore) == '[');
    assert(*rbuffer_consume(buffer,"{|[|,|\"|\\d|\\b",ignore) == '{');
    assert(*rbuffer_consume(buffer,"{|[|,|\"|\\d|\\b",ignore) == '"');
    assert(*rbuffer_consume(buffer,"{|[|,|\"|\\d|\\b",ignore) == '1');
    assert(*rbuffer_consume(buffer,"{|[|,|\"|\\d|\\b",ignore) == '3');
    assert(*rbuffer_consume(buffer,"{|[|,|\"|\\d|\\b",ignore) == '4');
    assert(*rbuffer_consume(buffer,"{|[|,|\"|\\d|\\b",ignore) == 't');
    rbuffer_pop(buffer);
    rbuffer_pop(buffer);
    rbuffer_pop(buffer);
    assert(*rbuffer_consume(buffer,"{|[|,|\"|\\d|\\b",ignore) == 'f');
    rbuffer_pop(buffer);
    rbuffer_pop(buffer);
    rbuffer_pop(buffer);
    rbuffer_pop(buffer);
    assert(*rbuffer_consume(buffer,"{|[|,|\"|\\d|\\b",ignore) == 't');
    rbuffer_pop(buffer);
    rbuffer_pop(buffer);
    rbuffer_pop(buffer);
    assert(*rbuffer_consume(buffer,"{|[|,|\"|\\d|\\b",ignore) == '"');
    assert(*rbuffer_consume(buffer,"{|[|,|\"|\\d|\\b|}",ignore) == '}');
    assert(*rbuffer_consume(buffer,"{|[|,|\"|\\d|\\b",ignore) == ',');
    assert(*rbuffer_consume(buffer,"{|[|,|\"|\\d|\\b|]",ignore) == ']');
   
}