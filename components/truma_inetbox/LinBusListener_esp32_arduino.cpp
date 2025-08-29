#include "LinBusListener.h"
#include "LinBusLog.h"
#include "LinBusProtocol.h"
#include "esphome/core/log.h"
#include "driver/uart.h"

namespace esphome {
namespace truma_inetbox {

static const char *TAG = "truma_inetbox.linbus.esp32";

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

  esp_err_t err = uart_intr_config(uart_num, &uart_intr);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to configure UART interrupts: %d", err);
  } else {
    ESP_LOGI(TAG, "UART %d interrupts configured", uart_num);
  }
}

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

}  // namespace truma_inetbox
}  // namespace esphome
