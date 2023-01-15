#define LOGGING_AS_SOURCE
#define LOGGING_CONF_DYNAMIC_LOG_FORMAT
#define LOGGING_LOGGER_ADD_CUSTOM_FORMAT_AS_FUNCTION
#include "Logging.h"
struct fnln { const char *f; int l; };
struct fnln fnln_collect(log_logger_t *l)
{
    struct fnln fnln;
    fnln.f = l->function;
    fnln.l = l->line;
    return fnln;
}
LOGGING_FORMAT_REGISTER(FUNCLINE, "FNLN", struct fnln,
                        fnln_collect(l),
                        "%s(%d)", v.f, v.l)

void LOGGING_LOGGER_ADD_CUSTOM_FORMAT(struct log_logger *l)
{
    LOGGING_LOGGER_ADD_FORMAT(l, "FNLN", LOGGING_FORMAT_INIT_FUNCLINE);
}
