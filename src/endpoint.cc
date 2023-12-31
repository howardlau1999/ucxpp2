#include "ucxpp/endpoint.h"

#include <cstddef>
#include <memory>
#include <ucs/type/status.h>
#include <utility>
#include <vector>

#include <ucp/api/ucp.h>

#include "ucxpp/address.h"
#include "ucxpp/awaitable.h"
#include "ucxpp/error.h"

#include "ucxpp/detail/debug.h"
#include "ucxpp/detail/serdes.h"

namespace ucxpp {

void endpoint::error_cb(void *ep, ucp_ep_h ep_h, ucs_status_t status) {
  UCXPP_LOG_ERROR("Endpoint error: ep=%p ep_h=%p status=%s", ep,
                  reinterpret_cast<void *>(ep_h), ::ucs_status_string(status));
  auto ep_ptr = reinterpret_cast<endpoint *>(ep);
  if (!ep_ptr->close_request_) {
    auto request = ::ucp_ep_close_nb(ep_h, UCP_EP_CLOSE_MODE_FLUSH);
    if (UCS_PTR_IS_ERR(request)) {
      UCXPP_LOG_ERROR(
          "ep=%p ep_h=%p close failed: %s", ep, reinterpret_cast<void *>(ep_h),
          ::ucs_status_string(UCS_PTR_STATUS(ep_ptr->close_request_)));
      ep_ptr->close_request_ = nullptr;
      ep_ptr->ep_ = nullptr;
    } else if (UCS_PTR_IS_PTR(request)) {
      ep_ptr->close_request_ = request;
    } else {
      ep_ptr->close_request_ = nullptr;
      ep_ptr->ep_ = nullptr;
    }
  }
}

endpoint::endpoint(worker *worker, remote_address const &peer)
    : worker_(worker), peer_(peer) {
  ucp_ep_params_t ep_params;
  ep_params.field_mask =
      UCP_EP_PARAM_FIELD_REMOTE_ADDRESS | UCP_EP_PARAM_FIELD_ERR_HANDLER;
  ep_params.address = peer.get_address();
  ep_params.err_handler.cb = &error_cb;
  ep_params.err_handler.arg = this;
  check_ucs_status(::ucp_ep_create(worker_->worker_, &ep_params, &ep_),
                   "failed to create ep");
}

worker *endpoint::worker_ptr() const { return worker_; }

void endpoint::print() const { ::ucp_ep_print_info(ep_, stdout); }

ucp_ep_h endpoint::handle() const { return ep_; }

const remote_address &endpoint::get_address() const { return peer_; }

stream_send_awaitable endpoint::stream_send(void const *buffer,
                                            size_t length) const {
  return stream_send_awaitable(ep_, buffer, length);
}

stream_recv_awaitable endpoint::stream_recv(void *buffer, size_t length) const {
  return stream_recv_awaitable(ep_, buffer, length);
}

tag_send_awaitable endpoint::tag_send(void const *buffer, size_t length,
                                      ucp_tag_t tag) const {
  return tag_send_awaitable(ep_, buffer, length, tag);
}

ep_flush_awaitable endpoint::flush() const {
  return ep_flush_awaitable(this);
}

ep_close_awaitable endpoint::close() {
  return ep_close_awaitable(this);
}

void endpoint::close_cb(void *request, ucs_status_t status, void *user_data) {
  UCXPP_LOG_DEBUG("endpoint closed request=%p status=%s user_data=%p", request,
                  ::ucs_status_string(status), user_data);
  ::ucp_request_free(request);
}

endpoint::~endpoint() {
  if (ep_ != nullptr && close_request_ == nullptr) {
    ucp_request_param_t param;
    param.op_attr_mask = UCP_OP_ATTR_FIELD_CALLBACK;
    param.cb.send = &close_cb;
    ::ucp_ep_close_nbx(ep_, &param);
  }
}

} // namespace ucxpp