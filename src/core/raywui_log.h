
#ifndef DRaywuiLog_H
#define DRaywuiLog_H

//namespace raywui {
    class Log {
        public:
            static void info(const char *tag, const char *fmt, ...);
            static void debug(const char *tag, const char *fmt, ...);
            static void error(const char *tag, const char *fmt, ...);
            static void warning(const char *tag, const char *fmt, ...);
    };
//}

#endif