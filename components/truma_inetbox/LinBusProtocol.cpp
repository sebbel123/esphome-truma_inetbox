#include "LinBusProtocol.h"
#include "LinBusListener.h"
#include "LinBusLog.h"
#include "esphome/core/log.h"

namespace esphome {
namespace truma_inetbox {

static const char *TAG = "truma_inetbox.protocol";

bool LinBusProtocol::check_frame(const uint8_t *data, uint8_t len) {
  if (len == 0 || data == nullptr) {
    QUEUE_LOG_MSG log_msg{};
    log_msg.type = QUEUE_LOG_MSG_TYPE::ERROR_READ_LIN_FRAME_LOST_MSG;
    TRUMA_LOGE(log_msg);
    return false;
  }

  // CRC-Check placeholder
  bool crc_ok = true;
  if (!crc_ok) {
    QUEUE_LOG_MSG log_msg{};
    log_msg.type = QUEUE_LOG_MSG_TYPE::WARN_READ_LIN_FRAME_LINv1_CRC;
    log_msg.current_PID = data[0];
    for (uint8_t i = 0; i < len; i++) {
      log_msg.data[i] = data[i];
    }
    log_msg.len = len;
    TRUMA_LOGW(log_msg);
    return false;
  }

  return true;
}

}  // namespace truma_inetbox
}  // namespace esphome
