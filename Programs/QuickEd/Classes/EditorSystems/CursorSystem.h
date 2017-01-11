#pragma once

#include "EditorSystems/BaseEditorSystem.h"
#include "EditorSystems/EditorSystemsManager.h"
#include <QMap>
#include <QPixmap>
#include <QString>
#include <QCursor>

class CursorSystem final : public BaseEditorSystem
{
public:
    explicit CursorSystem(DAVA::RenderWidget *renderWidget, EditorSystemsManager* doc);
    ~CursorSystem() override = default;

private:
    void OnActiveAreaChanged(const HUDAreaInfo& areaInfo);
    void OnDragStateChanged(EditorSystemsManager::eDragState currentState, EditorSystemsManager::eDragState previousState);

    QPixmap CreatePixmapForArea(float angle, const HUDAreaInfo::eArea area) const;
    QPixmap CreatePixmap(const QString& address) const;

    static QMap<QString, QPixmap> cursorpixes;
    QCursor lastCursor;
    
    DAVA::RenderWidget *renderWidget = nullptr;
};
