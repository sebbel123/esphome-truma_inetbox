#include "LinBusListener.h"
#include "LinBusLog.h"
#include "esphome/core/log.h"

namespace esphome {
namespace truma_inetbox {

// Platzhalter-Methoden für ISR/Queue
void LinBusListener::read_lin_frame_() {
  // Logik für LIN Frame hier
}

void LinBusListener::process_log_queue(TickType_t tick) {
  // Log Queue verarbeiten
}

}  // namespace truma_inetbox
}  // namespace esphome
