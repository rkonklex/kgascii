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

#ifndef KGASCII_SURFACE_FWD_HPP
#define KGASCII_SURFACE_FWD_HPP

namespace KG { namespace Ascii {

template<typename T>
class SurfaceBase;

template<typename T>
class SurfaceContainerBase;

typedef SurfaceBase<unsigned char> Surface8;
typedef SurfaceBase<const unsigned char> Surface8c;
typedef SurfaceBase<float> Surface32f;
typedef SurfaceBase<const float> Surface32fc;

typedef SurfaceContainerBase<unsigned char> SurfaceContainer8;
typedef SurfaceContainerBase<float> SurfaceContainer32f;

} } // namespace KG::Ascii

#endif // KGASCII_SURFACE_FWD_HPP

