#include <Rcpp.h>
using namespace Rcpp;



// [[Rcpp::export]]
NumericMatrix rcpp_matrix(){
  // Creating a vector object
  NumericVector v = {1,2,3,4};
  
  // Set the number of rows and columns to attribute dim of the vector object.
  v.attr("dim") = Dimension(2, 2);
  
  // Converting to Rcpp Matrix type
  NumericMatrix m = as<NumericMatrix>(v);
  
  // Return the vector to R
  return m;
}