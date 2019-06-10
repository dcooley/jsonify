#ifndef R_JSONIFY_FROM_JSON_H
#define R_JSONIFY_FROM_JSON_H

#include <Rcpp.h>

#include "from_json_utils.hpp"

namespace jsonify {
namespace from_json {

  // Extract all logical values from each named element of a nested list.
  inline void extract_lgl_vector(Rcpp::List& x) {
    df_out_lgl.clear();
    for(unsigned int i = 0; i < x.size(); ++ i) {
      pv_list = x[i];
      df_out_lgl.push_back(pv_list[temp_name]);
    }
  }

  // Extract all int values from each named element of a nested list.
  inline void extract_int_vector(Rcpp::List& x) {
    df_out_int.clear();
    for(unsigned int i = 0; i < x.size(); ++ i) {
      pv_list = x[i];
      df_out_int.push_back(pv_list[temp_name]);
    }
  }

  // Extract all double values from each named element of a nested list.
  inline void extract_dbl_vector(Rcpp::List& x) {
    df_out_dbl.clear();
    for(unsigned int i = 0; i < x.size(); ++ i) {
      pv_list = x[i];
      df_out_dbl.push_back(pv_list[temp_name]);
    }
  }

  // Extract all string values from each named element of a nested list.
  inline void extract_str_vector(Rcpp::List& x) {
    df_out_str.clear();
    for(unsigned int i = 0; i < x.size(); ++ i) {
      pv_list = x[i];
      df_out_str.push_back(pv_list[temp_name]);
    }
  }

  // Iterate over a rapidjson object and get the unique data types of each value.
  // Save unique data types as ints to unordered_set dtypes.
  // Compatible with rapidjson::Array and rapidjson::Value.
  template<typename T>
  inline void get_dtypes(T& doc, bool scalar_only = false) {
    // Clear all values from dtypes
    dtypes.clear();
    
    int doc_len = doc.Size();
    int curr_dtype;
    int i;
    for(i = 0; i < doc_len; ++i) {
      curr_dtype = doc[i].GetType();
      // rapidjson uses separate ints for types true (2) and false (1)...combine
      // them into one value such that bool is 1.
      if(curr_dtype == 2) {
        curr_dtype = 1;
      }
      
      // rapidjson uses the same int for types double and int...split them up,
      // such that double is 8 and int is 9.
      if(curr_dtype == 6) {
        if(doc[i].IsDouble()) {
          curr_dtype = 8;
        } else {
          curr_dtype = 9;
        }
      }
      
      dtypes.insert(curr_dtype);
      
      // If scalar_only is true, break if the current value is
      // type 3 (JSON object) or 4 (Array object).
      if(scalar_only) {
        if(curr_dtype == 3 || curr_dtype == 4) {
          break;
        }
      }
    }
  }
  
  
  // Dump objects from a rapidjson array to an R list.
  // Can handle JSON objects that have keys and those that do not have keys.
  template<typename T>
  inline Rcpp::List array_to_list(T& array, int& array_len) {
    Rcpp::List out(array_len);
    for(int i = 0; i < array_len; ++i) {
      
      switch(array[i].GetType()) {
      
      // bool - false
      case 1: {
        out[i] = array[i].GetBool();
        break;
      }
        
      // bool - true
      case 2: {
        out[i] = array[i].GetBool();
        break;
      }
        
      // string
      case 5: {
        out[i] = array[i].GetString();
        break;
      }
        
      // numeric
      case 6: {
        if(array[i].IsDouble()) {
          // double
          out[i] = array[i].GetDouble();
        } else {
          // int
          out[i] = array[i].GetInt();
        }
        break;
      }
      
      // null
      case 0: {
        out[i] = R_NA_VAL;
        break;
      }
        
      // array
      case 4: {
        int curr_array_len = array[i].Size();
        T curr_array = array[i].GetArray();
        out[i] = array_to_list<T>(curr_array, curr_array_len);
        break;
      }
        
      // some other data type not covered
      default: {
        Rcpp::stop("Uknown data type. Only able to parse int, double, string, bool, and array");
      }
        
      }
    }
    
    return out;
  }
  
  
  // Parse an array object, return an SEXP that contains the objects from the
  // array.
  // Can handle JSON objects that have keys and those that do not have keys.
  template<typename T>
  inline SEXP parse_array(T& array) {
    int array_len = array.Size();
    //Rcpp::Rcout << "array_len: " << array_len << std::endl;
    
    // Get set of unique data types of each value in the array.
    get_dtypes<T>(array);
    
    // If there's only one unique data type in the input array, or if the array
    // is made up a simple data type and nulls (simple being int, double,
    // string, bool), then return an R vector. Otherwise, return an R list.
    
    if(dtypes.size() > 2) {
      //Rcpp::Rcout << "dtypes.size == 2 " << std::endl;
      return array_to_list<T>(array, array_len);
    }
    
    int data_type;
    if(dtypes.size() == 1) {
      data_type = *dtypes.begin();
    } else {
      Rcpp::Rcout << "dtypes > 1 " << std::endl;
      // Dump dtypes values to a vector.
      std::vector<int> dtype_vect(dtypes.begin(), dtypes.end());
      
      // Check to see if 0 is in dtypes.
      if(dtypes.find(0) != dtypes.end()) {
        // If 0 is in dtypes and dtypes size is two, get the int in dtypes
        // that is not 0.
        data_type = dtype_vect[0];
        if(data_type == 0) {
          data_type = dtype_vect[1];
        }
        if(data_type == 3 || data_type == 4) {
          // If dtype_vect is [0, 3] or [0, 4], return R list.
          return array_to_list<T>(array, array_len);
        }
      } else {
        // If dtype_vect size is 2 and 0 is not one of the values, return R list.
        return array_to_list<T>(array, array_len);
      }
    }
    
    // Get current value
    switch(data_type) {
    Rcpp::Rcout << "switching data types " << std::endl;
    // bool
    case 1: {
      Rcpp::LogicalVector out(array_len);
      for(int i = 0; i < array_len; ++i) {
        if(array[i].GetType() == 0) {
          out[i] = NA_LOGICAL;
        } else {
          out[i] = array[i].GetBool();
        }
      }
      return out;
    }
      
    // string
    case 5: {
      Rcpp::CharacterVector out(array_len);
      for(int i = 0; i < array_len; ++i) {
        if(array[i].GetType() == 0) {
          out[i] = NA_STRING;
        } else {
          out[i] = array[i].GetString();
        }
      }
      return out;
    }
      
    // double
    case 8: {
      Rcpp::NumericVector out(array_len);
      for(int i = 0; i < array_len; ++i) {
        if(array[i].GetType() == 0) {
          out[i] = NA_REAL;
        } else {
          out[i] = array[i].GetDouble();
        }
      }
      return out;
    }
      
    // int
    case 9: {
      Rcpp::IntegerVector out(array_len);
      for(int i = 0; i < array_len; ++i) {
        if(array[i].GetType() == 0) {
          out[i] = NA_INTEGER;
        } else {
          out[i] = array[i].GetInt();
        }
      }
      return out;
    }
      
    // null
    case 0: {
      Rcpp::LogicalVector out(array_len, NA_LOGICAL);
      return out;
    }
    
    // JSON object
    case 3: {
      Rcpp::List out(array_len);
      for(int i = 0; i < array_len; ++i) {
        const rapidjson::Value& curr_val = array[i];
        out[i] = parse_value(curr_val);
      }
      return out;
    }
      
    // array
    case 4: {
      Rcpp::List out(array_len);
      for(int i = 0; i < array_len; ++i) {
        T curr_array = array[i].GetArray();
        out[i] = parse_array<T>(curr_array);
      }
      return out;
    }
    }
    
    return R_NilValue;
  }
  
  
  // Parse rapidjson::Value object.
  inline Rcpp::List parse_value(const rapidjson::Value& val) {
    Rcpp::Rcout << "parse_value() " << std::endl;
    int json_len = val.Size();
    Rcpp::Rcout << "json_len " << json_len << std::endl;
    Rcpp::List out(json_len);
    Rcpp::CharacterVector names(json_len);
    
    int i = 0;
    for (rapidjson::Value::ConstMemberIterator itr = val.MemberBegin(); itr != val.MemberEnd(); ++itr) {
      
      Rcpp::Rcout << "iterating i : " << i << std::endl;
      
      // Get current key
      names[i] = itr->name.GetString();
      Rcpp::Rcout << "value names: " << names << std::endl;
      
      // Get current value
      switch(itr->value.GetType()) {
      
      // bool - false
      case 1: {
        Rcpp::Rcout << "value is bool " << std::endl;
        out[i] = itr->value.GetBool();
        break;
      }
        
      // bool - true
      case 2: {
        Rcpp::Rcout << "value is bool " << std::endl;
        out[i] = itr->value.GetBool();
        break;
      }
        
      // string
      case 5: {
        Rcpp::Rcout << "value is string " << std::endl;
        out[i] = itr->value.GetString();
        break;
      }
        
      // numeric
      case 6: {
        Rcpp::Rcout << "value is numeric " << std::endl;
        if(itr->value.IsDouble()) {
          // double
          out[i] = itr->value.GetDouble();
        } else {
          // int
          out[i] = itr->value.GetInt();
        }
        break;
      }
      
      // null
      case 0: {
        Rcpp::Rcout << "value is null " << std::endl;
        out[i] = R_NA_VAL;
        break;
      }
        
      // array
      case 4: {
        Rcpp::Rcout << "value is array " << std::endl;
        rapidjson::Value::ConstArray curr_array = itr->value.GetArray();
        out[i] = parse_array<rapidjson::Value::ConstArray>(curr_array);
        break;
      }
        
      // JSON object
      case 3: {
        Rcpp::Rcout << "value is object " << std::endl;
        const rapidjson::Value& curr_val = itr->value;
        int curr_val_size = curr_val.Size();
        Rcpp::Rcout << "curr_val_size: " << curr_val_size << std::endl;
        //Rcpp::Rcout sub_out( curr_val_size );
        
        out[i] = parse_value(curr_val);
        //return out;
        break;
      }
        
      // some other data type not covered
      default: {
        Rcpp::stop("Uknown data type. Only able to parse int, double, string, bool, array, and json");
      }
      }
      
      // Bump i
      i++;
    }
    
    Rcpp::Rcout << "out names " << names << std::endl;
    out.attr("names") = names;
    return out;
  }
  
  
  // Parse rapidjson::Document object.
  inline SEXP parse_document(rapidjson::Document& doc) {
    int json_len = doc.Size();
    
    // If doc has length zero, return NULL.
    if(json_len == 0) {
      return R_NilValue;
    }
    
    Rcpp::List out(json_len);
    Rcpp::CharacterVector names(json_len);
    
    int i = 0;
    for(rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr) {

      // Get current key
      names[i] = itr->name.GetString();

      // Get current value
      switch(itr->value.GetType()) {

      // bool - false
      case 1: {
        out[i] = itr->value.GetBool();
        break;
      }

      // bool - true
      case 2: {
        out[i] = itr->value.GetBool();
        break;
      }

      // string
      case 5: {
        out[i] = itr->value.GetString();
        break;
      }

      // numeric
      case 6: {
        if(itr->value.IsDouble()) {
          // double
          out[i] = itr->value.GetDouble();
        } else {
          // int
          out[i] = itr->value.GetInt();
        }
        break;
      }

      // null
      case 0: {
        out[i] = R_NA_VAL;
        break;
      }

      // array
      case 4: {
        rapidjson::Value::ConstArray curr_array = itr->value.GetArray();
        out[i] = parse_array<rapidjson::Value::ConstArray>(curr_array);
        break;
      }

      // JSON object
      case 3: {
        const rapidjson::Value& temp_val = itr->value;
        out[i] = parse_value(temp_val);
        break;
      }

      // some other data type not covered
      default: {
        Rcpp::stop("Uknown data type. Only able to parse int, double, string, bool, array, and json");
      }
      }

      // Bump i
      i++;
    }
    
    out.attr("names") = names;
    return out;
  }
  
  
  // Parse rapidjson::Document object that contains "keyless" JSON data of the
  // same data type. Returns an R vector.
  inline SEXP doc_to_vector(rapidjson::Document& doc, int& dtype) {
    int doc_len = doc.Size();
    
    int i;
    // Get current value
    switch(dtype) {
    
    // bool
    case 1: {
      Rcpp::LogicalVector out(doc_len);
      for(i = 0; i < doc_len; ++i) {
        if(doc[i].GetType() == 0) {
          out[i] = NA_LOGICAL;
        } else {
          out[i] = doc[i].GetBool();
        }
      }
      return out;
    }
      
    // string
    case 5: {
      Rcpp::CharacterVector out(doc_len);
      for(i = 0; i < doc_len; ++i) {
        if(doc[i].GetType() == 0) {
          out[i] = NA_STRING;
        } else {
          out[i] = doc[i].GetString();
        }
      }
      return out;
    }
      
    // double
    case 8: {
      Rcpp::NumericVector out(doc_len);
      for(i = 0; i < doc_len; ++i) {
        if(doc[i].GetType() == 0) {
          out[i] = NA_REAL;
        } else {
          out[i] = doc[i].GetDouble();
        }
      }
      return out;
    }
      
    // int
    case 9: {
      Rcpp::IntegerVector out(doc_len);
      for(i = 0; i < doc_len; ++i) {
        if(doc[i].GetType() == 0) {
          out[i] = NA_INTEGER;
        } else {
          out[i] = doc[i].GetInt();
        }
      }
      return out;
    }
      
    // null
    case 0: {
      Rcpp::LogicalVector out(doc_len, NA_LOGICAL);
      return out;
    }
    }
    
    return R_NilValue;
  }
  
  
  // Parse rapidjson::Document object that contains "keyless" JSON data that
  // contains a variety of data types. Returns an R list.
  inline Rcpp::List doc_to_list(rapidjson::Document& doc, bool& simplifyDataFrame) {
    int doc_len = doc.Size();
    //Rcpp::Rcout << "doc_len: " << doc_len << std::endl; 
    Rcpp::List out(doc_len);
    
    bool return_df = true;
    names_map.clear();
    int i;
    
    for(i = 0; i < doc_len; ++i) {
      
      int tp = doc[i].GetType();
      //Rcpp::Rcout << "doc type " << tp << std::endl;
      
      // Get current valueB
      switch(doc[i].GetType()) {
      
      // bool - false
      case 1: {
        out[i] = doc[i].GetBool();
        return_df = false;
        break;
      }
        
      // bool - true
      case 2: {
        out[i] = doc[i].GetBool();
        return_df = false;
        break;
      }
        
      // string
      case 5: {
        out[i] = doc[i].GetString();
        return_df = false;
        break;
      }
        
      // numeric
      case 6: {
        if(doc[i].IsDouble()) {
          // double
          out[i] = doc[i].GetDouble();
        } else {
          // int
          out[i] = doc[i].GetInt();
        }
        return_df = false;
        break;
      }
      
      // null
      case 0: {
        out[i] = R_NA_VAL;
        return_df = false;
        break;
      }
        
      // array
      case 4: {
        //Rcpp::Rcout << "case 4 - array " << std::endl;
        rapidjson::Value::Array curr_array = doc[i].GetArray();
        out[i] = parse_array<rapidjson::Value::Array>(curr_array);
        return_df = false;
        break;
      }
        
      // JSON object
      case 3: {
        const rapidjson::Value& temp_val = doc[i];
        pv_list = parse_value(temp_val);
        Rcpp::Rcout << "value parsed" << std::endl;
        out[i] = pv_list;
        
        //return out;
        
        // If simplifyDataFrame is true and i is 0, record the data types of 
        // each named element of doc[i] in unordered_map names_map.
        if(simplifyDataFrame && i == 0) {
          //Rcpp::Rcout << "simplify & i == 0 " << std::endl;
          pv_len = pv_list.size();
          names = pv_list.attr("names");
          Rcpp::Rcout << "names: " << names << std::endl;
          for(unsigned int n = 0; n < names.size(); ++n) {
            temp_name = Rcpp::as<std::string>(names[n]);
            //Rcpp::Rcout << "names_map" << std::endl;
            names_map[temp_name] = TYPEOF(pv_list[n]);
            //Rcpp::Rcout << "names_map end " << std::endl;
          }
          break;
        }
        
        // If simplifyDataFrame and return_df are both true, compare the data 
        // types of each named element of doc[i] with the elements in 
        // names_map. If the names do not align, or the data types of the 
        // names do not align, set return_df to false.
        if(simplifyDataFrame && return_df) {
          if(pv_list.size() != pv_len) {
            return_df = false;
            break;
          }
          names = pv_list.attr("names");
          for(unsigned int n = 0; n < names.size(); ++n) {
            temp_name = Rcpp::as<std::string>(names[n]);
            if(names_map.count(temp_name) == 0) {
              return_df = false;
              break;
            }
            if(names_map[temp_name] != TYPEOF(pv_list[n])) {
              return_df = false;
              break;
            }
          }
        }
        
        break;
      }
        
      // some other data type not covered
      default: {
        Rcpp::stop("Uknown data type. Only able to parse int, double, string, bool, array, and json");
      }
      }
    }
    
    // If simplifyDataFrame and return_df are both true, convert List "out" to 
    // a dataframe, with the names of "out" making up the df col headers.
    if(simplifyDataFrame && return_df) {
      //Rcpp::Rcout << "simply && return_df" << std::endl;
      //Rcpp::Rcout << "pv_len " << pv_len << std::endl;
      Rcpp::List df_out = Rcpp::List(pv_len);
      
      for(i = 0; i < pv_len; ++i) {
        temp_name = names[i];
        int tp =names_map[ temp_name ];
        Rcpp::Rcout << "switching type " << tp << std::endl;
        switch(names_map[temp_name]) {
        case 10: {
          extract_lgl_vector(out);
          df_out[i] = df_out_lgl;
          break;
        }
        case 13: {
          extract_int_vector(out);
          df_out[i] = df_out_int;
          break;
        }
        case 14: {
          extract_dbl_vector(out);
          df_out[i] = df_out_dbl;
          break;
        }
        case 19: {
          // TODO - which R type is this? 
          // If it's a list, doc_to_list??
          Rcpp::Rcout << "case 19 not yet done" << std::endl;
          // extract list?
          //return out;
          df_out[i] = out;
          simplifyDataFrame = false;
          //return out;
          break;
          
        }
        default: { // string, case 16
          extract_str_vector(out);
          df_out[i] = df_out_str;
          break;
        }
        }
      }
      
      df_out.attr("names") = names;
      df_out.attr("class") = "data.frame";
      df_out.attr("row.names") = Rcpp::seq(1, doc_len);
      
      return df_out;
    }

    return out;
  }
  
  
  //' Parse JSON String
  //'
  //' Takes a JSON string as input, returns an R list of key-value pairs
  //'
  //' @param json const char, JSON string to be parsed. Coming from R, this
  //'  input should be a character vector of length 1.
  //' @export
  inline SEXP from_json(const char * json, bool& simplifyDataFrame) {
    rapidjson::Document doc;
    doc.Parse(json);
    
    // Make sure there were no parse errors
    if(doc.HasParseError()) {
      Rcpp::Rcerr << "parse error for json string: " << json << std::endl;
      Rcpp::stop("json parse error");
    }
    
    // If the input is a scalar value of type int, double, string, or bool, 
    // return Rcpp vector with length 1.
    if( doc.IsInt() ) {
      Rcpp::IntegerVector x(1);
      x[0] = doc.GetInt();
      return x;
    }
    
    if( doc.IsDouble() ) {
      Rcpp::NumericVector x(1);
      x[0] = doc.GetDouble();
      return x;
    }
    
    if( doc.IsString() ) {
      Rcpp::CharacterVector x(1);
      x[0] = doc.GetString();
      return x;
    }
    
    if( doc.IsBool() ) {
      Rcpp::LogicalVector x(1);
      x[0] = doc.GetBool();
      return x;
    }
    
    // If input is not an array, pass doc through parse_document(), and return
    // the result.
    if(!doc.IsArray()) {
      return parse_document(doc);
    }
    
    // If input is an empty array, return NULL.
    if(doc.Size() == 0) {
      return R_NilValue;
    }
    
    // Get set of unique data types in doc.
    get_dtypes<rapidjson::Document>(doc, true);
    int dtype_len = dtypes.size();

    // If dtype_len is greater than 2, return an R list of values.
    if(dtype_len > 2) {
      return doc_to_list(doc, simplifyDataFrame);
    }

    // If dtype_len is 2 and 0 does not appear in dtypes, return an
    // R list of values.
    if(dtype_len == 2 && dtypes.find(0) == dtypes.end()) {
      //Rcpp::Rcout << "doc_to_list 1" << std::endl;
      return doc_to_list(doc, simplifyDataFrame);
    }

    // If 3 or 4 is in dtypes, return an R list of values.
    if(dtypes.find(3) != dtypes.end() ||
       dtypes.find(4) != dtypes.end()) {
      Rcpp::Rcout << "doc_to_list 2" << std::endl;
      return doc_to_list(doc, simplifyDataFrame);
    }

    // Dump ints from dtypes to an std vector.
    std::vector<int> dtype_vect(dtypes.begin(), dtypes.end());
    int dt = dtype_vect[0];

    // If dtype_len is 1, return an R vector.
    if(dtype_len == 1) {
      return doc_to_vector(doc, dt);
    }

    // Else if dtype_len is 2 and 0 is in dtypes, return an R vector.
    if(dtype_len == 2) {
      if(dt == 0) {
        dt = dtype_vect[1];
      }
    }

    return doc_to_vector(doc, dt);
  }

} // namespace from_json
} // namespace jsonify

#endif
