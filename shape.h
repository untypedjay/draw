#pragma once

#ifndef MINILIB_SHAPE_H
#define MINILIB_SHAPE_H

#include <ml5/ml5.h>

struct shape : ml5::object {
    using context = ml5::paint_event::context_t;

    shape(wxPoint point, const wxPen &pen, const wxBrush &brush);

    void draw(context &con);
    auto contains(wxPoint pos) const noexcept -> bool;
    auto empty() const noexcept -> bool;
    void set_right_bottom(wxPoint point);
    void move(wxPoint offset);

protected:
    wxRect aabb;

private:
    virtual void draw_(context &) const = 0;

    const wxPen pen;
    const wxBrush brush;
}; // shape

struct line final : shape {
    using shape::shape; // inherit constructors

private:
    void draw_(context &cont) const override {
        cont.DrawLine(aabb.GetLeftTop(), aabb.GetBottomRight());
    }
}; // line

struct ellipse final : shape {
    using shape::shape; // inherit constructors

private:
    void draw_(context &cont) const override {
        cont.DrawEllipse(aabb);
    }
}; // ellipse

struct rectangle final : shape {
    using shape::shape; // inherit constructors

private:
    void draw_(context &cont) const override {
        cont.DrawRectangle(aabb);
    }
}; // rectangle

#endif //MINILIB_SHAPE_H
