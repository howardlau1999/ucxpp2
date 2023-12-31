#pragma once

#include <functional>
#include <memory>
#include <ucs/type/status.h>
#include <unordered_map>

#include <ucp/api/ucp.h>

#include "ucxpp/address.h"
#include "ucxpp/awaitable.h"
#include "ucxpp/context.h"

namespace ucxpp {

/**
 * @brief Abstraction for a UCX worker.
 *
 */
class worker {
  friend class local_address;
  friend class endpoint;
  ucp_worker_h worker_;
  context *ctx_;
  int event_fd_;

public:
  /**
   * @brief Construct a new worker object
   *
   * @param ctx UCX context
   */
  worker(context *ctx);

  /**
   * @brief Get the event fd for the worker. The wakeup feature must be enabled
   * for this to work.
   *
   * @return int
   */
  int event_fd() const;

  context *context_ptr() const;

  /**
   * @brief Get the worker's UCX address
   *
   * @return local_address The worker's UCX address
   */
  local_address get_address() const;

  /**
   * @brief Get the worker's native UCX handle
   *
   * @return ucp_worker_h The worker's native UCX handle
   */
  ucp_worker_h handle() const;

  /**
   * @brief Progress the worker
   *
   * @return true If progress was made
   * @return false If no progress was made
   */
  bool progress() const;

  /**
   * @brief Wait for an event on the worker. It should be called only after a
   * call to progress() returns false.
   *
   */
  void wait() const;

  /**
   * @brief Arm the worker for next event notification.
   *
   * @return true If the worker was armed
   * @return false If the worker has pending events. In this case, the user must
   * call progress() until it returns false.
   */
  bool arm() const;

  /**
   * @brief Tag receive to the buffer
   *
   * @param buffer The buffer to receive to
   * @param length The length of the buffer
   * @param tag The tag to receive with
   * @param tag_mask The bit mask for tag matching, 0 means accepting any tag
   * @return tag_recv_awaitable A coroutine that returns a pair of number of
   * bytes received and the sender tag upon completion
   */
  tag_recv_awaitable tag_recv(void *buffer, size_t length, ucp_tag_t tag,
                              ucp_tag_t tag_mask = 0xFFFFFFFFFFFFFFFF) const;

  /**
   * @brief Fence the worker. Operations issued on the worker before the fence
   * are ensured to complete before operations issued after the fence.
   *
   */
  void fence();

  /**
   * @brief Flush the worker
   *
   * @return worker_flush_awaitable A coroutine that returns when the worker is
   * flushed
   */
  worker_flush_awaitable flush();

  ~worker();
};

} // namespace ucxpp