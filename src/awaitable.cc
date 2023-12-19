#include "ucxpp/awaitable.h"

#include <memory>

#include "ucxpp/endpoint.h"
#include "ucxpp/worker.h"

namespace ucxpp {

ep_flush_awaitable::ep_flush_awaitable(endpoint const *endpoint)
    : endpoint_(endpoint) {}

bool ep_flush_awaitable::await_ready() noexcept {
  auto send_param = build_param();
  auto request = ::ucp_ep_flush_nbx(endpoint_->handle(), &send_param);
  return check_request_ready(request);
}

ep_close_awaitable::ep_close_awaitable(endpoint *endpoint)
    : endpoint_(endpoint) {}

bool ep_close_awaitable::await_ready() noexcept {
  auto send_param = build_param();
  auto request = ::ucp_ep_close_nbx(endpoint_->handle(), &send_param);
  if (check_request_ready(request)) {
    return true;
  }
  endpoint_->close_request_ = request;
  return false;
}

void ep_close_awaitable::await_resume() const {
  check_ucs_status(status_, "operation failed");
  endpoint_->ep_ = nullptr;
}

worker_flush_awaitable::worker_flush_awaitable(worker *worker)
    : worker_(worker) {}

bool worker_flush_awaitable::await_ready() noexcept {
  auto send_param = build_param();
  auto request = ::ucp_worker_flush_nbx(worker_->handle(), &send_param);
  return check_request_ready(request);
}

} // namespace ucxpp