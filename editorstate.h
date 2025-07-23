#ifndef EDITORSTATE_H
#define EDITORSTATE_H

class EditorState
{
public:
    EditorState() {};
    void switchEditingMode() { editingGridMode = !editingGridMode; }
    bool isEditingGrid() { return editingGridMode; }

private:
    bool editingGridMode = false;
};

#endif // EDITORSTATE_H
