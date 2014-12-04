#pragma once
#include <memory>
#include "rawmodel.hpp"

struct ModelDeliverMessage
{
    std::shared_ptr<RawModel> model;
};
