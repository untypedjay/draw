#ifndef MINILIB_DRAW_APPLICATION_H
#define MINILIB_DRAW_APPLICATION_H

#include "shape.h"

struct draw_application final : ml5::application {
    auto make_window() const -> std::unique_ptr<ml5::window> override;

private:
    struct window final : ml5::window {
        window() : ml5::window{"ML5.Draw"} {} // constructor

    private:
        enum class shape_type { line, ellipse, rectangle };
        shape_type next_shape{shape_type::line}; // initial draw a line
        enum class operation { none, moving, dragging };
        operation current_operation{operation::none};

        void on_init() override { // wird aufgerufen wenn window erzeugt wird
            add_menu("&Shape", {
                    {"&Line",      "Select the line drawing tool."},
                    {"&Ellipse",   "Select the ellipse drawing tool."},
                    {"&Rectangle", "Select the rectangle drawing tool."}
            });
            add_menu("&Brush", {
                    {"&Blue", "Select the blue brush."},
                    {"&Green", "Select the green brush."},
                    {"&Red", "Select the red brush."},
                    {"&White", "Select the white brush."},
                    {"&Light Gray", "Select the light gray brush."},
                    {"Blac&k", "Select the black brush."}, // K = KEY (CMYK)
            });
            add_menu("&Pen", {
                    {"&Blue", "Select the blue pen."},
                    {"&Green", "Select the green pen."},
                    {"&Red", "Select the red pen."},
                    {"&White", "Select the white pen."},
                    {"&Light Gray", "Select the light gray pen."},
                    {"Blac&k", "Select the black pen."}, // K = KEY (CMYK)
            });
            add_menu("&Back&ground", {
                    {"&Blue", "Select a blue background."},
                    {"&Green", "Select a green background."},
                    {"&Red", "Select a red background."},
                    {"&White", "Select a white background."},
                    {"&Light Gray", "Select a light gray background."},
                    {"Blac&k", "Select a black background."}, // K = KEY (CMYK)
            });
            set_status_text("Use the mouse to draw a shape.");
        }

        void on_menu(const ml5::menu_event &event) {
            const std::string item{event.get_title_and_item()};
            if (item == "Shape/Line")      { next_shape = shape_type::line; }
            if (item == "Shape/Ellipse")   { next_shape = shape_type::ellipse; }
            if (item == "Shape/Rectangle") { next_shape = shape_type::rectangle; }

            if (item == "Brush/Blue")       { brush = *wxBLUE_BRUSH; }
            if (item == "Brush/Red")        { brush = *wxRED_BRUSH; }
            if (item == "Brush/Green")      { brush = *wxGREEN_BRUSH; }
            if (item == "Brush/White")      { brush = *wxWHITE_BRUSH; }
            if (item == "Brush/Light Gray") { brush = *wxLIGHT_GREY_BRUSH; }
            if (item == "Brush/Black")      { brush = *wxBLACK_BRUSH; }

            if (item == "Pen/Blue")       { pen = *wxBLUE_PEN; }
            if (item == "Pen/Red")        { pen = *wxRED_PEN; }
            if (item == "Pen/Green")      { pen = *wxGREEN_PEN; }
            if (item == "Pen/White")      { pen = *wxWHITE_PEN; }
            if (item == "Pen/Light Gray") { pen = *wxLIGHT_GREY_PEN; }
            if (item == "Pen/Black")      { pen = *wxBLACK_PEN; }

            if (item == "Background/Blue")       { set_prop_background_brush(*wxBLUE_BRUSH); }
            if (item == "Background/Red")        { set_prop_background_brush(*wxRED_BRUSH); }
            if (item == "Background/Green")      { set_prop_background_brush(*wxGREEN_BRUSH); }
            if (item == "Background/White")      { set_prop_background_brush(*wxBLUE_BRUSH); }
            if (item == "Background/Light Gray") { set_prop_background_brush(*wxLIGHT_GREY_BRUSH); }
            if (item == "Background/Black")      { set_prop_background_brush(*wxBLACK_BRUSH); }

            if (event.get_title() == "Background") {
                refresh();
            }
        }

        auto make_shape(wxPoint pos) const -> std::unique_ptr<shape> {
            switch (next_shape) {
                case shape_type::line:
                    return std::make_unique<line>(pos, pen, brush);
                case shape_type::ellipse:
                    return std::make_unique<ellipse>(pos, pen, brush);
                case shape_type::rectangle:
                    return std::make_unique<rectangle>(pos, pen, brush);
                default: throw std::logic_error{"unknown shape type"};
            }
        }

        void on_paint(const ml5::paint_event &event) override { // wird aufgerufen wenn os sagt es it zeit zum malen
            auto &con{event.get_context()};
            for (const auto &s : shapes) {
                s->draw(con);
            }
            if (new_shape) {
                new_shape->draw(con);
            }
        }

        auto get_topmost_shape(wxPoint pos) -> std::unique_ptr<shape> {
            std::unique_ptr<shape> *tmp{nullptr};
            for (auto &s : shapes) {
                if (s->contains(pos)) {
                    tmp = &s;
                }
            }
            if (!tmp) {
                return {};
            }
            std::unique_ptr<shape> result{std::move(*tmp)};
            shapes.remove(*tmp);
            return result;
        }

        void on_mouse_left_down(const ml5::mouse_event &event) override { // left mouse pressed
            assert(!new_shape);
            assert(current_operation == operation::none);
            const auto pos{event.get_position()};
            if (new_shape = get_topmost_shape(pos)) {
                last_move_pos = pos;
                current_operation = operation::moving;
            } else {
                new_shape = make_shape(event.get_position());
                current_operation = operation::dragging;
            }
            refresh(); // force redraw of window; function of window
        }

        void on_mouse_left_up(const ml5::mouse_event &) override {
            if (!new_shape) {
                return;
            }
            if (!new_shape->empty()) {
                new_shape.reset(); // Speicher freigeben
            } else {
                shapes.add(std::move(new_shape)); // move constructor
            }
            current_operation = operation::none;
            refresh(); // force redraw of window
        }

        void on_mouse_move(const ml5::mouse_event &event) override {
            if (!new_shape) {
                return;
            }
            const auto pos{event.get_position()};
            switch (current_operation) {
                case operation::moving: {
                    const auto offset{pos - last_move_pos};
                    last_move_pos = pos;
                    new_shape->move(offset);
                } break;
                case operation::dragging: {
                    new_shape->set_right_bottom(event.get_position());
                } break;
                default: throw std::logic_error{"unknown state"};
            }
            refresh(); // force redraw of window
        }

        wxPoint last_move_pos;
        wxBrush brush{*wxGREEN_BRUSH};
        wxPen pen{*wxBLACK_PEN};
        ml5::vector<std::unique_ptr<shape>> shapes;
        std::unique_ptr<shape> new_shape;
    };
};

#endif //MINILIB_DRAW_APPLICATION_H
