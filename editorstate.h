#ifndef EDITORSTATE_H
#define EDITORSTATE_H

#include <QString>

class EditorState
{
public:
    EditorState() {};
    void switchEditingMode() { editingGridMode = !editingGridMode; }
    bool isEditingGrid() { return editingGridMode; }
    void setCurrentFile(QString filename) { currentFile = filename; }
    QString getCurrentFile() { return currentFile; }

private:
    bool editingGridMode = false;
    QString currentFile;
};

#endif // EDITORSTATE_H
