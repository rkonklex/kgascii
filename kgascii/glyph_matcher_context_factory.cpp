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

#include <kgascii/glyph_matcher_context_factory.hpp>
#include <kgascii/glyph_matcher_context.hpp>
#include <kgascii/glyph_matcher.hpp>
#include <kgascii/font_image.hpp>
#include <kgascii/policy_based_glyph_matcher.hpp>
#include <kgascii/mutual_information_glyph_matcher.hpp>
#include <kgascii/squared_euclidean_distance.hpp>
#include <kgascii/means_distance.hpp>
#include <kgascii/pca_glyph_matcher.hpp>
#include <kgascii/font_pcanalyzer.hpp>
#include <kgascii/font_pca.hpp>
#include <map>
#include <deque>
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace KG { namespace Ascii {

using namespace boost::algorithm;

GlyphMatcherContextFactory::GlyphMatcherContextFactory()
{
}

GlyphMatcherContextFactory::~GlyphMatcherContextFactory()
{
}

GlyphMatcherContext* GlyphMatcherContextFactory::create(const FontImage* font, 
        const std::string& options) const
{
    std::vector<std::string> options_tokens;
    split(options_tokens, options, is_any_of(":"), token_compress_on);
    std::string algo_name = "pca";
    if (!options_tokens.empty()) {
        algo_name = options_tokens.front();
    }
    std::map<std::string, std::string> options_map;
    for (size_t i = 1; i < options_tokens.size(); ++i) {
        std::vector<std::string> opt_tokens;
        split(opt_tokens, options_tokens[i], is_any_of("="), token_compress_on);
        std::string opt_name = opt_tokens[0];
        std::string opt_value = opt_tokens.size() > 1 ? opt_tokens[1] : "";
        options_map[opt_name] = opt_value;
    }
    if (algo_name == "pca") {
        size_t nfeatures = 10;
        try {
            nfeatures = boost::lexical_cast<size_t>(options_map["nf"]);
        } catch (boost::bad_lexical_cast&) { }
        FontPCAnalyzer* pcanalyzer = new FontPCAnalyzer(font);
        FontPCA* pca = new FontPCA(pcanalyzer, nfeatures);
        return new PcaGlyphMatcherContext(pca);
    } else if (algo_name == "sed") {
        return new PolicyBasedGlyphMatcherContext<SquaredEuclideanDistance>(font);
    } else if (algo_name == "md") {
        return new PolicyBasedGlyphMatcherContext<MeansDistance>(font);
    } else if (algo_name == "mi") {
        size_t bins = 16;
        try {
            bins = boost::lexical_cast<size_t>(options_map["bins"]);
        } catch (boost::bad_lexical_cast&) { }
        return new MutualInformationGlyphMatcherContext(font, bins);
    } else {
        throw std::runtime_error("unknown algo name");
    }
}

} } // namespace KG::Ascii
