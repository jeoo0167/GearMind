#include "DebugMsg.h"

// Texto de cada tipo
const char* DebugMsgs::typeToStr(msg_type type) {
    switch (type) {
        case INFO:  return "INFO";
        case DEBUG: return "DEBUG";
        case WARN:  return "WARN";
        case ERROR: return "ERROR";
        default:    return "UNKNOWN";
    }
}

// Color ANSI de cada tipo
const char* DebugMsgs::typeToColor(msg_type type) {
    if (!useColors) return ""; // Si no usamos colores, vacío

    switch (type) {
        case INFO:  return "\033[32m"; // Verde
        case DEBUG: return "\033[36m"; // Cian
        case WARN:  return "\033[33m"; // Amarillo
        case ERROR: return "\033[31m"; // Rojo
        default:    return "\033[0m";  // Reset
    }
}

void DebugMsgs::msg(msg_type type, const char* format, ...) {
    char buffer[128]; // buffer temporal
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args); // similar a printf
    va_end(args);

    if (useColors) Serial.print(typeToColor(type));
    Serial.print("[");
    Serial.print(filename);
    Serial.print("][");
    Serial.print(typeToStr(type));
    Serial.print("] ");
    Serial.println(buffer);
    if (useColors) Serial.print("\033[0m");
}