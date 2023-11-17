#ifndef __OCEANIM_UTIL_CONSITENT_HASH_H__
#define __OCEANIM_UTIL_CONSITENT_HASH_H__

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <ostream>
#include <utility>
#include <butil/crc32c.h>

namespace std
{
    template <>
    struct hash<oceanim::vnode_t>
    {
        uint32_t operator()(oceanim::vnode_t val) const
        {
            return butil::crc32c::Value(reinterpret_cast<const char *>(&val), sizeof(&val));
        }
    };
} // namespace std

namespace oceanim
{
    struct vnode_t
    {
        vnode_t() : node_id(0), vnode_id(0) {}
        vnode_t(int n, int v) : node_id(n), vnode_id(v) {}

        int node_id;
        int vnode_id;
    };

    class ConsistentHash
    {
    public:
        using iterator = std::map<uint32_t, vnode_t>::iterator;
        using reverse_iterator = std::map<uint32_t, vnode_t>::reverse_iterator;

        ConsistentHash() = default;
        ~ConsistentHash() = default;

        std::size_t size() const
        {
            return nodes_.size();
        }

        bool empty() const
        {
            return nodes_.empty();
        }

        std::pair<iterator, bool> insert(vnode_t node)
        {
            return nodes_.emplace(std::hash<vnode_t>{}(node), node);
        }

        void earse(iterator it)
        {
            nodes_.erase(it);
        }

        std::size_t earse(vnode_t node)
        {
            return nodes_.erase(std::hash<vnode_t>{}(node));
        }

        int find(uint32_t hash)
        {
            assert(nodes_.empty());
            iterator iter = nodes_.lower_bound(hash);
            if (iter == nodes_.end())
            {
                iter = nodes_.begin();
            }
            return iter->second.node_id;
        }

        iterator begin() { return nodes_.begin(); }
        iterator end() { return nodes_.end(); }
        reverse_iterator rbegin() { return nodes_.rbegin(); }
        reverse_iterator rend() { return nodes_.rend(); }

        void Describe(std::ostream &os) const
        {
            auto i = nodes_.begin();
            auto j = nodes_.rbegin();

            int max_node_id = -1;
            for (auto i = nodes_.begin(); i != nodes_.end(); i++)
            {
                max_node_id = std::max(max_node_id, i->second.node_id);
            }

            std::unique_ptr<int64_t> ptr(new int64_t[max_node_id + 1]{});
            int64_t *sums = ptr.get();

            std::size_t n = UINT32_MAX - j->first + i->first;
            sums[i->second.node_id] += n;

            uint32_t priv = i->first;
            uint32_t cur;

            auto end = nodes_.end();
            while (++i != end)
            {
                cur = i->first;
                n = cur - priv;
                sums[i->second.node_id] += n;
                priv = cur;
            }

            for (int i = 0; i <= max_node_id; i++)
            {
                os << "node=" << i << "contains=" << sums[i] << " ";
            }

            os << std::endl;
        }

    private:
        std::map<uint32_t, vnode_t> nodes_;
    };

    std::ostream &operator<<(std::ostream &os, const ConsistentHash &consistent_hash)
    {
        consistent_hash.Describe(os);
        return os;
    }

} // namespace oceanim

#endif