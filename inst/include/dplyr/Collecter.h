#ifndef dplyr_Collecter_H
#define dplyr_Collecter_H

namespace dplyr {
    
    class Collecter {
    public:
        virtual ~Collecter(){} ;
        virtual void collect( const SlicingIndex& index, SEXP v ) = 0 ;
        virtual SEXP get() = 0 ;
        virtual bool compatible(SEXP) const = 0 ;
        virtual bool can_promote(SEXP) const = 0 ;
    } ;
    
    template <int RTYPE>
    class Collecter_Impl : public Collecter {
    public:
        typedef typename Rcpp::traits::storage_type<RTYPE>::type STORAGE ;
        
        Collecter_Impl( int n_ ): data( n_, Rcpp::traits::get_na<RTYPE>() ){}
        
        void collect( const SlicingIndex& index, SEXP v ){
            Vector<RTYPE> source(v) ;
            STORAGE* source_ptr = Rcpp::internal::r_vector_start<RTYPE>(source) ;
            for( int i=0; i<index.size(); i++){
                data[index[i]] = source_ptr[i] ;
            }
        }
        
        inline SEXP get(){
            return data ;    
        }
        
        inline bool compatible(SEXP x) const{
            return RTYPE == TYPEOF(x) ;    
        }
        
        bool can_promote(SEXP x) const {
            return false ;    
        }
        
    protected:
        Vector<RTYPE> data ;
    } ;
    
    template <int RTYPE>
    class TypedCollecter : public Collecter_Impl<RTYPE>{
    public:    
        TypedCollecter( int n, CharacterVector types_) : 
            Collecter_Impl<RTYPE>(n), types(types_){}
        
        inline SEXP get(){
            Collecter_Impl<RTYPE>::data.attr("class") = types ;
            return Collecter_Impl<RTYPE>::data ;
        }
        
        inline bool compatible(SEXP x) const {
            String type = types[0] ;
            return Rf_inherits(x, type.get_cstring() ) ;    
        }
        
        inline bool can_promote(SEXP x) const {
            return false ;    
        }
        
    private:
        CharacterVector types ;
    } ;
    
    class POSIXctCollecter : public TypedCollecter<REALSXP> {
    public:
        POSIXctCollecter( int n) : 
            TypedCollecter<REALSXP>( n, CharacterVector::create( "POSIXct", "POSIXt" ) ){}
    } ;
    class DateCollecter : public TypedCollecter<REALSXP> {
    public:  
        DateCollecter( int n) : 
            TypedCollecter<REALSXP>( n, CharacterVector::create( "Date" ) ){}
    } ;
    
    class FactorCollecter : public Collecter {
    public:
        typedef boost::unordered_map<SEXP,int> LevelsMap ;
        
        FactorCollecter( int n ): 
            data(n, IntegerVector::get_na() ), levels_map(), current_level(1) {}
        
        void collect( const SlicingIndex& index, SEXP v ){
            IntegerVector source(v) ;
            SEXP levs = source.attr( "levels" ) ;
            if( ! Rf_isNull(levs) ){
                CharacterVector levels = levs ;
                SEXP* levels_ptr = Rcpp::internal::r_vector_start<STRSXP>(levels) ;
                int* source_ptr = Rcpp::internal::r_vector_start<INTSXP>(source) ;
                for( int i=0; i<index.size(); i++){ 
                    SEXP x = levels_ptr[ source_ptr[i] - 1 ] ;
                    LevelsMap::const_iterator it = levels_map.find( x ) ;
                    if( it == levels_map.end() ){
                        data[index[i]] = current_level ;
                        levels_map[x] = current_level++ ;   
                    } else {
                        data[index[i]] = it->second ;    
                    }
                }
            } else {
                for( int i=0; i<index.size(); i++){
                    int value = source[i] ;
                    if( value < current_level ){
                        data[index[i]] = source[i] ;
                    } else {
                        stop( "cannot coerce integer vector to factor" ) ;    
                    }
                }
            }
        }
        
        inline SEXP get() {
            int nlevels = levels_map.size() ;
            CharacterVector levels(nlevels);
            LevelsMap::iterator it = levels_map.begin() ;
            for( int i=0; i<nlevels; i++, ++it){
                levels[it->second - 1] = it->first ;
            }
            data.attr( "levels" ) = levels ;
            data.attr( "class" ) = "factor" ;
            return data ;
        }
        
        inline bool compatible(SEXP x) const{
            return Rf_inherits( x, "factor" ) ;    
        }
        
        bool can_promote(SEXP x) const {
            return TYPEOF(x) == STRSXP ;    
        }
        
    private:
        IntegerVector data ;
        LevelsMap levels_map ;
        int current_level ;
    } ;
    
    template <>
    inline bool Collecter_Impl<INTSXP>::compatible(SEXP x) const{
        return INTSXP == TYPEOF(x) && !Rf_inherits( x, "factor" ) ;    
    }
    
    template <>
    inline bool Collecter_Impl<INTSXP>::can_promote( SEXP x) const {
        return TYPEOF(x) == REALSXP || Rf_inherits( x, "factor" ) ;
    }
    
    template <>
    inline bool Collecter_Impl<LGLSXP>::can_promote( SEXP x) const {
        return TYPEOF(x) == INTSXP || TYPEOF(x) == REALSXP ;
    }
    
    inline Collecter* collecter(SEXP model, int n){
        switch( TYPEOF(model) ){
        case INTSXP: 
            if( Rf_inherits(model, "factor") )
                return new FactorCollecter(n) ;
            return new Collecter_Impl<INTSXP>(n) ;
        case REALSXP: 
            if( Rf_inherits( model, "POSIXct" ) )
                return new POSIXctCollecter(n) ;
            if( Rf_inherits( model, "Date" ) )
                return new DateCollecter(n) ;
            return new Collecter_Impl<REALSXP>(n) ;
        case LGLSXP: return new Collecter_Impl<LGLSXP>(n) ;
        case STRSXP: return new Collecter_Impl<STRSXP>(n) ;
        default: break ;
        }
        return 0 ;
    }
    
}

#endif
