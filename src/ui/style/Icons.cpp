#include "Icons.h"

#include <QApplication>
#include <QStyle>

namespace
{
    QStyle::StandardPixmap standardPixmap(IconId id)
    {
        switch (id)
        {
        case IconId::Home: return QStyle::SP_DirHomeIcon;
        case IconId::Projects: return QStyle::SP_FileDialogDetailedView;
        case IconId::Planning: return QStyle::SP_FileDialogContentsView;
        case IconId::Tasks: return QStyle::SP_FileDialogListView;
        case IconId::Library: return QStyle::SP_DirOpenIcon;
        case IconId::Analytics: return QStyle::SP_ComputerIcon;
        case IconId::Settings: return QStyle::SP_FileDialogDetailedView;
        }

        return QStyle::SP_FileIcon;
    }
}

QIcon Icons::get(IconId id)
{
    return QApplication::style()->standardIcon(standardPixmap(id));
}