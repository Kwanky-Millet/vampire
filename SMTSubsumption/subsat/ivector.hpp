#ifndef IVECTOR_HPP
#define IVECTOR_HPP

#include <cstdint>
#include <vector>

namespace SMTSubsumption { // TODO: remove namespace once I separate out Var/Lit from subsat.hpp

/// Get index by calling a member function 'index()'.
template <typename Key>
struct IndexMember {
  using index_type = std::invoke_result_t<decltype(&Key::index), Key>;
  index_type operator()(Key key) const
  {
    return key.index();
  }
};

/// The type itself is already the index.
template <typename Integer>
struct IndexIdentity {
  Integer operator()(Integer key) const noexcept
  {
    return key;
  }
};

/// Allows to defines a default indexing method for types.
template <typename Key>
struct DefaultIndex;

template <>
struct DefaultIndex<std::uint32_t> {
  using type = IndexIdentity<std::uint32_t>;
};

template <typename Key>
using DefaultIndex_t = typename DefaultIndex<Key>::type;

/// Vector-based map with type-safe indexing.
template <typename Key, typename T, typename Index = DefaultIndex_t<Key>>
class ivector {
public:
  using key_type = Key;
  using value_type = T;
  using reference = value_type&;
  using const_reference = value_type const&;
  using size_type = typename std::vector<T>::size_type;
  using iterator = typename std::vector<T>::iterator;
  using const_iterator = typename std::vector<T>::const_iterator;

  reference operator[](key_type key)
  {
    size_type const idx = index(key);
    assert(idx < size());
    return m_data[idx];
  }

  const_reference operator[](key_type key) const
  {
    size_type const idx = index(key);
    assert(idx < size());
    return m_data[idx];
  }

  void reserve(size_type new_cap) { m_data.reserve(new_cap); }
  size_type size() const noexcept { return m_data.size(); }

  iterator begin() noexcept { return m_data.begin(); }
  const_iterator begin() const noexcept { return m_data.begin(); }
  const_iterator cbegin() const noexcept { return m_data.cbegin(); }
  iterator end() noexcept { return m_data.end(); }
  const_iterator end() const noexcept { return m_data.end(); }
  const_iterator cend() const noexcept { return m_data.cend(); }

private:
  size_type index(Key key) const
  {
    Index index;
    return index(key);
  }

private:
  std::vector<T> m_data;
};

} // namespace SMTSubsumption

#endif /* !IVECTOR_HPP */
