/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  Types.cxx 

                        Types.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
#include "execution/Types.hpp"

#include <pkgconf/system.h>
#ifdef OROPKG_GEOMETRY
// Include geometry support
#include <geometry/frames.h>
#endif

// Cappellini Consonni Extension
#include <corelib/MultiVector.hpp>

#include "execution/TaskVariable.hpp"

namespace ORO_Execution
{
#ifdef OROPKG_GEOMETRY
  using ORO_Geometry::Frame;
  using ORO_Geometry::Vector;
  using ORO_Geometry::Rotation;
  using ORO_Geometry::Wrench;
  using ORO_Geometry::Twist;
#endif
  // Cappellini Consonni Extension
  using ORO_CoreLib::Double6D;

  template<typename T>
  class TemplateTypeInfo
    : public TypeInfo
  {
  public:
    TaskAttributeBase* buildConstant()
      {
        return new TaskConstant<T>();
      };
    TaskAttributeBase* buildVariable()
      {
        return new TaskVariable<T>();
      };
    TaskAttributeBase* buildAlias( DataSourceBase* b )
      {
        DataSource<T>* ds( dynamic_cast<DataSource<T>*>( b ) );
        if ( ! ds )
          return 0;
        return new TaskAliasAttribute<T>( ds );
      };
  };

    // Identical to above, but the variable is of the TaskIndexVariable type.
  template<typename T, typename IndexType, typename SetType, typename Pred>
  class TemplateIndexTypeInfo
    : public TypeInfo
  {
      Pred _p;
  public:
      TemplateIndexTypeInfo(Pred p) : _p (p) {}

    TaskAttributeBase* buildConstant()
      {
        return new TaskConstant<T>();
      }

    TaskAttributeBase* buildVariable()
      {
        return new TaskIndexVariable<T, IndexType, SetType, Pred>(_p);
      }

    TaskAttributeBase* buildAlias( DataSourceBase* b )
      {
        DataSource<T>* ds( dynamic_cast<DataSource<T>*>( b ) );
        if ( ! ds )
          return 0;
        return new TaskAliasAttribute<T>( ds );
      }
  };


  TypeInfo::~TypeInfo()
  {
  };

  TypeInfoRepository& TypeInfoRepository::instance()
  {
    static TypeInfoRepository s;
    return s;
  };

  TypeInfo* TypeInfoRepository::type( const std::string& name )
  {
    map_t::iterator i = data.find( name );
    if ( i == data.end() )
      return 0;
    else return i->second;
  };

  TypeInfoRepository::~TypeInfoRepository()
  {
  };

  // check the validity of an index
  bool D6IndexChecker( int i )
  {
    return i > -1 && i < 6;
  }

  // check the validity of an index
  bool D3IndexChecker( int i )
  {
    return i > -1 && i < 3;
  }


  TypeInfoRepository::TypeInfoRepository()
  {
#ifdef OROPKG_GEOMETRY
    data["frame"] = new TemplateTypeInfo<Frame>();
    data["rotation"] = new TemplateTypeInfo<Rotation>();
    data["wrench"] = new TemplateIndexTypeInfo<Wrench,int, double, bool (*)(int)>( &D6IndexChecker );
    data["twist"] = new TemplateIndexTypeInfo<Twist,int, double, bool (*)(int)>( &D6IndexChecker );
    data["vector"] = new TemplateIndexTypeInfo<Vector,int, double, bool (*)(int)>( &D3IndexChecker );
#endif
    data["int"] = new TemplateTypeInfo<int>();
    data["char"] = new TemplateTypeInfo<char>();
    data["string"] = new TemplateTypeInfo<std::string>();
    data["double"] = new TemplateTypeInfo<double>();
    data["bool"] = new TemplateTypeInfo<bool>();
    data["double6d"] = new TemplateIndexTypeInfo<Double6D,int, double, bool (*)(int)>( &D6IndexChecker );

  };
}
