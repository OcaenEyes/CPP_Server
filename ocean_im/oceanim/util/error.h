#ifndef __OCEANIM_UTIL_ERROR_H__
#define __OCEANIM_UTIL_ERROR_H__

namespace oceanim
{
    enum Errno : int
    {
        ECONFLICT = 10000,

        EHAVENOTHIS = 10001
    };
} // namespace oceanim

#endif