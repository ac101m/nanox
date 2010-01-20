/*************************************************************************************/
/*      Copyright 2009 Barcelona Supercomputing Center                               */
/*                                                                                   */
/*      This file is part of the NANOS++ library.                                    */
/*                                                                                   */
/*      NANOS++ is free software: you can redistribute it and/or modify              */
/*      it under the terms of the GNU Lesser General Public License as published by  */
/*      the Free Software Foundation, either version 3 of the License, or            */
/*      (at your option) any later version.                                          */
/*                                                                                   */
/*      NANOS++ is distributed in the hope that it will be useful,                   */
/*      but WITHOUT ANY WARRANTY; without even the implied warranty of               */
/*      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                */
/*      GNU Lesser General Public License for more details.                          */
/*                                                                                   */
/*      You should have received a copy of the GNU Lesser General Public License     */
/*      along with NANOS++.  If not, see <http://www.gnu.org/licenses/>.             */
/*************************************************************************************/

#ifndef _NANOS_CONFIG
#define _NANOS_CONFIG

#include <stdexcept>
#include <vector>
#include "compatibility.hpp"
#include <memory>
#include <sstream>
#include <string.h>

namespace nanos
{

   class Config
   {

      public:
         // Checking predicates
         // true predicate

         template<typename T> class CheckValue
         {

            public:
               virtual ~CheckValue() {}

               virtual bool operator() ( const T &value ) const { return true; };
         };

         // isPositive predicate

         template<typename T> class isPositive : public CheckValue<T>
         {

            public:
               virtual ~isPositive() {}

               virtual bool operator() ( const T &value ) const { return value > 0; }
         };

         /** Configuration options */
         // Abstract Base Class

         class Option
         {

            public:
               typedef enum { FLAG, VALUE } OptType;

            private:
               std::string _name;
               OptType _type;

            public:
               // constructors
               Option( const std::string &n, const OptType t ) : _name( n ), _type( t ) {}

               Option( const char *n, const OptType t ) : _name( n ), _type( t ) {}

               // copy constructor
               Option( const Option &opt ) : _name( opt._name ),_type( opt._type ) {}

               // assignment operator
               const Option & operator= ( const Option &opt );
               // destructors
               virtual ~Option() {};

               const std::string &getName() const { return _name; }

               const OptType& getType() const { return _type; }

               virtual void parse ( const char *value ) = 0;

               // clone idiom
               virtual Option * clone () = 0;
         };

         // Action Option, Base Class
         template<typename T, typename checkT = CheckValue<T> >

         class ActionOption : public Option
         {

            private:
               checkT _check;

            public:
               // constructors
               ActionOption( const std::string &name ) :
                     Option( name,Option::VALUE ) {}

               ActionOption( const char *name ) :
                     Option( name,Option::VALUE ) {}

               // copy constructors
               ActionOption( const ActionOption& opt ) : Option( opt ), _check( opt._check ) {}

               // assignment operator
               const ActionOption & operator= ( const ActionOption & opt );
               // destructor
               virtual ~ActionOption() {}

               virtual void parse ( const char *value );
               virtual void setValue ( const T &value ) = 0;

               bool checkValue ( const T &value ) const	{ return _check( value );  };

               virtual ActionOption * clone () = 0;
         };

         // VarOption: Option modifies a variable
         template<typename T, typename checkT= CheckValue<T> >

         class VarOption : public ActionOption<T,checkT>
         {

            private:
               T &_var;
               // assignment operator
               const VarOption & operator= ( const VarOption &opt );

            public:
               //constructors
               VarOption( const std::string &name,T &ref ) :
                     ActionOption<T,checkT>( name ),_var( ref ) {}

               VarOption( const char *name, T &ref ) :
                     ActionOption<T,checkT>( name ),_var( ref ) {}

               // copy constructor
               VarOption( const VarOption &opt ) :
                     ActionOption<T,checkT>( opt ),_var( opt._var ) {}

               //destructor
               virtual ~VarOption() {}

               virtual void setValue ( const T &value ) { _var = value; };

               virtual VarOption * clone () { return new VarOption( *this ); };
         };

         // shortcuts for VarOptions and ActionOptions

         typedef class VarOption<int>                       IntegerVar;

         typedef class VarOption<bool>                      BoolVar;

         typedef class VarOption<std::string>               StringVar;

         typedef class VarOption<int,isPositive<int> >      PositiveVar;
         typedef class VarOption<size_t,isPositive<size_t> > SizeVar;

         typedef class ActionOption<int>                    IntegerAction;

         typedef class ActionOption<bool>                   BoolAction;

         typedef class ActionOption<std::string>            StringAction;

         typedef class ActionOption<int,isPositive<int> >   PositiveAction;

         template<typename T> class MapAction : public Option
         {

            public:
               typedef std::pair<std::string,T> MapOption;
               typedef std::vector<MapOption> MapList;

            private:
               const MapList _options;

            public:
               // constructors
               MapAction( const std::string &name, const MapList &opts ) :
                     Option( name,Option::VALUE ), _options( opts ) {}

               MapAction( const char *name, const MapList &opts ) :
                     Option( name,Option::VALUE ), _options( opts ) {}

               // copy constructor
               MapAction( const MapAction &opt ) : Option( opt ),_options( opt._options ) {}

               // assignment operator
               const MapAction & operator= ( const MapAction &opt );
               // destructor
               virtual ~MapAction() {}

               virtual void parse ( const char *name );
               virtual void setValue ( const T &value ) = 0;
               virtual MapAction * clone () = 0;
         };

         template<typename T> class MapVar : public MapAction<T>
         {

            private:
               T &_var;
               // assignment operator
               const MapVar & operator= ( const MapVar & );

            public:
               typedef typename MapAction<T>::MapOption MapOption;
               typedef typename MapAction<T>::MapList MapList;

               //constructors
               MapVar( const std::string &name, T &ref, const MapList &opts ) :
                     MapAction<T>( name,opts ), _var( ref ) {}

               MapVar( const char *name, T &ref, const MapList &opts ) :
                     MapAction<T>( name,opts ), _var( ref ) {}

               // copy constructor
               MapVar( const MapVar &opt ) : MapAction<T>( opt ), _var( opt._var ) {}

               // destructor
               virtual ~MapVar() {}

               virtual void setValue ( const T &value ) { _var = value; };

               virtual MapVar * clone () { return new MapVar( *this ); };
         };

         class ActionFlag : public Option
         {
            public:
               // constructors
               ActionFlag( const std::string &name ) :
                     Option( name,Option::FLAG ) {}

               ActionFlag( const char *name ) :
                     Option( name,Option::FLAG ) {}

               // copy constructors
               ActionFlag( const ActionFlag& opt ) : Option( opt ) {}

               // assignment operator
               const ActionFlag & operator= ( const ActionFlag & opt );
               // destructor
               virtual ~ActionFlag() {}

               virtual void parse ( const char *value );
               virtual void setValue ( const bool &value ) = 0;

               virtual ActionFlag * clone () = 0;
         };

         class FlagOption : public ActionFlag
         {

            private:
               bool &_var;
               bool  _setTo;
               // assigment operator
               const FlagOption & operator= ( const FlagOption &opt );

            public:
               // constructors
               FlagOption ( const std::string &name, bool &ref, bool value=true ) :
                     ActionFlag( name ),_var( ref ),_setTo( value ) {}

               FlagOption ( const char *name, bool &ref, bool value=true ) :
                     ActionFlag( name ),_var( ref ),_setTo( value ) {}

               // copy constructors
               FlagOption( const FlagOption &opt ) : ActionFlag( opt ), _var( opt._var ), _setTo( opt._setTo ) {}

               // destructor
               virtual ~FlagOption() {}

               virtual void setValue ( const bool &value ) { _var = !( value ^ _setTo ); };

               virtual FlagOption * clone () { return new FlagOption( *this ); };
         };

         typedef TR1::unordered_map<std::string, Option *> OptionMap;
         typedef std::vector<Option *> OptionList;

      private:
         OptionList _envOptions;
         OptionMap  _argOptions;

      protected:

         virtual void setDefaults();
         void parseFiles();
         void parseArguments();
         void parseEnvironment();
         void clear();
         void copy( const Config &origin );

      public:
         // constructors
         Config() {}

         // copy constructors
         Config( const Config &cfg );
         // assignment operator
         const Config & operator= ( const Config &cfg );
         // destructor
         virtual ~Config ();

         void init();
         void registerEnvOption ( Option *opt );
         void registerArgOption ( Option *opt );
   };

   /** exceptions */

   class InvalidOptionException : public  std::runtime_error
   {

      public:
         InvalidOptionException( const Config::Option &option,
                                 const std::string &value ) :
               runtime_error( std::string( "Ignoring invalid value '" )+value
                              +"' for "+option.getName() ) {}

   };

   /** inline functions */

   inline const Config::Option & Config::Option::operator= ( const Config::Option &opt )
   {
      // self-assigment: ok
      this->_name = opt._name;
      this->_type = opt._type;
      return *this;
   }

   template<typename T,typename checkT>
   const Config::ActionOption<T,checkT> &
   Config::ActionOption<T,checkT>::operator=
   ( const Config::ActionOption<T,checkT> & opt )
   {
      // self-assigment: ok
      Option::operator=( opt );
      this->_check = opt._check;
      return *this;
   }

   template<typename T>
   const Config::MapAction<T> & Config::MapAction<T>::operator= ( const Config::MapAction<T> &opt )
   {
      // self->assigment: ok
      Option::operator=( opt );
      this->_options = opt._options;
      return *this;
   }

   template<typename T,typename checkT>
   void Config::ActionOption<T,checkT>::parse ( const char *value )
   {
      T t;
      std::istringstream iss( value );

      if ( ( iss >> t ).fail() )
         throw InvalidOptionException( *this,value );

      if ( !iss.eof() )
         throw InvalidOptionException( *this,value );

      if ( ! checkValue( t ) )
         throw InvalidOptionException( *this,value );

      setValue( t );
   }

   template<typename T>
   void Config::MapAction<T>::parse ( const char *value )
   {
      typename MapList::const_iterator it;

      for ( it = _options.begin(); it < _options.end(); it++ ) {
         if ( value == it->first ) {
            setValue( it->second );
            return;
         }
      }

      throw InvalidOptionException( *this,value );
   }

   inline void Config::ActionFlag::parse ( const char *value )
   {
      if ( strcasecmp(value, "yes" ) == 0) {
         setValue( true );
      } else if ( strcasecmp(value, "no" ) == 0 ) {
         setValue( false );
      } else
         throw InvalidOptionException( *this,value );
   }

};

#endif
