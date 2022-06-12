#include "SessionBase.h"

namespace efs {
SessionBase::SessionBase(int32_t buffer_size)
{
    p_in_buffer = std::make_unique<Buffer>(buffer_size);
    p_out_buffer = std::make_unique<Buffer>(buffer_size);
}
}