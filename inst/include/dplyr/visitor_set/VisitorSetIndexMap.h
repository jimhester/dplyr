#ifndef dplyr_VisitorSetIndexMap_H
#define dplyr_VisitorSetIndexMap_H

namespace dplyr{
                  
    template <typename VisitorSet, typename VALUE>
    class VisitorSetIndexMap : 
        public boost::unordered_map<int, VALUE, VisitorSetHasher<VisitorSet> , VisitorSetEqualPredicate<VisitorSet> > {
    private:
        typedef VisitorSetHasher<VisitorSet> Hasher ;
        typedef VisitorSetEqualPredicate<VisitorSet> EqualPredicate ;
        typedef typename boost::unordered_map<int, VALUE, Hasher, EqualPredicate> Base ;
        
    public:
        VisitorSetIndexMap() : Base(), visitors(0) {}
                   
        VisitorSetIndexMap( VisitorSet& visitors_ ) : 
            Base( 1024, Hasher(&visitors_), EqualPredicate(&visitors_) ), 
            visitors(&visitors_)
        {}
        
        VisitorSetIndexMap( VisitorSet* visitors_ ) : 
            Base( 1024, Hasher(visitors_), EqualPredicate(visitors_) ), 
            visitors(visitors_)
        {}
        
        VisitorSet* visitors ;
    
    } ;
    
    template <typename Map>
    inline void train_push_back( Map& map, int n){
        for( int i=0; i<n; i++) map[i].push_back(i) ;
    }
    
    template <typename Set>
    inline void train_insert( Set& set, int n){
        for( int i=0; i<n; i++) set.insert(i) ;
    }
    template <typename Set>
    inline void train_insert_right( Set& set, int n){
        for( int i=0; i<n; i++) set.insert(-i-1) ;
    }

}

#endif
