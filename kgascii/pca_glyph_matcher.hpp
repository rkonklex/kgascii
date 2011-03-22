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

#ifndef KGASCII_PCAGLYPHMATCHER_HPP
#define KGASCII_PCAGLYPHMATCHER_HPP

#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/gil/typedefs.hpp>
#include <Eigen/Dense>
#include <kgascii/glyph_matcher.hpp>
#include <kgascii/glyph_matcher_context.hpp>
#include <kgascii/kgascii_api.hpp>

namespace KG { namespace Ascii {

class FontImage;

class PcaGlyphMatcherContext: public GlyphMatcherContext
{
    friend class PcaGlyphMatcher;

public:
    explicit PcaGlyphMatcherContext(const FontImage& f);

public:
    GlyphMatcher* createMatcher() const;

private:
    std::vector<int> charcodes_;
    int glyphSize_;
    Eigen::VectorXf mean_;
    Eigen::VectorXf energies_;
    Eigen::MatrixXf features_;
    Eigen::MatrixXf glyphs_;
};

class KGASCII_API PcaGlyphMatcher: public GlyphMatcher
{
public:
    explicit PcaGlyphMatcher(const PcaGlyphMatcherContext& c);

public:
    const GlyphMatcherContext& context() const;

    char match(const boost::gil::gray8c_view_t& imgv);

private:
    const PcaGlyphMatcherContext& context_;
    Eigen::VectorXf imgvec_;
    Eigen::VectorXf components_;
};

} } // namespace KG::Ascii

#endif // KGASCII_PCAGLYPHMATCHER_HPP
