#ifndef __USER_DRIVER_H
#define __USER_DRIVER_H

#include <cstdint>

namespace user_driver
{

using accid_t = uint64_t;

void reserve(accid_t acc_id);
bool check_reserve(accid_t acc_id);
void release(accid_t acc_id);

}

#endif /* __USER_DRIVER_H */
