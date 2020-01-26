#include "draw_application.h"

auto draw_application::make_window() const -> std::unique_ptr<ml5::window> {
return std::make_unique<window>();
}