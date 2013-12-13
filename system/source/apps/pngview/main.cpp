#include <stdio.h>
#include <unistd.h>

#include <spoon/interface/Window.h>
#include <spoon/interface/Bitmap.h>

#include <spoon/translators/PNGTranslator.h>


int main( int argc, char *argv[] )
{
	if ( argc != 2 ) 
	{
		printf("%s%s%s\n","usage: ", argv[0], " filename.png" );
		return -1;
	}


	PNGTranslator *loader = new PNGTranslator();
	Bitmap *bitmap = loader->load( argv[1] );
	delete loader;

	if ( bitmap == NULL )
	{
		printf("%s\n", "unable to load the file.");
		return -1;
	}

	Rect position = bitmap->Frame();

		position.left 	+= 20;
		position.top 	+= 20;
		position.right 	+= 20;
		position.bottom += 20;

	Window *win = new Window( position, argv[1] );
			win->Show();

	for ( int i = 0; i < bitmap->Frame().Width(); i++ )
	 for ( int j = 0; j < bitmap->Frame().Height(); j++ )
	 {
		win->PutPixel( i, j, bitmap->GetPixel( i, j ) );
	 }
	
	// ************************


	win->Wait();

	return 0;
}

