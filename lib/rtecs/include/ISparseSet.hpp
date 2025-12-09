#pragma once

namespace rtecs {

class ISparseSet
{
   public:
    virtual ~ISparseSet() = default;

    DynamicBitSet getMask();

    /**
     * @brief Check if the sparse-set has the given entity.
     *
     * @param id The id of the entity.
     * @return `true` if the entity is present in the sparse-set, `false`
     * otherwise.
     */
    [[nodiscard]]
    virtual bool has(size_t id) const noexcept = 0;

    /**
     * @brief Remove the entity associated component from the sparse-set.
     *
     * @param id The entity to remove from the sparse-set.
     */
    virtual void remove(size_t id) noexcept = 0;

    /**
     * Clear the sparse-set.
     */
    virtual void clear() noexcept = 0;
};

}  // namespace rtecs
