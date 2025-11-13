#ifndef DEBUG_MSGS_H
#define DEBUG_MSGS_H

#include <Arduino.h>
#include <stdarg.h>


/*
-------------STRUCTURE------------

-------------Messages--------------
DebugMsgs debug("class.cpp",false);
debug(debug.INFO,"Hello Wolrd");
-------------OUTPUT----------------
[class.cpp][INFO][Hello World]

------------Plotter-----------------
DebugMsgs debug("class.cpp",false);
debug.plot({"tag1","tag2"},value1,value2);
-------------OUTPUT-----------------
>tag1,value1,tag2,value2

*/

class DebugMsgs {
    public:
        enum msg_type { INFO, DEBUG, WARN, ERROR };

        // Constructor: name = nombre de la clase / módulo, colors = activar colores
        explicit DebugMsgs(const String& name, bool colors = false)
            : filename(name), useColors(colors) {}

        void msg(msg_type type, const char* format, ...);

        template <typename... Args>
        static void plot(const String names[], const Args&... values)
        {
            const String vals[] = { String(values)... };
            constexpr size_t n = sizeof...(values);

            Serial.print(">");
            for (size_t i = 0; i < n; ++i) {
                Serial.print(names[i]);
                Serial.print(":");
                Serial.print(vals[i]);
                if (i < n - 1) Serial.print(",");
            }
            Serial.println();
        }
    private:
        String filename;
        bool useColors;

        char buffer[64];

        const char* typeToStr(msg_type type);
        const char* typeToColor(msg_type type);
};

#endif
