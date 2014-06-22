#ifndef TEXTIP_UTILS_DEBUG_TOOLS_H
#define TEXTIP_UTILS_DEBUG_TOOLS_H

#ifndef RELEASE
#define profile_noinline __attribute__((noinline))
#else
#define profile_noinline
#endif

#endif