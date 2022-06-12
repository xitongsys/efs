#include <memory>

#include "Buffer.h"

namespace efs {
class SessionBase {
public:
    std::unique_ptr<Buffer> p_in_buffer, p_out_buffer;

public:
    

public:
    SessionBase(int32_t buffer_size);
};
}