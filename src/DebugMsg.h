#ifndef DEBUG_MSGS_H
#define DEBUG_MSGS_H

#include <Arduino.h>
#include <stdarg.h>

/*
Structure
DebugMsgs debug("class.cpp",false);
debug(debug.INFO,"Hello Wolrd");
-------------OUTPUT----------------
[class.cpp][INFO][Hello World]
*/
class DebugMsgs {
public:
    enum msg_type { INFO, DEBUG, WARN, ERROR };

    // Constructor: name = nombre de la clase / módulo, colors = activar colores
    explicit DebugMsgs(const String& name, bool colors = false)
        : filename(name), useColors(colors) {}

    void msg(msg_type type, const char* format, ...);

private:
    String filename;
    bool useColors;

    char buffer[64];

    const char* typeToStr(msg_type type);
    const char* typeToColor(msg_type type);
};

#endif
