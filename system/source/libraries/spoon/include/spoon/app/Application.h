#ifndef _SPOON_APP_APPLICATION_H
#define _SPOON_APP_APPLICATION_H

#include <spoon/base/Lockable.h>
#include <spoon/support/String.h>
#include <spoon/collections/List.h>
#include <spoon/ipc/Looper.h>
#include <spoon/ipc/Message.h>
#include <spoon/ipc/Pulse.h>

class Window;

extern Application *application;

/**  \ingroup app
 *
 * The Application class is the main class in your whole process. You
 * will typically derive an Application class and set it running. The
 * logic of your application will be implemented in the Application
 * itself or in the Windows that your Application creates.
 *
 * The Application implements a very similar loop to the Looper and
 * you will want to override the MessageReceived and the PulseReceived
 * to handle incoming events.  The Application should listen on port 0 
 * in the system. In fact, all Applications should be accessable on 
 * port 0.
 *
 * Apon construction, the Application sets the global variable
 * application to point to itself.  You can access your own application
 * by referencing this variable but be sure to cast it to your
 * child class if you wish to use any methods you implemented
 * yourself.
 *
 * Example:  
 *
 * \code
 * 		((MyApp*)application)->MyFunction();
 * \endcode
 *
 *
 * Instantiation should be following by an immediate Run() call - 
 * which will not return until the Application has quit.  Here is a 
 * very simple child Application and the main function.
 * 
 * \code
 * 
 * #include <spoon/app/Application.h>
 *
 * 		class MyApp : public Application
 * 		{
 * 			public:
 * 				MyApp() : Application("MyApp") {};
 * 		}
 *
 * 		int main( int argc, char *argv[] )
 * 		{
 * 				MyApp app = new MyApp();
 * 					  app->Run();
 * 				delete app;
 *
 * 			return 0;
 * 		}
 * 
 * 
 * \endcode
 *
 * Also, perhaps the most important part of the Application to is manage
 * and maintain a list of Windows. All Windows which are created within
 * the context of the process (so every single one) register themselves
 * with the Application if it exists.
 * 
 */

class Application : public Lockable 
{
   
   public:
     Application( const char *name );
     virtual ~Application();


	 /** Returns the name of the application. */
     String getName() { return app_name; }
	 

	 virtual void MessageReceived( Message *msg );
	 virtual void PulseReceived( Pulse *pulse );

	 virtual void QuitRequested();
	 
	 
	 virtual int Init();
	 virtual void Shutdown();
	 
	 int Run();
	 
	 
	 bool    AddWindow( Window *win );
	 Window* WindowAt( int index );
	 int     CountWindows();
	 bool    RemoveWindow( Window *win ); 
	 


   private:
     String app_name;			///< Application name set during construction.
	 List _window_list;			///< Local Collection used to maintain the Window list.
	 bool _app_terminated;		///< A flag used to indicate to the main loop that it's time to die.
	 
};


#endif

