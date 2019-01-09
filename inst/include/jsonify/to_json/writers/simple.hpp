#ifndef R_JSONIFY_WRITERS_SIMPLE_H
#define R_JSONIFY_WRITERS_SIMPLE_H

#include <Rcpp.h>
#include "jsonify/to_json/utils.hpp"
#include "jsonify/to_json/writers/scalars.hpp"

using namespace rapidjson;

namespace jsonify {
namespace writers {
namespace simple {


  // ---------------------------------------------------------------------------
  // vectors
  // ---------------------------------------------------------------------------
  template <typename Writer>
  inline void write_value( Writer& writer, Rcpp::StringVector& sv, bool unbox ) {
    int n = sv.size();
    bool will_unbox = jsonify::utils::should_unbox( n, unbox );
    jsonify::utils::start_array( writer, will_unbox );
    
    for ( int i = 0; i < n; i++ ) {
      if (Rcpp::StringVector::is_na( sv[i] ) ) {
        writer.Null();
      } else{
        jsonify::writers::scalars::write_value( writer, sv[i] );
      }
    }
    jsonify::utils::end_array( writer, will_unbox );
  }
  
  /*
   * for writing a single value of a vector
   */
  template <typename Writer >
  inline void write_value( Writer& writer, Rcpp::StringVector& sv, size_t row, bool unbox ) {
    if ( Rcpp::StringVector::is_na( sv[ row ] ) ) {
      writer.Null();
    } else {
      const char *s = sv[ row ];
      jsonify::writers::scalars::write_value( writer, s );
    }
  }
  
  template< typename Writer>
  inline void write_value( Writer& writer, Rcpp::NumericVector& nv, bool unbox, 
                           int digits, bool numeric_dates ) {

    Rcpp::CharacterVector cls = jsonify::utils::getRClass( nv );
    
    if( !numeric_dates && jsonify::dates::is_in( "Date", cls ) ) {
      
      Rcpp::StringVector sv = jsonify::dates::date_to_string( nv );
      write_value( writer, sv, unbox );
      
    } else if ( !numeric_dates && jsonify::dates::is_in( "POSIXt", cls ) ) {
      
      Rcpp::StringVector sv = jsonify::dates::posixct_to_string( nv );
      write_value( writer, sv, unbox );
      
    } else {
    
      int n = nv.size();
      bool will_unbox = jsonify::utils::should_unbox( n, unbox );
      
      jsonify::utils::start_array( writer, will_unbox );
    
      for ( int i = 0; i < n; i++ ) {
        if( Rcpp::NumericVector::is_na( nv[i] ) ) {
          writer.Null();
        } else {
          jsonify::writers::scalars::write_value( writer, nv[i], digits );
        }
      }
    jsonify::utils::end_array( writer, will_unbox );
    }
  }
  
  /*
   * For writing a single value of a vector
   */
  template< typename Writer >
  inline void write_value( Writer& writer, Rcpp::NumericVector& nv, 
                           size_t row, bool unbox, int digits, 
                           bool numeric_dates ) {

    // Rcpp::Rcout << "writing nv with row: " << row << std::endl;
    
    Rcpp::CharacterVector cls = jsonify::utils::getRClass( nv );
    
    if( !numeric_dates && jsonify::dates::is_in( "Date", cls ) ) {

      Rcpp::StringVector sv = jsonify::dates::date_to_string( nv );
      write_value( writer, sv, row, unbox );
      
    } else if ( !numeric_dates && jsonify::dates::is_in( "POSIXt", cls ) ) {
      
      Rcpp::StringVector sv = jsonify::dates::posixct_to_string( nv );
      write_value( writer, sv, row, unbox );
      
    } else {
      if ( Rcpp::NumericVector::is_na( nv[ row ] ) ) {
        writer.Null();
      } else {
        double n = nv[ row ];
        jsonify::writers::scalars::write_value( writer, n, digits );
      }
    }
  }
  
  template < typename Writer >
  inline void write_value( Writer& writer, Rcpp::IntegerVector& iv, bool unbox, 
                           bool numeric_dates,
                           bool factors_as_string) {
    
    Rcpp::CharacterVector cls = jsonify::utils::getRClass( iv );

    if( !numeric_dates && jsonify::dates::is_in( "Date", cls ) ) {

      Rcpp::StringVector sv = jsonify::dates::date_to_string( iv );
      write_value( writer, sv, unbox );
      
    } else if ( !numeric_dates && jsonify::dates::is_in( "POSIXt", cls ) ) {
      
      Rcpp::StringVector sv = jsonify::dates::posixct_to_string( iv );
      write_value( writer, sv, unbox );
      
    } else if ( factors_as_string && Rf_isFactor( iv ) ) {
      Rcpp::CharacterVector lvls = iv.attr( "levels" );
      if (lvls.length() == 0 ) {
        // no level s- from NA_character_ vector
        Rcpp::StringVector s(1);
        s[0] = NA_STRING;
        write_value( writer, s, 0, unbox );
      } else {
        write_value( writer, lvls, unbox );
      }
    } else {
    
      int n = iv.size();
      bool will_unbox = jsonify::utils::should_unbox( n, unbox );
      jsonify::utils::start_array( writer, will_unbox );
      
      for ( int i = 0; i < n; i++ ) {
        if( Rcpp::IntegerVector::is_na( iv[i] ) ) {
          writer.Null();
        } else {
          jsonify::writers::scalars::write_value( writer, iv[i] );
        }
      }
      jsonify::utils::end_array( writer, will_unbox );
    }
  }
  

  /*
   * For writing a single value of a vector
   */
  template< typename Writer >
  inline void write_value( Writer& writer, Rcpp::IntegerVector& iv, size_t row, 
                           bool unbox, bool numeric_dates ) {
    
    Rcpp::CharacterVector cls = jsonify::utils::getRClass( iv );
    
    
    if( !numeric_dates && jsonify::dates::is_in( "Date", cls ) ) {
      
      Rcpp::StringVector sv = jsonify::dates::date_to_string( iv );
      write_value( writer, sv, row, unbox );
    } else if ( !numeric_dates && jsonify::dates::is_in( "POSIXt", cls ) ) {
      
      Rcpp::StringVector sv = jsonify::dates::posixct_to_string( iv );
      write_value( writer, sv, row, unbox );
      
    } else {
    
      if ( Rcpp::IntegerVector::is_na( iv[ row ] ) ) {
        writer.Null();
      } else {
        int i = iv[ row ];
        jsonify::writers::scalars::write_value( writer, i );
      }
    }
  }
  
  template <typename Writer>
  inline void write_value( Writer& writer, Rcpp::LogicalVector& lv, bool unbox ) {
    int n = lv.size();
    bool will_unbox = jsonify::utils::should_unbox( n, unbox );
    jsonify::utils::start_array( writer, will_unbox );
    
    for ( int i = 0; i < n; i++ ) {
      if (Rcpp::LogicalVector::is_na( lv[i] ) ) {
        writer.Null();
      } else {
        bool l = lv[i];             // required for logical vectors
        jsonify::writers::scalars::write_value( writer, l );
      }
    }
    jsonify::utils::end_array( writer, will_unbox );
  }
  
  template < typename Writer >
  inline void write_value( Writer& writer, Rcpp::LogicalVector& lv, size_t row, bool unbox ) {
    if ( Rcpp::LogicalVector::is_na( lv[ row ] ) ) { 
      writer.Null();
    } else {
      bool l = lv[ row ];
      jsonify::writers::scalars::write_value( writer, l );
    }
  }
  
  template < typename Writer >
  inline void write_value( Writer& writer, SEXP sexp, bool unbox, 
                           int digits, bool numeric_dates,
                           bool factors_as_string ) {
    
    switch( TYPEOF( sexp ) ) {
    case REALSXP: {
      Rcpp::NumericVector nv = Rcpp::as< Rcpp::NumericVector >( sexp );
      write_value( writer, nv, unbox, digits, numeric_dates );
      break;
    }
    case INTSXP: {
      Rcpp::IntegerVector iv = Rcpp::as< Rcpp::IntegerVector >( sexp );
      write_value( writer, iv, unbox, numeric_dates, factors_as_string );
      break;
    }
    case LGLSXP: {
      Rcpp::LogicalVector lv = Rcpp::as< Rcpp::LogicalVector >( sexp );
      write_value( writer, lv, unbox );
      break;
    }
    case STRSXP: {
      Rcpp::StringVector sv = Rcpp::as< Rcpp::StringVector >( sexp );
      write_value( writer, sv, unbox );
      break;
    }
    default: {
      Rcpp::stop("Unknown R object type");
    }
    }
  }
  
  // template < typename Writer, typename T >
  // inline void write_value( Writer& writer, T& sexp, bool unbox,
  //                          int digits, bool numeric_dates ) {
  // 
  //   bool factors_as_string = true;
  //   write_value( writer, sexp, unbox, digits, numeric_dates, factors_as_string );
  // }
  // 
  // template < typename Writer, typename T >
  // inline void write_value( Writer& writer, T& sexp, bool unbox, int digits) {
  // 
  //   bool numeric_dates = true;
  //   write_value( writer, sexp, unbox, digits, numeric_dates );
  // }
  // 
  // template < typename Writer, typename T >
  // inline void write_value( Writer& writer, T& sexp, bool unbox ) {
  // 
  //   int digits = -1;
  //   write_value( writer, sexp, unbox, digits );
  // }
  // 
  // template < typename Writer, typename T >
  // inline void write_value( Writer& writer, T& sexp) {
  // 
  //   bool unbox = false;
  //   write_value( writer, sexp, unbox );
  // }
  
  /*
   * template for R SEXPs for single-row from a vector
   */
  template < typename Writer >
  inline void write_value( Writer& writer, SEXP sexp, size_t row, bool unbox,
                           int digits, bool numeric_dates, bool factors_as_string) {

    // Rcpp::Rcout << "writing value: factors_as_string: " << factors_as_string << std::endl;
    
    // Rcpp::Rcout << "writing value from vector" << std::endl;

    switch( TYPEOF( sexp ) ) {
    case REALSXP: {
      Rcpp::NumericVector nv = Rcpp::as< Rcpp::NumericVector >( sexp );
      write_value( writer, nv, row, unbox, digits, numeric_dates );
      break;
    }
    case INTSXP: {
      Rcpp::IntegerVector iv = Rcpp::as< Rcpp::IntegerVector >( sexp );
      // TODO( do we need factors_as_string here, or will it be sorted by the time it comes to this step?)
      write_value( writer, iv, row, unbox, numeric_dates );
      break;
    }
    case LGLSXP: {
      Rcpp::LogicalVector lv = Rcpp::as< Rcpp::LogicalVector >( sexp );
      write_value( writer, lv, row, unbox );
      break;
    }
    case STRSXP: {
      Rcpp::StringVector sv = Rcpp::as< Rcpp::StringVector >( sexp );
      write_value( writer, sv, row, unbox );
      break;
    }
    default: {
      Rcpp::stop("Unknown R object type");
    }
    }
  }

  // template < typename Writer, typename T >
  // inline void write_value( Writer& writer, T& sexp, size_t row, bool unbox,
  //                          int digits, bool numeric_dates ) {
  // 
  //   bool factors_as_string = true;
  //   write_value( writer, sexp, row, unbox, digits, numeric_dates, factors_as_string );
  // }
  // 
  // template < typename Writer, typename T >
  // inline void write_value( Writer& writer, T& sexp, size_t row, bool unbox, int digits) {
  // 
  //   bool numeric_dates = true;
  //   write_value( writer, sexp, row, unbox, digits, numeric_dates );
  // }
  // 
  // template < typename Writer, typename T >
  // inline void write_value( Writer& writer, T& sexp, size_t row, bool unbox ) {
  // 
  //   int digits = -1;
  //   write_value( writer, sexp, row, unbox, digits );
  // }
  // 
  // template < typename Writer, typename T >
  // inline void write_value( Writer& writer, T& sexp, size_t row) {
  // 
  //   bool unbox = false;
  //   write_value( writer, sexp, row, unbox );
  // }
  
  
  /*
   * template for C++ single object types
   */
  // template < typename Writer, typename T >
  // inline void write_value( Writer& writer, T& val, int digits = -1 ) {
  //   switch( TYPEOF( val ) ) {
  //   case REALSXP: {
  //     write_value( writer, val, digits );
  //     break;
  //   }
  //   default: {
  //     write_value( writer, val );
  //     break;
  //   }
  //   }
  // }
  

  // ---------------------------------------------------------------------------
  // matrix values
  // ---------------------------------------------------------------------------
  
  template < typename Writer >
  inline void write_value( Writer& writer, Rcpp::IntegerMatrix& mat, bool unbox = false,
                           std::string by = "row" ) {
    
    bool will_unbox = false;
    jsonify::utils::start_array( writer, will_unbox );
    int n;
    int i;
    
    if ( by == "row" ) {
      n = mat.nrow();
      for ( i = 0; i < n; i++ ) {
        Rcpp::IntegerVector this_row = mat(i, Rcpp::_);
        write_value( writer, this_row, unbox, true, true );  // true, true : numeric_dates, factors_as_string
      }
    } else { // by == "column"
      n = mat.ncol();
      for( i = 0; i < n; i++ ) {
        Rcpp::IntegerVector this_col = mat( Rcpp::_, i );
        write_value( writer, this_col, unbox, true, true ); // true, true : numeric_dates, factors_as_string
      }
    }
    jsonify::utils::end_array( writer, will_unbox );
  }
  
  template < typename Writer >
  inline void write_value( Writer& writer, Rcpp::NumericMatrix& mat, bool unbox = false, 
                           int digits = -1, std::string by = "row" ) {
    
    bool will_unbox = false;
    jsonify::utils::start_array( writer, will_unbox );
    
    int n;
    int i;
    if ( by == "row" ) {
      n = mat.nrow();
      for ( i = 0; i < n; i++ ) {
        Rcpp::NumericVector this_row = mat(i, Rcpp::_);
        write_value( writer, this_row, unbox, digits, true );  // true : numeric dates
      }
    } else { // by == "column"
      n = mat.ncol();
      for( i = 0; i < n; i++ ) {
        Rcpp::NumericVector this_col = mat( Rcpp::_, i );
        write_value( writer, this_col, unbox, digits, true );  // true : numeric dates
      }
    }
    jsonify::utils::end_array( writer, will_unbox );
  }
  
  template < typename Writer >
  inline void write_value( Writer& writer, Rcpp::CharacterMatrix& mat, bool unbox = false,
                           std::string by = "row" ) {
    
    bool will_unbox = false;
    jsonify::utils::start_array( writer, will_unbox );
    int i;
    int n;
    
    if( by == "row" ) {
      n = mat.nrow();
      for ( i = 0; i < n; i++ ) {
        Rcpp::StringVector this_row = mat( i, Rcpp::_ );
        write_value( writer, this_row, unbox );
      }
    } else { // by == column
      n = mat.ncol();
      for ( i = 0; i < n; i++ ) {
        Rcpp::StringVector this_col = mat( Rcpp::_, i );
        write_value( writer, this_col, unbox );
      }
    }
    jsonify::utils::end_array( writer, will_unbox );
  }
  
  
  template < typename Writer >
  inline void write_value( Writer& writer, Rcpp::LogicalMatrix& mat, bool unbox = false, 
                           std::string by = "row" ) {
    
    bool will_unbox = false;
    jsonify::utils::start_array( writer, will_unbox );
    int i;
    int n;
    
    if( by == "row" ) {
      n = mat.nrow();
      
      for ( i = 0; i < n; i++ ) {
        Rcpp::LogicalVector this_row = mat(i, Rcpp::_);
        write_value( writer, this_row, unbox );
      }
    } else { // by == "column;
      n = mat.ncol();
      
      for( i = 0; i < n; i++ ) {
        Rcpp::LogicalVector this_col = mat( Rcpp::_, i );
        write_value( writer, this_col, unbox);
      }
    }
    jsonify::utils::end_array( writer, will_unbox );
  }

} // namespace simple
} // namespace writers
} // namespace jsonify

#endif
