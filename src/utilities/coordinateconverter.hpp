#pragma once
#include "glm.hpp"

int64_t wrapInt(int64_t kX, int64_t const kLowerBound, int64_t const kUpperBound);

template<typename SourceType, typename TargetType, int32_t factor, typename Enable = void>
class CoordinateFineConvert
{
};

template<typename SourceType, typename TargetType, int32_t factor>
class CoordinateFineConvert<SourceType, TargetType, factor, typename std::enable_if<(std::is_integral<decltype(SourceType().x)>::value), void>::type>
{
    public:
        static TargetType convert(const SourceType& input)
        {
            return (TargetType)(input * (decltype(input.x))factor);
        }
};


template<typename SourceType, typename TargetType, int32_t factor>
class CoordinateFineConvert<SourceType, TargetType, factor, typename std::enable_if<(std::is_floating_point<decltype(SourceType().x)>::value), void>::type>
{
    public:
        static TargetType convert(const SourceType& input)
        {
            return (TargetType)(glm::floor(input) * (decltype(input.x))factor);
        }
};

template<typename SourceType, typename TargetType, int32_t factor, typename Enable = void>
class CoordinateCoarseConvert
{
};

template<typename SourceType, typename TargetType, int32_t factor>
class CoordinateCoarseConvert<SourceType, TargetType, factor, typename std::enable_if<(std::is_integral<decltype(SourceType().x)>::value), void>::type>
{
    public:
        static TargetType convert(const SourceType& input)
        {
            decltype(input.x) x = input.x < (decltype(input.x))0 ? ((input.x+(decltype(input.x))1) / (decltype(input.x))factor) - (decltype(input.x))1: input.x / (decltype(input.x))factor;
            decltype(input.y) y = input.y < (decltype(input.y))0 ? ((input.y+(decltype(input.y))1) / (decltype(input.y))factor) - (decltype(input.y))1: input.y / (decltype(input.y))factor;
            decltype(input.z) z = input.z < (decltype(input.z))0 ? ((input.z+(decltype(input.z))1) / (decltype(input.z))factor) - (decltype(input.z))1: input.z / (decltype(input.z))factor;

            return TargetType((decltype(TargetType::x))x, (decltype(TargetType::y))y, (decltype(TargetType::z))z);
        }
};

template<typename SourceType, typename TargetType, int32_t factor>
class CoordinateCoarseConvert<SourceType, TargetType, factor, typename std::enable_if<(std::is_floating_point<decltype(SourceType().x)>::value), void>::type>
{
    public:
        static TargetType convert(const SourceType& input)
        {
            return (TargetType)(glm::floor(input / (decltype(input.x))factor));
        }
};

template<typename SourceType, typename TargetType, int32_t lower, int32_t upper, typename Enable = void>
class CoordinateWrapConvert
{
};

template<typename SourceType, typename TargetType, int32_t lower, int32_t upper>
class CoordinateWrapConvert<SourceType, TargetType, lower, upper, typename std::enable_if<(std::is_floating_point<decltype(SourceType().x)>::value), void>::type>
{
    public:
        static TargetType convert(const SourceType& input)
        {
            int64_t x = wrapInt((int64_t)std::floor(input.x), lower, upper);
            int64_t y = wrapInt((int64_t)std::floor(input.y), lower, upper);
            int64_t z = wrapInt((int64_t)std::floor(input.z), lower, upper);

            return TargetType((decltype(TargetType::x))x, (decltype(TargetType::y))y, (decltype(TargetType::z))z);
        }
};


template<typename SourceType, typename TargetType, int32_t lower, int32_t upper>
class CoordinateWrapConvert<SourceType, TargetType, lower, upper, typename std::enable_if<(std::is_integral<decltype(SourceType().x)>::value), void>::type>
{
    public:
        static TargetType convert(const SourceType& input)
        {
            int64_t x = wrapInt((int64_t)input.x, lower, upper);
            int64_t y = wrapInt((int64_t)input.y, lower, upper);
            int64_t z = wrapInt((int64_t)input.z, lower, upper);

            return TargetType((decltype(TargetType::x))x, (decltype(TargetType::y))y, (decltype(TargetType::z))z);
        }
};
