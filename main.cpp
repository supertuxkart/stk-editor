#include "editor.hpp"

int main(int argc, char* argv[])
{
	Editor* editor = Editor::getEditor();

	editor->run();

	return 0;
}
