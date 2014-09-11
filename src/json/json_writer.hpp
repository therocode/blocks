//
// json0
//
// 2011 Michael Nicolella
//

#ifndef JSON0_JSON_WRITER_H
#define JSON0_JSON_WRITER_H

#include "json_value.hpp"

#include "detail/json_internal.hpp"
#include "detail/json_internal_writer.hpp"

#include <ostream>

namespace json
{
  inline void write( std::ostream& out, json::Value& root )
  {
    //root element must be Object
    if( root.type == kValueObject )
    {
      detail::json_write_impl(out, root, 0);
    }
    else
    {
      detail::RaiseException("root object passed to json::write must be an Object");
    }
  }
}

#endif
