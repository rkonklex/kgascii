// This file is part of KG::Ascii.
//
// Copyright (C) 2011 Robert Konklewski <nythil@gmail.com>
//
// KG::Ascii is free software; you can redistribute it and/or modify 
// it under the terms of the GNU Lesser General Public License as published by 
// the Free Software Foundation; either version 3 of the License, or 
// (at your option) any later version.
//
// KG::Ascii is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License 
// along with KG::Ascii. If not, see <http://www.gnu.org/licenses/>.

#include "sequentialasciifier.hpp"
#include "glyphmatcher.hpp"
#include "textsurface.hpp"
#include <boost/gil/algorithm.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>

namespace KG { namespace Ascii {

using namespace boost::gil;

SequentialAsciifier::SequentialAsciifier(const GlyphMatcher& m)
    :Asciifier(m)
    ,cornerImg_(m.glyphWidth(), m.glyphHeight())
{
}

void SequentialAsciifier::generate(const gray8c_view_t& imgv, TextSurface& text)
{
    //single character size
    int char_w = matcher().glyphWidth();
    int char_h = matcher().glyphHeight();
    //text surface size
    int text_w = text.cols() * char_w;
    int text_h = text.rows() * char_h;
    //processed image region size
    int roi_w = std::min(imgv.width(), text_w);
    int roi_h = std::min(imgv.height(), text_h);

    gray8_view_t tmpview = view(cornerImg_);

    int y = 0, r = 0;
    for (; y + char_h <= roi_h; y += char_h, ++r) {
        int x = 0, c = 0;
        for (; x + char_w <= roi_w; x += char_w, ++c) {
            text(r, c) = matcher().match(subimage_view(imgv, x, y, char_w, char_h));
        }
        if (x < roi_w) {
            int dx = roi_w - x;
            fill_pixels(tmpview, 0);
            copy_pixels(subimage_view(imgv, x, y, dx, char_h), 
                    subimage_view(tmpview, 0, 0, dx, char_h));
            text(r, c) = matcher().match(tmpview);
        }
    }
    if (y < roi_h) {
        int dy = roi_h - y;
        int x = 0, c = 0;
        fill_pixels(tmpview, 0);
        for (; x + char_w <= roi_w; x += char_w, ++c) {
            copy_pixels(subimage_view(imgv, x, y, char_w, dy), 
                    subimage_view(tmpview, 0, 0, char_w, dy));
            text(r, c) = matcher().match(tmpview);
        }
        if (x < roi_w) {
            int dx = roi_w - x;
            fill_pixels(tmpview, 0);
            copy_pixels(subimage_view(imgv, x, y, dx, dy), 
                    subimage_view(tmpview, 0, 0, dx, dy));
            text(r, c) = matcher().match(tmpview);
        }
    }
}

size_t SequentialAsciifier::threadCount() const
{
    return 1;
}

} } // namespace KG::Ascii

