#include "debug.hpp"
#include "plugin.hpp"

using namespace nanos;

std::string PluginManager::pluginsDir( "." );
std::vector<Plugin *> PluginManager::activePlugins;

bool PluginManager::load ( const char *name )
{
   std::string filename;
   void * handler;

   //TODO: check if already loaded

   verbose0 ( "trying to load plugin " << name );

   filename = pluginsDir + "/libnanox-" + name + ".so";
   /* open the module */
   handler = dlopen ( filename.c_str(), RTLD_GLOBAL | RTLD_NOW );

   if ( !handler ) {
      warning0 ( "plugin error=" << dlerror() );
      return false;
   }

   Plugin *plugin = ( Plugin * ) dlsym ( handler, "NanosXPlugin" );

   if ( !plugin ) {
      warning0 ( "plugin error=" << dlerror() );
      return false;
   }

   plugin->init();

   //TODO: register

   return true;
}
