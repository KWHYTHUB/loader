#include <utils/general.hpp>

#ifdef SAPPHIRE_IS_WINDOWS
#define SAPPHIRE_CTIME() ctime_s(buf, sizeof buf, &t);
#else
#define SAPPHIRE_CTIME() strcpy(buf, ctime(&t));
#endif

std::string sapphire::utils::timePointAsString(const std::chrono::system_clock::time_point& tp) {
    auto t = std::chrono::system_clock::to_time_t(tp);
    char buf[128];
    SAPPHIRE_CTIME();
    std::string res = buf;
    res.pop_back();
    return res;
}
