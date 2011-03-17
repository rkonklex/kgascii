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

#include "glyphmatcher.hpp"
#include "fontimage.hpp"
#include <limits>

namespace KG { namespace Ascii {

using namespace boost::gil;

GlyphMatcher::GlyphMatcher(const FontImage& f)
    :font_(f)
    ,charcodes_(f.charcodes())
{
}

int GlyphMatcher::glyphWidth() const
{
    return font_.glyphWidth();
}

int GlyphMatcher::glyphHeight() const
{
    return font_.glyphHeight();
}

char GlyphMatcher::match(const gray8c_view_t& imgv) const
{
    assert(imgv.width() == glyphWidth());
    assert(imgv.height() == glyphHeight());
    int d2_min = std::numeric_limits<int>::max();
    int cc_min = ' ';
    for (size_t ci = 0; ci < charcodes_.size(); ++ci) {
        int cc = charcodes_[ci];
        int d2 = distance(imgv, font_.getGlyph(cc));
        if (d2 < d2_min) {
            d2_min = d2;
            cc_min = cc;
        }
    }
    return (char)cc_min;
}

int GlyphMatcher::distance(const gray8c_view_t& img1, const gray8c_view_t& img2) const
{
    assert(img1.size() == img2.size());
    int result = 0;
    for (int y = 0; y < img1.height(); ++y) {
        gray8c_view_t::x_iterator img1_it = img1.row_begin(y);
        gray8c_view_t::x_iterator img2_it = img2.row_begin(y);
        for (int x = 0; x < img1.width(); ++x, ++img1_it, ++img2_it) {
            int df = *img1_it - *img2_it;
            result += df * df;
        }
    }
    return result;
}

} } // namespace KG::Ascii

