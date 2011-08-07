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

#include <map>
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/noncopyable.hpp>
#include <Eigen/Dense>
#include <kgascii/font_pca.hpp>
#include <kgascii/dynamic_glyph_matcher.hpp>

namespace KG { namespace Ascii {

template<class TFontImage>
class PcaGlyphMatcher: boost::noncopyable
{
public:
    typedef TFontImage FontImageT;
    typedef typename FontImageT::PixelT PixelT;
    typedef typename FontImageT::ImageT ImageT;
    typedef typename FontImageT::ViewT ViewT;
    typedef typename FontImageT::ConstViewT ConstViewT;

    class PcaContext
    {
        friend class PcaGlyphMatcher;
    public:
        typedef PcaGlyphMatcher GlyphMatcherT;

    private:
        explicit PcaContext(const PcaGlyphMatcher* matcher)
            :components_(matcher->pca()->featureCount())
            ,imageData_(matcher->font()->glyphSize() * boost::gil::num_channels<ViewT>::value)
        {
        }

    private:
        Eigen::VectorXf components_;
        Eigen::VectorXf imageData_;
    };
    typedef PcaContext ContextT;

public:
    explicit PcaGlyphMatcher(const FontPCA<FontImageT>* pca)
        :font_(pca->font())
        ,pca_(pca)
    {
    }

public:
    const FontImageT* font() const
    {
        return font_;
    }

    unsigned cellWidth() const
    {
        return font_->glyphWidth();
    }

    unsigned cellHeight() const
    {
        return font_->glyphHeight();
    }

    PcaContext createContext() const
    {
        return PcaContext(this);
    }

    template<typename TSomeView>
    Symbol match(PcaContext& ctx, const TSomeView& imgv) const
    {
        boost::gil::gil_function_requires<boost::gil::ImageViewConcept<TSomeView> >();
        boost::gil::gil_function_requires<boost::gil::ColorSpacesCompatibleConcept<
                                    typename boost::gil::color_space_type<TSomeView>::type,
                                    typename boost::gil::color_space_type<ConstViewT>::type> >();
        boost::gil::gil_function_requires<boost::gil::ChannelsCompatibleConcept<
                                    typename boost::gil::channel_type<TSomeView>::type,
                                    typename boost::gil::channel_type<ConstViewT>::type> >();

        assert(static_cast<size_t>(imgv.width()) <= cellWidth());
        assert(static_cast<size_t>(imgv.height()) <= cellHeight());

        typedef boost::gil::layout<
                typename boost::gil::color_space_type<ConstViewT>::type,
                typename boost::gil::channel_mapping_type<TSomeView>::type
                > LayoutT;
        typedef typename float_channel_type<typename boost::gil::channel_type<ConstViewT>::type>::type FloatChannelT;
        typedef typename boost::gil::pixel_value_type<FloatChannelT, LayoutT>::type FloatPixelT;
        typedef typename boost::gil::type_from_x_iterator<FloatPixelT*>::view_t FloatViewT;

        FloatViewT tmp_glyph_view = boost::gil::interleaved_view(
                cellWidth(), cellHeight(),
                reinterpret_cast<FloatPixelT*>(ctx.imageData_.data()),
                cellWidth() * sizeof(FloatPixelT));

        boost::gil::fill_pixels(tmp_glyph_view, FloatPixelT());
        boost::gil::copy_and_convert_pixels(imgv, subimage_view(
                tmp_glyph_view, 0, 0, imgv.width(), imgv.height()));

        pca()->project(ctx.imageData_, ctx.components_);
        return font()->getSymbol(pca()->findClosestGlyph(ctx.components_));
    }

    const FontPCA<FontImageT>* pca() const
    {
        return pca_;
    }

private:
    const FontImageT* font_;
    const FontPCA<FontImageT>* pca_;
};

template<class TFontImage>
class PcaGlyphMatcherFactory
{
public:
    typedef PcaGlyphMatcher<TFontImage> GlyphMatcherT;

    DynamicGlyphMatcher<TFontImage>* operator()(const TFontImage* font, const std::map<std::string, std::string>& options)
    {
        size_t nfeatures = 10;
        if (options.count("nf")) {
            try {
                nfeatures = boost::lexical_cast<size_t>(options.find("nf")->second);
            } catch (boost::bad_lexical_cast&) { }
        }

        FontPCAnalyzer<TFontImage>* pcanalyzer = new FontPCAnalyzer<TFontImage>(font);
        if (options.count("cache") && !options.find("cache")->second.empty()) {
            pcanalyzer->loadFromCache(options.find("cache")->second);
        } else {
            pcanalyzer->analyze();
        }
        if (options.count("makecache") && !options.find("makecache")->second.empty()) {
            pcanalyzer->saveToCache(options.find("makecache")->second);
        }

        FontPCA<TFontImage>* pca = new FontPCA<TFontImage>(pcanalyzer, nfeatures);

        boost::scoped_ptr<GlyphMatcherT> impl_holder(new GlyphMatcherT(pca));
        return new DynamicGlyphMatcher<TFontImage>(impl_holder);
    }
};


} } // namespace KG::Ascii


#endif // KGASCII_PCAGLYPHMATCHER_HPP
