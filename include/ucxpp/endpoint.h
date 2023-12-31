#pragma once

#include <coroutine>
#include <cstdint>
#include <memory>
#include <ucs/type/status.h>
#include <vector>

#include <ucp/api/ucp.h>

#include "ucxpp/address.h"
#include "ucxpp/awaitable.h"
#include "ucxpp/error.h"
#include "ucxpp/memory.h"
#include "ucxpp/worker.h"

#include "ucxpp/detail/noncopyable.h"

namespace ucxpp {

/**
 * @brief Abstraction for a UCX endpoint.
 *
 */
class endpoint : public noncopyable {
  friend class worker;
  friend class local_memory_handle;
  friend class remote_memory_handle;
  friend class ep_close_awaitable;
  worker *worker_;
  ucp_ep_h ep_;
  void *close_request_;
  remote_address peer_;

public:
  /**
   * @brief Construct a new endpoint object
   *
   * @param worker UCX worker
   * @param peer Remote UCX address
   */
  endpoint(worker *worker, remote_address const &peer);

  /**
   * @brief Error handler for all endpoints
   *
   * @param ep endpoint object
   * @param ep_h UCX endpoint handle
   * @param status error status
   */
  static void error_cb(void *ep, ucp_ep_h ep_h, ucs_status_t status);

  worker *worker_ptr() const;

  /**
   * @brief Print the endpoint's information
   *
   */
  void print() const;

  /**
   * @brief Get the endpoint's native UCX handle
   *
   * @return ucp_ep_h The endpoint's native UCX handle
   */
  ucp_ep_h handle() const;

  /**
   * @brief Get the endpoint's remote address
   *
   * @return remote_address The endpoint's remote address
   */
  const remote_address &get_address() const;

  /**
   * @brief Stream send the buffer
   *
   * @param buffer The buffer to send
   * @param length The length of the buffer
   * @return stream_send_awaitable A coroutine that returns upon completion
   */
  stream_send_awaitable stream_send(void const *buffer, size_t length) const;

  /**
   * @brief Stream receive to the buffer
   *
   * @param buffer The buffer to receive to
   * @param length The length of the buffer
   * @return stream_recv_awaitable A coroutine that returns number of bytes
   * received upon completion
   */
  stream_recv_awaitable stream_recv(void *buffer, size_t length) const;

  /**
   * @brief Tag send the buffer
   *
   * @param buffer The buffer to send
   * @param length The length of the buffer
   * @param tag The tag to send with
   * @return tag_send_awaitable A coroutine that returns upon completion
   */
  tag_send_awaitable tag_send(void const *buffer, size_t length,
                              ucp_tag_t tag) const;

  /**
   * @brief Flush the endpoint
   *
   * @return ep_flush_awaitable A coroutine that returns upon completion
   */
  ep_flush_awaitable flush() const;

  /**
   * @brief Close the endpoint. You should not use the endpoint after calling
   * this function.
   *
   * @return task<void> A coroutine that returns upon completion
   */
  ep_close_awaitable close();

  /**
   * @brief Endpoint close callback
   *
   * @param request UCX request handle
   * @param status UCX status
   * @param user_data User data
   */
  static void close_cb(void *request, ucs_status_t status, void *user_data);

  /**
   * @brief Destroy the endpoint object. If the endpoint is not closed yet, it
   * will be closed.
   *
   */
  ~endpoint();
};

} // namespace ucxpp

/** \example helloworld.cc
 * This is an example of how to use the endpoint class.
 */