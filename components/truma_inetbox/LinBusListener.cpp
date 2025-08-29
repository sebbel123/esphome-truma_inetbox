#include "LinBusListener.h"
#include "LinBusLog.h"
#include "LinBusProtocol.h"
#include "TrumaiNetBoxApp.h"
#include "esphome/core/log.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace truma_inetbox {

static const char *TAG = "truma_inetbox.linbus";

// ----------------- Setup Framework -----------------
void LinBusListener::setup_framework() {
  auto uartComp = this->parent_->get_uart();
  if (!uartComp) {
    ESP_LOGE(TAG, "UART component not found");
    return;
  }

  uart_port_t uart_num = static_cast<uart_port_t>(uartComp->get_hw_serial_number());

  uart_intr_config_t uart_intr{};
  uart_intr.intr_enable_mask = UART_RXFIFO_FULL_INT_ENA_M | UART_RXFIFO_TOUT_INT_ENA_M;
  uart_intr.rxfifo_full_thresh = 1;
  uart_intr.rx_timeout_thresh = 10;
  uart_intr.txfifo_empty_intr_thresh = 10;

  uart_intr_config(uart_num, &uart_intr);

  ESP_LOGI(TAG, "UART %d interrupts configured", uart_num);
}

// ----------------- Write LIN Answer -----------------
void LinBusListener::write_lin_answer_(const uint8_t *data, uint8_t len) {
  QUEUE_LOG_MSG log_msg{};
  log_msg.type = QUEUE_LOG_MSG_TYPE::VERBOSE_LIN_ANSWER_RESPONSE;
  log_msg.current_PID = this->current_PID_;
  for (uint8_t i = 0; i < len; i++) {
    log_msg.data[i] = data[i];
  }
  log_msg.len = len;

  TRUMA_LOGV(log_msg);
}

// ----------------- Read LIN Frame -----------------
void LinBusListener::read_lin_frame_() {
  // Beispielhafte Logik für Frame-Lesen
  QUEUE_LOG_MSG log_msg{};
  log_msg.type = QUEUE_LOG_MSG_TYPE::VERBOSE_READ_LIN_FRAME_MSG;
  log_msg.current_PID = this->current_PID_with_parity_;

  // Simuliere Frame-Daten
  for (uint8_t i = 0; i < this->current_data_count_; i++) {
    log_msg.data[i] = this->current_data_[i];
  }
  log_msg.len = this->current_data_count_;

  TRUMA_LOGVV(log_msg);

  // Zusätzliche Warnungen
  if (log_msg.len == 0) {
    ESP_LOGW(TAG, "Received empty LIN frame for PID %02X", log_msg.current_PID);
  }
}

// ----------------- Prozessiere Log Queue -----------------
void LinBusListener::process_log_queue(TickType_t ticks) {
  QUEUE_LOG_MSG log_msg{};
  while (xQueueReceive(this->log_queue_, &log_msg, ticks) == pdTRUE) {
    switch (log_msg.type) {
      case QUEUE_LOG_MSG_TYPE::VERBOSE_LIN_ANSWER_RESPONSE:
        ESP_LOGV(TAG, "LIN Answer PID %02X: %s", log_msg.current_PID,
                 format_hex_pretty(log_msg.data, log_msg.len).c_str());
        break;
      case QUEUE_LOG_MSG_TYPE::VERBOSE_READ_LIN_FRAME_MSG:
        ESP_LOGVV(TAG, "LIN Frame PID %02X: %s", log_msg.current_PID,
                  format_hex_pretty(log_msg.data, log_msg.len).c_str());
        break;
      case QUEUE_LOG_MSG_TYPE::WARN_READ_LIN_FRAME_SID_CRC:
        ESP_LOGW(TAG, "LIN CRC warning for PID %02X", log_msg.current_PID);
        break;
      default:
        ESP_LOGI(TAG, "Unknown log type received");
        break;
    }
  }
}

}  // namespace truma_inetbox
}  // namespace esphome
