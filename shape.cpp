#include "shape.h"

shape::shape(wxPoint point, const wxPen &pen, const wxBrush &brush) : aabb{point, point}, pen{pen}, brush{brush} {} // axis-aligned bounding box

void shape::draw(context &con) { // renderoperation
    con.SetPen(pen);
    con.SetBrush(brush);
    draw_(con); // generiert die Operation die jedes Shape implementieren muss (die eigentliche Renderlogik)
}

auto shape::contains(wxPoint pos) const noexcept -> bool {
    return aabb.Contains(pos);
}

auto shape::empty() const noexcept -> bool { // prüft ob bounding box leer ist (keine Ausdehnung in Höhe und Breite)
    return !aabb.GetWidth() && !aabb.GetHeight();
}

void shape::set_right_bottom(wxPoint point) { // bb wird von oben links aufgezogen
    aabb.SetRightBottom(point);
}

void shape::move(wxPoint offset) {
    aabb.Offset(offset);
}