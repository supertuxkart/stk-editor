#include "editor.hpp"

int main(int argc, char* argv[])
{
	Editor* editor = Editor::getEditor();

    if (editor!=0)
	    editor->run();

	return 0;
}
