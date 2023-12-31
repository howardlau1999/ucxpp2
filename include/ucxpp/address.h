#pragma once

#include <memory>
#include <vector>

#include <ucp/api/ucp.h>

#include "ucxpp/detail/noncopyable.h"
namespace ucxpp {

class worker;

/**
 * @brief Represents a local UCX address.
 *
 */
class local_address : public noncopyable {
  worker const *worker_;
  ucp_address_t *address_;
  uint32_t address_length_;
  friend class endpoint;

public:
  /**
   * @brief Construct a new local address object
   *
   * @param worker UCX worker
   * @param address UCP address
   * @param address_length UCP address length
   */
  local_address(worker const *worker, ucp_address_t *address,
                size_t address_length);

  /**
   * @brief Construct a new local address object
   *
   * @param other Another local address object to move from
   */
  local_address(local_address &&other);

  /**
   * @brief Move assignment operator
   *
   * @param other Another local address object to move from
   * @return local_address& This object
   */
  local_address &operator=(local_address &&other);

  /**
   * @brief Serialize the address to a buffer ready to be sent to a remote peer
   *
   * @return std::vector<char> The serialized address
   */
  std::vector<char> serialize() const;

  /**
   * @brief Get the UCP address
   *
   * @return const ucp_address_t* The UCP address
   */
  const ucp_address_t *get_address() const;

  /**
   * @brief Get the length of the address
   *
   * @return size_t The address length
   */
  uint32_t get_length() const;

  /**
   * @brief Destroy the local address object and release the buffer
   *
   */
  ~local_address();
};

/**
 * @brief Represents a remote UCX address.
 *
 */
class remote_address {
  std::vector<char> address_;

public:
  /**
   * @brief Construct a new remote address object
   *
   * @param address The received address buffer
   */
  remote_address(std::vector<char> const &address);

  /**
   * @brief Construct a new remote address object
   *
   * @param address Another remote address object to move from
   */
  remote_address(std::vector<char> &&address);

  /**
   * @brief Move construct a new remote address object
   *
   * @param other Another remote address object to move from
   */
  remote_address(remote_address &&other) = default;

  /**
   * @brief Construct a new remote address object
   *
   * @param other Another remote address object to copy from
   */
  remote_address(remote_address const &other) = default;

  /**
   * @brief Copy assignment operator
   *
   * @param other Another remote address object to copy from
   * @return remote_address& This object
   */
  remote_address &operator=(remote_address const &other) = default;

  /**
   * @brief Move assignment operator
   *
   * @param other Another remote address object to move from
   * @return remote_address& This object
   */
  remote_address &operator=(remote_address &&other) = default;

  /**
   * @brief Get the UCP address
   *
   * @return const ucp_address_t* The UCP address
   */
  const ucp_address_t *get_address() const;

  /**
   * @brief Get the length of the address
   *
   * @return size_t The length of the address
   */
  size_t get_length() const;
};

} // namespace ucxpp